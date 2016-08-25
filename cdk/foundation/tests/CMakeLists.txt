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

ADD_DEFINITIONS(-DPORT=9876)

IF (WITH_TESTS)


ADD_EXECUTABLE(foundation_tests_server test_server.cc)
TARGET_LINK_LIBRARIES(foundation_tests_server cdk)
SET_TARGET_PROPERTIES(foundation_tests_server
  PROPERTIES OUTPUT_NAME test_server
)

add_custom_command(TARGET foundation_tests_server
                   POST_BUILD
                   COMMAND cmake -E copy $<TARGET_FILE:foundation_tests_server> "${CMAKE_CURRENT_BINARY_DIR}/test_server${CMAKE_EXECUTABLE_SUFFIX}")


ADD_TEST_ENVIRONMENT("FOUNDATION_TEST_SERVER=${CMAKE_CURRENT_BINARY_DIR}/test_server${CMAKE_EXECUTABLE_SUFFIX}")


ADD_NG_TEST(foundation-t
  error_t.cc time_t.cc
  opaque_t.cc opaque_t_impl.cc
  stream_t.cc connection_tcpip_t.cc
  diagnostics_t.cc codec_t.cc
)


ENDIF()
