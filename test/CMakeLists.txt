#
# Copyright (c) 2008, 2016, Oracle and/or its affiliates. All rights reserved.
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


#Use Options to configure default test server params

SET(TEST_DEFAULT_HOST   "tcp://localhost:3306"  CACHE STRING "Defines Unit Tests default server" )
SET(TEST_DEFAULT_DB     "test"                  CACHE STRING "Defines Unit Tests default Database" )
SET(TEST_DEFAULT_LOGIN  "root"                  CACHE STRING "Defines Unit Tests default login user" )
SET(TEST_DEFAULT_PASSWD "root"                  CACHE STRING "Defines Unit Tests default login user password" )


configure_file(tests_config.h.in
               ${CMAKE_BINARY_DIR}/tests_config.h)

INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}")

IF(WIN32)
  IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    LINK_DIRECTORIES(${MYSQL_DIR}/lib/debug)
  ELSEIF(CMAKE_BUILD_TYPE STREQUAL "")
    LINK_DIRECTORIES(${MYSQL_DIR}/lib/opt)
  ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

  ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")
  ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")

  SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn)
ELSEIF(NOT WIN32)
  SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn)
ENDIF(WIN32)

IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
  SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)

ADD_EXECUTABLE(static_test static_test.cpp)

SET_TARGET_PROPERTIES(static_test PROPERTIES
        LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV} ${MYSQL_LINK_FLAGS}"
        COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV}")

TARGET_LINK_LIBRARIES(static_test ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})


LINK_DIRECTORIES(${GLIB_DIR}/lib)
ADD_EXECUTABLE(driver_test driver_test.cpp)

SET_TARGET_PROPERTIES(driver_test PROPERTIES
          LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV} ${MYSQL_LINK_FLAGS}"
          COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV}")

TARGET_LINK_LIBRARIES(driver_test ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})


MESSAGE(STATUS "Configuring test cases")
