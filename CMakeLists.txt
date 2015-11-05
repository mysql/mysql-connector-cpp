# Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
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


CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
CMAKE_POLICY(VERSION 2.8.11)

#
# Prevent cmake from setting its default value of CMAKE_INSTALL_PREFIX
# inside PROJECT() command.
# Thanks to this, we can detect if user has set CMAKE_INSTALL_PREFIX
# or not, and then set our own default in the latter case.
#
set(CMAKE_INSTALL_PREFIX "" CACHE PATH "Install location")

PROJECT(MySQL_CONCPLS)

message("Building on system: ${CMAKE_SYSTEM} (${CMAKE_SYSTEM_PROCESSOR})")
message("Using cmake generator: ${CMAKE_GENERATOR}")


#
# Ngcommon path
# =============
#

if(NOT DEFINED WITH_NGCOMMON)
  set(WITH_NGCOMMON $ENV{WITH_NGCOMMON})
  if(NOT WITH_NGCOMMON)
    set(WITH_NGCOMMON "${PROJECT_SOURCE_DIR}/cdk/common")
  endif()
endif()

message("ngcommon location: ${WITH_NGCOMMON}")

SET(WITH_NGCOMMON ${WITH_NGCOMMON} CACHE PATH "Path to ngcommon sources")


IF (NOT EXISTS "${WITH_NGCOMMON}/cmake")
MESSAGE(WARNING "Could not find required ngcommon files under location"
                " specified with WITH_NGCOMMON: ${WITH_NGCOMMON}")
ENDIF()


#
# Set up cmake module search patch
# ================================
#

# First, use modules that are local to this project

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Then use modules from CDK sub-project

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cdk/cmake)

# Finally ngcommon modules

LIST(APPEND CMAKE_MODULE_PATH ${WITH_NGCOMMON}/cmake)


#
# Compiler settings
# =================
#

#
# Tell cmake to not use compiler specific extensions
#

INCLUDE(compiler_features)
SET(CMAKE_CXX_EXTENSIONS no)

#
# If Clang is used and deployment target is not specified
# with MACOSX_DEPLOYMENT_TARGET environment variable, make
# sure that clang's native implementation of C++ runtime
# libarary (libc++) is used. Otherwise clang defaults to
# GNU version (libstdc++) which is outdated and does
# not handle C++11 well.
#
# TODO: Add option to use the default runtime if user wishes
# so.
#

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang"
   AND NOT DEFINED ENV{MACOSX_DEPLOYMENT_TARGET})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

#
# Configure static runtime library on Windows if requested
#

OPTION(STATIC_MSVCRT "Use static MSVC runtime library" OFF)

IF(WIN32 AND STATIC_MSVCRT)

message("Using static runtime library")

foreach(LANG C CXX)
  set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} /MT")
  foreach(TYPE RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_${LANG}_FLAGS_${TYPE} "${CMAKE_${LANG}_FLAGS_${TYPE}} /MT")
  endforeach()
  set(CMAKE_${LANG}_FLAGS_DEBUG "${CMAKE_${LANG}_FLAGS_DEBUG} /MTd")
endforeach(LANG)

ENDIF()

#message("flags: ${CMAKE_C_FLAGS}")
#message("c++ flags: ${CMAKE_CXX_FLAGS}")
#foreach(TYPE DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
#  message("${TYPE} flags: ${CMAKE_C_FLAGS_${TYPE}}")
#  message("c++ ${TYPE} flags: ${CMAKE_CXX_FLAGS_${TYPE}}")
#endforeach()

#
# Main project and its dependencies
# =================================
#

#
# Add Boost
#

INCLUDE(boost)
SETUP_BOOST()

#
# CDK
#
SET(WITH_CDK_DOC 0)
OPTION(WITH_CDK_TESTS "cdk tests" OFF)
ADD_SUBDIRECTORY(cdk)
INCLUDE_DIRECTORIES(${CDK_INCLUDE_DIR})
INCLUDE_DIRECTORIES(cdk/parser)

#
# Unit tests framework
#

INCLUDE(testing)
SETUP_TESTING()

#
# Project's public headers
#

ADD_SUBDIRECTORY(include)
INCLUDE_DIRECTORIES(include)

#
# Set higher warning level on Windows to catch non-portable
# code.
#

IF(WIN32)

foreach(LANG C CXX)
  set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} /W4 /wd4512")
  foreach(TYPE RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_${LANG}_FLAGS_${TYPE} "${CMAKE_${LANG}_FLAGS_${TYPE}} /W4 /wd4512")
  endforeach()
endforeach(LANG)

ENDIF()

#
# Connector/C++ components
#

ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(connector)

#
#  Target which builds the final connector library.
#
#  TODO: static/dynamic runtime variant for Win (use different output names)
#        also, different variants for different MSVC versions.
#  TODO: debug/non-debug variant
#  TODO: naming strategy
#  TODO: interface options for libconcpp target
#  TODO: restrict exported symbols
#

INCLUDE(libutil)

MERGE_STATIC_LIBS(libconcpp "mysql_concpp"
                  "connector")
INSTALL(TARGETS libconcpp DESTINATION lib)

#
# Tests
# =====
#

ADD_TEST_TARGET()

#
# Sample code to try things out
#

ADD_EXECUTABLE(try try.cc)
TARGET_LINK_LIBRARIES(try libconcpp)
SET_INTERFACE_OPTIONS(try connector)
#  ADD_GCOV(try)

#
# Show dynamic library dependencies for try program.
#

find_program(LDD ldd)
if(NOT LDD)
  find_program(LDD otool)
  if(LDD)
    set(LDD_OPTS "-L" CACHE INTERNAL "options for linker debugger")
  endif()
endif()

if(LDD)
  add_custom_command(TARGET try POST_BUILD
    COMMAND ${LDD} ${LDD_OPTS} $<TARGET_FILE:try>
    COMMENT "Checking dynamic library dependencies:"
  )
endif()

#
# Target for running all declared tests (run_unit_tests)
# TODO: add unit tests
#

#
# Testing framework
#
#INCLUDE(testing)
#SETUP_TESTING()

#
#IF(WITH_TESTS)
#  ADD_TEST_TARGET()
#  ADD_DEPENDENCIES(run_unit_tests foundation_tests_server ngs_mockup)
#ENDIF()


#
# Packaging specifications
# ========================
#

if(NOT CMAKE_INSTALL_PREFIX)

  if(WIN32)

    if(DEFINED ENV{HOMEPATH})
      string(REPLACE "\\" "/" install_prefix "$ENV{HOMEDRIVE}$ENV{HOMEPATH}")
    else()
      set(install_prefix "C:/Program Files (x86)")
    endif()
    set(CMAKE_INSTALL_PREFIX "${install_prefix}/MySQL/MySQL Connector C++ 2.0")

  else()

    set(CMAKE_INSTALL_PREFIX "/usr/local/mysql/connector-c++-2.0")

  endif()

endif()

message("Install location: ${CMAKE_INSTALL_PREFIX}")

option(WITH_PACKAGES "Configure for building binary/source packages" OFF)
if(WITH_PACKAGES)
  include(PackageSpecs.cmake)
endif()

