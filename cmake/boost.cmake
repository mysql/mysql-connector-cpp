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
#  Setup boost for targets in given folder and its sub-folders.
#

# Option to build project linking boost libraries
OPTION(BOOST_SYSTEM_LIBS "Link with system boost libraryes" OFF)
SET(WITH_BOOST $ENV{WITH_BOOST} CACHE PATH "Location of Boost library")


# The minimal required version of boost: major version must match, minor version
# can be later than required.
#
SET(BOOST_REQUIRED_VERSION_MAJOR 1)
SET(BOOST_REQUIRED_VERSION_MINOR 41)


MACRO(SETUP_BOOST)
  IF(NOT APPLE)
    ADD_DEFINITIONS(-DBOOST_NO_CXX11_STATIC_ASSERT)
  ENDIF(NOT APPLE)

  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

  IF(BOOST_SYSTEM_LIBS)
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
  ENDIF(BOOST_SYSTEM_LIBS)

  IF(WIN32)
    #
    # Otherwise boost headers generate warnings
    #
    ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
  ENDIF()
ENDMACRO(SETUP_BOOST)


#
# Add minimal required boost libraries to given target. This assumes that
# Boost was set-up in given folder.
#
MACRO(ADD_BOOST target)

  IF(NOT WIN32 AND BOOST_SYSTEM_LIBS)
    TARGET_LINK_LIBRARIES(${target} ${Boost_LIBRARIES} pthread)
  ENDIF()

ENDMACRO(ADD_BOOST)

IF (NOT BOOST_ROOT AND WITH_BOOST)
  SET(BOOST_ROOT ${WITH_BOOST})
ENDIF()

# Finding Boost Libraries
IF(BOOST_SYSTEM_LIBS)

  FIND_PACKAGE(Boost
    ${BOOST_REQUIRED_VERSION_MAJOR}.${BOOST_REQUIRED_VERSION_MINOR}.0
    REQUIRED COMPONENTS system)

ELSE()

  FIND_PACKAGE(Boost
    ${BOOST_REQUIRED_VERSION_MAJOR}.${BOOST_REQUIRED_VERSION_MINOR}.0
    REQUIRED)

  ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB )

ENDIF(BOOST_SYSTEM_LIBS)
