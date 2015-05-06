#-------------------------------------------------------------------------------
# File: CMakeLists.txt
# Author: Elvin-Alin Sindrilaru - CERN
#-------------------------------------------------------------------------------

# ************************************************************************
# * EOS - the CERN Disk Storage System                                   *
# * Copyright (C) 2013 CERN/Switzerland                                  *
# *                                                                      *
# * This program is free software: you can redistribute it and/or modify *
# * it under the terms of the GNU General Public License as published by *
# * the Free Software Foundation, either version 3 of the License, or    *
# * (at your option) any later version.                                  *
# *                                                                      *
# * This program is distributed in the hope that it will be useful,      *
# * but WITHOUT ANY WARRANTY; without even the implied warranty of       *
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        *
# * GNU General Public License for more details.                         *
# *                                                                      *
# * You should have received a copy of the GNU General Public License    *
# * along with this program.  If not, see <http://www.gnu.org/licenses/>.*
# ************************************************************************

include_directories(
  ${PROJECT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR})

add_library(
  PluginManager SHARED
  Plugin.hh
  PluginManager.cc   PluginManager.hh
  DynamicLibrary.cc  DynamicLibrary.hh)

set_target_properties(
  PluginManager
  PROPERTIES
  VERSION ${VERSION}
  SOVERSION ${VERSION_MAJOR}
  INTERFACE_LINK_LIBRARIES ""
  MACOSX_RPATH TRUE)

add_library(
  PluginLayer0 SHARED
  Plugin.hh
  LayerInterface.hh
  PluginLayer0.cc  PluginLayer0.hh)

set_target_properties(
  PluginLayer0
  PROPERTIES
  VERSION ${VERSION}
  SOVERSION ${VERSION_MAJOR}
  INTERFACE_LINK_LIBRARIES ""
  MACOSX_RPATH TRUE)

add_library(
  PluginLayer1 SHARED
  Plugin.hh
  LayerInterface.hh
  PluginLayer1.cc  PluginLayer1.hh)

set_target_properties(
  PluginLayer1
  PROPERTIES
  VERSION ${VERSION}
  SOVERSION ${VERSION_MAJOR}
  INTERFACE_LINK_LIBRARIES ""
  MACOSX_RPATH TRUE)

add_executable(
  loadPlugin
  main.cc
  Plugin.hh
  LayerInterface.hh
  Application.cc Application.hh)

target_link_libraries(loadPlugin PluginManager dl)
