# Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

# Check for C++ 11 support
include(CheckCXXCompilerFlag)

function(CHECK_CXX11)
  check_cxx_compiler_flag("-std=c++11" support_11)
  check_cxx_compiler_flag("-std=c++0x" support_0x)

  if(support_11)
    set(CXX11_FLAG "-std=c++11" PARENT_SCOPE)
  elseif(support_0x)
    set(CXX11_FLAG "-std=c++0x" PARENT_SCOPE)
  else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} does not support C++11 standard")
  endif()
  set(CMAKE_CXX_FLAGS ${CXX11_FLAG} PARENT_SCOPE)
endfunction()

if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  check_cxx11()
  set(${CMAKE_CXX_FLAGS} "${CMAKE_CXX_FLAGS} -Werror -Wall -Wextra -Wconversion -Wpedantic -Wshadow")
  if(ENABLE_GCOV)
    message(STATUS "Enabling code coverage using Gcov")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
  endif()

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX11_FLAG}")

elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # Overview of MSVC versions: http://www.cmake.org/cmake/help/v3.3/variable/MSVC_VERSION.html
  if("${MSVC_VERSION}" VERSION_LESS 1800)
    message(FATAL_ERROR "Need at least ${CMAKE_CXX_COMPILER} 12.0")
  endif()
  # /TP is needed so .cc files are recognoized as C++ source files by MSVC
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /TP")

  if(ENABLE_GCOV)
    message(FATAL_ERROR "Code coverage not supported with MSVC")
  endif()
else()
  message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER} is not supported")
endif()
