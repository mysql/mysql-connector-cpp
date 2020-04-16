# Copyright (c) 2020, Oracle and/or its affiliates. All rights reserved.
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
# Setup testing infrastructure.
#

if(NOT WITH_TESTS)
  return()
endif()

#
# Note: if building static connector on windows, the type of
# runtime library used by tests must match that used by the connector
# (which is statically linked with tests). This must also match the
# runtime type used by gtest library (which we also link statically to).
#

if(BUILD_STATIC)
  set(STATIC_TESTS_MSVCRT ${STATIC_MSVCRT})
endif()

add_library(test_harness STATIC test_harness.cc)
set_target_properties(test_harness PROPERTIES FOLDER "Tests")

add_test_libraries(test_harness connector)
add_test_includes(${CMAKE_CURRENT_SOURCE_DIR})


if(MSVC)

  set(TEST_COMPILE_FLAGS
    -D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
    -D_CRT_SECURE_NO_WARNINGS
    -D_SCL_SECURE_NO_WARNINGS
  )

  target_compile_definitions(test_harness PUBLIC
    -D_SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
  )

else()

  set(TEST_COMPILE_FLAGS
    -Wno-sign-compare
  )

endif()


