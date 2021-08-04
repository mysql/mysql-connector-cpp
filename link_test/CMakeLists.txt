# Copyright (c) 2015, 2020, Oracle and/or its affiliates. All rights reserved.
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


cmake_minimum_required(VERSION 3.0)
cmake_policy(SET CMP0022 NEW)

#set_property(GLOBAL PROPERTY GLOBAL_DEPENDS_DEBUG_MODE ON)

##############################################################################

PROJECT(MySQL_CONCPP_JDBC_TEST)

if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
  message("Stand-alone configuration")
  message("Building on system: ${CMAKE_SYSTEM}")
  set(jdbc_test_stand_alone 1)
else()
  set(jdbc_test_stand_alone 0)
endif()

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
# WITH_BOOST              where to look for Boost if can not be found
#                         at standard locations.
#
# Note: Either WITH_CONCPP or CONCPP_INCLUDE_DIR and CONCPP_LIB_DIR must be
# set.
#

# ========================================================================

if(jdbc_test_stand_alone)

if((CMAKE_SIZEOF_VOID_P EQUAL 8))
  set(IS64BIT 1)
  message("Generating 64bit code")
else()
  message("Generating 32bit code")
endif()


#
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


option(BUILD_STATIC "Link statically with the connector library" OFF)

if(BUILD_STATIC)
  message("Linking statically")
else()
  message("Linking dynamically")
endif()

option(STATIC_MSVCRT "Use static MSVC runtime library" OFF)

if(STATIC_MSVCRT)
  message("Using static runtime library.")
else()
  message("Using dynamic runtime library.")
endif()


endif(jdbc_test_stand_alone)

# ========================================================================


if(NOT EXISTS "${CONCPP_INCLUDE_DIR}/jdbc/mysql_driver.h")
  message(FATAL_ERROR
    "Could not find MySQL Connector/C++ 8.0 JDBC headers at specified"
    " location: ${CONCPP_INCLUDE_DIR}"
  )
endif()



# ========================================================================
# Dependencies

#
# Find Connector/C++ legacy library
#
# Installation layout is as follows
#
# On Windows the install layout is as follows, where NN is the MSVC version
# used to build the connector, A is the major ABI version:
#
#  {lib,lib64}/mysqlcppconn-A-vsNN.dll            <-- shared library
#  {lib,lib64}/vsNN/mysqlcppconn-static.lib       <-- static with /MD
#  {lib,lib64}/vsNN/mysqlcppconn-static-mt.lib    <-- static with /MT
#  {lib,lib64}/vsNN/mysqlcppconn.lib              <-- import library for DLL
#
# On Linux it is as follows, where X.Y.Z is the connector version
#
#  {lib,lib64}/libmysqlcppconn.so.A.X.Y.Z         <-- shared library
#  {lib,lib64}/libmysqlcppconn.so.A               <-- soname link
#  {lib,lib64}/libmysqlcppconn.so                 <-- development link
#  {lib,lib64}/libmysqlcppconn-static.a          <-- static library
#
# Additionally, if connector is built in debug mode, the libraries are installed
# in debug/ subfolder of {lib,lib64}/ or {lib,lib64}/vsNN/.
#

set(find_name mysqlcppconn)
set(find_dir  "${CONCPP_LIB_DIR}")

#
# Note: On Windows we link with the import library located in the static
# library dir and named the same as the shared library.
#

if(BUILD_STATIC OR WIN32)
  set(find_dir  ${CONCPP_STATIC_LIB_DIR})
endif()

if(BUILD_STATIC)
  set(find_name mysqlcppconn-static)
  if(WIN32 AND STATIC_MSVCRT)
    set(find_name "${find_name}-mt")
  endif()
endif()

#message("-- looking for: ${find_name}")
#message("-- looking in: ${find_dir}")

# This will cause find_libary() to perform search each time.

set(CONCPP_LIB force-NOTFOUND CACHE PATH "" FORCE)
set(CONCPP_LIB_DEBUG force-NOTFOUND CACHE BOOL "" FORCE)

find_library(CONCPP_LIB
  NAMES ${find_name}
  PATHS "${find_dir}"
  NO_DEFAULT_PATH
)

find_library(CONCPP_LIB_DEBUG
  NAMES ${find_name}
  PATHS "${find_dir}/debug"
  NO_DEFAULT_PATH
)


if(NOT CONCPP_LIB AND NOT CONCPP_LIB_DEBUG)
  message(FATAL_ERROR
    "Could not find Connector/C++ JDBC libraries at: ${find_dir}"
  )
endif()

message("Using legacy connector lib at: ${CONCPP_LIB}")

#
# If debug or optimized version of the library is missing, replace it
# with the other one. This will not work on Windows (compiler will complain)
# but it might work on other platforms.
#

if(NOT CONCPP_LIB_DEBUG)
  message(WARNING
    "Using non-debug legacy library for debug builds - this will not work on Windows"
    )
  set(CONCPP_LIB_DEBUG "${CONCPP_LIB}")
elseif(NOT CONCPP_LIB)
  message(WARNING
    "Using debug legacy library for non-debug builds - this will not work on Windows"
  )
  set(CONCPP_LIB "${CONCPP_LIB_DEBUG}")
endif()


#
# Define imported target for the connector library.
#

add_library(MySQL::conncpp-jdbc STATIC IMPORTED)

#message("-- location: ${CONCPP_LIB}")
#message("-- debug location: ${CONCPP_LIB_DEBUG}")

set_target_properties(MySQL::conncpp-jdbc PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES   "${CONCPP_INCLUDE_DIR}"
  IMPORTED_LOCATION               "${CONCPP_LIB}"
  IMPORTED_LOCATION_DEBUG         "${CONCPP_LIB_DEBUG}"
)


#
# When linking statically, we need to link with libraries on which
# connector depends (however, no need to do it on Windows).
#

if(BUILD_STATIC AND NOT WIN32)

  set(libs)

  # Connector/C++ requires these libraries on Unix.

  if(NOT APPLE)
    list(APPEND libs pthread dl rt)
  endif()

  # On Solaris we additionally need couple more libs.

  if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
    list(APPEND libs socket nsl)
  endif()

  if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    list(APPEND libs execinfo unwind)
  else()
    list(APPEND libs resolv)
  endif()

  # OpenSSL dependency

  if(WITH_SSL MATCHES "^(system|yes)$")

    list(APPEND libs ssl crypto)

  else()

    if(NOT OPENSSL_FOUND)

      if(EXISTS ${WITH_SSL}/include/openssl/ssl.h)
        set(OPENSSL_ROOT_DIR  "${WITH_SSL}")
      endif()

      find_package(OpenSSL REQUIRED)

      MESSAGE(STATUS "OPENSSL_VERSION = ${OPENSSL_VERSION}")
      MESSAGE(STATUS "OPENSSL_SSL_LIBRARY = ${OPENSSL_SSL_LIBRARY}")
      MESSAGE(STATUS "OPENSSL_CRYPTO_LIBRARY = ${OPENSSL_CRYPTO_LIBRARY}")

    endif()

    list(APPEND libs ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})

  endif()

  set_property(TARGET MySQL::conncpp-jdbc
    APPEND PROPERTY INTERFACE_LINK_LIBRARIES
    ${libs}
  )

endif()


# ========================================================================
# Compiler settings
#

if(jdbc_test_stand_alone)

#
# To use JDBC API we must enable C++11
#

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

#
# Use clang's native C++ runtime library to match the one used
# by Connector/C++.
#

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()


#
# Use static MSVC runtime on Windows if requested. In this case the -mt
# variant of the static library is used.
#

IF(WIN32 AND STATIC_MSVCRT)

  set(CMAKE_CXX_FLAGS "${CMAKE_${LANG}_FLAGS} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_${LANG}_FLAGS_DEBUG} /MTd")

ENDIF()

#
# When linking with satic library, this macro must be defined.
#

if(BUILD_STATIC)
  add_definitions(-DSTATIC_CONCPP)
endif()

endif(jdbc_test_stand_alone)


# ========================================================================
#
# Arrange for executables to be built in either "run" or "run/debug" location,
# depending on the build type. This is to simplify dependencies management
# (see below).
#

if(jdbc_test_stand_alone)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/run)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/run/debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY run)
foreach(config ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER "${config}" CONFIG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} run)
endforeach()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG run/debug)

#
# Make sure that bundled external dependencies are used, if available.
#

if(NOT WIN32)
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-L${PROJECT_BINARY_DIR}/run/debug")
  set(CMAKE_EXE_LINKER_FLAGS "-L${PROJECT_BINARY_DIR}/run")
endif()


endif(jdbc_test_stand_alone)


# ------------------------------------------------------------------------
#
# Target to build test applications.
#

add_executable(jdbc_test jdbc_test.cc)
target_link_libraries(jdbc_test PRIVATE MySQL::conncpp-jdbc)


#
# Set RPATH so that libraries in run/ folder are found by the executable
# (this is default behavior on Windows).
#

set_property(TARGET jdbc_test PROPERTY BUILD_WITH_INSTALL_RPATH ON)

set(rpath)
if(APPLE)
  set(rpath "@loader_path")
elseif(NOT WIN32)
  set(rpath "$ORIGIN")
endif()

if(rpath)
  set_property(TARGET jdbc_test
    APPEND PROPERTY INSTALL_RPATH "${rpath}"
  )
endif()


# ========================================================================
#
# Copy shared libraries to the location where the executable is built.
# This is necessary for Windows to find the required DLLs when running
# the executable. (Note: On Unix it can be arranged by correctly setting
# rpath, but this does not work on Windows)
#

if(jdbc_test_stand_alone)

message("Installing shared library dependencies.")

file(GLOB libs "${CONCPP_LIB_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
file(INSTALL ${libs} DESTINATION run)

# Also copy to debug folder in case debug variants are not available.

file(INSTALL ${libs} DESTINATION run/debug)

#
# Overwrite libs in run/debug with debug versions if present.
#

file(GLOB debug_libs "${CONCPP_LIB_DIR}/debug/*${CMAKE_SHARED_LIBRARY_SUFFIX}*")
file(INSTALL ${debug_libs} DESTINATION run/debug)

#
# If non-debug libraries were not found and we are not on Windows, try
# using debug ones, if present.
#

if (NOT libs)
  file(INSTALL ${debug_libs} DESTINATION run)
endif()

endif(jdbc_test_stand_alone)
