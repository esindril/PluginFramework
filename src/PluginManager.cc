//------------------------------------------------------------------------------
// File PluginManager.cc
// Author Elvin-Alin Sindrilaru <esindril@cern.ch>
//------------------------------------------------------------------------------

/************************************************************************
 * EOS - the CERN Disk Storage System                                   *
 * Copyright (C) 2013 CERN/Switzerland                                  *
 *                                                                      *
 * This program is free software: you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation, either version 3 of the License, or    *
 * (at your option) any later version.                                  *
 *                                                                      *
 * This program is distributed in the hope that it will be useful,      *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
 * GNU General Public License for more details.                         *
 *                                                                      *
 * You should have received a copy of the GNU General Public License    *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*
 ************************************************************************/

/*----------------------------------------------------------------------------*/
#include <string>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
/*----------------------------------------------------------------------------*/
#include "PluginManager.hh"
#include "DynamicLibrary.hh"
/*----------------------------------------------------------------------------*/

static std::string dynLibExtension("so");

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
PluginManager::PluginManager()
{
  mPlatformServices.version.major = 0;
  mPlatformServices.version.minor = 1;
  mPlatformServices.invokeService = NULL; // can be populated during loadAll()
  mPlatformServices.registerObject = RegisterObject;
}


//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
PluginManager::~PluginManager()
{
  // Just in case it wasn't called earlier
  Shutdown();
}


//------------------------------------------------------------------------------
// Get instance of PluginManager
//------------------------------------------------------------------------------
PluginManager&
PluginManager::GetInstance()
{
  static PluginManager instance;
  return instance;
}


//------------------------------------------------------------------------------
// Shutdown method
//------------------------------------------------------------------------------
int32_t PluginManager::Shutdown()
{
  int32_t result = 0;

  for (auto func = mExitFuncVec.begin(); func != mExitFuncVec.end(); ++func)
  {
    try
    {
      result += (*func)();
    }
    catch (...)
    {
      result = -1;
    }
  }

  mDynamicLibMap.clear();
  mExactMatchMap.clear();
  mWildCardVec.clear();
  mExitFuncVec.clear();
  return result;
}


//------------------------------------------------------------------------------
// The registration params may be received from an external plugin so it is
// crucial to validate it, because it was never subjected to our tests.
//------------------------------------------------------------------------------
static bool IsValid(const char* objectType,
                    const PF_RegisterParams* params)
{
  // No object type specified
  if (!objectType || !(*objectType))
    return false;

  // Plugin does not implement the interface
  if (!params || !params->CreateFunc || !params->DestroyFunc)
    return false;

  return true;
}


//------------------------------------------------------------------------------
// Initialize plugin
//------------------------------------------------------------------------------
int32_t
PluginManager::InitializePlugin(PF_InitFunc initFunc)
{
  PluginManager& pm = PluginManager::GetInstance();
  PF_ExitFunc exitFunc = initFunc(&pm.mPlatformServices);

  if (!exitFunc)
    return -1;

  // Store the exit func so it can be called when unloading this plugin
  pm.mExitFuncVec.push_back(exitFunc);
  return 0;
}


//------------------------------------------------------------------------------
// Plugin registers the objects that it provides through this function
//------------------------------------------------------------------------------
int32_t
PluginManager::RegisterObject(const char* objectType,
                              const PF_RegisterParams* params)
{
  // Check parameters
  if (!IsValid(objectType, params))
    return -1;

  PluginManager& pm = PluginManager::GetInstance();
  
  // Verify that versions match
  PF_PluginAPI_Version v = pm.mPlatformServices.version;

  if (v.major != params->version.major)
  {
    std::cerr << "Plugin manager API and plugin API version missmatch" << std::endl;
    return -1;
  }
  
  std::string key(objectType);

  // If it's a wild card registration just add it
  if (key == std::string("*"))
  {
    pm.mWildCardVec.push_back(*params);
    return 0;
  }

  // Fail if item already exists (only one can handle)
  if (pm.mExactMatchMap.find(key) != pm.mExactMatchMap.end())
    return -1;

  pm.mExactMatchMap[key] = *params;
  return 0;
}


//------------------------------------------------------------------------------
// Load all dynamic libraries from directory
//------------------------------------------------------------------------------
int32_t
PluginManager::LoadAll(const std::string& pluginDir,
                       PF_InvokeServiceFunc func)
{
  std::string dir_path(pluginDir);
  
  if (dir_path.empty())
  {
    std::cerr << "Plugin path is empty" << std::endl;
    return -1;
  }

  // If relative path, get current working directory
  if (dir_path[0] == '.')
  {
    char* cwd = 0;
    size_t size = 0;
    cwd = getcwd(cwd, size);
    
    if (cwd)
    {
      std::string tmp_path = cwd;
      dir_path = dir_path.erase(0, 1);
      dir_path = tmp_path + dir_path;       
      free(cwd);
    }
  }

  // Add backslash at the end 
  if (dir_path[dir_path.length() - 1] != '/')
    dir_path += '/';

  if (func)
    mPlatformServices.invokeService = func;

  auto dir = opendir(dir_path.c_str());

  if (dir == 0)
  {
    std::cerr << "Cannot open dir: " << dir_path << std::endl;
    return -1;
  }

  std::string full_path;
  struct dirent* entity = 0; 
  
  while ((entity = readdir(dir)))
  {
    // Skip directories and files with wrong extension
    if (entity->d_type & DT_DIR)
      continue;
    
    full_path= dir_path + entity->d_name;

    if (full_path.find_last_of(dynLibExtension) !=
        full_path.length() - dynLibExtension.length() + 1)
    {
      continue;
    }
    
    // Load library 
    LoadByPath(full_path);
  }

  return 0;
}


//------------------------------------------------------------------------------
// Get plugin object from dynamic library
//------------------------------------------------------------------------------
int32_t
PluginManager::LoadByPath(const std::string& pluginPath)
{
  // Don't load the same dynamic library twice
  if (mDynamicLibMap.find(pluginPath) != mDynamicLibMap.end())
    return -1;

  std::string error;
  DynamicLibrary* dyn_lib = LoadLibrary(pluginPath, error);

  if (!dyn_lib)
  {
    std::cerr << error << std::endl;
    return -1;
  }
  
  // Get the *_initPlugin() function
  PF_InitFunc initFunc = (PF_InitFunc)(dyn_lib->GetSymbol("PF_initPlugin"));

  // Expected entry point missing from dynamic library
  if (!initFunc)
    return -1;
  
  int32_t res = InitializePlugin(initFunc);
  
  if (res < 0)
    return res;

  return 0;
}

//------------------------------------------------------------------------------
// Create object using the loaded plugin handlers
//------------------------------------------------------------------------------
void*
PluginManager::CreateObject(const std::string& objectType)
{
  // "*" is not a valid object type
  if (objectType == std::string("*"))
    return NULL;

  // Prepare object params
  PF_ObjectParams np;
  np.objectType = objectType.c_str();
  np.platformServices = &mPlatformServices;

  // Exact match found
  if (mExactMatchMap.find(objectType) != mExactMatchMap.end())
  {
    PF_RegisterParams& rp = mExactMatchMap[objectType];
    void* object = rp.CreateFunc(&np);
    
    if (object)
    {
      // Register the new plugin object for the appropriate layer
      
      return object;
    }
  }

  // Try to find a wild card match
  for (size_t i = 0; i < mWildCardVec.size(); ++i)
  {
    PF_RegisterParams& rp = mWildCardVec[i];
    void* object = rp.CreateFunc(&np);

    if (object) 
    {
      // Promote registration to exactMatc_
      // (but keep also the wild card registration for other object types)
      int32_t res = RegisterObject(np.objectType, &rp);

      if (res < 0)
      {
        // Should report or log it
        rp.DestroyFunc(object);
        return NULL;
      }

      return object;
    }
  }
  
  return NULL;
}



//------------------------------------------------------------------------------
//! Initialize the plugin stack
//------------------------------------------------------------------------------
int32_t
PluginManager::InitPluginStack()
{
  for (auto plugin = mExactMatchMap.begin(); plugin != mExactMatchMap.end(); ++plugin)
  {
    LayerInterface* layer = static_cast<LayerInterface*>(CreateObject(plugin->first));
    
  }
}


//------------------------------------------------------------------------------
// Load dynamic library
//------------------------------------------------------------------------------
DynamicLibrary*
PluginManager::LoadLibrary(const std::string& path, std::string& error)
{
  DynamicLibrary* dyn_lib = DynamicLibrary::Load(path, error);

  if (!dyn_lib)
    return NULL;

  // Add library to map, so it can be unloaded
  mDynamicLibMap[path] = std::shared_ptr<DynamicLibrary>(dyn_lib);
  return dyn_lib;
}


//------------------------------------------------------------------------------
// Get registration map
//------------------------------------------------------------------------------
const PluginManager::RegistrationMap&
PluginManager::GetRegistrationMap() const
{
  return mExactMatchMap;
}


//------------------------------------------------------------------------------
// Get available platform services
//------------------------------------------------------------------------------
PF_PlatformServices&
PluginManager::GetPlatformServices()
{
  return mPlatformServices;
}
