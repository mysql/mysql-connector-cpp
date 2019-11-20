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

cmake_minimum_required(VERSION 2.8)

# Disable warnings in code which is not ours.

if(MSVC)
  add_compile_options(
    /wd4244 /wd4311 /wd4302
  )
endif()

SET (UUID_SRCS uuid_gen.cc)

ADD_LIBRARY(uuid_gen STATIC ${UUID_SRCS})

SET_TARGET_PROPERTIES(uuid_gen PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(uuid_gen PROPERTIES FOLDER "Misc")

get_filename_component(INC_DIR "../include" ABSOLUTE)
target_include_directories(uuid_gen PUBLIC ${INC_DIR})

