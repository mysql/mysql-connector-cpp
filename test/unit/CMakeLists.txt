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
	LINK_DIRECTORIES(${MYSQL_DIR}/lib/$(ConfigurationName))
	LINK_DIRECTORIES(../framework/$(ConfigurationName))
	SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn libmysql test_framework)
	SET(MY_TARGET_LINK_LIBRARIES_STATIC_CPPLIB mysqlcppconn-static libmysql test_framework)
#	ADD_DEFINITIONS("-D_SECURE_SCL")
	ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")
	ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")

ELSEIF(NOT WIN32)
	SET(MY_TARGET_LINK_LIBRARIES mysqlcppconn mysqlclient_r test_framework)
ENDIF(WIN32)

IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
	SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)

IF(MYSQLCPPCONN_TEST_NOT_IMPLEMENTED)
	ADD_DEFINITIONS("-DINCLUDE_NOT_IMPLEMENTED_METHODS=1")
ENDIF(MYSQLCPPCONN_TEST_NOT_IMPLEMENTED)

MESSAGE(STATUS "Configuring unit tests")

ADD_SUBDIRECTORY(example/)
ADD_SUBDIRECTORY(classes/)
ADD_SUBDIRECTORY(performance/)
ADD_SUBDIRECTORY(bugs/)

# Copy&Paste template: change directory name and uncomment
# ADD_SUBDIRECTORY(template_bug_group)
ADD_SUBDIRECTORY(template_bug_group)
