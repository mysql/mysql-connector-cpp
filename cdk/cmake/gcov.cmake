# Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
#
# This code is licensed under the terms of the GPLv2
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
