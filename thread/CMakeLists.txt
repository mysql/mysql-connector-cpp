# Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0, as
# published by the Free Software Foundation.
#
# This program is also distributed with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an
# additional permission to link the program and your derivative works
# with the separately licensed software that they have included with
# MySQL.
#
# Without limiting anything contained in the foregoing, this file,
# which is part of MySQL Connector/C++, is also subject to the
# Universal FOSS Exception, version 1.0, a copy of which can be found at
# http://oss.oracle.com/licenses/universal-foss-exception.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA


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
          COMPONENT JDBCDev
  )
ELSE(WIN32)
  INSTALL(TARGETS mysqlcppconn_thread mysqlcppconn_thread-static
          LIBRARY DESTINATION ${INSTALL_LIBDIR}
          ARCHIVE DESTINATION ${INSTALL_LIBDIR}
          COMPONENT JDBCDev
  )
ENDIF(WIN32)


MESSAGE(STATUS "Configuring thread")
