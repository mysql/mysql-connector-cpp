# Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
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


include_directories("${WITH_UUID}/include")

#
# TODO: Fix XAPI code instead of disabling these warnings
#

set(warning_list)

IF(WIN32)

  list(APPEND warnings_list "/wd4244")
  list(APPEND warnings_list "/wd4267")
  list(APPEND warnings_list "/wd4127")
  list(APPEND warnings_list "/wd4996")
  list(APPEND warnings_list "/wd4702")

ENDIF()

string(REPLACE ";" " " warning_options "${warnings_list}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${warning_options}")


SET(_libmysqlx_cc_src
         crud.cc
         result.cc
         mysqlx.cc
         session.cc
)

ADD_LIBRARY(xapi OBJECT ${_libmysqlx_cc_src})
ADD_COVERAGE(xapi)

ADD_SUBDIRECTORY(tests)

