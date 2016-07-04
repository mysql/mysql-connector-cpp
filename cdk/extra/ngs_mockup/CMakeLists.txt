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


CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(NGSMockupServer)

IF(NOT DEFINED WITH_NGS_MOCKUP)
  OPTION(WITH_NGS_MOCKUP "Build NGS mockup server" OFF)
ENDIF()

IF(WITH_NGS_MOCKUP)
MESSAGE("Building NGS mockup server")

# We do not apply strict warning requirements to this code.

if (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")
endif()

ADD_DEFINITIONS(-DDEFAULT_PORT=33060)

ADD_EXECUTABLE(ngs_mockup ngs_mockup.cc)
TARGET_LINK_LIBRARIES(ngs_mockup ${target_proto_mysqlx})

#
# Copy executable to location that is independent from
# build configuration.
#

add_custom_command(TARGET ngs_mockup
                   POST_BUILD
                   COMMAND cmake -E copy $<TARGET_FILE:ngs_mockup> "${CMAKE_CURRENT_BINARY_DIR}/ngs_mockup${CMAKE_EXECUTABLE_SUFFIX}")

ENDIF()
