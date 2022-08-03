# Copyright (c) 2021, Oracle and/or its affiliates. All rights reserved.
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

# #############################################################################
#
# Targets:
#
# (re)build-{zlib,lz4,zstd}
#
# Imported/alias targets:
#
# ext::z
# ext::lz4
# ext::zstd
#

if(TARGET ext::z)
  return()
endif()

message(STATUS "Setting up compression libraries.")

#######
# ZLIB
#
add_ext(zlib zlib.h z ext_zlib)
if(NOT ZLIB_FOUND)
  message(FATAL_ERROR "Can't build without zlib support")
endif()


#######
# LZ4
#
add_ext(lz4 lz4frame.h lz4 ext_lz4)

if(NOT LZ4_FOUND)
  message(FATAL_ERROR "Can't build without lz4 support")
endif()


#######
# ZSTD
#
add_ext(zstd zstd.h zstd ext_zstd)

if(NOT LZ4_FOUND)
  message(FATAL_ERROR "Can't build without zstd support")
endif()

