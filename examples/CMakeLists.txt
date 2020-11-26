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


SET(CMAKE_VERBOSE_MAKEFILE 0)

INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/include")
INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/include/jdbc")
INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/include/jdbc/cppconn")

SET(MYSQLCPPCONN_BUILD_EXAMPLES 1 CACHE BOOL "build-examples")
IF(MYSQLCPPCONN_BUILD_EXAMPLES)
  LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/cppconn)

  IF(WIN32)
    IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
      LINK_DIRECTORIES(${MYSQL_DIR}/lib/debug)
    ELSEIF(CMAKE_BUILD_TYPE STREQUAL "")
      LINK_DIRECTORIES(${MYSQL_DIR}/lib/opt)
    ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

    ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")

    SET(MY_TARGET_LINK_LIBRARIES connector-jdbc)
    IF(MYSQLCLIENT_STATIC_BINDING)
      SET(MY_TARGET_LINK_LIBRARIES ${MY_TARGET_LINK_LIBRARIES})
    ENDIF(MYSQLCLIENT_STATIC_BINDING)

    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
    FIND_LIBRARY(MYSQLCPPCONN_DYNLOAD_MYSQL_LIB
      NAMES
        libmysql
      PATHS
        ${MYSQL_LIB_DIR}
        ${MYSQL_LIB_DIR}/lib/${libsuffixDist}
        ${MYSQL_LIB_DIR}/lib  #mysqlclient may be in lib for some c/c distros
        ${MYSQL_LIB_DIR}/libmysql/${libsuffixBuild}
        ${MYSQL_LIB_DIR}/client/${libsuffixBuild}
        $ENV{MYSQL_LIB_DIR}
        $ENV{MYSQL_LIB_DIR}/lib/${libsuffixDist}
        $ENV{MYSQL_LIB_DIR}/lib  #mysqlclient may be in lib for some c/c distros
        $ENV{MYSQL_LIB_DIR}/libmysql/${libsuffixBuild}
        $ENV{MYSQL_LIB_DIR}/client/${libsuffixBuild}
        $ENV{MYSQL_DIR}/lib/${libsuffixDist}
        $ENV{MYSQL_DIR}/lib  #mysqlclient may be in lib for some c/c distros
        $ENV{MYSQL_DIR}/libmysql/${libsuffixBuild}
        $ENV{MYSQL_DIR}/client/${libsuffixBuild}
        $ENV{ProgramFiles}/MySQL/*/lib/${libsuffixDist}
        $ENV{ProgramFiles}/MySQL/*/lib
        $ENV{SystemDrive}/MySQL/*/lib/${libsuffixDist}
        $ENV{SystemDrive}/MySQL/*/lib
        NO_DEFAULT_PATH
    )

    ELSEIF(NOT WIN32)
      SET(MY_TARGET_LINK_LIBRARIES connector-jdbc)
      IF(MYSQLCLIENT_STATIC_BINDING)
        SET(MY_TARGET_LINK_LIBRARIES ${MY_TARGET_LINK_LIBRARIES})
      ENDIF(MYSQLCLIENT_STATIC_BINDING)

      FIND_LIBRARY(MYSQLCPPCONN_DYNLOAD_MYSQL_LIB
        NAMES
          libmysqlclient_r.so
          libmysqlclient.so
        PATHS
          ${MYSQL_LIB_DIR}
          ${MYSQL_LIB_DIR}/libmysql_r/.libs
          ${MYSQL_LIB_DIR}/lib
          ${MYSQL_LIB_DIR}/lib/mysql
          $ENV{MYSQL_LIB_DIR}
          $ENV{MYSQL_LIB_DIR}/libmysql_r/.libs
          $ENV{MYSQL_LIB_DIR}/lib
          $ENV{MYSQL_LIB_DIR}/lib/mysql
          $ENV{MYSQL_DIR}/libmysql_r/.libs
          $ENV{MYSQL_DIR}/lib
          $ENV{MYSQL_DIR}/lib/mysql
          ${MYSQL_CLIB_DIR}
          ${MYSQL_CLIB_DIR}/libmysql_r/.libs
          ${MYSQL_CLIB_DIR}/lib
          ${MYSQL_CLIB_DIR}/lib/mysql
          /usr/lib/mysql
          /usr/local/lib/mysql
          /usr/local/mysql/lib
          /usr/local/mysql/lib/mysql
          /opt/mysql/mysql/lib
          /opt/mysql/mysql/lib/mysql
          NO_DEFAULT_PATH
      )
    ENDIF(WIN32)

    ADD_DEFINITIONS("-DDYNLOAD_MYSQL_LIB=\"${MYSQLCPPCONN_DYNLOAD_MYSQL_LIB}\"")
    MESSAGE(STATUS "MySQL dynamic load test library: ${MYSQLCPPCONN_DYNLOAD_MYSQL_LIB}")

    IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
      SET(MY_GCOV_LINK_LIBRARIES gcov)
    ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)


    foreach(example
      connect
      connection_meta_schemaobj
      debug_output
      exceptions
      prepared_statement
      resultset
      resultset_binary
      resultset_meta
      resultset_types
      statement
      dynamic_load
    )

      ADD_EXECUTABLE(${example} ${example}.cpp)
      SET_TARGET_PROPERTIES(${example} PROPERTIES
        LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV} ${MYSQL_LINK_FLAGS}"
        COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV}"
        FOLDER "Tests/jdbc"
      )
      TARGET_LINK_LIBRARIES(${example}
        ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES}
        ${MYSQLCPPCONN_BOOST_SYSTEM_LIBS}
        ${MYSQLCPPCONN_BOOST_THREAD_LIBS} ${MYSQLCPPCONN_ICU_LIBRARY}
      )

      ADD_TEST(NAME jdbc_${example} COMMAND ${example})

    endforeach()


ENDIF(MYSQLCPPCONN_BUILD_EXAMPLES)

MESSAGE(STATUS "Configuring examples")
