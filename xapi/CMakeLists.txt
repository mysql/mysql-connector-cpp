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


SET(_libmysqlx_cc_src
         crud.cc
         result.cc
         row.cc
         mysqlx.cc
         compat.cc
         session.cc
)

ADD_LIBRARY(xapi OBJECT ${_libmysqlx_cc_src})

lib_interface_link_libraries(xapi cdk)

ADD_SUBDIRECTORY(tests)

