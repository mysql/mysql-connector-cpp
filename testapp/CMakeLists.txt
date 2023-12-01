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


cmake_minimum_required(VERSION 2.8.12)
PROJECT(MySQL_CONCPP_TEST)

#
# This project builds test application sources in this folder using an external 
# installation of the connector.
#
# Note: The project is *not* using connector library taken form the build tree 
# of this source tree.
#
# Settings using by this project:
#
# BUILD_STATIC            whether to link with connector statically
#                         or dynamically (the default)
#
# WITH_SSL                if we use static linking with connector, we may have
#                         to pass path to openssl package
#
# WITH_JDBC               also build JDBC API test application
#
# REQUIRED_COMPONENTS     list of required components passed to find_package()
#
# To locate `mysql-concpp` package which is installed in non-standard location
# the `mysql-concpp_DIR` variable can be set as usual (see find_package()).
# Alternatively if the variable is not set but `WITH_CONCPP` is defined then
# the latter path is used.
#


# ========================================================================
# Dependencies


if(WITH_SSL)

  message(STATUS "Using custom OpenSSL libraries at: ${WITH_SSL}")
  
  add_library(openssl INTERFACE)
  target_link_directories(openssl INTERFACE "${WITH_SSL}")

  if(WIN32)
    set(ssl_libs libssl libcrypto)
  else()
    set(ssl_libs ssl crypto)
  endif()

  target_link_libraries(openssl INTERFACE ${ssl_libs})
  add_library(mysql::openssl ALIAS openssl)

endif()

if(NOT DEFINED mysql-concpp_DIR AND DEFINED WITH_CONCPP)
  set(mysql-concpp_DIR "${WITH_CONCPP}")
endif()


find_package(mysql-concpp REQUIRED ${REQUIRED_COMPONENTS})


#
# ========================================================================
#
# Arrange for executables to be built in either "run" or "run/debug" location,
# depending on the build type. This is to simplify runtime dependencies
# management (see below).
#

set(OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/run")

file(MAKE_DIRECTORY "${OUTPUT_DIRECTORY}")
file(MAKE_DIRECTORY "${OUTPUT_DIRECTORY}/debug")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIRECTORY}")
foreach(config ${CMAKE_CONFIGURATION_TYPES})
  string(TOUPPER "${config}" CONFIG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${OUTPUT_DIRECTORY}")
endforeach()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIRECTORY}/debug")


#
# Targets to build test applications.
#

set(static_suffix)
if(BUILD_STATIC)
  set(devapi_lib mysql::concpp-static)
  set(jdbc_lib mysql::concpp-jdbc-static)
else()
  set(devapi_lib mysql::concpp)
  set(jdbc_lib mysql::concpp-jdbc)
endif()


if(NOT TARGET ${devapi_lib})
  message(SEND_ERROR "The Con/C++ library target not defined")
endif()

add_executable(devapi_test devapi_test.cc)
target_link_libraries(devapi_test ${devapi_lib})

add_executable(xapi_test xapi_test.c)
target_link_libraries(xapi_test ${devapi_lib})

# FIXME: make it work with static libs on Linux

if(WITH_JDBC AND (NOT BUILD_STATIC OR WIN32))

  if(NOT TARGET ${jdbc_lib})
    message(SEND_ERROR "The legacy Con/C++ library target not defined")
  endif()

  add_executable(jdbc_test jdbc_test.cc)
  target_link_libraries(jdbc_test ${jdbc_lib})

endif()


#
# Set RPATH so that executables find libraries that are next to it
# (this is default behavior on Windows).
#

set(rpath)
if(APPLE)
  set(rpath "@loader_path")
elseif(NOT WIN32)
  set(rpath "$ORIGIN")
endif()

set(targets devapi_test xapi_test)
if(TARGET jdbc_test)
  list(APPEND targets jdbc_test)
endif()

set_property(TARGET ${targets}
  PROPERTY BUILD_WITH_INSTALL_RPATH ON
)

if(rpath)
  set_property(TARGET ${targets}
    APPEND PROPERTY INSTALL_RPATH "${rpath}"
  )
endif()


#
# ========================================================================
#
# Copy shared libraries to the location where the executable is built.
# This is necessary for Windows to find the required DLLs when running
# the executable. (Note: On Unix it can be arranged by correctly setting
# rpath, but this does not work on Windows)
#
# Note: This needs to be done also when linking with static connector library
# because resulting executable will be looking for connector dependencies
# (e.g., OpenSSL DLLs)
#

if(MYSQL_CONCPP_RUNTIME_LIBRARY_DIR)

  message("Installing shared library dependencies.")

  file(GLOB libs 
    "${MYSQL_CONCPP_RUNTIME_LIBRARY_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
  )
  file(INSTALL ${libs} DESTINATION run)

  # Also copy to debug folder in case debug variants are not available.

  file(INSTALL ${libs} DESTINATION run/debug)

  #
  # Overwrite libs in run/debug with debug versions if present.
  #

  file(GLOB debug_libs
    "${MYSQL_CONCPP_RUNTIME_LIBRARY_DIR}/debug/*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
  )
  file(INSTALL ${debug_libs} DESTINATION run/debug)

  #
  # If non-debug libraries were not found and we are not on Windows, try
  # using debug ones, if present.
  #

  if (NOT libs)
    file(INSTALL ${debug_libs} DESTINATION run)
  endif()

endif()
