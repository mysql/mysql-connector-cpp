#   Copyright (c) 2013, Oracle and/or its affiliates. All rights reserved.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPLv2
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPLv2 as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published
#   by the Free Software Foundation; version 2 of the License.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#   or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
#   for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program; if not, write to the Free Software Foundation, Inc.,
#   51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

##########################################################################

# The parameter can be either T or D
MACRO(CHANGE_CRT _switch2use)
  # Or should that be check for VS?
  IF(WIN32)
      SET(switch2change "D")

      IF(_switch2use STREQUAL "D" OR _switch2use STREQUAL "MD" OR _switch2use STREQUAL "/MD")
        SET(switch2use "D")
        SET(switch2change "T")
      ELSE(switch2use STREQUAL "D" OR _switch2use STREQUAL "MD" OR _switch2use STREQUAL "/MD")
        #Default is to change to /MT
        SET(switch2use "T")
      ENDIF(_switch2use STREQUAL "D" OR _switch2use STREQUAL "MD" OR _switch2use STREQUAL "/MD")

      FOREACH(flags CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        IF(${flags} MATCHES "/M${switch2change}")
          STRING(REGEX REPLACE "/M${switch2change}" "/M${switch2use}" ${flags} "${${flags}}")
        ENDIF(${flags} MATCHES "/M${switch2change}")

        MESSAGE(STATUS, "CHANGE_CRT ${flags} ${${flags}}")

      ENDFOREACH(flags)
  ENDIF(WIN32)

ENDMACRO(CHANGE_CRT _switch2use)

