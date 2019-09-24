# Copyright (c) 2014, 2019, Oracle and/or its affiliates. All rights reserved.
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


SET(CONNECTOR_MAJOR "1")
SET(CONNECTOR_MINOR "1")
SET(CONNECTOR_PATCH "13")
SET(CONNECTOR_LEVEL "")     # "-alpha", "-beta", empty if GA
SET(CONNECTOR_QUALITY "GA")

# Bump this every time we change the API/ABI
SET(MYSQLCPPCONN_SOVERSION "7")

IF(CONNECTOR_MINOR LESS 10)
        SET(CONNECTOR_MINOR_PADDED "0${CONNECTOR_MINOR}")
ELSE(CONNECTOR_MINOR LESS 10)
        SET(CONNECTOR_MINOR_PADDED "${CONNECTOR_MINOR}")
ENDIF(CONNECTOR_MINOR LESS 10)

# If driver survives 100th patch this has to be changed
IF(CONNECTOR_PATCH LESS 10)
        SET(CONNECTOR_PATCH_PADDED "000${CONNECTOR_PATCH}")
ELSE(CONNECTOR_PATCH LESS 10)
        SET(CONNECTOR_PATCH_PADDED "00${CONNECTOR_PATCH}")
ENDIF(CONNECTOR_PATCH LESS 10)

SET(CONNECTOR_BASE_VERSION    "${CONNECTOR_MAJOR}.${CONNECTOR_MINOR}")
SET(CONNECTOR_BASE_PREVIOUS   "1.0")
SET(CONNECTOR_NUMERIC_VERSION "${CONNECTOR_BASE_VERSION}.${CONNECTOR_PATCH}")
SET(CONNECTOR_VERSION         "${CONNECTOR_NUMERIC_VERSION}${CONNECTOR_LEVEL}")

SET(CPACK_PACKAGE_VERSION_MAJOR ${CONNECTOR_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${CONNECTOR_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${CONNECTOR_PATCH})
SET(CPACK_PACKAGE_RELEASE_TYPE "")

# Needed for CPACK configuraiton, used for file name construction. Thus leaving it as is
IF(EXTRA_VERSION)
  SET(MYSQLCPPCONN_VERSION "${CONNECTOR_NUMERIC_VERSION}${EXTRA_VERSION}${CPACK_PACKAGE_RELEASE_TYPE}")
ELSE(EXTRA_VERSION)
  SET(MYSQLCPPCONN_VERSION "${CONNECTOR_NUMERIC_VERSION}${CPACK_PACKAGE_RELEASE_TYPE}")
ENDIF(EXTRA_VERSION)

