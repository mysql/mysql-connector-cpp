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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

cmake_minimum_required(VERSION 2.8)

ADD_EXECUTABLE(tests_uuid tests_uuid.cc)

LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib)

IF(WIN32)
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib/debug)
  ELSEIF(CMAKE_BUILD_TYPE STREQUAL "")
    LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib/opt)
  ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")
  TARGET_LINK_LIBRARIES(tests_uuid uuid_gen)
ELSE(WIN32)
    TARGET_LINK_LIBRARIES(tests_uuid uuid_gen pthread)
ENDIF(WIN32)

SET_TARGET_PROPERTIES(tests_uuid PROPERTIES LINKER_LANGUAGE CXX)
