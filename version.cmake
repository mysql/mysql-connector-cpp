# Copyright (c) 2016, 2023, Oracle and/or its affiliates.
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

#
# Connector/C++ version
#

set(CONCPP_VERSION_MAJOR  8 CACHE INTERNAL "version info")
set(CONCPP_VERSION_MINOR  2 CACHE INTERNAL "version info")
set(CONCPP_VERSION_MICRO  0 CACHE INTERNAL "version info")
# Level is "-alpha", "-beta", empty if GA
set(CONCPP_VERSION_LEVEL  "" CACHE INTERNAL "version info")

set(CONCPP_VERSION
  "${CONCPP_VERSION_MAJOR}.${CONCPP_VERSION_MINOR}.${CONCPP_VERSION_MICRO}"
  CACHE INTERNAL "version info"
)

# Note version number format is XYYZZZZ
# TODO: Handle 3-digit versions

if(CONCPP_VERSION_MINOR LESS 10)
  set(PAD_MINOR "0")
else()
  set(PAD_MINOR "")
endif()

if(CONCPP_VERSION_MICRO LESS 10)
  set(PAD_MICRO "000")
else()
  set(PAD_MICRO "00")
endif()

set(CONCPP_VERSION_NUMBER
  "${CONCPP_VERSION_MAJOR}${PAD_MINOR}${CONCPP_VERSION_MINOR}${PAD_MICRO}${CONCPP_VERSION_MICRO}"
  CACHE INTERNAL "version info"
)

#
#  Generate version info
#

if(EXTRA_NAME_SUFFIX STREQUAL "-commercial")
  SET(CONCPP_LICENSE "COMMERCIAL" CACHE INTERNAL "license info")
else()
  SET(CONCPP_LICENSE "GPL-2.0" CACHE INTERNAL "license info")
endif()

#
#  ABI versions
#
# Note: When updating ABI version, a corresponding MYSQLX_ABI_X_Y macro
# needs to be added in include/mysqlx/common/api.h (see comments there).
#

set(ABI_VERSION_MAJOR 2 CACHE INTERNAL "version info")
set(ABI_VERSION_MINOR 1 CACHE INTERNAL "version info")
set(
  ABI_VERSION "${ABI_VERSION_MAJOR}.${ABI_VERSION_MINOR}"
  CACHE INTERNAL "version info"
)

set(JDBC_ABI_VERSION_MAJOR 9 CACHE INTERNAL "version info")
set(JDBC_ABI_VERSION_MINOR 0 CACHE INTERNAL "version info")
set(
  JDBC_ABI_VERSION "${JDBC_ABI_VERSION_MAJOR}.${JDBC_ABI_VERSION_MINOR}"
  CACHE INTERNAL "version info"
)


message(
  "Building version "
  "${CONCPP_VERSION_MAJOR}.${CONCPP_VERSION_MINOR}.${CONCPP_VERSION_MICRO}"
  "${CONCPP_VERSION_LEVEL}"
)
