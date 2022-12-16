# Copyright (c) 2019, 2023, Oracle and/or its affiliates. All rights reserved.
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
# TOOLSET and CXX_FRONTEND
#
if(WIN32)
  set(TOOLSET "MSVC" CACHE INTERNAL "")
  set(TOOLSET_MSVC "1" CACHE INTERNAL "")
  if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(CXX_FRONTEND "MSVC" CACHE INTERNAL "")
    set(CXX_FRONTEND_MSVC "1" CACHE INTERNAL "")
    # clang-cl behaves has MSVC
    set(MSVC ${compiler_version} CACHE INTERNAL "")
  else()
    set(CXX_FRONTEND "GCC" CACHE INTERNAL "")
    set(CXX_FRONTEND_GCC "1" CACHE INTERNAL "")
  endif()
else()
  set(TOOLSET "GCC" CACHE INTERNAL "")
  set(CXX_FRONTEND "GCC" CACHE INTERNAL "")
  set(TOOLSET_GCC "1" CACHE INTERNAL "")
  set(CXX_FRONTEND_GCC "1" CACHE INTERNAL "")
endif()

function(enable_cxx17)

  add_flags(CXX -std=c++17)

  #
  # If Clang is used on macOS and deployment target is not specified
  # with MACOSX_DEPLOYMENT_TARGET environment variable, make
  # sure that clang's native implementation of C++ std
  # libarary (libc++) is used. Otherwise clang defaults to
  # GNU version (libstdc++) which is outdated and does
  # not handle C++17 well.
  #
  # TODO: Add option to use the default runtime if user wishes
  # so.
  #

  if (MACOS)
    add_flags(CXX -stdlib=libc++)
  endif()

endfunction()

function(enable_pic)
  if(TOOLSET_GCC)
    add_compile_options(-fPIC)
  endif()
endfunction()

function(set_visibility)
  add_compile_options(-fvisibility-ms-compat)
endfunction()

