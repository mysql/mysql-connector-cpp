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

#
# Settings using by this project:
#
# WITH_CONCPP (required)  location of Connector/C++ 8.0 installation
# BUILD_STATIC            whether to link with connector statically
#                         or dynamically (the default)
# WITH_BOOST  (optional)  where to look for Boost (required only when building
#                         JDBC API application)
#
# WITH_JDBC               build JDBC API test application
#

option(WITH_JDBC "Also build the JDBC API test application" OFF)

set(WITH_CONCPP $ENV{WITH_CONCPP} CACHE PATH "MySQL Connector/C++ 8.0 install location")

if(NOT WITH_CONCPP)
  message(FATAL_ERROR
    "This project requires MySQL Connector/C++ 8.0, please specify install location"
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

#
# If connector was built with OpenSSL then WITH_SSL should be set accordingly
# and we need to take care of the dependency on the OpenSSL libraries
# (see below).
#

set(WITH_SSL $ENV{WITH_SSL} CACHE STRING
 "\"no\" = connector built without SSL support,"
 "\"bundled\" = connector with built-in SSL support,"
 " any other value or not set implies that connector is using shared OpenSSL libraries."
)

if(NOT DEFINED WITH_SSL)
 set(WITH_SSL "system")
endif()


if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(IS64BIT 1)
  message("Generationg 64bit code")
else()
  message("Generating 32bit code")
endif()

if(WIN32)
  set(VS "vs14")
endif()


#
# Find MySQL Connector/C++ at specified location.
#

set(CONCPP_INCLUDES "${WITH_CONCPP}/include")

if(NOT EXISTS "${CONCPP_INCLUDES}/mysqlx/xdevapi.h")
  message(FATAL_ERROR
    "Could not find MySQL Connector/C++ 8.0 headers at specified"
    " location: ${CONCPP_INCLUDES}"
  )
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

#
#  There are 2 libraries for Connector/C++ 8.0. The new library with base
#  name mysqlcppconn8 and the legacy library mysqlcppconn which is backward
#  compatible with Connector/C++ 1.1.
#
#  Static versions of connecotr library have -static suffix. Additionally,
#  on Windows they have -mt suffix if the library was built using static
#  runtime libs (/MT compiler setting).
#

set(libconcpp_name mysqlcppconn8)
set(libconcpp_jdbc_name mysqlcppconn)

foreach(name libconcpp_name libconcpp_jdbc_name)

if(BUILD_STATIC)
  set(${name} "${${name}}-static")
  if(WIN32 AND STATIC_MSVCRT)
    set(${name} "${${name}}-mt")
  endif()
endif()

endforeach()


message("Looking for connector libraries here: ${LIB_PATH_STATIC}")

message("Looking for the main library  ${libconcpp_name}")

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
    "Could not find MySQL Connector/C++ 8.0 library ${libconcpp_name} at specified"
    " location: ${LIB_PATH_STATIC}"
  )
endif()

# Set CONCPP_LIBS

if(CONCPP_LIB)
  list(APPEND CONCPP_LIBS optimized "${CONCPP_LIB}")
  if(NOT CONCPP_LIB_DEBUG AND NOT WIN32)
    message(WARNING "Using generic library also for debug builds")
    list(APPEND CONCPP_LIBS general "${CONCPP_LIB}")
  else()
    message(WARNING "Could not find debug libraries, building in debug mode will not work")
  endif()
endif()

if(CONCPP_LIB_DEBUG)
  list(APPEND CONCPP_LIBS debug "${CONCPP_LIB_DEBUG}")
  if(NOT CONCPP_LIB AND NOT WIN32)
    message(WARNING "Using debug library also for non-debug builds")
    list(APPEND CONCPP_LIBS general "${CONCPP_LIB_DEBUG}")
  else()
    message(WARNING "Only debug libraries found, building in release mode will not work")
  endif()
endif()

message("Using Connector/C++ 8.0 libraries: ${CONCPP_LIBS}")


message("Looking for the legacy library  ${libconcpp_jdbc_name}")

find_library(CONCPP_JDBC NAMES ${libconcpp_jdbc_name}
  PATHS "${LIB_PATH_STATIC}"
  NO_DEFAULT_PATH
)

find_library(CONCPP_JDBC_DEBUG NAMES ${libconcpp_jdbc_name}
  PATHS "${LIB_PATH_STATIC}/debug"
  NO_DEFAULT_PATH
)

if(CONCPP_JDBC OR CONCPP_JDBC_DEBUG)
  set(HAS_JDBC ON)
elseif(WITH_JDBC)
  message(FATAL_ERROR
    "Could not find MySQL Connector/C++ 8.0 legacy library"
    " ${libconcpp_jdbc_name} at spcified location ${LIB_PATH_STATIC}"
  )
endif()


if(CONCPP_JDBC)
  list(APPEND CONCPP_JDBC_LIBS optimized "${CONCPP_JDBC}")
  if(NOT CONCPP_JDBC_DEBUG AND NOT WIN32)
    message(WARNING "Using generic library also for debug builds")
    list(APPEND CONCPP_JDBC_LIBS general "${CONCPP_JDBC}")
  else()
    message(WARNING "Could not find debug libraries, building in debug mode will not work")
  endif()
endif()

if(CONCPP_JDBC_DEBUG)
  list(APPEND CONCPP_JDBC_LIBS debug "${CONCPP_JDBC_DEBUG}")
  if(NOT CONCPP_JDBC AND NOT WIN32)
    message(WARNING "Using debug library also for non-debug builds")
    list(APPEND CONCPP_JDBC_LIBS general "${CONCPP_JDBC_DEBUG}")
  else()
    message(WARNING "Only debug libraries found, building in release mode will not work")
  endif()
endif()

message("Using Connector/C++ 8.0 legacy libraries: ${CONCPP_JDBC_LIBS}")


#
# Connector/C++ requires pthread library on Unix. Legacy connector needs
# even more.
#

IF(CMAKE_HOST_UNIX)
  list(APPEND CONCPP_LIBS pthread)
  if(NOT APPLE)
    list(APPEND CONCPP_JDBC_LIBS pthread rt dl)
  endif()
ENDIF()

#
# On Solaris we additionally need socket and nsl libraries.
#

if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
  list(APPEND CONCPP_LIBS socket nsl)
  # TODO: Could be couple more libs reqired..
  # MYSQL_LIBRARIES: stdc++;gcc_s;CrunG3;c;..;socket;nsl;m;rt
  list(APPEND CONCPP_JDBC_LIBS socket nsl rt)
endif()

#
# If building statically and connector uses OpenSSL libraries, we need to
# link to them too (however, no need to do it on Windows).
#

if(NOT WIN32 AND BUILD_STATIC AND WITH_SSL AND NOT (WITH_SSL STREQUAL "bundled"))
  list(APPEND CONCPP_LIBS ssl crypto)
  list(APPEND CONCPP_JDBC_LIBS ssl crypto)
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
# Look for Boost which is required when building JDBC API application.
#

if(HAS_JDBC)

  if(DEFINED WITH_BOOST)
    set(BOOST_ROOT "${WITH_BOOST}")
    set(Boost_NO_SYSTEM_PATHS ON)
  endif()

  find_package(Boost)

  if (Boost_FOUND)

    set(BOOST_INCLUDES "${Boost_INCLUDE_DIRS}")
    set(WITH_JDBC ON)

  else()

    if(DEFINED WITH_BOOST)
      message("Boost could not be found at specified location: ${WITH_BOOST}")
    endif()

    if(WITH_JDBC)
      message(FATAL_ERROR
        "Can not build JDBC API application because Boost is missing"
        " (use WITH_BOOST)."
      )
    else()
      message(WARNING
        "JDBC API application will not be built because required Boost headers"
        " could not be found (use WITH_BOOST)."
      )
    endif()

  endif()

endif()

#
# Arrange for executables to be built in either "run" or "run/debug" location,
# depending on the build type. This is to simplify dependencies management
# (see below).
#

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/run)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/run/debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY run)

foreach(config ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER "${config}" CONFIG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} run)
endforeach()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG run/debug)


#
# Test programs that use MySQL Connector/C++.
#

include_directories(${CONCPP_INCLUDES})

#
# Note: Adding this so that bundled libraries such as openSSL can be found
# at link time in case they are not installed on the system.
#

link_directories(${LIB_PATH_STATIC})


add_executable(xapi_test xapi_test.c)
target_link_libraries(xapi_test ${CONCPP_LIBS})

#
# Note: Connector/C++ library depends on C++ runtime library.
# For that reason, even code that is otherwise plain C, should
# be linked using C++ linker, so that dependency on the C++ runtime
# is correctly resolved. In cmake this is achieved by setting
# LINKER_LANGUAGE to CXX. Alternatively one could use plain C linker
# and specify stdc++ (and m) as additional libraries to be linked in.
#

set_target_properties(xapi_test PROPERTIES LINKER_LANGUAGE CXX)


# To use DevAPI or JDBC API we must enable C++11

if(NOT MSVC)
  add_compile_options("-std=c++11")
endif()


add_executable(devapi_test devapi_test.cc)
target_link_libraries(devapi_test ${CONCPP_LIBS})


if(WITH_JDBC)

  if(BUILD_STATIC)
    add_definitions(-DCPPCONN_PUBLIC_FUNC=)
  endif()

  include_directories(${BOOST_INCLUDES})
  add_executable(jdbc_test jdbc_test.cc)
  target_link_libraries(jdbc_test ${CONCPP_JDBC_LIBS})

endif()


#
# Copy shared libraries to the location where the executable is built.
# This is necessary for Windows to find the required DLLs when running
# the executable. (Note: On Unix it can be arranged by correctly setting
# rpath, but this does not work on Windows)
#

message("Installing shared library dependencies.")

file(GLOB libs "${LIB_PATH}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
file(INSTALL ${libs} DESTINATION run)

#
# Note: Using non-debug libraries with code which was build in debug mode
# (or vice versa) does not work on Windows, but on other platforms we try
# it.
#

if (NOT WIN32)
  file(INSTALL ${libs} DESTINATION run/debug)
endif()

#
# Overwrite libs in run/debug with debug versions if present.
#

file(GLOB debug_libs "${LIB_PATH}/debug/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
file(INSTALL ${debug_libs} DESTINATION run/debug)

#
# If non-debug libraries were not found and we are not on Windows, try
# using debug ones, if present.
#

if (NOT WIN32 AND NOT libs)
  flie(INSTALL ${debug_libs} DESTINATAION run)
endif()

