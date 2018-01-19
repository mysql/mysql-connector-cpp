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


SET(bug123_sources
  ../unit_fixture.cpp
  ../main.cpp
  bug123.cpp)

IF(WIN32)
  SET(bug123_sources
        ${bug123_sources}
  ../unit_fixture.h
  bug123.h)
ENDIF(WIN32)

ADD_EXECUTABLE(bug123 ${bug123_sources})
SET_TARGET_PROPERTIES(bug123 PROPERTIES
          OUTPUT_NAME "bug123"
          LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV} ${MYSQL_LINK_FLAGS}"
          COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV}")

TARGET_LINK_LIBRARIES(bug123 ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

#
# End of the instructions for building binary bug123 from bug123.cpp|h
#

#
# Instructions for compiling bug456.cpp|h into the binary bug456
#

SET(bug456_sources
  ../unit_fixture.cpp
  ../main.cpp
  bug456.cpp)

IF(WIN32)
  SET(bug456_sources
        ${bug456_sources}
  ../unit_fixture.h
  bug456.h)
ENDIF(WIN32)

ADD_EXECUTABLE(bug456 ${bug456_sources})
SET_TARGET_PROPERTIES(bug456 PROPERTIES
          OUTPUT_NAME "bug456"
          LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV} ${MYSQL_LINK_FLAGS}"
          COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV}")
TARGET_LINK_LIBRARIES(bug456 ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring unit tests - group template_bug")
