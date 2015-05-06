//------------------------------------------------------------------------------
// main.cc
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
#include <iostream>
#include <string.h>
#include "LayerInterface.hh"
#include "PluginManager.hh"
#include "Application.hh"
/*----------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
// Invoke application services outside of the plugin object
//------------------------------------------------------------------------------
int32_t
InvokeService(const char* serviceName, void* serviceParam)
{
  if (strncmp(serviceName, "discovery", strlen(serviceName)) == 0)
  {
    // Check if the object requesting a service is already registered
    auto discover = static_cast<PF_Discovery_Service*>(serviceParam);
    pf::PluginManager& pm = pf::PluginManager::GetInstance();
    auto reg_map = pm.GetRegistrationMap();
    auto reg_obj = reg_map.find(std::string(discover->objType));

    if (reg_obj != reg_map.end())
    {
      // Try to get previous layer implementation if this is not the bottom one
      if (reg_obj->second.layer - 1 >= 0)
      {
	PF_Plugin_Layer search_layer = (PF_Plugin_Layer)(reg_obj->second.layer - 1);
	Application& app = Application::GetInstance();
	auto layer_imp = app.mLayerImp.find(search_layer);

	if (layer_imp != app.mLayerImp.end())
	{
	  std::cout << "Found the lower layer implementation ..." << std::endl;
	  discover->ptrService = static_cast<void*>(layer_imp->second);
	  return 0;
	}
      }
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
// Main function
//------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cerr << "Usage: loadPlugin <plugins_dir>" << std::endl;
    return -1;
  }

  pf::PluginManager& pm = pf::PluginManager::GetInstance();
  pm.GetPlatformServices().invokeService = InvokeService;
  pm.LoadAll(argv[1]);

  Application& app = Application::GetInstance();
  app.InitPluginStack();

  for (auto&& impl: app.mLayerImp)
  {
    std::cout << "Calling from Layer: " << impl.first << std::endl;
    impl.second->MethodCall();
  }
}
