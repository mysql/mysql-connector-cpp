# Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


#
# Set up gtest for use by targets in given folder and its sub-folders.
#
MACRO(SETUP_GTEST)
  IF (WITH_GTEST)
    INCLUDE_DIRECTORIES(${GTEST_INCLUDE_DIRS})
  ENDIF (WITH_GTEST)
ENDMACRO(SETUP_GTEST)


SET(WITH_GTEST $ENV{WITH_GTEST} CACHE PATH "Location of gtest")

#
# TODO: Try to find gtest in system-wide locations if WITH_GTEST is
# not set
#

if(DEFINED WITH_GTEST)
  set(GTEST_ROOT ${WITH_GTEST})
endif()

find_package(GTest)

if(NOT GTEST_FOUND)

IF(NOT DEFINED WITH_GTEST)
  MESSAGE(FATAL_ERROR "This project requires gtest libraries"
          " but WITH_GTEST option was not specified."
          " Please set it to the location where gtest"
          " was built from sources using cmake.")
ENDIF()

message("Looking for gtest build in: ${WITH_GTEST}")

#
# TODO: Configure gtest build if sources location is given
#

IF(NOT EXISTS "${WITH_GTEST}/CMakeCache.txt")
MESSAGE(FATAL_ERROR
  "Could not find gtest build in this location: ${WITH_GTEST}"
)
ENDIF()

#
# Read source location from build configuration cache and set
# GTEST_INCLUDE_DIR.
#

LOAD_CACHE(${WITH_GTEST} READ_WITH_PREFIX GTEST_
	  CMAKE_PROJECT_NAME)
#MESSAGE(STATUS "Gtest project name: ${GTEST_CMAKE_PROJECT_NAME}")

LOAD_CACHE(${WITH_GTEST} READ_WITH_PREFIX GTEST_
	  ${GTEST_CMAKE_PROJECT_NAME}_SOURCE_DIR)

FIND_PATH(GTEST_INCLUDE_DIRS
        NAMES gtest/gtest.h
        PATHS ${GTEST_${GTEST_CMAKE_PROJECT_NAME}_SOURCE_DIR}/include ${GTEST_${GTEST_CMAKE_PROJECT_NAME}_SOURCE_DIR}/gtest/include
        NO_DEFAULT_PATH
          )

IF(NOT EXISTS "${GTEST_INCLUDE_DIRS}/gtest/gtest.h")
  MESSAGE(FATAL_ERROR "Could not find gtest headers at: ${GTEST_INCLUDE_DIRS}")
ENDIF()

#
# TODO: Run build if libraries can not be found in expected locations
#

FIND_LIBRARY(gtest_location
  NAMES libgtest gtest
  PATHS ${WITH_GTEST} ${WITH_GTEST}/gtest
  PATH_SUFFIXES . Release RelWithDebInfo Debug
  NO_DEFAULT_PATH
)

FIND_LIBRARY(gtest_main_location
  NAMES libgtest_main gtest_main
  PATHS ${WITH_GTEST} ${WITH_GTEST}/gtest
  PATH_SUFFIXES . Release RelWithDebInfo Debug
  NO_DEFAULT_PATH
)

else()

list(GET GTEST_LIBRARIES 0 gtest_location)
list(GET GTEST_MAIN_LIBRARIES 0  gtest_main_location)

endif(NOT GTEST_FOUND)

MESSAGE(STATUS "GTEST_INCLUDE_DIRS: ${GTEST_INCLUDE_DIRS}")

#
# Import gtest and gtest_main libraries as targets of this project
#

MESSAGE("gtest location: ${gtest_location}")
MESSAGE("gtest_main location: ${gtest_main_location}")


add_library(gtest STATIC IMPORTED)
add_library(gtest_main STATIC IMPORTED)

set_target_properties(gtest PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
  IMPORTED_LOCATION "${gtest_location}"
)

set_target_properties(gtest_main PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
  IMPORTED_LINK_INTERFACE_LIBRARIES "gtest"
  IMPORTED_LOCATION "${gtest_main_location}"
)

#
#  Setup configuration-specific locations for Win
#  TODO: Should the same be done for OSX?
#

IF(WIN32)

  get_filename_component(gtest_path ${gtest_location} PATH)

  file(GLOB tmp "${gtest_path}/*/gtest.lib")
  if(NOT tmp)
    file(GLOB tmp "${gtest_path}/../*/gtest.lib")
  endif()

  list(GET tmp 0 tmp)
  get_filename_component(gtest_path ${tmp} REALPATH)
  get_filename_component(gtest_path ${gtest_path} PATH)
  get_filename_component(gtest_path ${gtest_path} PATH)

  #message("Setting-up per build configuration locations using base:"
  #        " ${gtest_path}")

  FOREACH(Config Debug RelWithDebInfo MinSizeRel Release)

    if(EXISTS "${gtest_path}/${Config}/gtest.lib")

    message("found per configuration variant: ${gtest_path}/${Config}/gtest.lib")

    STRING(TOUPPER ${Config} CONFIG)

    set_property(TARGET gtest APPEND PROPERTY IMPORTED_CONFIGURATIONS ${CONFIG})
    set_target_properties(gtest PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_${CONFIG} "CXX"
      IMPORTED_LOCATION_${CONFIG} "${gtest_path}/${Config}/gtest.lib"
      )

    set_property(TARGET gtest_main APPEND PROPERTY IMPORTED_CONFIGURATIONS ${CONFIG})
    set_target_properties(gtest_main PROPERTIES
      IMPORTED_LINK_INTERFACE_LANGUAGES_${CONFIG} "CXX"
      IMPORTED_LINK_INTERFACE_LIBRARIES_${CONFIG} "gtest"
      IMPORTED_LOCATION_${CONFIG} "${gtest_path}/${Config}/gtest_main.lib"
      )

    endif()

  ENDFOREACH(Config)

ELSE(WIN32)

  # On unix gtest depends on pthread library
  set_property(TARGET gtest APPEND PROPERTY INTERFACE_LINK_LIBRARIES pthread)
  set_property(TARGET gtest APPEND PROPERTY IMPORTED_LINK_INTERFACE_LIBRARIES pthread)

ENDIF(WIN32)

