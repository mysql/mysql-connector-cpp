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

if(NOT DEFINED JDBC_DIR)
  message(FATAL_ERROR "JDBC_DIR not specified")
endif()

if(NOT DEFINED JDBC_INCLUDES)
  message(FATAL_ERROR "JDBC_INCLUDES not specified")
endif()

if(NOT DEFINED CMAKE_GENERATOR)
  message(FATAL_ERROR "CMAKE_GENERATOR not specified")
endif()

if(NOT EXISTS "${JDBC_DIR}/install/include/jdbc/mysql_driver.h")
  message(FATAL_ERROR "JDBC headers could not be found at ${JDBC_DIR}/install/include/jdbc. Have you run build_jdbc target?")
endif()

if(NOT EXISTS "${HEADERS_DIR}")
  message(FATAL_ERROR "Location of header checks infrastructure not specified with HEADERS_DIR or it could not be found there (${HEADERS_DIR})")
endif()

if(NOT DEFINED CHECK_DIR)
  set(CHECK_DIR "${JDBC_DIR}/headers_check")
endif()

if(NOT EXISTS "${CHECK_DIR}")
  file(MAKE_DIRECTORY "${CHECK_DIR}")
endif()


message("Checking JDBC public headers in: ${JDBC_DIR}")
message("configuring header checks in: ${CHECK_DIR}")

#
# Generate CMakeLists.txt for the check project.
#

set(hdr_include_dir ${JDBC_INCLUDES})

configure_file(${HEADERS_DIR}/check.cmake.in
               ${CHECK_DIR}/CMakeLists.txt @ONLY)

#
# Collect all headers and generate check sources for each of them.
#

file(GLOB headers "${JDBC_DIR}/install/include/jdbc/*.h")
set(all_headers ${headers})
set(checks)

foreach(hdr ${headers})

  get_filename_component(HEADERN "${hdr}" NAME_WE)
  #message("processing header: ${HEADERN}.h")

  set(HEADER "jdbc/${HEADERN}.h")

  configure_file(${HEADERS_DIR}/check.source.in
                 ${CHECK_DIR}/${HEADERN}.cc @ONLY)

  list(APPEND checks ${HEADERN}.cc)

endforeach()

file(APPEND "${CHECK_DIR}/CMakeLists.txt"
  "add_library(check_jdbc STATIC ${checks})\n"
)


file(GLOB headers "${JDBC_DIR}/install/include/jdbc/cppconn/*.h")
list(APPEND all_headers ${headers})
set(checks)

foreach(hdr ${headers})

  get_filename_component(HEADERN "${hdr}" NAME_WE)
  #message("processing header: cppconn/${HEADERN}.h")

  set(HEADER "jdbc/cppconn/${HEADERN}.h")

  configure_file(${HEADERS_DIR}/check.source.in
                 ${CHECK_DIR}/cppconn_${HEADERN}.cc @ONLY)

  list(APPEND checks cppconn_${HEADERN}.cc)

endforeach()

file(APPEND "${CHECK_DIR}/CMakeLists.txt"
  "add_library(check_jdbc_cppconn STATIC ${checks})\n"
)

#
# Configure check project with cmake and then build it.
#

message("Configuring JDBC header checks using cmake generator: ${CMAKE_GENERATOR}")

execute_process(
  COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  WORKING_DIRECTORY ${CHECK_DIR}
)

message("Running header checks")

execute_process(
  COMMAND ${CMAKE_COMMAND} --build . --clean-first
  WORKING_DIRECTORY ${CHECK_DIR}
)


