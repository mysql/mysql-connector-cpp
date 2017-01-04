# Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
#
# This code is licensed under the terms of the GPLv2
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



CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)


PROJECT(MySQLCDK)

cmake_policy(VERSION 2.8.12)
cmake_policy(SET CMP0022 NEW)  # consistently use INTERFACE_LINK_LIBRARIES property

#
# Detect if we are configured as stand-alone project, or sub-project.
#

IF (PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
  MESSAGE("Stand-alone configuration")
  MESSAGE("Building on system: ${CMAKE_SYSTEM}")
  SET(cdk_stand_alone 1)
ELSE()
  SET(cdk_stand_alone 0)
  INCLUDE(SubProject.cmake)
ENDIF()


#
# Set up cmake module search patch.
#

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
#MESSAGE("added module path: ${PROJECT_SOURCE_DIR}/cmake")

include(libutils)

#
# Infrastructure for defining code configuration settings
# (the ADD_CONFIG() macro for declaring contents of config.h).
#

INCLUDE(config)


#
# Detect endianess
#

if(NOT DEFINED CDK_BIG_ENDIAN)

  INCLUDE(TestBigEndian)
  TEST_BIG_ENDIAN(CDK_BIG_ENDIAN)

endif()

message(STATUS "BIG_ENDIAN: ${BIG_ENDIAN}")
add_config(CDK_BIG_ENDIAN ${CDK_BIG_ENDIAN})

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
# Position independent code support.
#

option(WITH_PIC "Generate position independet code (PIC)" OFF)
if(WITH_PIC)
  message("Generating position independent code")
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

#
# Gcov support (Linux only).
#

include(gcov)

#
# Disable 64-bit warnings until we have dealt with them
# TODO: fix all 64-bit warnings
#

option(DISABLE_WARNINGS "Do not show selected compiler warnings" OFF)
mark_as_advanced(DISABLE_WARNINGS)

if(DISABLE_WARNINGS)

  set(warning_list)

  IF(WIN32)

    list(APPEND warnings_list "/wd4244")
    list(APPEND warnings_list "/wd4267")

  ENDIF()

  string(REPLACE ";" " " warning_options "${warnings_list}")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warning_options}")

endif()


#
# We want CDK code to compile both under C++ and C++11 compiler.
# In case of C++11 build we can use certain generic C++11 features
# that are detected here.
#

INCLUDE(CheckCXXSourceCompiles)
CHECK_CXX_SOURCE_COMPILES(
  "void test(int&&) {}; int main() { return 0; }"
  HAVE_MOVE_SEMANTICS
)
ADD_CONFIG(HAVE_MOVE_SEMANTICS)

#
# Handling of THROW() statements.
#

option(THROW_AS_ASSERT
 "Turn THROW statements in the code into assertions for easier debugging"
 true)
mark_as_advanced(THROW_AS_ASSERT)

if(THROW_AS_ASSERT)
add_definitions(-DTHROW_AS_ASSERT)
endif()


#
# Add Boost
#

INCLUDE(boost)
SETUP_BOOST()


#
# Add Protobuf (bundled)
#

INCLUDE(protobuf)


#
# Add SSL
#

if(NOT DEFINED WITH_SSL)
SET(WITH_SSL "bundled" CACHE STRING "TODO: Document me!")
endif()

message("WITH_SSL: ${WITH_SSL}")

IF(WITH_SSL)
  IF(WITH_SSL STREQUAL "bundled")
    ADD_DEFINITIONS(-DWITH_SSL_YASSL)
    if(CDK_BIG_ENDIAN)
      ADD_DEFINITIONS(-DWORDS_BIGENDIAN)
    endif()
    ADD_SUBDIRECTORY(extra/yassl)
  ELSE()
    MESSAGE(WARNING "WITH_SSL option set to unrecognized value '${WITH_SSL}'"
                  " - will be ignored. Currently the only value allowed is"
                  " 'bundled', which uses the bundled YaSSL library to implement"
                  " TLS connections.")
  ENDIF()
  ADD_DEFINITIONS(-DWITH_SSL)
ENDIF()

#
# Clean exports file
#

file(REMOVE ${PROJECT_BINARY_DIR}/exports.cmake)


#
# Project's public headers
#

ADD_SUBDIRECTORY(include)
INCLUDE_DIRECTORIES(include)
#
# Add the binary directory to include path so that generated
# headers like config.h are found
#
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/include)
set(configuration_header "${PROJECT_BINARY_DIR}/include/mysql/cdk/config.h")

#
# Testing framework
#
INCLUDE(testing)
SETUP_TESTING()

if(WITH_TESTS)

  ADD_LIBRARY(process_launcher STATIC
    extra/process_launcher/process_launcher.cc
    extra/common/exception.cc)
  target_include_directories(process_launcher PRIVATE
    ${PROJECT_SOURCE_DIR}/extra/process_launcher/
    ${PROJECT_SOURCE_DIR}/extra/common)

  # quiet compile warnings outside of our code
  if (MSVC)
    target_compile_options(process_launcher PRIVATE /W2)
  else()
    target_compile_options(process_launcher PRIVATE -Wno-unused-result)
  endif()

  ADD_TEST_LIBRARIES(process_launcher cdk)
  ADD_TEST_INCLUDES(
    ${PROJECT_SOURCE_DIR}/extra/process_launcher/
    ${PROJECT_SOURCE_DIR}/extra/common
  )

  # This folder contains test headers that can be used by all tests
  # TODO: move it to a better place

  add_test_includes(${PROJECT_SOURCE_DIR}/mysqlx/tests)

endif()


#
# Deal with broken optimization in gcc 4.8.
#
# We observed very strange behaviour of exceptions when compiling
# fully optimized code wtih gcc 4.8. Downgrade optimization to -O1
# in this case. To get trully optimized code use gcc 4.9+ or clang.
#

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9")
  foreach(LANG C CXX)
  foreach(TYPE RELEASE RELWITHDEBINFO)
    string(REPLACE "-O3" "-O1" CMAKE_${LANG}_FLAGS_${TYPE} "${CMAKE_${LANG}_FLAGS_${TYPE}}")
    string(REPLACE "-O2" "-O1" CMAKE_${LANG}_FLAGS_${TYPE} "${CMAKE_${LANG}_FLAGS_${TYPE}}")
  endforeach(TYPE)
  endforeach(LANG)
endif()
endif()


#
# Set higher warning level on Windows to catch non-portable
# code.
#

foreach(LANG C CXX)

  set(SAVED_${LANG}_FLAGS ${CMAKE_${LANG}_FLAGS})

IF(WIN32)

  # 4127 = conditional expression is constant (needed for do {...} while(false))
  # 4512 = assignment operator could not be generated
  # 4100 = unreferenced formal parameter
  # 4820 = byte padding added
  # 4571 = SEH exceptions not caught by catch(...)
  # 4710 = function not inlined
  # 4514 = unreferenced inline function was removed
  # 4464 = relative incl. path contains '..'
  # 4625 = copy ctor was implicitly deleted (*)
  # 4626 = copy assignment was implicitly deleted (*)
  # 4711 = function 'function' selected for inline expansion
  #
  # Note: 4512 is disabled because according to C++11 standard the situations
  # that triggers this warning should be handled automatically by the compiler
  # (and this is the case for MSVC 2015).
  # See: http://en.cppreference.com/w/cpp/language/copy_assignment
  #
  # Note: 4711 is only informative (see https://msdn.microsoft.com/en-us/library/k402bt7y.aspx)
  #
  # Note (*): We use this a lot when inheriting from foundation::nocopy - this
  # is our C++ way to disable copy semantics.
  #

  set(CMAKE_${LANG}_FLAGS
    "${CMAKE_${LANG}_FLAGS} /Wall /wd4512 /wd4127 /wd4100
     /wd4820 /wd4571 /wd4710 /wd4514 /wd4625 /wd4626 /wd4464 /wd4711"
  )

  # Note: We disable warnings related to C++11 language because we want this
  # to be pure C++ code.
  #
  # 5026 = move ctor was implicitly deleted
  # 5027 = move assignment was implicitly deleted

  if (MSVC_VERSION GREATER 1800)
    set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} /wd5026 /wd5027")
  endif()

  # This is warning that has been fixed in later versions of MSVC:
  #
  # 4640 = construction of local static object is not thread-safe

  if (MSVC_VERSION LESS 1900)
    set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} /wd4640")
  endif()

ELSE()

  set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} -Wall")

ENDIF()

endforeach(LANG)

#message("flags: ${CMAKE_C_FLAGS}")
#message("c++ flags: ${CMAKE_CXX_FLAGS}")
#foreach(TYPE DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
#  message("${TYPE} flags: ${CMAKE_C_FLAGS_${TYPE}}")
#  message("c++ ${TYPE} flags: ${CMAKE_CXX_FLAGS_${TYPE}}")
#endforeach()


#
# Parser library
#

ADD_SUBDIRECTORY(parser)
INCLUDE_DIRECTORIES(parser)

#
# CDK components
#

ADD_SUBDIRECTORY(foundation)
ADD_SUBDIRECTORY(protocol)
ADD_SUBDIRECTORY(mysqlx)
ADD_SUBDIRECTORY(core)

# Restore default warnings level

foreach(LANG C CXX)
  set(CMAKE_${LANG}_FLAGS ${SAVED_${LANG}_FLAGS})
endforeach()

ADD_SUBDIRECTORY(extra)

#
# Write configuration with detected settings.
#

WRITE_CONFIG_HEADER(${configuration_header})

#
# Tests
#

IF(WITH_TESTS)

  # Target (run_unit_tests) for running all tests declared with ADD_NG_TESTS()
  # (see cmake/testing.cmake)

  ADD_TEST_TARGET()
  ADD_DEPENDENCIES(${target_run_unit_tests} foundation_tests_server)
  IF(WITH_NGS_MOCKUP)
  ADD_DEPENDENCIES(${target_run_unit_tests} ngs_mockup)
  ENDIF()
  # Checks for public headers declared with ADD_HEADERS() (see cmake/headers.cmake)

  ADD_HEADERS_TEST()

ENDIF()

#
# Sample code to try things out
#

IF(cdk_stand_alone)
  ADD_EXECUTABLE(try try.cc)
  TARGET_LINK_LIBRARIES(try cdk)
  #ADD_COVERAGE(try)
ENDIF()

