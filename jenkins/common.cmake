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



macro(check_param name msg)
if (NOT DEFINED ${name})
  set(${name} $ENV{${name}})
endif()
if (NOT DEFINED ${name})
  message(FATAL_ERROR "Set ${msg} using ${name} variable")
endif()
endmacro()


macro(set_param name doc)
if (NOT DEFINED ${name})
  set(${name} $ENV{${name}})
endif()
if (NOT DEFINED ${name} AND ${ARGC} GREATER 2)
  set(${name} ${ARGN})
endif()
endmacro()


macro(start branch)
message("\n==== START ====\n")
if(branch MATCHES "master.*")
  ctest_start("Continuous")
else()
  ctest_start("Experimental")
endif()
message("\n===============\n")
endmacro()

macro(step msg)
message("\n---- ${msg}\n")
endmacro()

macro(fail msg)
if(NOT DEFINED SUBMIT OR SUBMIT)
  step("submitting result to cdash")
  ctest_submit()
endif()
message(FATAL_ERROR ${msg})
endmacro()

macro(done)
if(NOT DEFINED SUBMIT OR SUBMIT)
  step("submitting result to cdash")
  ctest_submit()
endif()
message("\n==== DONE ====\n")
endmacro()
