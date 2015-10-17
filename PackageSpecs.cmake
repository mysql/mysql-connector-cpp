# Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; version 2 of the
# License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301  USA

#
# Specifications for Connector/C++ binary and source packages
#
# TODO
# - Generate HTML docs with Doxygen and include in bin packages
#
# Note: CPACK_XXX variables must be set before include(CPack)
#

set(CPACK_PACKAGE_NAME "mysql-connector-c++")
set(CPACK_PACKAGE_VENDOR "Oracle Corporation")
set(CPACK_PACKAGE_VERSION "2.0.1_preview")

if(WIN32)
  set(CPACK_GENERATOR ZIP)
  set(CPACK_SOURCE_GENERATOR ZIP)
else()
  set(CPACK_GENERATOR TGZ)
  set(CPACK_SOURCE_GENERATOR TGZ)
endif()

# TODO: Decide on installation layout
#set(CPACK_PACKAGE_INSTALL_DIRECTORY "mysql/connector-cpp/2.0")

#
# Architecture tag
#
# TODO: Cover OSX and other architectures we suport
#
# TODO: Decide on proper tagging for Windows: VS version, static/dynamic
# runtime, debug/non-debug etc. Note: some of these differences can/should
# be covered by having several variants of the library added to the package.
#

set(CPACK_SYSTEM_NAME "?")

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(CPACK_SYSTEM_NAME "linux-${CMAKE_SYSTEM_PROCESSOR}")
endif()

if(WIN32)
  set(CPACK_SYSTEM_NAME "winXX")
endif()

if(APPLE)

  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "To create packages for OSX, build with clang compiler.")
  endif()

  if(NOT DEFINED ENV{MACOSX_DEPLOYMENT_TARGET})
    message(FATAL_ERROR "To create packages on OSX, set deployment target"
            " using MACOSX_DEPLOYMENT_TARGET environment variable")
  endif()

  set(osx_version $ENV{MACOSX_DEPLOYMENT_TARGET})
  set(CPACK_SYSTEM_NAME "osx${osx_version}-${CMAKE_SYSTEM_PROCESSOR}")

endif()

message("Binary package name: ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")

#
# Specs for source package
#

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-src")

#
# note: Using regex patterns for CPACK_SOURCE_IGNORE_FILES is fragile because
# they are matched against the full path which can vary depending on where the
# build takes place. Unfortunatelly, I (Rafal) could not find any other mechanism
# for specifying what source files should be excluded from the source package.
#
# note: Double escaping required to get correct pattern string (with single
# escapes) in CPackSourceConfig.cmake
#

list(APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.git.*")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/jenkins/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CTestConfig.cmake")
list(APPEND CPACK_SOURCE_IGNORE_FILES "PackageSpecs.cmake")

include(CPack)

#
# Custom target to build packages.
#

add_custom_target(build_packages
  COMMAND cpack --config CPackConfig.cmake --verbose -C $<CONFIGURATION>
  COMMAND cpack --config CPackSourceConfig.cmake
  DEPENDS clean_source_tree
)

add_custom_target(clean_source_tree
  COMMAND git clean -x -d -f
  COMMAND git submodule foreach --recursive git clean -x -d -f
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  COMMENT "Cleaning source tree"
)

set_property(TARGET clean_source_tree build_packages
  PROPERTY EXCLUDE_FROM_ALL 1
)

set_property(TARGET clean_source_tree build_packages
  PROPERTY EXCLUDE_FROM_DEFAULT_BUILD 1
)
