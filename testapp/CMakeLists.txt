# Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0, as
# published by the Free Software Foundation.
#
# This program is also distributed with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an
# additional permission to link the program and your derivative works
# with the separately licensed software that they have included with
# MySQL.
#
# Without limiting anything contained in the foregoing, this file,
# which is part of MySQL Connector/C++, is also subject to the
# Universal FOSS Exception, version 1.0, a copy of which can be found at
# http://oss.oracle.com/licenses/universal-foss-exception.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA


cmake_minimum_required(VERSION 2.8)
PROJECT(MySQL_CONCPP_TEST)

#
# Settings using by this project:
#
# WITH_CONCPP             location of Connector/C++ 8.0 installation
# CONCPP_INCLUDE_DIR      location of Connector/C++ 8.0 headers
# CONCPP_LIB_DIR          location of Connector/C++ 8.0 libraries
#
# BUILD_STATIC            whether to link with connector statically
#                         or dynamically (the default)
#
# WITH_SSL                tells if connector uses built-in SSL implementation
#                         or relies on external OpenSSL libraries (*)
#
# WITH_JDBC               also build JDBC API test application
#
# WITH_BOOST              where to look for Boost (required only when building
#                         JDBC API application)
#
# *) If WITH_SSL is set to "builtin" or if it points at WoflSSL sources then
#    it is assumed that connector has SSL support built-in. In all other cases
#    it is assumed that connector uses OpenSSL libraries.
#
# Note: Either WITH_CONCPP or CONCPP_INCLUDE_DIR and CONCPP_LIB_DIR must be
# set.
#


if((CMAKE_SIZEOF_VOID_P EQUAL 8) OR (CMAKE_CXX_COMPILER_ID MATCHES "SunPro"))
  set(IS64BIT 1)
  message("Generating 64bit code")
else()
  message("Generating 32bit code")
endif()

#
# ========================================================================
# Build configuration options
#

set(WITH_CONCPP $ENV{WITH_CONCPP} CACHE PATH
  "MySQL Connector/C++ 8.0 install location"
)

set(CONCPP_INCLUDE_DIR $ENV{CONCPP_INCLUDE_DIR} CACHE PATH
  "Location of Connector/C++ 8.0 headers"
)

set(CONCPP_LIB_DIR $ENV{CONCPP_LIB_DIR} CACHE PATH
  "Location of Connector/C++ 8.0 libraries"
)

# Set include and library paths if not given

if(WITH_CONCPP)
  if(NOT CONCPP_INCLUDE_DIR)
     set(CONCPP_INCLUDE_DIR "${WITH_CONCPP}/include")
  endif()
  if(NOT CONCPP_LIB_DIR)
    if(IS64BIT)
      set(CONCPP_LIB_DIR "${WITH_CONCPP}/lib64")
    else()
      set(CONCPP_LIB_DIR "${WITH_CONCPP}/lib")
    endif()
  endif()
endif()

# Location for static libraries (differs from base location on Windows)

set(CONCPP_STATIC_LIB_DIR "${CONCPP_LIB_DIR}")
if(WIN32)
  set(VS "vs14")
  set(CONCPP_STATIC_LIB_DIR "${CONCPP_LIB_DIR}/vs14")
endif()


if(NOT CONCPP_INCLUDE_DIR OR NOT CONCPP_LIB_DIR)
  message(FATAL_ERROR
    "This project requires MySQL Connector/C++ 8.0, please specify install location"
    " using WITH_CONCPP setting or set header/library paths with CONCPP_INCLUDE_DIR"
    " and CONCPP_LIB_DIR settings."
  )
endif()

if(NOT EXISTS "${CONCPP_INCLUDE_DIR}/mysqlx/xdevapi.h")
  message(FATAL_ERROR
    "Could not find MySQL Connector/C++ 8.0 headers at specified"
    " location: ${CONCPP_INCLUDE_DIR}"
  )
endif()


set(WITH_SSL $ENV{WITH_SSL} CACHE STRING
  "Set to 'builtin' if connector was built with built-in SSL support"
)

if(NOT WITH_SSL)
  # Set to the default if WITH_SSL is not defined
  set(WITH_SSL "system")
elseif(EXISTS "${WITH_SSL}/wolfssl/openssl/ssl.h")
  set(WITH_SSL "builtin")
endif()


option(WITH_JDBC "Also build the JDBC API test application" OFF)

option(BUILD_STATIC "Link statically with the connector library" OFF)

if(BUILD_STATIC)
  message("Linking statically")
endif()

option(STATIC_MSVCRT "Use static MSVC runtime library" OFF)

if(STATIC_MSVCRT)
  message("Using static runtime library.")
else()
  message("Using dynamic runtime library.")
endif()


#
# ========================================================================
# Compiler settings
#

include_directories("${CONCPP_INCLUDE_DIR}")

if(WITH_JDBC)

  #
  # Look for Boost which is required when building JDBC API application.
  #

  if(DEFINED WITH_BOOST)
    set(BOOST_ROOT "${WITH_BOOST}")
    set(Boost_NO_SYSTEM_PATHS ON)

    # Setting this variable to ON disables attempts to use the cmake
    # configuration script provided by Boost and use only the logic
    # implemented in FindBoost. The cmake configuration script from Boost
    # proved to be buggy on some platforms and breaks our builds.

    set(Boost_NO_BOOST_CMAKE ON)
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

  include_directories(${BOOST_INCLUDES})

endif()


#
# To use DevAPI or JDBC API we must enable C++11
#

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "SunPro")

  add_compile_options(
    -m64 -fast
  )

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -library=stdcpp,CrunG3")

  foreach(type EXE STATIC SHARED)

    set(CMAKE_${type}_LINKER_FLAGS
      "-m64 -std=c++11 -library=stdcpp,CrunG3 -fast"
    )

  endforeach()

endif()

#
# Use clang's native C++ runtime library to match the one used
# by Connector/C++.
#

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()


#
# Use static MSVC runtime on Windows if requested. If linking with static
# connector library, a -mt variant of the library is needed in this case.
#

IF(WIN32 AND STATIC_MSVCRT)

  foreach(LANG C CXX)
    set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} /MT")
    set(CMAKE_${LANG}_FLAGS_DEBUG "${CMAKE_${LANG}_FLAGS_DEBUG} /MTd")
  endforeach(LANG)

ENDIF()


#
# ========================================================================
# Set CONCPP_LIBS and CONCPP_JDBC_LIBS.
#

if(NOT BUILD_STATIC)

  #
  # Using dynamic connector library (that is easier).
  #

  if(WIN32)
    # On Windows we link with the import library which is in the static
    # library folder.
    link_directories("${CONCPP_STATIC_LIB_DIR}")
  else()
    link_directories("${CONCPP_LIB_DIR}")
  endif()

  # Debug libraries, if present, are in the debug/ sub-folder of the main
  # library folder. If debug libraries are not present, we use the same
  # library for optimized and debug builds (debug_prefix is empty).

  if(EXISTS "${CONCPP_LIB_DIR}/debug")
    set(debug_prefix "debug/")
  else()
    set(debug_prefix)
  endif()

  set(CONCPP_LIBS
    optimized mysqlcppconn8
    debug     ${debug_prefix}mysqlcppconn8
  )

  set(CONCPP_JDBC_LIBS
    optimized mysqlcppconn
    debug     ${debug_prefix}mysqlcppconn8
  )


else(NOT BUILD_STATIC)

  #
  # Linking with static connector library
  #

  link_directories("${CONCPP_STATIC_LIB_DIR}")

  if(EXISTS "${CONCPP_STATIC_LIB_DIR}/debug")
    set(debug_prefix "debug/")
  else()
    set(debug_prefix)
  endif()

  #
  #  Static versions of connector library have -static suffix. Additionally,
  #  on Windows they have -mt suffix if the library was built using static
  #  runtime libs (/MT compiler setting).
  #

  if(WIN32 AND STATIC_MSVCRT)
    set(mt_suffix "-mt")
  else()
    set(mt_suffix)
  endif()


  set(CONCPP_LIBS
    optimized mysqlcppconn8-static${mt_suffix}
    debug     ${debug_prefix}mysqlcppconn8-static${mt_suffix}
  )

  set(CONCPP_JDBC_LIBS
    optimized mysqlcppconn-static${mt_suffix}
    debug     ${debug_prefix}mysqlcppconn-static${mt_suffix}
  )

  #
  # These macros must be defined to correctly adjust declarations
  # in connector public headers for linking with static library.
  #

  add_definitions(-DSTATIC_CONCPP)

  if(WITH_JDBC)
      add_definitions(-DCPPCONN_PUBLIC_FUNC=)
  endif()

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
    # TODO: Could be couple more libs required..
    # MYSQL_LIBRARIES: stdc++;gcc_s;CrunG3;c;..;socket;nsl;m;rt
    list(APPEND CONCPP_JDBC_LIBS socket nsl rt)
  endif()

  #
  # If connector uses OpenSSL libraries, we need to
  # link to them too (however, no need to do it on Windows).
  #

  if(NOT WIN32 AND NOT WITH_SSL STREQUAL "builtin")
      list(APPEND CONCPP_LIBS ssl crypto)
      list(APPEND CONCPP_JDBC_LIBS ssl crypto)
  endif()


endif(NOT BUILD_STATIC)


#
# ========================================================================
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
# Targets to build test applications.
#

add_executable(devapi_test devapi_test.cc)
target_link_libraries(devapi_test ${CONCPP_LIBS})

add_executable(xapi_test xapi_test.c)
target_link_libraries(xapi_test ${CONCPP_LIBS})

if(WITH_JDBC)

  add_executable(jdbc_test jdbc_test.cc)
  target_link_libraries(jdbc_test ${CONCPP_JDBC_LIBS})

endif()


#
# Note: Connector/C++ library depends on C++ runtime library.
# For that reason, even code that is otherwise plain C, should
# be linked using C++ linker, so that dependency on the C++ runtime
# is correctly resolved. In cmake this is achieved by setting
# LINKER_LANGUAGE to CXX. Alternatively one could use plain C linker
# and specify stdc++ (and m) as additional libraries to be linked in.
#

set_target_properties(xapi_test PROPERTIES LINKER_LANGUAGE CXX)



#
# ========================================================================
#
# Copy shared libraries to the location where the executable is built.
# This is necessary for Windows to find the required DLLs when running
# the executable. (Note: On Unix it can be arranged by correctly setting
# rpath, but this does not work on Windows)
#

message("Installing shared library dependencies.")

file(GLOB libs "${CONCPP_LIB_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
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

file(GLOB debug_libs "${CONCPP_LIB_DIR}/debug/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
file(INSTALL ${debug_libs} DESTINATION run/debug)

#
# If non-debug libraries were not found and we are not on Windows, try
# using debug ones, if present.
#

if (NOT WIN32 AND NOT libs)
  file(INSTALL ${debug_libs} DESTINATION run)
endif()


