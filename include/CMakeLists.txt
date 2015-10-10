# Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; version 2 of the
# License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301  USA


#
# MySQL Connector/C++ Public Headers
#
# see headers.cmake for explanations how project's public
# headers are declared and processed.
#

INCLUDE(headers)

SETUP_HEADERS(${CMAKE_CURRENT_SOURCE_DIR})

SET(headers mysqlx.h common.h result.h task.h document.h)

ADD_HEADERS(${headers})

#IF(cdk_stand_alone)
  ADD_HEADERS_TARGET()
#ENDIF()

INSTALL(FILES ${headers} DESTINATION include)

