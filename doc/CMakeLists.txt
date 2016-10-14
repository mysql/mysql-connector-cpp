# Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
#
# The MySQL Connector/C++ is licensed under the terms of the GPLv2
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
# Generate project documentation using Doxygen.
#

IF(NOT DEFINED WITH_DOC)
  OPTION(WITH_DOC "Build Project's documentation" OFF)
ENDIF()

IF(WITH_DOC)

set(WITH_DOC OFF)

# Find Doxygen

if(DEFINED WITH_DOXYGEN)

  find_program(DOXYGEN_EXECUTABLE
    NAMES doxygen
    PATHS ${WITH_DOXYGEN}
    DOC "Doxygen documentation generation tool"
  )

  if(NOT DOXYGEN_EXECUTABLE)
    message(WARNING "Could not find Doxygen at specified location: ${WITH_DOXYGEN},"
            " will not build documentation")
    return()
  endif()

  set(DOXGEN_FOUND YES)
  execute_process(COMMAND ${DOXYGEN_EXECUTABLE} "--version" OUTPUT_VARIABLE DOXYGEN_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

else()

  find_package(Doxygen)

  if(NOT DOXYGEN_FOUND)
    message(WARNING "Could not find Doxygen, will not build documentation."
            " Custom Doxygen location can be specified using WITH_DOXYGEN option")
    set(WITH_DOXYGEN "" CACHE PATH "Doxygen install location")
    return()
  endif()

endif()

set(WITH_DOC ON)
MESSAGE("Building C/C++ documentation")


#
# TODO: remove conflict with CDK's build_docs target
#

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
configure_file(doxygen.cfg.in doxygen.cfg @ONLY)
configure_file(DoxygenLayout.xml.in DoxygenLayout.xml @ONLY)

set(OUTPUT_DIRECTORY "$(OUTPUT_DIRECTORY)")
configure_file(doxygen.cfg.in ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cfg @ONLY)
configure_file(DoxygenLayout.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/DoxygenLayout.xml @ONLY)

FILE(GLOB sources *.txt)
LIST(APPEND sources doxygen.cfg.in)


ADD_CUSTOM_TARGET(build_docs
  COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cfg
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Building project documentation"
  SOURCES ${sources}
)

ENDIF(WITH_DOC)
