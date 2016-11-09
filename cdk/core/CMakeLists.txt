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


cmake_minimum_required(VERSION 2.8)

ADD_SUBDIRECTORY(tests)

# required by codec.cc
INCLUDE_DIRECTORIES(${PROTOBUF_INCLUDE_DIR})

set(cdk_libs protobuf-lite)

if(NOT PROTOBUF_LITE)
  list(APPEND cdk_libs protobuf)
endif()

if(WITH_SSL STREQUAL "bundled")
  list(APPEND cdk_libs yassl taocrypt)
endif()

add_library_ex(cdk STATIC session.cc codec.cc
  OBJECTS
    ${target_mysqlx}
    ${target_proto_mysqlx}
    ${target_foundation}
    ${target_parser}
  LIBS
    ${cdk_libs}
)

add_coverage(cdk)

#MESSAGE("cdk depends on: ${target_mysqlx} ${target_parser}")

EXPORT(TARGETS cdk
  APPEND FILE ${PROJECT_BINARY_DIR}/exports.cmake
)


