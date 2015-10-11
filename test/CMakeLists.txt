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


cmake_minimum_required(VERSION 2.8)
PROJECT(MySQL_CONCPLS_TEST)

set(WITH_CONCPLS $ENV{WITH_CONCPLS} CACHE PATH "MySQL Connector/C++ 2.0 install location")

if(NOT WITH_CONCPLS)
  message(FATAL_ERROR
    "This project requires MySQL Connector/C++ 2.0, please specify install location"
    " using WITH_CONCPLS setting"
  )
endif()

#
# Find MySQL Connector/C++ 2.0 at specified location.
#

set(CONCPLS_INCLUDES "${WITH_CONCPLS}/include")

if(NOT EXISTS "${CONCPLS_INCLUDES}/mysqlx.h")
  message(FATAL_ERROR
    "Could not find MySQL Connector/C++ 2.0 headers at specified"
    " location: ${WITH_CONCPLS}/include"
  )
endif()

find_library(CONCPLS_LIB mysql_concpp
  PATHS "${WITH_CONCPLS}/lib"
  NO_DEFAULT_PATH
)

if(NOT CONCPLS_LIB)
  message(FATAL_ERROR
    "Could not find MySQL Connector/C++ 2.0 library concpp-2.0 at specified"
    " location: ${WITH_CONCPLS}/lib"
  )
endif()

list(APPEND CONCPLS_LIBS "${CONCPLS_LIB}")

#
# Use static runtime library on Windows, beacuase MySQL Connector/C++ is
# built using static runtime.
#

IF(WIN32)

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
# Test program that uses MySQL Connector/C++ 2.0.
#

INCLUDE_DIRECTORIES(${CONCPLS_INCLUDES})

ADD_EXECUTABLE(test test.cc)
TARGET_LINK_LIBRARIES(test ${CONCPLS_LIBS})

#
# Connector/C++ requires pthread library on Unix.
#

IF(CMAKE_HOST_UNIX)
TARGET_LINK_LIBRARIES(test pthread)
ENDIF()

# We must enable C++11 to use Connector/C++ 2.0

if (CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(test PRIVATE "-std=c++11")
endif()
