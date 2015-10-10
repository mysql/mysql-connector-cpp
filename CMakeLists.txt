# Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; version 2 of the
# License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301  USA


CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
CMAKE_POLICY(VERSION 2.8)

PROJECT(MySQL_CONCPLS)

#
# Set up cmake module search patch.
#

#
# First, use modules that are local to this project
#

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

#
# If WITH_NGCOMMON option is set, add modules defined there.
#

SET(WITH_NGCOMMON $ENV{WITH_NGCOMMON} CACHE PATH "Path to ngcommon sources")

#IF(NOT WITH_NGCOMMON)
#  SET(WITH_NGCOMMON ${PROJECT_SOURCE_DIR}/common)
#ENDIF()

IF (WITH_NGCOMMON AND NOT EXISTS "${WITH_NGCOMMON}/cmake")
MESSAGE(WARNING "Could not find required ngcommon files under location"
                " specified with WITH_NGCOMMON: ${WITH_NGCOMMON}")
ENDIF()

IF(WITH_NGCOMMON)
  LIST(APPEND CMAKE_MODULE_PATH ${WITH_NGCOMMON}/cmake)
  #MESSAGE("added module path: ${WITH_NGCOMMON}/cmake")

ELSE()

#
# Otherwise use modules from CDK sub-project
#

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cdk/cmake)
LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cdk/common/cmake)

ENDIF()


#
# Tell cmake to not use compiler specific extensions
#

INCLUDE(compiler_features)
SET(CMAKE_CXX_EXTENSIONS no)

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
# Add Boost
#

INCLUDE(boost)
SETUP_BOOST()

#
# CDK
#
SET(WITH_CDK_DOC 0)
ADD_SUBDIRECTORY(cdk)
INCLUDE_DIRECTORIES(${CDK_INCLUDE_DIR})
INCLUDE_DIRECTORIES(cdk/parser)

#
# Project's public headers
#

ADD_SUBDIRECTORY(include)
INCLUDE_DIRECTORIES(include)


#
# Testing framework
#
#INCLUDE(testing)
#SETUP_TESTING()

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
# CDK components
#

ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(connector)

#
# Target for running all declared tests (run_unit_tests)
#

#IF(WITH_TESTS)
#  ADD_TEST_TARGET()
#  ADD_DEPENDENCIES(run_unit_tests foundation_tests_server ngs_mockup)
#ENDIF()

#
#  Target which builds the final connector library.
#
#  TODO: static/dynamic runtime variant for Win (use different output names)
#        also, different variants for different MSVC versions.
#  TODO: debug/non-debug variant
#  TODO: naming strategy
#  TODO: interface options for libconcpp target
#

INCLUDE(libutil)
MERGE_STATIC_LIBS(libconcpp "concpp-2.0" connector)
INSTALL(TARGETS libconcpp DESTINATION lib)

#
# Sample code to try things out
#

ADD_EXECUTABLE(try try.cc)
TARGET_LINK_LIBRARIES(try libconcpp)
SET_INTERFACE_OPTIONS(try connector)
#  ADD_GCOV(try)

