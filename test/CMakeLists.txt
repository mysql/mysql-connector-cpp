# Copyright (c) 2008, 2020, Oracle and/or its affiliates.
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

#Use Options to configure default test server params

SET(TEST_DEFAULT_HOST   "tcp://localhost:3306"  CACHE STRING "Defines Unit Tests default server" )
SET(TEST_DEFAULT_DB     "test"                  CACHE STRING "Defines Unit Tests default Database" )
SET(TEST_DEFAULT_LOGIN  "root"                  CACHE STRING "Defines Unit Tests default login user" )
SET(TEST_DEFAULT_PASSWD ""                      CACHE STRING "Defines Unit Tests default login user password" )


configure_file(tests_config.h.in
               ${CMAKE_BINARY_DIR}/include/tests_config.h)

INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/include")
INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/include/jdbc")
INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/include/jdbc/cppconn")

IF(WIN32)
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    LINK_DIRECTORIES(${MYSQL_DIR}/lib/debug)
  ELSEIF(CMAKE_BUILD_TYPE STREQUAL "")
    LINK_DIRECTORIES(${MYSQL_DIR}/lib/opt)
  ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

  ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")
  ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")

  SET(MY_TARGET_LINK_LIBRARIES connector-jdbc)
ELSEIF(NOT WIN32)
  SET(MY_TARGET_LINK_LIBRARIES connector-jdbc)
ENDIF(WIN32)

IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
  SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)

ADD_EXECUTABLE(static_test static_test.cpp)

SET_TARGET_PROPERTIES(static_test PROPERTIES
  LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV} ${MYSQL_LINK_FLAGS}"
  COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV}"
  FOLDER "Tests/jdbc"
)

TARGET_LINK_LIBRARIES(static_test ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

#TODO: Check if this test should be added since it is failing!
#ADD_TEST(NAME jdbc_static_test COMMAND static_test)

LINK_DIRECTORIES(${GLIB_DIR}/lib)
ADD_EXECUTABLE(driver_test driver_test.cpp)

SET_TARGET_PROPERTIES(driver_test PROPERTIES
  LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV} ${MYSQL_LINK_FLAGS}"
  COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV}"
  FOLDER "Tests/jdbc"
)

TARGET_LINK_LIBRARIES(driver_test ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

#TODO: Check if this test should be added since it is failing!
#ADD_TEST(NAME jdbc_driver_test COMMAND driver_test)

MESSAGE(STATUS "Configuring test cases")

add_subdirectory(CJUnitTestsPort)
add_subdirectory(framework)
add_subdirectory(unit)
