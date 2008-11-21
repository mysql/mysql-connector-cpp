
IF(EXISTS MiniCppUnit)

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
	ADD_DEFINITIONS("-DHAVE_ISINF")
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
#	MiniCppUnit/TestsRunner.cxx
#	MiniCppUnit/MiniCppUnit.cxx
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


ENDIF(EXISTS MiniCppUnit)
