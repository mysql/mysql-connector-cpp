# Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


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
         row.cc
         mysqlx.cc
         session.cc
         schema.cc
         collection.cc
         table.cc
         view.cc
)

ADD_LIBRARY(xapi OBJECT ${_libmysqlx_cc_src})
ADD_COVERAGE(xapi)

ADD_SUBDIRECTORY(tests)

