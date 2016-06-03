# Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
#
# The MySQL Connector/C++ is licensed under the terms of the GPLv2
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
# MySQL Connectors. There are special exceptions to the terms and
# conditions of the GPLv2 as it is applied to this software, see the
# FLOSS License Exception
# <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA


#
#  Setup boost for targets in given folder and its sub-folders.
#

# Option to build project linking boost libraries
OPTION(BOOST_SYSTEM_LIBS "Link with system boost libraryes" OFF)
SET(WITH_BOOST $ENV{WITH_BOOST} CACHE PATH "Location of Boost library")


# The minimal required version of boost: major version must match, minor version
# can be later than required.
#
# Note: we use boost/predef which requires at least 1.55
#
SET(BOOST_REQUIRED_VERSION_MAJOR 1)
SET(BOOST_REQUIRED_VERSION_MINOR 55)


MACRO(SETUP_BOOST)

  # Seems to be a default now and defining it triggers a lot of macro
  # redefinition warnings.
  # ADD_DEFINITIONS(-DBOOST_NO_CXX11_STATIC_ASSERT)

  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

  IF(BOOST_SYSTEM_LIBS)
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
  ENDIF(BOOST_SYSTEM_LIBS)

  IF(WIN32)
    #
    # Otherwise boost headers generate warnings
    # TODO: Define it only for targets that use boost
    ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
    ADD_DEFINITIONS(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
  ENDIF()
ENDMACRO(SETUP_BOOST)


#
# Add minimal required boost libraries to given target. This assumes that
# Boost was set-up in given folder.
#
MACRO(ADD_BOOST target)

  IF(NOT WIN32)
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
    COMPONENTS system)

ELSE()

  FIND_PACKAGE(Boost
    ${BOOST_REQUIRED_VERSION_MAJOR}.${BOOST_REQUIRED_VERSION_MINOR}.0
    )

  ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB )

ENDIF(BOOST_SYSTEM_LIBS)

if(NOT Boost_FOUND)
  message(FATAL_ERROR "Could not find required Boost version "
          "${BOOST_REQUIRED_VERSION_MAJOR}.${BOOST_REQUIRED_VERSION_MINOR}. "
          "You can point to a location where Boost is installed using "
          "WITH_BOOST option.")
endif()

set(BOOST_VERSION "${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}")

if (MSVC
    AND (MSVC_VERSION GREATER 1700)
    AND (BOOST_VERSION VERSION_LESS "1.59"))
  message(WARNING "MSVC 2015 or higher requires Boost version at least 1.59 but "
                  "version ${BOOST_VERSION} is used")
endif()
