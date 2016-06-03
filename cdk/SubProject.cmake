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

SET(CDK_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include;${PROJECT_BINARY_DIR}/include"
    CACHE PATH "CDK include path")
MESSAGE("CDK include path: ${CDK_INCLUDE_DIR}")

#
# Note: CDK public headers depend on Boost ones.
#

MACRO(CDK_SETUP)
  INCLUDE_DIRECTORIES(${CDK_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
  #MESSAGE("Adding CDK include path: ${CDK_INCLUDE_DIR}")
ENDMACRO(CDK_SETUP)

MACRO(ADD_CDK target)
  TARGET_INCLUDE_DIRECTORIES(${target}
    PRIVATE ${CDK_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(${target} cdk)
  MESSAGE("Configured target ${target} for using CDK")
ENDMACRO(ADD_CDK)
