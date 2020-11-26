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
  LINK_DIRECTORIES(../framework/$(ConfigurationName))
  SET(MY_TARGET_LINK_LIBRARIES connector-jdbc test_framework)
#	ADD_DEFINITIONS("-D_SECURE_SCL")
  ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")
  ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")

ELSEIF(NOT WIN32)
  SET(MY_TARGET_LINK_LIBRARIES connector-jdbc test_framework)
ENDIF(WIN32)

IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
  SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)

ADD_DEFINITIONS("-DWE_HAVE_DATATYPE_AGAIN")

IF(MYSQLCPPCONN_TEST_NOT_IMPLEMENTED)
  ADD_DEFINITIONS("-DINCLUDE_NOT_IMPLEMENTED_METHODS=1")
ENDIF(MYSQLCPPCONN_TEST_NOT_IMPLEMENTED)

INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/include/jdbc")

SET(jdbctests_sources
  ccpptests.cpp
  BaseTestFixture.cpp
  ../common/stringutils.cpp
  resources.cpp
  regression/EscapeProcessorRegressionTest.cpp
  compliance/ConnectionTest.cpp
  compliance/DatabaseMetaDataTest.cpp
  compliance/StatementTest.cpp
  compliance/ResultSetMetadataTest.cpp
  compliance/PreparedStatementTest.cpp
  simple/BlobTest.cpp
  regression/BlobRegressionTest.cpp
  regression/PreparedStatementRegressionTest.cpp
  simple/TransactionTest.cpp
  regression/SubqueriesRegressionTest.cpp
  compliance/UnbufferedRsStmtTest.cpp
  )

# It's convinient for VS users to have headers as part of project
IF(WIN32)
  SET(jdbctests_sources
        ${jdbctests_sources}
    BaseTestFixture.h
    ../common/stringutils.h
    resources.h
    regression/EscapeProcessorRegressionTest.h
    compliance/ConnectionTest.h
    compliance/DatabaseMetaDataTest.h
    compliance/StatementTest.h
    compliance/ResultSetMetadataTest.h
    compliance/PreparedStatementTest.h
    simple/BlobTest.h
    regression/BlobRegressionTest.h
    regression/PreparedStatementRegressionTest.h
    simple/TransactionTest.h
    regression/SubqueriesRegressionTest.h
    compliance/UnbufferedRsStmtTest.h
    )
ENDIF(WIN32)

ADD_EXECUTABLE(CJUnitTestsPort ${jdbctests_sources})
TARGET_LINK_LIBRARIES(CJUnitTestsPort ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

set_target_properties(CJUnitTestsPort PROPERTIES FOLDER "Tests/jdbc")

ADD_TEST(NAME jdbc_CJUnitTestsPort COMMAND CJUnitTestsPort)

#Copy sql.properties file to build dir
file(COPY sql.properties DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

MESSAGE(STATUS "Configuring C/J junit tests port")
