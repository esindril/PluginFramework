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
#include "LayerInterface.hh"
#include "PluginManager.hh"
/*----------------------------------------------------------------------------*/


//------------------------------------------------------------------------------
// Invoke application services outside of the plugin object 
//------------------------------------------------------------------------------
int32_t
InvokeService(const char* serviceName, void* serviceParam)
{
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

  PluginManager& pm = PluginManager::GetInstance();
  pm.GetPlatformServices().invokeService = InvokeService;
  pm.LoadAll(argv[1]);
  pm.InitializeStack();
}
