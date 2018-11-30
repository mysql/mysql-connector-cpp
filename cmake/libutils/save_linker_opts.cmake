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

##########################################################################
#
# This script is invoked as a wrapper for linker invocation
# (via RULE_LAUNCH_LINK property) to save arguments passed to the linker
# for later processing by merge_archives.cmake script.
#
# OUTPUT_FILE = file where to save linker arguments
#

#message("-- out: ${OUTPUT_FILE}")
#message("-- argc: ${CMAKE_ARGC}")

if(NOT DEFINED OUTPUT_FILE)
  message(FATAL_ERROR "No output file given")
endif()

if(EXISTS "${OUTPUT_FILE}")
  file(REMOVE ${OUTPUT_FILE})
endif()

set(argn 1)

#
# Ingore all arguments up to first "--"
#

while(argn LESS CMAKE_ARGC)

  if("${CMAKE_ARGV${argn}}" STREQUAL "--")
    # Note: we also skip the next argument, which is the linker executable
    math(EXPR argn "${argn}+2")
    break()
  endif()

  math(EXPR argn "${argn}+1")

endwhile()

#
# Remaining arguments are the arguments of the linker invocation. Save them
# in the output file but ignore the "-o <output file>" part.
#

while(argn LESS CMAKE_ARGC)

  if("${CMAKE_ARGV${argn}}" STREQUAL "-o")
    math(EXPR argn "${argn}+2")
  endif()

  #message("-- arg ${argn}: ${CMAKE_ARGV${argn}}")
  file(APPEND ${OUTPUT_FILE} "${CMAKE_ARGV${argn}}\n")
  math(EXPR argn "${argn}+1")

endwhile()
