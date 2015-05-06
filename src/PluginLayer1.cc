//------------------------------------------------------------------------------
// File PluginLayer1.cc
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
#include "PluginLayer1.hh"
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
  std::cout << "Register PluginLayer1 ..." << std::endl;
  PF_RegisterParams rp;
  rp.version.major = 0;
  rp.version.minor = 1;
  rp.layer = PF_Plugin_Layer::L1;
 
  // Register the PluginLayer1 object provided by the current plugin
  rp.CreateFunc = PluginLayer1::Create;
  rp.DestroyFunc = PluginLayer1::Destroy;
  int res = services->registerObject("PluginLayer1", &rp);

  if (res)
  {
    std::cerr << "PluginLayer1 registration failed" << std::endl;
    return NULL;
  }

  return ExitFunc;
}

//------------------------------------------------------------------------------
// Static plugin interface - create object 
//------------------------------------------------------------------------------
void* PluginLayer1::Create(PF_PlatformServices* services)
{
  // Call the descovery service for the lower layer object
  LayerInterface* lowerLayer = static_cast<LayerInterface*>(0);
  PF_Discovery_Service discover = { "PluginLayer1", 0};
  int retc = services->invokeService( "discovery", (void*) &discover);

  if (retc)
  {
    std::cerr << "Error while invoking discovery service" << std::endl;
    return NULL;
  }
  
  lowerLayer = static_cast<LayerInterface*>(discover.ptrService);
  return new PluginLayer1(lowerLayer);
}

//------------------------------------------------------------------------------
// Static plugin interface - destroy object
//------------------------------------------------------------------------------
int32_t
PluginLayer1::Destroy(void* obj)
{
  if (!obj)
    return -1;

  delete static_cast<PluginLayer1*>(obj);
  return 0;
}

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
PluginLayer1::PluginLayer1(LayerInterface* lowerLayer):
  LayerInterface(),
  mLowerLayer(lowerLayer)
{
  // empty
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
PluginLayer1::~PluginLayer1()
{
  // empty
}

//------------------------------------------------------------------------------
// MethodCall implementation
//------------------------------------------------------------------------------
int32_t
PluginLayer1::MethodCall()
{
  std::cout << __PRETTY_FUNCTION__ << " calling method" << std::endl;
  return (mLowerLayer ? mLowerLayer->MethodCall() : 0);
}

