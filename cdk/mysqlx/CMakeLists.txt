# Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
#
# This code is licensed under the terms of the GPLv2
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


SET(target_mysqlx ${cdk_target_prefix}mysqlx CACHE INTERNAL "CDK mysqlx target")

#
# TODO: Fix compile warnings in auth_mysql41.cc
#

if(WIN32)
  set_property(SOURCE auth_mysql41.cc
    PROPERTY COMPILE_FLAGS "/W3"
  )
endif()

ADD_LIBRARY(${target_mysqlx} OBJECT session.cc result.cc auth_mysql41.cc)
ADD_COVERAGE(${target_mysqlx})
#TARGET_LINK_LIBRARIES(${target_mysqlx} ${target_proto_mysqlx})


ADD_SUBDIRECTORY(tests)
