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


CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
CMAKE_POLICY(VERSION 2.8.11)

#
# Prevent cmake from setting its default value of CMAKE_INSTALL_PREFIX
# inside PROJECT() command.
# Thanks to this, we can detect if user has set CMAKE_INSTALL_PREFIX
# or not, and then set our own default in the latter case.
#
set(CMAKE_INSTALL_PREFIX "" CACHE PATH "Install location")

PROJECT(MySQL_CONCPP)

message("Building on system: ${CMAKE_SYSTEM} (${CMAKE_SYSTEM_PROCESSOR})")
message("Using cmake generator: ${CMAKE_GENERATOR}")

include(version.cmake)


# Base name for the connector libraries.

set(LIB_NAME_BASE "mysqlcppconn2")

#
# Set up cmake module search patch
# ================================
#

# First, use modules that are local to this project

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Then use modules from CDK sub-project

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cdk/cmake)


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
# Globally enable C++11
#

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  # Use C++11 also for CHECK_CXX_SOURCE_COMPILES()
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++11")
endif()


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

#
# Enable MSVC Debug Format Information
# Produces an .obj file containing full symbolic debugging information for use
# with the debugger.
#

if(WIN32)
  foreach(LANG C CXX)
    set(CMAKE_${LANG}_FLAGS_DEBUG "${CMAKE_${LANG}_FLAGS_DEBUG} /Z7")
  endforeach()
endif()

#
# Disable MSVC unreachable code warnings unless requested.
#

option(UNREACHABLE_CODE_WARNINGS "Enable unreachable code compiler warnings")
mark_as_advanced(UNREACHABLE_CODE_WARNINGS)

if(WIN32 AND NOT UNREACHABLE_CODE_WARNINGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4702")
endif()

#
# Disable MSVC warning C4297 as it seems to be buggy.
# Seehttps://connect.microsoft.com/VisualStudio/feedback/details/897611/incorrect-warning-of-c4297-for-destructor-with-try-catch-statement
#

if(WIN32 AND NOT UNREACHABLE_CODE_WARNINGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4297")
endif()


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

OPTION(WITH_TESTS "Build project's unit tests" 0)

OPTION(THROW_AS_ASSERT
  "Turn THROW() statements in the code into asserts for easier debugging"
  0)

mark_as_advanced(THROW_AS_ASSERT)

if(THROW_AS_ASSERT)
  add_definitions(-DTHROW_AS_ASSERT)
endif()


#
# Add Boost
#

ADD_DEFINITIONS(-DBOOST_NO_AUTO_PTR=1)
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

foreach(LANG C CXX)

IF(WIN32)

  # 4127 = conditional expression is constant (needed for do {...} while(false))
  # 4512 = assignment operator could not be generated
  #
  # Note: 4512 is disabled because according to C++11 standard the situations
  # that triggers this warning should be handled automatically by the compiler
  # (and this is the case for MSVC 2015).
  # See: http://en.cppreference.com/w/cpp/language/copy_assignment

  set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} /W4 /wd4512 /wd4127")

ENDIF()

endforeach(LANG)


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

MERGE_STATIC_LIBS(libconcpp "${LIB_NAME_BASE}-static"
                  "connector")

# Pick install location for the main library

option(LIB_DIR    "Library subdirectory")

set(VS_DIR)
if(WIN32)
  if(MSVC12)
    set(VS_DIR "/vs12")
  elseif(MSVC14)
    set(VS_DIR "/vs14")
  endif()
endif()

if(NOT LIB_DIR)
  set(LIB_DIR "lib${VS_DIR}")
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(LIB_DIR "${LIB_DIR}/debug")
  endif()
endif()

INSTALL(TARGETS libconcpp DESTINATION ${LIB_DIR})
message("Connector library will be installed at: ${LIB_DIR}")


#
# Tests
# =====
#

IF (WITH_TESTS)
  # Unit tests declared with ADD_NG_TEST() (see cdk/cmake/testing.cmake)
  ADD_TEST_TARGET()

  # Test for public headers declared with ADD_HEADERS()
  # (see cdk/cmake/headers.cmake)
  ADD_HEADERS_TEST()
ENDIF (WITH_TESTS)

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

