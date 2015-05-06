//------------------------------------------------------------------------------
//! @file Application.hh
//! @author Elvin-Alin Sindrilaru <esindril@cern.ch>
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
/*----------------------------------------------------------------------------*/
#include "Application.hh"
#include "LayerInterface.hh"
#include "PluginManager.hh"
/*----------------------------------------------------------------------------*/

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
Application::Application()
{
  // empty
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
Application::~Application()
{
  // empty
}

//------------------------------------------------------------------------------
// Get instance
//------------------------------------------------------------------------------
Application&
Application::GetInstance()
{
  static Application app;
  return app;
}

//------------------------------------------------------------------------------
// Initialize the plugin stack
//------------------------------------------------------------------------------
void
Application::InitPluginStack()
{
  pf::PluginManager& pm = pf::PluginManager::GetInstance();
  auto obj_map = pm.GetRegistrationMap();
  LayerInterface* layer_imp = static_cast<LayerInterface*>(0);
    
  for (auto&& plugin: obj_map)
  {
    PF_Plugin_Layer layer = plugin.second.layer;
    
    if (mLayerImp.count(layer))
    {
      std::cerr << "Error: double plugin layer implementation" << std::endl;
      continue;
    }
    
    layer_imp = static_cast<LayerInterface*>(pm.CreateObject(plugin.first));
    mLayerImp[layer] = layer_imp;
  }
}


