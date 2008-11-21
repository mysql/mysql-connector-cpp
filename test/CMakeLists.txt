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

INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR})
IF(WIN32)
	IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
		LINK_DIRECTORIES(${MYSQL_DIR}/lib/debug)
	ELSEIF(CMAKE_BUILD_TYPE STREQUAL "")
		LINK_DIRECTORIES(${MYSQL_DIR}/lib/opt)
	ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

	ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")
	ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")

	SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn libmysql)
ELSEIF(NOT WIN32)
	SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn mysqlclient_r)
ENDIF(WIN32)

IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
	SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)

ADD_EXECUTABLE(static_test static_test.cpp)
TARGET_LINK_LIBRARIES(static_test ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})



#INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR}/cppconn)
LINK_DIRECTORIES(${GLIB_DIR}/lib)
ADD_EXECUTABLE(driver_test driver_test.cpp)
TARGET_LINK_LIBRARIES(driver_test ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})


MESSAGE(STATUS "Configuring test cases")
