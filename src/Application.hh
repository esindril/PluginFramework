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

#ifndef __PF_APPLICATION_HH__
#define __PF_APPLICATION_HH__

/*----------------------------------------------------------------------------*/
#include <map>
/*----------------------------------------------------------------------------*/
#include "Plugin.hh"
/*----------------------------------------------------------------------------*/

//! Forward delcaration
class LayerInterface;

//------------------------------------------------------------------------------
//! Class Application
//------------------------------------------------------------------------------
class Application
{
 public:

  typedef std::map<PF_Plugin_Layer, LayerInterface*> LayerImpMap;

  //----------------------------------------------------------------------------
  //! Get instance
  //----------------------------------------------------------------------------
  static Application& GetInstance();

  //----------------------------------------------------------------------------
  //! Destructor
  //----------------------------------------------------------------------------
  ~Application();

  //------------------------------------------------------------------------------
  //! Initialize the plugin stack
  //------------------------------------------------------------------------------
  void InitPluginStack();

  LayerImpMap mLayerImp; ///< map between layer and implementation object

 private:

  //----------------------------------------------------------------------------
  //! Constructor
  //----------------------------------------------------------------------------
  Application();
};

# endif // __PF_APPLICATION_HH__
