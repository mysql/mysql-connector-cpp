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


#
# We use UUID generator from common sub-module.
#

set(WITH_UUID "${PROJECT_SOURCE_DIR}/cdk/extra/uuid")

if(NOT EXISTS "${WITH_UUID}/src/CMakeLists.txt")
  message(FATAL_ERROR "Could not find UUID generator sources at: ${WITH_UUID}")
endif()

include_directories("${WITH_UUID}/include")

add_subdirectory("${WITH_UUID}/src" uuid)

#
# Disable warnings in code which is not ours.
#
if(MSVC)
target_compile_options(uuid_gen PRIVATE /wd4244 /wd4311 /wd4302)
endif()


#
#  Connector code
#

ADD_LIBRARY(connector STATIC
  session.cc
  result.cc
  document.cc
  collection_crud.cc
  table_crud.cc
)

ADD_CDK(connector)
target_link_libraries(connector uuid_gen)

if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(connector PRIVATE -Wall -Wextra)

  # Required because of possible exception-related GCC bug.
  # (observed in GCC 4.8.4)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O1")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O1")
endif()

add_subdirectory(tests)
