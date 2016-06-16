# Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
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
# Specifications for Connector/C++ binary and source packages
#
# TODO
# - Generate HTML docs with Doxygen and include in bin packages
#
# Note: CPACK_XXX variables must be set before include(CPack)
#

# ======================================================================
# Set some initial CPack variables
# ======================================================================

set(CPACK_PACKAGE_NAME    "mysql-connector-c++")
set(CPACK_PACKAGE_VERSION "${CONCPP_PACKAGE_VERSION}")
set(CPACK_PACKAGE_VENDOR  "Oracle Corporation")
set(CPACK_PACKAGE_CONTACT "MySQL Release Engineering <mysql-build@oss.oracle.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "MySQL Connector/C++, a C++ connector library")

# ======================================================================
# Set the default CPack generator
# ======================================================================

if(WIN32)
  set(CPACK_GENERATOR ZIP)
  set(CPACK_SOURCE_GENERATOR ZIP)
else()
  set(CPACK_GENERATOR TGZ)
  set(CPACK_SOURCE_GENERATOR TGZ)
endif()

# ======================================================================
# Set the platform name, if not set from -DPLATFORM_NAME=...
# ======================================================================
#
# TODO: Cover Windows and other architectures we suport
#
# TODO: Decide on proper tagging for Windows: VS version, static/dynamic
# runtime, debug/non-debug etc. Note: some of these differences can/should
# be covered by having several variants of the library added to the package.

if(PLATFORM_NAME)

  # Override with our own name
  set(CPACK_SYSTEM_NAME "${PLATFORM_NAME}")

elseif(WIN32)

  include(CheckTypeSize)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(CPACK_SYSTEM_NAME "winx64")
  else()
    set(CPACK_SYSTEM_NAME "win32")
  endif()

elseif(APPLE)

  if(NOT DEFINED ENV{MACOSX_DEPLOYMENT_TARGET})
    message(FATAL_ERROR "To create packages on OSX, set deployment target"
            " using MACOSX_DEPLOYMENT_TARGET environment variable")
  endif()

  set(osx_version $ENV{MACOSX_DEPLOYMENT_TARGET})
  set(CPACK_SYSTEM_NAME "osx${osx_version}-${CMAKE_SYSTEM_PROCESSOR}")

elseif(NOT CPACK_SYSTEM_NAME)

  # If for some reason not set by CMake
  if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CPACK_SYSTEM_NAME "linux-${CMAKE_SYSTEM_PROCESSOR}")
  else()
    message(FATAL_ERROR "Can't deternine how to set the platform name")
  endif()

endif()

set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}")
set(CPACK_PACKAGE_FILE_NAME         "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}-debug")
endif()

message("Binary package name: ${CPACK_PACKAGE_FILE_NAME}")

# FIXME maybe place elsewhere?
if(APPLE AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR "To create packages for OSX, build with clang compiler.")
endif()

# ======================================================================
# Licenses for binary packages
# ======================================================================

if(EXISTS "${CMAKE_SOURCE_DIR}/COPYING.txt")
  set(LIC_FILE "COPYING")       # Without ".txt" extension
else()
  set(LIC_FILE "LICENSE.mysql") # Without ".txt" extension
endif()

if(WIN32)
  set(info_ext ".txt")
  set(newline WIN32)
else()
  set(info_ext "")
  set(newline UNIX)
endif()

set(info_files README ${LIC_FILE})

foreach(file ${info_files})

  set(file_src "${CMAKE_SOURCE_DIR}/${file}.txt")
  set(file_bin "${CMAKE_BINARY_DIR}/${file}${info_ext}")

  configure_file("${file_src}" "${file_bin}" NEWLINE_STYLE ${newline})
  install(FILES "${file_bin}" DESTINATION .)

endforeach()

set(CPACK_RESOURCE_FILE_README  "README${info_ext}")
set(CPACK_RESOURCE_FILE_LICENSE "${LIC_FILE}${info_ext}")
#set(CPACK_RESOURCE_FILE_INSTALL "...")    # FIXME


# ======================================================================
# Specs for source package
# ======================================================================

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-src")

message("Source package name: ${CPACK_SOURCE_PACKAGE_FILE_NAME}")

# note: Using regex patterns for CPACK_SOURCE_IGNORE_FILES is fragile because
# they are matched against the full path which can vary depending on where the
# build takes place. Unfortunatelly, I (Rafal) could not find any other mechanism
# for specifying what source files should be excluded from the source package.
#
# note: Double escaping required to get correct pattern string (with single
# escapes) in CPackSourceConfig.cmake

list(APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.git.*")
list(APPEND CPACK_SOURCE_IGNORE_FILES "/jenkins/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CTestConfig.cmake")

include(CPack)


# ======================================================================
# Custom target to build packages.
# ======================================================================

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
