# Copyright (c) 2016, 2018, Oracle and/or its affiliates. All rights reserved.
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
set(CONCPP_VERSION_MINOR  0 CACHE INTERNAL "version info")
set(CONCPP_VERSION_MICRO 15 CACHE INTERNAL "version info")
# Level is "-alpha", "-beta", empty if GA
set(CONCPP_VERSION_LEVEL  "" CACHE INTERNAL "version info")

set(CONCPP_VERSION
  "${CONCPP_VERSION_MAJOR}.${CONCPP_VERSION_MINOR}.${CONCPP_VERSION_MICRO}"
  CACHE INTERNAL "version info"
)

#
#  ABI versions
#

set(ABI_VERSION_MAJOR 1 CACHE INTERNAL "version info")
set(ABI_VERSION_MINOR 0 CACHE INTERNAL "version info")
set(
  ABI_VERSION "${ABI_VERSION_MAJOR}.${ABI_VERSION_MINOR}"
  CACHE INTERNAL "version info"
)

set(JDBC_ABI_VERSION_MAJOR 7 CACHE INTERNAL "version info")
set(JDBC_ABI_VERSION_MINOR 0 CACHE INTERNAL "version info")
set(
  JDBC_ABI_VERSION "${JDBC_ABI_VERSION_MAJOR}.${JDBC_ABI_VERSION_MINOR}"
  CACHE INTERNAL "version info"
)

# **** IMPORTANT ****
#
# The code below needs to be replaced when moving from one version
# series to another. I.e. when moving from 4.0 to 4.1, from 4.13 to
# 5.0 and so on.
#
# You DON'T change this code for patchlevel version changes, i.e.
# when only the third part of the version is changed.
#
# You can use any GUID generator that produces random GUID codes. You
# can also or invent a code of your own if you follow the syntax rules.

set(CONCPP_MINORMAJOR_UPGRADE_CODE "a1195164-bc2d-45fb-a5e5-1ba834771ce8")

set(CONCPP_PACKAGE_BASE_VERSION
    "${CONCPP_VERSION_MAJOR}.${CONCPP_VERSION_MINOR}")
set(CONCPP_PACKAGE_NUMERIC_VERSION
    "${CONCPP_PACKAGE_BASE_VERSION}.${CONCPP_VERSION_MICRO}")
set(CONCPP_PACKAGE_VERSION
    "${CONCPP_PACKAGE_NUMERIC_VERSION}${CONCPP_VERSION_LEVEL}")

message("Building version ${CONCPP_PACKAGE_VERSION}")
