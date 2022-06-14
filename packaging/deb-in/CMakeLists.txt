# Copyright (c) 2018, 2022, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0,
# as published by the Free Software Foundation.
#
# This program is also distributed with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation.  The authors of MySQL hereby grant you an additional
# permission to link the program and your derivative works with the
# separately licensed software that they have included with MySQL.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License, version 2.0, for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

IF(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  RETURN()
ENDIF()
IF (NOT DEFINED DEB_CODENAME)
  execute_process(
    COMMAND lsb_release -cs
    OUTPUT_VARIABLE DEB_CODENAME
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  SET (DEB_CODENAME ${DEB_CODENAME} CACHE STRING "")
ENDIF()

# debian, ubuntu
IF (NOT DEFINED DEB_ID)
  execute_process(
    COMMAND lsb_release -is
    OUTPUT_VARIABLE DEB_ID
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  SET (DEB_ID ${DEB_ID} CACHE STRING "")
ENDIF()

# 22.04, 11.x, etc
IF (NOT DEFINED DEB_RELEASE)
  execute_process(
    COMMAND lsb_release -rs
    OUTPUT_VARIABLE DEB_RELEASE
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  SET (DEB_RELEASE ${DEB_RELEASE} CACHE STRING "")
ENDIF()

# Change uppercase first letter of Ubuntu/Debian
string(TOLOWER "${DEB_ID}" DEB_ID)

# lsb_release on Debian reports the full version number, e.g. 9.9, and we just want the major version
IF(DEB_ID STREQUAL "debian")
  string(REGEX REPLACE "\\..*" "" DEB_RELEASE "${DEB_RELEASE}")
ENDIF()

# ubuntu22.04, debian11, etc
SET (DEB_PLATFORMRELEASE "${DEB_ID}${DEB_RELEASE}")

IF(DEB_CODENAME STREQUAL "sid")
  IF (DEFINED DEB_GCC_SNAPSHOT)
    SET (DEB_CMAKE_EXTRAS "${DEB_CMAKE_EXTRAS} -DCMAKE_C_COMPILER=/usr/lib/gcc-snapshot/bin/gcc -DCMAKE_CXX_COMPILER=/usr/lib/gcc-snapshot/bin/g++ -DMYSQL_MAINTAINER_MODE=0 -DCMAKE_CXX_COMPILER_LAUNCHER=ccache")
  ENDIF()
  SET (DEB_PLATFORMRELEASE "debianunstable")
ENDIF()

# Timestamp for use in debian/changelog
IF (NOT DEFINED DEB_CHANGELOG_TIMESTAMP)
  execute_process(
    COMMAND date --rfc-2822
    OUTPUT_VARIABLE DEB_CHANGELOG_TIMESTAMP
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  SET (DEB_CHANGELOG_TIMESTAMP ${DEB_CHANGELOG_TIMESTAMP} CACHE STRING "")
ENDIF()

# Commercial or community
IF (DEB_PRODUCT STREQUAL "commercial")
  SET (DEB_COPYRIGHT_UPSTREAMNAME "MySQL Commercial ${MYSQL_BASE_VERSION}")
  SET (DEB_PRODUCTNAME "-commercial")
  SET (DEB_LICENSENAME "Commercial")
  SET (DEB_VERSION "+commercial-1")
  SET (DEB_SERVERPRODUCT "commercial")
ELSE()
  SET (DEB_COPYRIGHT_UPSTREAMNAME "MySQL ${MYSQL_BASE_VERSION}")
  SET (DEB_PRODUCTNAME "")
  SET (DEB_LICENSENAME "GPL")
  SET (DEB_VERSION "-1")
  SET (DEB_SERVERPRODUCT "community")
ENDIF()

# All files are configured and copied to the debian/ directory, which is used
# by debuild to make the packages
SET (DEB_ROOT ${CMAKE_SOURCE_DIR}/packaging/deb-in)
FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/debian)
FILE(GLOB_RECURSE SOURCEFILES RELATIVE "${DEB_ROOT}" "${DEB_ROOT}/*")
# Skip the copyright file from the "other" product type
LIST(REMOVE_ITEM SOURCEFILES "copyright.${DEB_REMOVEPATTERN}")
LIST(REMOVE_ITEM SOURCEFILES "CMakeLists.txt")
FOREACH (SOURCEFILE ${SOURCEFILES})
  STRING(REGEX REPLACE "-packagesource-" "${DEB_PRODUCTNAME}-"
    TMPFILE ${SOURCEFILE})
  # Strip away the input file endings from the destination filenames
  STRING(REGEX REPLACE ".in$" "" DESTFILE ${TMPFILE})
  CONFIGURE_FILE("${DEB_ROOT}/${SOURCEFILE}"
    "${CMAKE_BINARY_DIR}/debian/${DESTFILE}" @ONLY)
ENDFOREACH()
EXECUTE_PROCESS(
  COMMAND chmod +x ${CMAKE_BINARY_DIR}/debian/rules
)

