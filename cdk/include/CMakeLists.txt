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


#
# MySQL CDK Public Headers
#
# see headers.cmake for explanations how project's public
# headers are declared and processed.
#

INCLUDE(headers)

SETUP_HEADERS(${CMAKE_CURRENT_SOURCE_DIR}/mysql)

#
# For generated header
#
HEADER_CHECKS_INCLUDE(${CMAKE_CURRENT_BINARY_DIR})
#
# TODO: Remove dependency on Boost headers
#
HEADER_CHECKS_INCLUDE(${Boost_INCLUDE_DIR})

ADD_HEADERS_DIR(mysql)

IF(cdk_stand_alone)
  ADD_HEADERS_TARGET()
ENDIF()

