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

SETUP_BOOST()

#
# Detect std::shared_ptr<>
#

INCLUDE(CheckCXXSourceCompiles)
CHECK_CXX_SOURCE_COMPILES(
  "#include <memory>
   void test(std::shared_ptr<int>) {};
   int main() { return 0; }"
  HAVE_SHARED_PTR
)
#message("HAVE_SHARED_PTR: ${HAVE_SHARED_PTR}")
ADD_CONFIG(HAVE_SHARED_PTR)


ADD_SUBDIRECTORY(tests)

SET(sources error.cc stream.cc connection_tcpip.cc socket.cc diagnostics.cc
            string.cc socket_detail.cc)

IF(WITH_SSL STREQUAL "bundled")
  SET(sources ${sources} connection_yassl.cc)
ENDIF()

#if linking free build, use boost_code because of boost_system implementation
IF(NOT BOOST_SYSTEM_LIBS)

  LIST(APPEND sources boost_code.cc)

  # Disable any optimization for the boost_code.cc source - otherwise
  # the code gets optimized out and linker can not find required symbols.

  IF(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    SET_PROPERTY(SOURCE boost_code.cc PROPERTY COMPILE_FLAGS -O0)
  ENDIF()

ENDIF(NOT BOOST_SYSTEM_LIBS)

SET(target_foundation ${cdk_target_prefix}foundation
    CACHE INTERNAL "CDK foundation target")

ADD_LIBRARY(${target_foundation} OBJECT ${sources})
ADD_COVERAGE(${target_foundation})

IF(WITH_SSL STREQUAL "bundled")
  lib_interface_link_libraries(${target_foundation} yassl)
ENDIF()

IF(WIN32)
  lib_interface_link_libraries(${target_foundation} ws2_32)
  target_compile_definitions(${target_foundation}
                             PRIVATE _CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
  lib_interface_link_libraries(${target_foundation} socket nsl)
endif()

