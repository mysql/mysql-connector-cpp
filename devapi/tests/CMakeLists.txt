# Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
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
# Undefine macros which indicate building of the connector library
# (as opposed to using it from test code). This is used by public headers.
#

remove_definitions(-DCONCPP_BUILD_STATIC)
remove_definitions(-DCONCPP_BUILD_SHARED)

#
# If linking with connector statically, define macro that indicates this
# to the public headers.
#

if(BUILD_STATIC)
  add_definitions(-DSTATIC_CONCPP)
endif()

ADD_NG_TEST(devapi-t
  first-t.cc crud-t.cc types-t.cc batch-t.cc ddl-t.cc session-t.cc
)
