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

IF(WIN32)
  LINK_DIRECTORIES(${MYSQL_DIR}/lib/$(ConfigurationName))
  LINK_DIRECTORIES(../framework/$(ConfigurationName))
  SET(MY_TARGET_LINK_LIBRARIES connector-jdbc test_framework)
  SET(MY_TARGET_LINK_LIBRARIES_STATIC_CPPLIB connector-jdbc-static test_framework)
#	ADD_DEFINITIONS("-D_SECURE_SCL")
  ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")
  ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")

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
  SET(MY_TARGET_LINK_LIBRARIES connector-jdbc test_framework)
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

IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
  SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)

ADD_DEFINITIONS("-DDYNLOAD_MYSQL_LIB=\"${MYSQLCPPCONN_DYNLOAD_MYSQL_LIB}\"")
MESSAGE(STATUS "MySQL dynamic load test library: ${MYSQLCPPCONN_DYNLOAD_MYSQL_LIB}")
IF(MYSQLCPPCONN_TEST_NOT_IMPLEMENTED)
  ADD_DEFINITIONS("-DINCLUDE_NOT_IMPLEMENTED_METHODS=1")
ENDIF(MYSQLCPPCONN_TEST_NOT_IMPLEMENTED)


function(add_unit_test NAME)

  if (ARGN)
    list(GET ARGN 0 TGT_NAME)
  else()
    set(TGT_NAME test_${NAME})
  endif()

  ADD_EXECUTABLE(${TGT_NAME} ${test_${NAME}_sources})
  SET_TARGET_PROPERTIES(${TGT_NAME} PROPERTIES
    OUTPUT_NAME "${NAME}"
    LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV} ${MYSQL_LINK_FLAGS}"
    COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV}"
    FOLDER "Tests/jdbc"
  )
  TARGET_LINK_LIBRARIES(${TGT_NAME} ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

  ADD_TEST(NAME jdbc_test_${NAME} COMMAND ${TGT_NAME})

  MESSAGE(STATUS "Configuring unit tests - ${NAME}")

endfunction()


MESSAGE(STATUS "Configuring unit tests")

ADD_SUBDIRECTORY(example/)
ADD_SUBDIRECTORY(classes/)
ADD_SUBDIRECTORY(performance/)
ADD_SUBDIRECTORY(bugs/)

# Copy&Paste template: change directory name and uncomment
# ADD_SUBDIRECTORY(template_bug_group)
ADD_SUBDIRECTORY(template_bug_group)
