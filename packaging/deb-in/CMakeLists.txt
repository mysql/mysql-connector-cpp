# Copyright (c) 2018, 2024, Oracle and/or its affiliates.
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

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message("Skipping DEB package configuration: not a Linux system")
  return()
endif()

find_program(LSB_RELEASE lsb_release)
if(NOT LSB_RELEASE)
  message("Skipping DEB package configuration: lsb_release was not found")
  return()
endif()


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

# Determine multiarch library install location

execute_process(
  COMMAND dpkg-architecture -q DEB_TARGET_MULTIARCH
  OUTPUT_VARIABLE DEB_TARGET_MULTIARCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  RESULT_VARIABLE res
)

if(res)
  message(FATAL_ERROR "Failed to execute dpkg-architecture")
endif()

set(LIB_DIR "/usr/lib/${DEB_TARGET_MULTIARCH}")
message(STATUS "multiarch lib location: ${LIB_DIR}")


#Follow MYSQLCLIENT_STATIC_LINKING option on packaging
if(DEFINED MYSQLCLIENT_STATIC_LINKING)
  set(DEB_CMAKE_EXTRAS
    "${DEB_CMAKE_EXTRAS} -DMYSQLCLIENT_STATIC_LINKING=${MYSQLCLIENT_STATIC_LINKING}"
  )
endif()

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
  SET (DEB_CONTRIBUTING_STAGE "")
ELSE()
  SET (DEB_COPYRIGHT_UPSTREAMNAME "MySQL ${MYSQL_BASE_VERSION}")
  SET (DEB_PRODUCTNAME "")
  SET (DEB_LICENSENAME "GPL")
  SET (DEB_VERSION "-1")
  SET (DEB_SERVERPRODUCT "community")
  SET (DEB_CONTRIBUTING_STAGE "\tinstall CONTRIBUTING.md debian/tmp/usr/CONTRIBUTING.md")
ENDIF()

# All files are configured and copied to the debian/ directory, which is used
# by debuild to make the packages

SET (DEB_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/debian)

foreach(CF changelog control rules compat copyright)
  set(src "${DEB_ROOT}/${CF}")
  set(dst "${CMAKE_BINARY_DIR}/debian/${CF}")
  # message(STATUS "Preparing: ${dst}")
  if(EXISTS "${src}")
    configure_file("${src}" "${dst}" COPYONLY)
  elseif(EXISTS "${src}.in")
    configure_file("${src}.in" "${dst}" @ONLY)
  else()
    message(SEND_ERROR "Control file not found: ${CF}")
  endif()
endforeach()

set(pkg_xdevapi "libmysqlcppconnx${ABI_VERSION_MAJOR}")
set(pkg_jdbc   "libmysqlcppconn${JDBC_ABI_VERSION_MAJOR}")
set(pkg_devel  "libmysqlcppconn-dev")

foreach(pkg xdevapi jdbc devel)
  foreach(ext .install .lintian-overrides -dbgsym.install .postinst)
    set(PACKAGE_BASE_NAME "${pkg_${pkg}}")
    set(src "${DEB_ROOT}/${pkg}${ext}.in")
    set(dst "${CMAKE_BINARY_DIR}/debian/${PACKAGE_BASE_NAME}${ext}")
    if(DEB_PRODUCT STREQUAL "commercial")
      set(DEB_CONTRIBUTING_DOC_ENTRY "")
    elseif(ext STREQUAL ".install")
      set(DEB_CONTRIBUTING_DOC_ENTRY "usr/CONTRIBUTING.md /usr/share/doc/${PACKAGE_BASE_NAME}")
    elseif(ext STREQUAL "-dbgsym.install")
      set(DEB_CONTRIBUTING_DOC_ENTRY "usr/CONTRIBUTING.md /usr/share/doc/${PACKAGE_BASE_NAME}-dbgsym")
    else()
      set(DEB_CONTRIBUTING_DOC_ENTRY "")
    endif()
    if(EXISTS "${src}")
      # message(STATUS "Preparing: ${dst}, from: ${src}")
      configure_file("${src}" "${dst}" @ONLY)
    endif()
  endforeach()
endforeach()

EXECUTE_PROCESS(
  COMMAND chmod +x ${CMAKE_BINARY_DIR}/debian/rules
)
