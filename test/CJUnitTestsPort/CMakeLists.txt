#   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; version 2 of the License.
#
#   There are special exceptions to the terms and conditions of the GPL
#   as it is applied to this software. View the full text of the
#   exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
#   software distribution.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR})
INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR}/cppconn)

# no need to look for libmysql - working with C/C++ only!
#INCLUDE(${CMAKE_SOURCE_DIR}/FindMySQL.cmake)

IF(WIN32)
	LINK_DIRECTORIES(${MYSQL_DIR}/lib/$(ConfigurationName))
	LINK_DIRECTORIES(framework/$(ConfigurationName))
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

SET(jdbctests_sources
	ccpptests.cpp
	BaseTestFixture.cpp
	common/stringutils.cpp
	resources.cpp
	regression/EscapeProcessorRegressionTest.cpp
	compliance/ConnectionTest.cpp
	compliance/DatabaseMetaDataTest.cpp
	compliance/StatementTest.cpp
	compliance/ResultSetMetadataTest.cpp
	compliance/PreparedStatementTest.cpp
	simple/BlobTest.cpp
	regression/BlobRegressionTest.cpp
	regression/PreparedStatementTest.cpp
	)

# It's convinient for VS users to have headers as part of project
IF(WIN32)
	SET(jdbctests_sources
    		${jdbctests_sources}
		BaseTestFixture.h
		common/stringutils.h
		resources.h
		regression/EscapeProcessorRegressionTest.h
		compliance/ConnectionTest.h
		compliance/DatabaseMetaDataTest.h
		compliance/StatementTest.h
		compliance/ResultSetMetadataTest.h
		compliance/PreparedStatementTest.h
		simple/BlobTest.h
		regression/BlobRegressionTest.h
		regression/PreparedStatementTest.h
		)
ENDIF(WIN32)

ADD_EXECUTABLE(CJUnitTestsPort ${jdbctests_sources})
TARGET_LINK_LIBRARIES(CJUnitTestsPort ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring C/J junit tests port")
