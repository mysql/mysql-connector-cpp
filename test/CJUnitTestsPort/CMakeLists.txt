#   Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.
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

IF(EXISTS ${MYSQLCPPCONN_SOURCE_DIR}/test/CJUnitTestsPort/MiniCppUnit)

INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR})
INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR}/cppconn)
INCLUDE(CheckSymbolExists)
INCLUDE(CheckFunctionExists)

# no need to look for libmysql - working with C/C++ only!
#INCLUDE(${CMAKE_SOURCE_DIR}/FindMySQL.cmake)

IF(WIN32)
	LINK_DIRECTORIES(${MYSQL_DIR}/lib/$(ConfigurationName))
	SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn libmysql)
#	ADD_DEFINITIONS("-D_SECURE_SCL")
	ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")
	ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")

ELSEIF(NOT WIN32)
	SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn mysqlclient_r)
ENDIF(WIN32)

CHECK_FUNCTION_EXISTS("isinf" HAVE_ISINF)
CHECK_SYMBOL_EXISTS("isinf" "ieeefp.h" HAVE_ISINF_IN_IEEEFP_H)
CHECK_SYMBOL_EXISTS("finite" "ieeefp.h" HAVE_FINITE_IN_IEEEFP_H)

IF(HAVE_ISINF)
	IF(CMAKE_SYSTEM MATCHES "^AIX")
		MESSAGE(STATUS "Compiling isinf test program on AIX")
		INCLUDE (CheckCXXSourceCompiles)
		CHECK_CXX_SOURCE_COMPILES("_check_isinf_cpp.cpp" HAVE_ISINF_IN_STD)
		IF(HAVE_ISINF_IN_STD)
			MESSAGE(STATUS "std::isinf available on AIX")
			ADD_DEFINITIONS("-DHAVE_ISINF")
		ELSEIF(NOT HAVE_ISINF_IN_STD)
			MESSAGE(STATUS "using C isinf on AIX")
			ADD_DEFINITIONS("-DHAVE_ISINF_IN_MATH")
		ENDIF(HAVE_ISINF_IN_STD)
	ELSEIF(NOT CMAKE_SYSTEM EQUAL "AIX")
		MESSAGE(STATUS "isinf found")
		ADD_DEFINITIONS("-DHAVE_ISINF")
	ENDIF(CMAKE_SYSTEM MATCHES "^AIX")
ENDIF(HAVE_ISINF)
IF(HAVE_ISINF_IN_IEEEFP_H)
	MESSAGE(STATUS "isinf found in ieeefp.h")
	ADD_DEFINITIONS("-DHAVE_ISINF_IN_IEEEFP_H")
ENDIF(HAVE_ISINF_IN_IEEEFP_H)
IF(HAVE_FINITE_IN_IEEEFP_H)
	MESSAGE(STATUS "finite found in ieeefp.h")
	ADD_DEFINITIONS("-DHAVE_FINITE_IN_IEEEFP_H")
ENDIF(HAVE_FINITE_IN_IEEEFP_H)

IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
	SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)

SET(jdbctests_sources
	BaseTestCase.h
	BaseTestCase.inl
	BaseTestCase.cpp
	MiniCppUnit/TestsRunner.cxx
	MiniCppUnit/MiniCppUnit.cxx
	../../common/stringutils.cpp
	../../common/stringutils.h
	resources.cpp
	resources.h
	regression/EscapeProcessorRegressionTest.cpp
	regression/EscapeProcessorRegressionTest.h
	compliance/ConnectionTest.h
	compliance/ConnectionTest.cpp
	compliance/DatabaseMetaDataTest.h
	compliance/DatabaseMetaDataTest.cpp
	compliance/StatementTest.h
	compliance/StatementTest.cpp
	compliance/ResultSetMetadataTest.h
	compliance/ResultSetMetadataTest.cpp
	compliance/PreparedStatementTest.h
	compliance/PreparedStatementTest.cpp
	)
#	simple/ResultSetTest.cpp
#	simple/ResultSetTest.h


ADD_EXECUTABLE(CJUnitTestsPort ${jdbctests_sources})
TARGET_LINK_LIBRARIES(CJUnitTestsPort ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

#
MESSAGE(STATUS "Configuring C/J junit tests port")


ENDIF(EXISTS ${MYSQLCPPCONN_SOURCE_DIR}/test/CJUnitTestsPort/MiniCppUnit)
