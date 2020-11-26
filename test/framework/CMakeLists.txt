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
#	LINK_DIRECTORIES(${MYSQL_DIR}/lib/$(ConfigurationName))
#	ADD_DEFINITIONS("-D_SECURE_SCL")
  ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")
  ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")

ELSEIF(NOT WIN32)
ENDIF(WIN32)

INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/include")
INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/include/jdbc")
INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/include/jdbc/cppconn")

IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
  SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)

SET(testframework_sources
  test_suite.cpp
  test_factory.cpp
  test_listener.cpp
  test_asserts.cpp
  test_tapOutputter.cpp
  test_runner.cpp
  test_container.cpp
  test_timer.cpp
  ../common/stringutils.cpp
  ../common/file.cpp
  start_options.cpp
  test_filter.cpp
  )

# It's convinient for VS users to have headers as part of project
IF(WIN32)
  SET(testframework_sources
        ${testframework_sources}
    framework.h
    test_runner.h
    test_suite.h
    test_case.h
    test_factory.h
    test_listener.h
    test_asserts.h
    test_outputter.h
    test_tapOutputter.h
    test_container.h
    test_timer.h
    start_options.h
    test_filter.h
    ../common/stringutils.h
    ../common/file.h
    )
ENDIF(WIN32)

ADD_LIBRARY(test_framework STATIC ${testframework_sources})
SET_TARGET_PROPERTIES(test_framework PROPERTIES
  LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV}"
  COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV}"
  FOLDER "Tests/jdbc"
)

target_link_libraries(test_framework PUBLIC Boost::boost)

MESSAGE(STATUS "Configuring tests framework lib")
