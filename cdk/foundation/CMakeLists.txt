# Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
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

set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++11")

INCLUDE(CheckCXXSourceCompiles)

CHECK_CXX_SOURCE_COMPILES(
  "#include <memory>
   void test(std::shared_ptr<int>) {};
   int main() { return 0; }"
  HAVE_SHARED_PTR
)
#message("HAVE_SHARED_PTR: ${HAVE_SHARED_PTR}")
ADD_CONFIG(HAVE_SHARED_PTR)

if (NOT HAVE_SHARED_PTR)
  message(FATAL_ERROR "Type std::shared_ptr required by CDK is not available")
endif()


CHECK_CXX_SOURCE_COMPILES(
  "#include <system_error>
   void test(std::system_error) {};
   int main() { return 0; }"
  HAVE_SYSTEM_ERROR
)
#message("HAVE_SYSTEM_ERROR: ${HAVE_SYSTEM_ERROR}")
ADD_CONFIG(HAVE_SYSTEM_ERROR)

if (NOT HAVE_SYSTEM_ERROR)
  message(FATAL_ERROR "Type std::system_error required by CDK is not available")
endif()


CHECK_CXX_SOURCE_COMPILES(
  "#include <codecvt>
   void test(std::codecvt_utf8<wchar_t>) {};
   int main() { return 0; }"
  HAVE_CODECVT_UTF8
)
#message("HAVE_CODECVT_UTF8: ${HAVE_CODECVT_UTF8}")
ADD_CONFIG(HAVE_CODECVT_UTF8)

#
# Note: If codecvt_utf8 is not avaliable we use codecvt of the "en_US.utf8"
# locale hoping that it will cover reasonable range of unicode chars. This seems
# to be the case for the GNU standard C++ library implementation but it is not
# clear if it holds also for other implementations (for example, this is not
# true for MSVC on Windows). Thus, if not using GNU then we insist on having
# codecvt_utf8.
#

if (NOT HAVE_CODECVT_UTF8) #AND NOT CMAKE_COMPILER_IS_GNUCXX)
  message("Type std::codecvt_utf8 not available on this platform, using boost/locale as a fallback.")
  include(boost)
  setup_boost()
endif()


#
# -------------------------------------------
#

ADD_SUBDIRECTORY(tests)

SET(sources error.cc stream.cc connection_tcpip.cc socket.cc diagnostics.cc
            string.cc socket_detail.cc)

IF(WITH_SSL)

  list(APPEND sources connection_openssl.cc)

  if(WIN32 AND OPENSSL_APPLINK_C)
    message("adding source: ${OPENSSL_APPLINK_C}")
    list(APPEND sources "${OPENSSL_APPLINK_C}")
    # Remove warnings from openssl applink.c
    set_source_files_properties("${OPENSSL_APPLINK_C}"  PROPERTIES
                                COMPILE_FLAGS "/wd4152 /wd4996")
  endif()

ENDIF()

file(GLOB HEADERS *.h)

#message("cdk foundation sources: ${sources}")

SET(target_foundation ${cdk_target_prefix}foundation
    CACHE INTERNAL "CDK foundation target")

ADD_LIBRARY(${target_foundation} OBJECT ${sources} ${HEADERS})
ADD_COVERAGE(${target_foundation})
add_ssl(${target_foundation})


IF(WIN32)
  lib_interface_link_libraries(${target_foundation} ws2_32)
  target_compile_definitions(${target_foundation}
                             PRIVATE _CRT_SECURE_NO_WARNINGS)
ENDIF(WIN32)

if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
  lib_interface_link_libraries(${target_foundation} socket nsl)
endif()

