# Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.
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

#
# Sample cmakes (keep as reference)
#
# For Windows:
#  cd <ngcommon-root>
#  mkdir MY_BUILD
#  cd MY_BUILD  
#  cmake -DBOOST_INCLUDE_DIR=C:\src\mysql-server\boost\boost_1_56_0 -DBOOST_LIB_DIR=C:\src\mysql-server\boost\boost_1_56_0\stage\lib -DMY_INCLUDE_DIR=C:\src\mysql-server\MyIncludes -DMY_GMOCK_LIB_DIR=C:\src\mysql-server\MyLibs -DGMOCK_LIBRARIES=gmock ..\process_launcher
# For Linux:
#  cmake -DBOOST_INCLUDE_DIR=/usr/local/include -DBOOST_LIB_DIR=/usr/local/lib -DMY_INCLUDE_DIR=/usr/local/include:/usr/include  -DGMOCK_LIBRARIES=gmock ../process_launcher
#  cmake -DBOOST_INCLUDE_DIR=/usr/local/include -DBOOST_LIB_DIR=/usr/local/lib -DMY_INCLUDE_DIR=/usr/local/include:/usr/include  -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ECLIPSE_VERSION=4.4 -DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE -DGMOCK_LIBRARIES=gmock -G "Eclipse CDT4 - Unix Makefiles" ../process_launcher
#

cmake_minimum_required (VERSION 2.8)

project (process_launcher)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/../cmake)

include(msvc)

# Common warning flags for GCC, G++, Clang and Clang++
set(MY_WARNING_FLAGS "-Wall -Wextra -Wformat-security -Wvla")

# Common warning flags for GCC and Clang
set(MY_C_WARNING_FLAGS
    "${MY_WARNING_FLAGS} -Wwrite-strings -Wdeclaration-after-statement")

include_directories("${PROJECT_SOURCE_DIR}/../common/")
include_directories("${BOOST_INCLUDE_DIR}")
include_directories("${MY_INCLUDE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}")

link_directories(${BOOST_LIB_DIR})
link_directories(${MY_GMOCK_LIB_DIR})

file(GLOB process_launcher_SRC
	"${PROJECT_SOURCE_DIR}/process_launcher.cc"
	"${PROJECT_SOURCE_DIR}/process_launcher.h"
	"${PROJECT_SOURCE_DIR}/../common/exception.cc"
)

add_library(process_launcher ${process_launcher_SRC})

if(GMOCK_LIBRARIES)

  file(GLOB process_launcher_tests_SRC
	"${PROJECT_SOURCE_DIR}/tests/*.cc"
  )

  add_definitions( -DPROCESS_LAUNCHER_TESTS_DIR="${PROJECT_SOURCE_DIR}/tests/scripts" )

  add_executable(process_launcher.tests ${process_launcher_tests_SRC})
  target_link_libraries(process_launcher.tests process_launcher)
  target_link_libraries(process_launcher.tests gmock)
  if (NOT WIN32)
    target_link_libraries(process_launcher.tests pthread)
  endif()

  if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_link_libraries(process_launcher.tests boost_system)
  endif()

  target_link_libraries(process_launcher.tests ${GMOCK_LIBRARIES})
  
endif()

# Force project be use MT, so they dont conflict a link time with clients of process_launcher library.
if(WIN32)
  CHANGE_MD_2_MT()
endif()

