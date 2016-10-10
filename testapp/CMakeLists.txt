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

option(BUILD_STATIC "Link statically with the connector library" OFF)

if(BUILD_STATIC)
  message("Linking statically")
  add_definitions(-DSTATIC_CONCPP)
endif()

option(STATIC_MSVCRT "Use static MSVC runtime library" OFF)

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

if(WIN32)
  if(MSVC12)
   set(VS "vs12")
  elseif(MSVC14)
   set(VS "vs14")
  endif()
endif()


#
# Find MySQL Connector/C++ 2.0 at specified location.
#

set(CONCPLS_INCLUDES "${WITH_CONCPP}/include")

if(NOT EXISTS "${CONCPLS_INCLUDES}/mysql_devapi.h")
  message(FATAL_ERROR
    "Could not find MySQL Connector/C++ 2.0 headers at specified"
    " location: ${WITH_CONCPP}/include"
  )
endif()


set(libconcpp_name mysqlcppconn2)
if(BUILD_STATIC)
  set(libconcpp_name "${libconcpp_name}-static")
  if(WIN32 AND STATIC_MSVCRT)
    set(libconcpp_name "${libconcpp_name}-mt")
  endif()
endif()


if(IS64BIT)
  set(LIB_PATH "${WITH_CONCPP}/lib64")
else()
  set(LIB_PATH "${WITH_CONCPP}/lib")
endif()

set(LIB_PATH_STATIC "${LIB_PATH}")
if(VS)
   set(LIB_PATH_STATIC "${LIB_PATH_STATIC}/${VS}")
endif()


message("Looking for the connector library ${libconcpp_name} here: ${LIB_PATH_STATIC}")

find_library(CONCPP_LIB NAMES ${libconcpp_name}
  PATHS "${LIB_PATH_STATIC}"
  NO_DEFAULT_PATH
)

find_library(CONCPP_LIB_DEBUG NAMES ${libconcpp_name}
  PATHS "${LIB_PATH_STATIC}/debug"
  NO_DEFAULT_PATH
)

if(NOT CONCPP_LIB AND NOT CONCPP_LIB_DEBUG)
  message(FATAL_ERROR
    "Could not find MySQL Connector/C++ 2.0 library ${libconcpp_name} at specified"
    " location: ${LIB_PATH_STATIC}"
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
# Copy shared library to a pre-defined location where also the executable
# will be built. This is necessary for Windows to find the DLL when running
# the executable.
#

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/run)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/run/debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY run)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE run)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG run/debug)

if(NOT BUILD_STATIC)

  set(libconcpp_name_shared mysqlcppconn2)
  if(VS)
    set(libconcpp_name_shared "${libconcpp_name_shared}-${VS}")
  endif()

  set(CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_SHARED_LIBRARY_SUFFIX})

  find_library(CONCPP_LIB_SHARED NAMES ${libconcpp_name_shared}
    PATHS "${LIB_PATH}"
    NO_DEFAULT_PATH
  )

  find_library(CONCPP_LIB_SHARED_DEBUG NAMES ${libconcpp_name_shared}
    PATHS "${LIB_PATH}/debug"
    NO_DEFAULT_PATH
  )

  if(NOT CONCPP_LIB_SHARED AND NOT CONCPP_LIB_SHARED_DEBUG)
    message(FATAL_ERROR
      "Could not find MySQL Connector/C++ 2.0 shared library ${libconcpp_name_shared} at specified"
      " location: ${LIB_PATH}"
    )
  endif()


  if(CONCPP_LIB_SHARED)
    message("Copying shared library: ${CONCPP_LIB_SHARED}")
    file(COPY "${CONCPP_LIB_SHARED}" DESTINATION ${PROJECT_BINARY_DIR}/run)
  endif()

  if(CONCPP_LIB_SHARED_DEBUG)
    message("Copying debug shared library: ${CONCPP_LIB_SHARED_DEBUG}")
    file(COPY "${CONCPP_LIB_SHARED_DEBUG}" DESTINATION ${PROJECT_BINARY_DIR}/run/debug)
  endif()

endif(NOT BUILD_STATIC)

#
# Connector/C++ requires pthread library on Unix.
#

IF(CMAKE_HOST_UNIX)
  list(APPEND CONCPP_LIBS pthread)
ENDIF()

#
# On Solaris we additionally need socket and nsl libraries.
#

if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
  list(APPEND CONCPP_LIBS socket nsl)
endif()

#
# Use clang's native C++ runtime library to match the one used
# by Connector/C++.
#

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

#
# On Windows, in case of static linking, if MySQL Connector/C++ was built
# with the static runtime then we must do the same here. Option STATIC_MSVCRT
# selects the static runtime.
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

ADD_EXECUTABLE(devapi_test devapi_test.cc)

TARGET_LINK_LIBRARIES(devapi_test ${CONCPP_LIBS})

# To use DevAPI we must enable C++11

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(devapi_test PRIVATE "-std=c++11")
endif()

ADD_EXECUTABLE(xapi_test xapi_test.c)
TARGET_LINK_LIBRARIES(xapi_test ${CONCPP_LIBS})

#
# Note: Connector/C++ library depends on C++ runtime library.
# For that reason, even code that is otherwise plain C, should
# be linked using C++ linker, so that dependency on the C++ runtime
# is correctly resolved. In cmake this is achieved by setting
# LINKER_LANGUAGE to CXX. Alternatively one could use plain C linker
# and specify stdc++ (and m) as additional libraries to be linked in.
#

set_target_properties(xapi_test PROPERTIES LINKER_LANGUAGE CXX)
