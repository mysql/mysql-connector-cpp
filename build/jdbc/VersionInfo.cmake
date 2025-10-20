# Copyright (c) 2014, 2024, Oracle and/or its affiliates.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0, as
# published by the Free Software Foundation.
#
# This program is designed to work with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms, as
# designated in a particular file or component or in included license
# documentation. The authors of MySQL hereby grant you an additional
# permission to link the program and your derivative works with the
# separately licensed software that they have either included with
# the program or referenced in the documentation.
#
# Without limiting anything contained in the foregoing, this file,
# which is part of Connector/C++, is also subject to the
# Universal FOSS Exception, version 1.0, a copy of which can be found at
# https://oss.oracle.com/licenses/universal-foss-exception.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA


SET(CONCPP_VERSION "9.4.0")
SET(CONCPP_VERSION_NUMBER 9040000)
SET(CONNECTOR_MAJOR "9")
SET(CONNECTOR_MINOR "4")
SET(CONNECTOR_PATCH "0")
SET(CONNECTOR_LEVEL "")
SET(CONNECTOR_QUALITY "GA")

# Bump this every time we change the API/ABI
SET(MYSQLCPPCONN_SOVERSION "10")


SET(CONNECTOR_BASE_VERSION    "${CONNECTOR_MAJOR}.${CONNECTOR_MINOR}")
SET(CONNECTOR_BASE_PREVIOUS   "1.0")
SET(CONNECTOR_NUMERIC_VERSION "${CONNECTOR_BASE_VERSION}.${CONNECTOR_PATCH}")
SET(CONNECTOR_VERSION         "${CONNECTOR_NUMERIC_VERSION}${CONNECTOR_LEVEL}")
