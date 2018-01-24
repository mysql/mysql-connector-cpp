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


if (NOT DEFINED WITH_COVERAGE)
  # use ENABLE_GCOV here to make it compatible with server conventions
  option(WITH_COVERAGE "Enable gcov (debug, Linux builds only)" ${ENABLE_GCOV})
endif()

if (WITH_COVERAGE AND NOT BUILD_STATIC)
  message(WARNING "To get good coverage results use static build of the"
          " connector (BUILD_STATIC)")
endif()


macro(ADD_COVERAGE target)

  if(CMAKE_COMPILER_IS_GNUCXX AND WITH_COVERAGE)

    message(STATUS "Enabling gcc coverage support for target: ${target}")

    # Note: we use set_property(... APPEND ...) to not override other sttings
    # for the target.

    set_property(TARGET ${target} APPEND
      PROPERTY COMPILE_DEFINITIONS_DEBUG WITH_COVERAGE
    )

    # Note: COMPILE_OPTIONS property does not have per build configuration
    # variant. Generator expression is used instead.

    set_property(TARGET ${target} APPEND
      PROPERTY COMPILE_OPTIONS
        $<$<CONFIG:Debug>:-O0;-fprofile-arcs;-ftest-coverage>
    )

    set_property(TARGET ${target} APPEND
      PROPERTY LINK_FLAGS_DEBUG -fprofile-arcs -ftest-coverage
    )

    set_property(TARGET ${target} APPEND
      PROPERTY INTERFACE_LINK_LIBRARIES gcov
    )

  endif()

endmacro(ADD_COVERAGE)
