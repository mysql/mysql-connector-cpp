#   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPL
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPL as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR})
INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR}/cppconn)

IF(WIN32)
	LINK_DIRECTORIES(${MYSQL_DIR}/lib/$(ConfigurationName))
	LINK_DIRECTORIES(../framework/$(ConfigurationName))
	SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn libmysql test_framework)
#	ADD_DEFINITIONS("-D_SECURE_SCL")
	ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")
	ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")

ELSEIF(NOT WIN32)
	SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn mysqlclient_r test_framework)
ENDIF(WIN32)

IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
	SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)

ADD_DEFINITIONS("-DWE_HAVE_DATATYPE_AGAIN")

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

MESSAGE(STATUS "Configuring C/J junit tests port")
