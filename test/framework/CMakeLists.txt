#
# Copyright (c) 2008, 2011, Oracle and/or its affiliates. All rights reserved.
#
# The MySQL Connector/C++ is licensed under the terms of the GPLv2
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
# MySQL Connectors. There are special exceptions to the terms and
# conditions of the GPLv2 as it is applied to this software, see the
# FLOSS License Exception
# <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published
# by the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
#


INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR})
INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR}/cppconn)

IF(WIN32)
#	LINK_DIRECTORIES(${MYSQL_DIR}/lib/$(ConfigurationName))
#	ADD_DEFINITIONS("-D_SECURE_SCL")
	ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")
	ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")

ELSEIF(NOT WIN32)
ENDIF(WIN32)

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
					COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV}")

MESSAGE(STATUS "Configuring tests framework lib")
