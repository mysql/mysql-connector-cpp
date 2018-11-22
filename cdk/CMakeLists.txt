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



CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
cmake_policy(VERSION 2.8.12)
cmake_policy(SET CMP0022 NEW)  # consistently use INTERFACE_LINK_LIBRARIES property
cmake_policy(SET CMP0023 OLD)


PROJECT(MySQLCDK)


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

#
# Basic compiler settings.
#

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(cdk_stand_alone)

  if(CMAKE_CXX_COMPILER_ID MATCHES "SunPro")

    set(options
      -m64
      -mt
      -std=c++11
      -library=stdcpp
      -xatomic=studio
      -xtarget=generic     # more portable code
      -xdebuginfo=no%decl  # faster build times
      # optimization options
      -xbuiltin=%all
      -xlibmil
    )

    if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "sparc")
      list(APPEND options -nofstore)
    endif()

    string(REPLACE ";" " " options "${options}")

    set(CMAKE_CXX_FLAGS "${options} ${CMAKE_CXX_FLAGS}")

    #foreach(type EXE STATIC SHARED MODULE)
    #  set(CMAKE_${type}_LINKER_FLAGS
    #    "${options} ${CMAKE_${type}_LINKER_FLAGS}"
    #  )
    #endforeach()

    set(CMAKE_CXX_FLAGS_DEBUG "-g ${CMAKE_CXX_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
      "-g -xO2 ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
    )
    set(CMAKE_CXX_FLAGS_RELEASE "-g0 -xO3 ${CMAKE_CXX_FLAGS_RELEASE}")
    set(CMAKE_CXX_FLAGS_MINSIZEREL
      "-g0 -xO3 -xsize ${CMAKE_CXX_FLAGS_MINSIZEREL}"
    )

    add_definitions(
      -D_POSIX_PTHREAD_SEMANTICS
      -D_REENTRANT
    )

  endif()

endif(cdk_stand_alone)


#
# Library utilities (add_library_ex)
#

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

#message(STATUS "BIG_ENDIAN: ${CDK_BIG_ENDIAN}")
add_config(CDK_BIG_ENDIAN ${CDK_BIG_ENDIAN})


#
# Make sure WIN32 is defined on Windows
#

if(WIN32)
  add_definitions(-DWIN32)
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
# Add SSL
#

message("WITH_SSL: ${WITH_SSL}")

include(install_macros)
include(ssl)
MYSQL_CHECK_SSL(${CDK_BIG_ENDIAN})
add_config(WITH_SSL)
add_config(WITH_SSL_WOLFSSL)


#
# Enable C++11
#

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

set(HAVE_MOVE_SEMANTICS 1)
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
# Add Protobuf (bundled)
#

INCLUDE(protobuf)


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


# Bundled code (with normal warning level)

ADD_SUBDIRECTORY(extra)


#
# Set higher warning level on Windows to catch non-portable
# code.
#

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
  # 4520 = The class has multiple default constructors. The first constructor is
  #        used.
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
  #
  # Note: 4520 only present on Visual Studio 12

  add_compile_options(
    /Wall /wd4512 /wd4127 /wd4100
    /wd4820 /wd4571 /wd4710 /wd4514 /wd4625 /wd4626 /wd4464 /wd4711 /wd4520
  )

  # Note: We disable warnings related to C++11 language because we want this
  # to be pure C++ code.
  #
  # 5026 = move ctor was implicitly deleted
  # 5027 = move assignment was implicitly deleted

  if (MSVC_VERSION GREATER 1800)
    add_compile_options(/wd5026 /wd5027)
  endif()

  # This is warning that has been fixed in later versions of MSVC:
  #
  # 4640 = construction of local static object is not thread-safe

  if (MSVC_VERSION LESS 1900)
    add_compile_options(/wd4640)
  endif()

ELSE()

  if(CMAKE_CXX_COMPILER_ID MATCHES "SunPro")
    # TODO: Enable -Wall after fixing warnings
    add_compile_options(-errtags=yes -erroff=hidevf)
  else()
    add_compile_options(-Wall)
  endif()

ENDIF()


if(0)
  message("flags: ${CMAKE_C_FLAGS}")
  message("c++ flags: ${CMAKE_CXX_FLAGS}")
  foreach(TYPE DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    message("${TYPE} flags: ${CMAKE_C_FLAGS_${TYPE}}")
    message("c++ ${TYPE} flags: ${CMAKE_CXX_FLAGS_${TYPE}}")
  endforeach()
endif()

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

