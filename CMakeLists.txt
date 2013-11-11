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

################################################################################
# Version Definition
################################################################################
set(VERSION_MAJOR "0" )
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")

################################################################################
# Main section
################################################################################
cmake_minimum_required (VERSION 2.6)
project (eosplugin)

if (RELEASE)
else (RELEASE)
set(RELEASE "1")
endif (RELEASE)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# force /usr/ installation prefix
set(CMAKE_INSTALL_PREFIX /usr)
set(CMAKE_INSTALL_SYSCONFDIR /etc)
set(CMAKE_INSTALL_SBINDIR /sbin)

if (PREFIX) 
  set (CMAKE_INSTALL_PREFIX ${PREFIX})
endif (PREFIX)

set(CMAKE_CXX_FLAGS "-g -Wall -O2 -msse4.2 -std=gnu++0x -DVERSION=\\\"${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}\\\" ")

if(MacOSX)
  set(LIB_INSTALL_DIR lib)
else(MacOSX)
  set(LIB_INSTALL_DIR lib64)
endif(MacOSX)

IF (DEFINED PACKAGEONLY)
  MESSAGE("Run CMake in package-only mode - no dependencies")
ELSE (DEFINED PACKAGEONLY) 
  MESSAGE("Run CMake in normal mode - check all dependencies")
ENDIF (DEFINED PACKAGEONLY)

add_subdirectory(src)
add_subdirectory(tests)

################################################################################
# Packaging 
################################################################################
add_custom_target( dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source )

set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_SOURCE_GENERATOR "RPM")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${RELEASE}")
set(CPACK_SOURCE_IGNORE_FILES "/dsi/;/build/;/.deps/;/.bzr/;~$;'.'o$;/CMakeFiles/;Makefile;/_CPack_Packages/;.tar.gz$;.tar.bz2;CMakeCache.txt;build;install_manifest.txt;lib;/.git/;${CPACK_SOURCE_IGNORE_FILES};")

include(CPack)
include(cmake/UseRPMTools.cmake)	
IF(RPMTools_FOUND)
  RPMTools_ADD_RPM_TARGETS(eosplugin ${CMAKE_SOURCE_DIR}/eosplugin.spec.in)
ENDIF(RPMTools_FOUND)

