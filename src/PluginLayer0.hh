//------------------------------------------------------------------------------
//! @file PluginLayer0.hh
//! @author Elvin-Alin Sindrilaru <esindril@cern.ch>
//! @brief Demo plugin implementation for layer 0
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

#ifndef __PF_PLUGIN_LAYER0_HH__
#define __PF_PLUGIN_LAYER0_HH__

/*----------------------------------------------------------------------------*/
#include "Plugin.hh"
#include "LayerInterface.hh"
/*----------------------------------------------------------------------------*/


//------------------------------------------------------------------------------
//! Class PluginLayer0
//------------------------------------------------------------------------------
class PluginLayer0 : public LayerInterface
{
 public:

  //----------------------------------------------------------------------------
  //! Static plugin interface - create object 
  //----------------------------------------------------------------------------
  static void* Create(PF_ObjectParams* params);


  //----------------------------------------------------------------------------
  //! Static plugin interface - destroy object
  //----------------------------------------------------------------------------
  static int32_t Destroy(void *);

  
  //----------------------------------------------------------------------------
  //! Destructor
  //----------------------------------------------------------------------------
  virtual ~PluginLayer0();


  //----------------------------------------------------------------------------
  //! Impelement LayerInterface method
  //----------------------------------------------------------------------------
  virtual int32_t MethodCall();

 private:

  LayerInterface* mLowerLayer; ///< pointer to lower layer interface

  //----------------------------------------------------------------------------
  //! Constructor
  //!
  //! @param lowerLayer lower layer interface provided by the PluginManager
  //!
  //----------------------------------------------------------------------------
  PluginLayer0(LayerInterface* lowerLayer);
};


//------------------------------------------------------------------------------
//! Plugin exit function called by the PluginManager when doing cleanup
//------------------------------------------------------------------------------
extern "C" int32_t ExitFunc();


//------------------------------------------------------------------------------
//! Plugin initialization entry point called by the PluginManager
//------------------------------------------------------------------------------
extern "C" PF_ExitFunc PF_initPlugin(const PF_PlatformServices* params);

#endif // __PF_PLUGIN_LAYER0_HH__
