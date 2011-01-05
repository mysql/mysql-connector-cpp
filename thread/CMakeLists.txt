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


SET(MYSQLCPPCONN_THREAD_SOURCES
	my_pthread.c
	my_wincond.c
	my_winthread.c
	my_thr_init.c
	thr_mutex.c)

ADD_LIBRARY(mysqlcppconn_thread SHARED ${MYSQLCPPCONN_THREAD_SOURCES})
ADD_LIBRARY(mysqlcppconn_thread-static STATIC ${MYSQLCPPCONN_THREAD_SOURCES})

IF(WIN32)
	ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")

	#Disables bunch of pretty useless warnings on win
	ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")


	SET(MYTHREAD_TARGET_LINK_LIBRARIES_DYNAMIC ws2_32)
	SET(MYTHREAD_TARGET_LINK_LIBRARIES_STATIC ws2_32)
ELSEIF(NOT WIN32)

	SET(MYTHREAD_TARGET_LINK_LIBRARIES_DYNAMIC pthread)
	SET(MYTHREAD_TARGET_LINK_LIBRARIES_STATIC pthread)
ENDIF(WIN32)

TARGET_LINK_LIBRARIES(mysqlcppconn_thread ${MYTHREAD_TARGET_LINK_LIBRARIES_DYNAMIC})

TARGET_LINK_LIBRARIES(mysqlcppconn_thread-static ${MYTHREAD_TARGET_LINK_LIBRARIES_STATIC})


SET_TARGET_PROPERTIES(mysqlcppconn_thread
					PROPERTIES SOVERSION "1"
					LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV}"
					COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV} $ENV{CFLAGS}")


SET_TARGET_PROPERTIES(mysqlcppconn_thread-static PROPERTIES
					LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV}"
					COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV} $ENV{CFLAGS}")



IF(WIN32)
	INSTALL(TARGETS mysqlcppconn_thread mysqlcppconn_thread-static
		RUNTIME DESTINATION lib
		ARCHIVE DESTINATION lib
	)
ELSE(WIN32)
	INSTALL(TARGETS mysqlcppconn_thread mysqlcppconn_thread-static
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
	)
ENDIF(WIN32)


MESSAGE(STATUS "Configuring thread")
