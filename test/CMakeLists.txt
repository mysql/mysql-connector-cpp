#   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPL
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPL as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR})
IF(WIN32)
	IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
		LINK_DIRECTORIES(${MYSQL_DIR}/lib/debug)
	ELSEIF(CMAKE_BUILD_TYPE STREQUAL "")
		LINK_DIRECTORIES(${MYSQL_DIR}/lib/opt)
	ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

	ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")
	ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")

	SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn libmysql ${MYSQLCPPCONN_STLPORT_LIB})
ELSEIF(NOT WIN32)
	SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn mysqlclient_r ${MYSQLCPPCONN_STLPORT_LIB})
ENDIF(WIN32)

IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
	SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)

ADD_EXECUTABLE(static_test static_test.cpp)

SET_TARGET_PROPERTIES(static_test PROPERTIES
					LINK_FLAGS "${MYSQL_LINK_FLAGS} $ENV{LDFLAGS}"
					COMPILE_FLAGS "$ENV{CPPFLAGS} ${MYSQL_CXXFLAGS} $ENV{CXXFLAGS}")


TARGET_LINK_LIBRARIES(static_test ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})


LINK_DIRECTORIES(${GLIB_DIR}/lib)
ADD_EXECUTABLE(driver_test driver_test.cpp)

SET_TARGET_PROPERTIES(driver_test PROPERTIES
					LINK_FLAGS "${MYSQL_LINK_FLAGS} $ENV{LDFLAGS}"
					COMPILE_FLAGS "$ENV{CPPFLAGS} ${MYSQL_CXXFLAGS} $ENV{CXXFLAGS}")

TARGET_LINK_LIBRARIES(driver_test ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})


MESSAGE(STATUS "Configuring test cases")
