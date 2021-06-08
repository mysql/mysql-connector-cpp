# Copyright (c) 2015, 2020, Oracle and/or its affiliates.
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

# The files in protobuf-3.11.4/ are almost unmodified versions of google
# source files taken from protobuf-cpp-3.11.4.tar.gz archive.
# Changes:
# - remove directories and files
#    protobuf-3.11.4/benchmarks/
#    protobuf-3.11.4/conformance/
#    protobuf-3.11.4/editors/
#    protobuf-3.11.4/examples/
#    protobuf-3.11.4/m4/
#    protobuf-3.11.4/objectivec/
#    protobuf-3.11.4/python/
#    protobuf-3.11.4/third_party/
#    protobuf-3.11.4/util/
#    protobuf-3.11.4/ar-lib
#    protobuf-3.11.4/compile
#    protobuf-3.11.4/config.guess
#    protobuf-3.11.4/config.h.in
#    protobuf-3.11.4/config.sub
#    protobuf-3.11.4/configure
#    protobuf-3.11.4/depcomp
#    protobuf-3.11.4/install-sh
#    protobuf-3.11.4/ltmain.sh
#    protobuf-3.11.4/missing
#    protobuf-3.11.4/test-driver
#    protobuf-3.11.4/update_file_lists.sh
# - disable configuration check of cmake's mimimum version
# - disable configuration zlib check
# - disable installation script
# - fix minor compilation issues on SunPro 5.14.0


cmake_minimum_required(VERSION 3.1)
PROJECT(Protobuf)

include(../setup.cmake)
include(platform)

SET(PROTO_SRC_DIR  "${PROJECT_SOURCE_DIR}/protobuf-3.11.4")

#
# Produce position independent code.
# Note: This is crucial on Solaris
#

enable_pic()

#
# Do not export any symbols
#
# Note: The LIBPROTOBUF_EXPORTS macro should *not* be defined
#

set_visibility(hidden)

if(NOT MSVC)
  add_compile_options(-Wno-stringop-overflow)
endif()

#
# Configure static runtime library on Windows if requested
#

OPTION(STATIC_MSVCRT "Use static MSVC runtime library" OFF)
SET(protobuf_MSVC_STATIC_RUNTIME ${STATIC_MSVCRT} CACHE INTERNAL "")


SET(protobuf_BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
SET(protobuf_BUILD_TESTS OFF CACHE INTERNAL "")
SET(protobuf_BUILD_EXAMPLES OFF CACHE INTERNAL "")
SET(protobuf_WITH_ZLIB OFF CACHE INTERNAL "")

ADD_SUBDIRECTORY(${PROTO_SRC_DIR}/cmake)

# Export targets

file(WRITE "${PROJECT_BINARY_DIR}/exports.cmake"
  "set(PROTOBUF_INCLUDE_DIR \"${PROTO_SRC_DIR}/src\")\n\n"
)

macro(pb_export)
  export(TARGETS ${ARGV} NAMESPACE pb_
         APPEND FILE "${PROJECT_BINARY_DIR}/exports.cmake")
endmacro()

pb_export(libprotobuf-lite)
pb_export(libprotobuf)
pb_export(protoc)
