# Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
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
#  Configuration for building CDK as part of another cmake project.
#

MESSAGE("Configuring CDK as part of ${CMAKE_PROJECT_NAME} project")

SET(cdk_target_prefix "cdk_")

OPTION(WITH_CDK_TESTS "Build CDK unit tests" ${WITH_TESTS})

#
# Override cached WITH_TESTS value for CDK sub-project
#
SET(WITH_TESTS ${WITH_CDK_TESTS})
MESSAGE("WITH_TESTS: ${WITH_TESTS}")


OPTION(WITH_CDK_DOC "Build CDK documentation" ${WITH_DOC})

#
# Override cached WITH_DOC value for CDK sub-project
#
SET(WITH_DOC ${WITH_CDK_DOC})
#MESSAGE("WITH_DOC: ${WITH_DOC}")

#
# Different default for WITH_NGS_MOCKUP option: build only if testing enabled
#

OPTION(WITH_NGS_MOCKUP "Build CDK's NGS mockup server" ${WITH_TESTS})

#
# Disable public headers checks if CDK is part of another project
#
SET(WITH_HEADER_CHECKS OFF)

#
# Infrastructure for adding CDK to main project targets
#

set(CDK_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
list(APPEND CDK_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/extra/rapidjson/include")
list(APPEND CDK_INCLUDE_DIR "${PROJECT_BINARY_DIR}/include")

SET(CDK_INCLUDE_DIR ${CDK_INCLUDE_DIR} CACHE PATH "CDK include path" FORCE)
MESSAGE("CDK include path: ${CDK_INCLUDE_DIR}")

#
# Note: Currently no extra setup is needed for CDK
#

MACRO(CDK_SETUP)
ENDMACRO(CDK_SETUP)

include(libutils)  # lib_link_libraries()

MACRO(ADD_CDK target)

  TARGET_INCLUDE_DIRECTORIES(${target} PRIVATE
    ${CDK_INCLUDE_DIR}
  )

  LIB_LINK_LIBRARIES(${target} cdk)
  MESSAGE("Configured target ${target} for using CDK")
ENDMACRO(ADD_CDK)
