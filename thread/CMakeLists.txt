
#   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPL
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPL as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

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
					LINK_FLAGS "$ENV{LDFLAGS}"
					COMPILE_FLAGS "$ENV{CFLAGS} $ENV{CFLAGS} $ENV{CXXFLAGS}")


SET_TARGET_PROPERTIES(mysqlcppconn_thread-static PROPERTIES
					LINK_FLAGS "$ENV{LDFLAGS}"
					COMPILE_FLAGS "$ENV{CFLAGS} $ENV{CFLAGS} $ENV{CXXFLAGS}")



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
