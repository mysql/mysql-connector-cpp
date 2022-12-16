# Copyright (c) 2015, 2020, Oracle and/or its affiliates. All rights reserved.
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


#
# Detect required std features.
#

set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++17")

include(CheckCXXSourceCompiles)
include(CheckTypeSize)

check_type_size("wchar_t" WCHAR_SIZE)
add_config(WCHAR_SIZE ${WCHAR_SIZE})

#
# -------------------------------------------
#

ADD_SUBDIRECTORY(tests)

SET(sources error.cc stream.cc connection_tcpip.cc socket.cc diagnostics.cc
            socket_detail.cc connection_openssl.cc)

file(GLOB HEADERS *.h)

#message("cdk foundation sources: ${sources}")

add_library(cdk_foundation STATIC ${sources} ${HEADERS})
add_coverage(cdk_foundation)

target_include_directories(cdk_foundation PUBLIC
  "${PROJECT_SOURCE_DIR}/include"
  "${PROJECT_BINARY_DIR}/include"  # generated config.h
)

target_link_libraries(cdk_foundation
  PUBLIC RapidJSON::rapidjson
  PRIVATE SSL::ssl
)

IF(WIN32)
  target_link_libraries(cdk_foundation PRIVATE dnsapi)
ELSEIF(NOT FREEBSD)
  target_link_libraries(cdk_foundation PRIVATE resolv)
ENDIF()

IF(WIN32)
  target_link_libraries(cdk_foundation PRIVATE ws2_32)
ENDIF(WIN32)

if(SUNOS)
  target_link_libraries(cdk_foundation PRIVATE socket nsl)
endif()

