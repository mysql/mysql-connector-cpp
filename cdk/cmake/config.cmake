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
# Infrastructure for generating configuration header
# ==================================================
#
# Call this macro to add define to the configuration header based on
# a cmake variable.
#
#  ADD_CONNFIG(<var> [<val>])
#
# Like with #cmakedefine, the define will have the same name as cmake
# variable used and will be defined only if this variable is true (at
# the time of generating the configuration header). If value is provided,
# the variable is set to the given value.
#

set(CONFIG_VARS "" CACHE INTERNAL "configuration settings" FORCE)

macro(ADD_CONFIG var)
  #message("- adding configuration setting: ${var}")
  list(APPEND CONFIG_VARS ${var})
  list(REMOVE_DUPLICATES CONFIG_VARS)
  set(CONFIG_VARS ${CONFIG_VARS} CACHE INERNAL "configuration settings" FORCE)

  # set variable to the value if given

  if(ARGN)
    set(${var} ${ARGN})
    list(GET ${var} 0 ${var})
  endif()

endmacro(ADD_CONFIG)

#
# Call this macro to write a configuration header containing defines
# declared with ADD_CONFIG() calls. The header is generated from config.h.in
# template with @GENERATED_CONFIG_DEFS@ replaced by previously declared
# defines.
#
#  WRITE_CONFIG_HEADER(<path relative to PROJECT_BINARY_DIR>)
#

macro(WRITE_CONFIG_HEADER path)

  set(GENERATED_CONFIG_DEFS)

  foreach(var ${CONFIG_VARS})
    if(${var})
      set(DEFINE "#define ${var}")
    else()
      set(DEFINE "/* #undef ${var} */")
    endif()
    set(GENERATED_CONFIG_DEFS "${GENERATED_CONFIG_DEFS}\n${DEFINE}")
  endforeach()

  configure_file(${PROJECT_SOURCE_DIR}/config.h.in ${path})
  message("Wrote configuration header: ${path}")

endmacro(WRITE_CONFIG_HEADER)




