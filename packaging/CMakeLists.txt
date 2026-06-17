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

# Note: Version 3.15 is required becausae package build instructions
# depend on cmake recognizing --install opton.

cmake_minimum_required(VERSION 3.15)

project(MySQL_CONCPP_Packages)

# Note: version.cmake can be already included if this is used as part of
# the top-level project.

if(NOT DEFINED CONCPP_VERSION)
  include(${CMAKE_CURRENT_SOURCE_DIR}/../version.cmake)
endif()

include(PackageSpecs.cmake)


# ======================================================================
# RPM/DEB package specifications
# ======================================================================


add_subdirectory(deb-in)

# RPM stuff

find_program(RPM rpm)

if(RPM)

  # Get RPM version

  execute_process(COMMAND ${RPM} "--version"
    RESULT_VARIABLE res
    OUTPUT_VARIABLE RPM_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )

  if(res)
    message(FATAL_ERROR "Could not determine RPM version (${RPM} --version command failed)")
  endif()

  # Set RPM_VER_NUM to version number in XYYZZ format

  string(REGEX REPLACE "^RPM version *" "" RPM_VERSION ${RPM_VERSION})
  string(REPLACE "." ";" ver ${RPM_VERSION})
  list(GET ver 0 ver_major)
  list(GET ver 1 ver_minor)
  list(GET ver 2 ver_patch)
  string(REGEX MATCH "..$" ver_minor "0${ver_minor}")
  string(REGEX MATCH "..$" ver_patch "0${ver_patch}")
  set(RPM_VER_NUM "${ver_major}${ver_minor}${ver_patch}")

  message(STATUS "Using RPM tools version: ${RPM_VERSION} (${RPM_VER_NUM})")

endif()


# Gnerate RPM specification

if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
  set(rpm_spec "${CMAKE_CURRENT_BINARY_DIR}/SPECS/mysql-connector-c++.spec")
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/SOURCES)
else()
  # Note: Preserve old behavior when included from the main CMakeLists.txt
  set(rpm_spec "${CMAKE_CURRENT_BINARY_DIR}/mysql-connector-c++.spec")
endif()

configure_file(mysql-connector-c++.spec.in "${rpm_spec}" @ONLY)


#
# If building this project stand-alone we stop here after generating RPM/DEB
# package specifications. Otherwise the rest of the project contains
# specifications for building packages using CPack (works with TGZ generator).
#

if(PROJECT_NAME STREQUAL CMAKE_PROJECT_NAME)
  return()
endif()


# ======================================================================
# Install manifest
# ======================================================================

# If requested, generate install manifest file which contains information
# about install components and files in each component. INSTALL_MANIFEST
# should be a path to the manifest file.
# See the install_mainfest.cmake script in this directory.

if(DEFINED INSTALL_MANIFEST)

  if(NOT MySQL_CONCPP_BINARY_DIR)
    abort("Top-level binary dir unknown...")
  endif()

  add_custom_target(generate_manifest
    ALL
    COMMAND ${CMAKE_COMMAND}
      -D MANIFEST_FILE=${INSTALL_MANIFEST}
      -D BUILD_DIR=${MySQL_CONCPP_BINARY_DIR}
      -D CONFIG=$<CONFIG>
      -P "${CMAKE_CURRENT_SOURCE_DIR}/install_manifest.cmake"
    WORKING_DIRECTORY ${MySQL_CONCPP_BINARY_DIR}
    COMMENT "Generating install manifest"
  )

endif()


# ======================================================================
# Licenses for binary packages
# ======================================================================

if(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE.mysql.txt")
  set(LIC_FILE "LICENSE.mysql")       # Without ".txt" extension
else()
  set(LIC_FILE "LICENSE") # Without ".txt" extension
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
  install(FILES "${file_bin}" DESTINATION ${INSTALL_DOC_DIR} COMPONENT Readme)

endforeach()

if(CONCPP_LICENSE STREQUAL "GPL-2.0")
  set(contributing_file "${CMAKE_BINARY_DIR}/CONTRIBUTING.md")
  configure_file(
    "${CMAKE_SOURCE_DIR}/CONTRIBUTING.md"
    "${contributing_file}"
    NEWLINE_STYLE ${newline}
  )
  install(FILES "${contributing_file}" DESTINATION ${INSTALL_DOC_DIR} COMPONENT Readme)
endif()


# ======================================================================
# Package definitions
# ======================================================================

if(APPLE AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR "To create packages for OSX, build with clang compiler.")
endif()


include(CPack)

set(CPACK_RESOURCE_FILE_README  "${CMAKE_BINARY_DIR}/README${info_ext}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/${LIC_FILE}${info_ext}")
#set(CPACK_RESOURCE_FILE_INSTALL "...")    # FIXME


# Define install component groups.

cpack_add_component_group(main)
cpack_add_component_group(tests)
cpack_add_component_group(debug)

cpack_add_component(DevCommon GROUP main)
cpack_add_component(XDevAPIDev GROUP main)
cpack_add_component(XDevAPIDll GROUP main)
cpack_add_component(JDBCDev GROUP main)
cpack_add_component(JDBCDll GROUP main)
cpack_add_component(OpenSSLDll GROUP main)
cpack_add_component(OpenSSLDev GROUP main)
cpack_add_component(Readme GROUP main)
cpack_add_component(Debuginfo GROUP debug)
cpack_add_component(JDBCTests GROUP tests)
cpack_add_component(XDevAPITests GROUP tests)


# ======================================================================
# Custom target to build packages.
# ======================================================================
#
# Note: target build_packages builds TGZ/ZIP packages using cpack.
# Other package types are built outside of cmake/cpack, cmake is only
# used to prepare information necessary to build them.
#
# Note: For information on building MSI packages look at WiX/CMakeLists.txt
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

