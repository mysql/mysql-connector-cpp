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


#
# We use UUID generator from common sub-module.
#

set(WITH_UUID "${PROJECT_SOURCE_DIR}/cdk/common/uuid")

if(NOT EXISTS "${WITH_UUID}/src/CMakeLists.txt")
  message(FATAL_ERROR "Could not find UUID generator sources at: ${WITH_UUID}")
endif()


include_directories("${WITH_UUID}/include")

add_subdirectory("${WITH_UUID}/src" uuid)

ADD_LIBRARY(connector STATIC session.cc result.cc crud.cc document.cc)
ADD_CDK(connector)
target_link_libraries(connector uuid_gen)
target_compile_features(connector PUBLIC cxx_rvalue_references)
