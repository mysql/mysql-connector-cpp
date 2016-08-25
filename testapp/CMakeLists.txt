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


cmake_minimum_required(VERSION 2.8)
PROJECT(MySQL_CONCPP_TEST)

set(WITH_CONCPP $ENV{WITH_CONCPP} CACHE PATH "MySQL Connector/C++ 2.0 install location")

if(NOT WITH_CONCPP)
  message(FATAL_ERROR
    "This project requires MySQL Connector/C++ 2.0, please specify install location"
    " using WITH_CONCPP setting"
  )
endif()

OPTION(STATIC_MSVCRT "Use static MSVC runtime library" OFF)

if(STATIC_MSVCRT)
  message("Using static runtime library.")
else()
  message("Using dynamic runtime library.")
endif()


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(IS64BIT 1)
  message("Generationg 64bit code")
else()
  message("Generating 32bit code")
endif()


#
# Find MySQL Connector/C++ 2.0 at specified location.
#

set(libconcpp_name mysqlcppconn2-static)
if(STATIC_MSVCRT)
  set(libconcpp_name "${libconcpp_name}-mt")
endif()

set(CONCPLS_INCLUDES "${WITH_CONCPP}/include")

if(NOT EXISTS "${CONCPLS_INCLUDES}/mysql_devapi.h")
  message(FATAL_ERROR
    "Could not find MySQL Connector/C++ 2.0 headers at specified"
    " location: ${WITH_CONCPP}/include"
  )
endif()

if(IS64BIT)
  set(LIB_PATH "${WITH_CONCPP}/lib64")
else()
  set(LIB_PATH "${WITH_CONCPP}/lib")
endif()

if(WIN32)
  if(MSVC12)
   set(LIB_PATH "${LIB_PATH}/vs12")
  elseif(MSVC14)
   set(LIB_PATH "${LIB_PATH}/vs14")
  endif()
endif()

message("Looking for the connector library ${libconcpp_name} here: ${LIB_PATH}")

find_library(CONCPP_LIB NAMES ${libconcpp_name}
  PATHS "${LIB_PATH}"
  NO_DEFAULT_PATH
)

find_library(CONCPP_LIB_DEBUG NAMES ${libconcpp_name}
  PATHS "${LIB_PATH}/debug"
  NO_DEFAULT_PATH
)

if(NOT CONCPP_LIB AND NOT CONCPP_LIB_DEBUG)
  message(FATAL_ERROR
    "Could not find MySQL Connector/C++ 2.0 library ${libconcpp_name} at specified"
    " location: ${LIB_PATH}"
  )
endif()


if(CONCPP_LIB)
  message("Using Connector/C++ 2.0 library: ${CONCPP_LIB}")
  if(CONCPP_LIB_DEBUG)
    list(APPEND CONCPP_LIBS optimized "${CONCPP_LIB}")
  else()
    message(WARNING "Using generic library also for debug builds")
    list(APPEND CONCPP_LIBS general "${CONCPP_LIB}")
  endif()
endif()

if(CONCPP_LIB_DEBUG)
  message("Debug library: ${CONCPP_LIB_DEBUG}")
  if(CONCPP_LIB)
    list(APPEND CONCPP_LIBS debug "${CONCPP_LIB_DEBUG}")
  else()
    message(WARNING "Using debug library also for non-debug builds")
    list(APPEND CONCPP_LIBS general "${CONCPP_LIB_DEBUG}")
  endif()
endif()


#
# Use clang's native C++ runtime library to match the one used
# by Connector/C++.
#

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

#
# On Windows, if MySQL Connector/C++ was built with the static runtime
# then we must do the same here. Option STATIC_MSVCRT selects the static
# runtime.
#

IF(WIN32)

  set(runtime "dynamic")

  IF(STATIC_MSVCRT)

  set(runtime "static")

  foreach(LANG C CXX)
    set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} /MT")
    foreach(TYPE RELEASE RELWITHDEBINFO MINSIZEREL)
      set(CMAKE_${LANG}_FLAGS_${TYPE} "${CMAKE_${LANG}_FLAGS_${TYPE}} /MT")
    endforeach()
    set(CMAKE_${LANG}_FLAGS_DEBUG "${CMAKE_${LANG}_FLAGS_DEBUG} /MTd")
  endforeach(LANG)

  ENDIF()

ENDIF()


#
# Test program that uses MySQL Connector/C++ 2.0.
#

INCLUDE_DIRECTORIES(${CONCPLS_INCLUDES})

ADD_EXECUTABLE(cpptest test.cc)
TARGET_LINK_LIBRARIES(cpptest ${CONCPP_LIBS})

#
# Connector/C++ requires pthread library on Unix.
#

IF(CMAKE_HOST_UNIX)
  TARGET_LINK_LIBRARIES(cpptest pthread)
ENDIF()

# To use Connector/C++ 2.0 we must enable C++11

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(cpptest PRIVATE "-std=c++11")
endif()
