//------------------------------------------------------------------------------
// File PluginLayer0.cc
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
#include "PluginLayer0.hh"
/*----------------------------------------------------------------------------*/
#include <iostream>
/*----------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
// Plugin exit function called by the PluginManager when doing cleanup
//------------------------------------------------------------------------------
int32_t ExitFunc()
{
  return 0;  
}

//------------------------------------------------------------------------------
// Plugin registration entry point called by the PluginManager
//------------------------------------------------------------------------------
PF_ExitFunc PF_initPlugin(const PF_PlatformServices* services)
{
  std::cout << "Register PluginLayer0 ..." << std::endl;
  PF_RegisterParams rp;
  rp.version.major = 0;
  rp.version.minor = 1;
  rp.layer = PF_Plugin_Layer::L0;

  // Register the PluginLayer0 object provided by the current plugin
  rp.CreateFunc = PluginLayer0::Create;
  rp.DestroyFunc = PluginLayer0::Destroy;
  int res = services->registerObject("PluginLayer0", &rp);

  if (res)
  {
    std::cerr << "PluginLayer0 registration failed" << std::endl;
    return NULL;
  }

  return ExitFunc;
}

//------------------------------------------------------------------------------
// Static plugin interface - create object 
//------------------------------------------------------------------------------
void* PluginLayer0::Create(PF_PlatformServices* services)
{
  // Call the discovery service for the lower layer object
  PF_Discovery_Service discover = { "PluginLayer0", 0};
  int retc = services->invokeService( "discovery", (void*) &discover);

  if (retc)
  {
    std::cerr << "Error while invoking discovery service" << std::endl;
    return NULL;
  }
  
  LayerInterface* lowerLayer = static_cast<LayerInterface*>(discover.ptrService);
  return new PluginLayer0(lowerLayer);
}

//------------------------------------------------------------------------------
// Static plugin interface - destroy object
//------------------------------------------------------------------------------
int32_t
PluginLayer0::Destroy(void* obj)
{
  if (!obj)
    return -1;

  delete static_cast<PluginLayer0*>(obj);
  return 0;
}

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
PluginLayer0::PluginLayer0(LayerInterface* lowerLayer):
  LayerInterface(),
  mLowerLayer(lowerLayer)
{
  // empty
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
PluginLayer0::~PluginLayer0()
{
  // empty
}

//------------------------------------------------------------------------------
// MethodCall implementation
//------------------------------------------------------------------------------
int32_t
PluginLayer0::MethodCall()
{
  std::cout << __PRETTY_FUNCTION__ << " calling method" << std::endl;
  return (mLowerLayer ? mLowerLayer->MethodCall() : 0);
}
