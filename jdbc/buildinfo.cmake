# Copyright (c) 2018, Oracle and/or its affiliates. All rights reserved.
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

# Try find out information about the build environment that
# might be useful to the user of the C/C++ libraries

set(buildinfo_in  "${PROJECT_SOURCE_DIR}/BUILDINFO.in")
set(buildinfo_out "${PROJECT_BINARY_DIR}/BUILDINFO")

configure_file(
  "${buildinfo_in}"
  "${buildinfo_out}"
  @ONLY
)

# TODO: OpenSSL information

if(WIN32)

  if(STATIC_MSVCRT)
    file(APPEND ${buildinfo_out}
      "MSVC runtime   : linked statically (/MT)\n"
    )
  else()
    file(APPEND ${buildinfo_out}
      "MSVC runtime   : linked dynamically (/MD)\n"
    )
  endif()

else()

  execute_process(
    COMMAND ldd --version
    COMMAND head -1
    ERROR_QUIET
    OUTPUT_VARIABLE _glibc_version
    RESULT_VARIABLE _result_code
  )

  if (_result_code STREQUAL "0")
    string(REGEX REPLACE "ldd *" "" _glibc_version "${_glibc_version}")
    file(APPEND ${buildinfo_out}
      "GLIBC version   : ${_glibc_version}\n"
    )
  endif()

endif()

file(APPEND ${buildinfo_out} "MySQL version  : ${MYSQL_VERSION}\n"
)
  # TODO: Boost version

install(FILES "${buildinfo_out}" DESTINATION ${DOC_DESTINATION} COMPONENT Readme)

