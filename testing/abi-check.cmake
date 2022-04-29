# Copyright (c) 2022, Oracle and/or its affiliates. All rights reserved.
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
# Compare exported symbol information dumped to abi.map file in the
# given build location with symbols in the given list:
#
# BUILD_DIR -- build location where abi.map file should be found.
# ABIDEF  -- path to a file listing current ABI symbols.
# 
# Note: The abi.map should be generated from the .exp file produced
# by inker when building DLL. The list can be extracted from the .edata
# section of the .exp file using: dumpbin /section:.edata /symbols
#

cmake_policy(SET CMP0007 NEW)


macro(main)

  if(NOT ABIDEF OR NOT EXISTS ${ABIDEF})
    message(FATAL_ERROR "Missing ABI definition file: ${ABIDEF}")
  endif()

  # Look for abi.map in the build location
  # FIXME: REQUIRED

  find_file(MAP abi.map
    PATH_SUFFIXES Debug Release RelWithDebInfo MinSizeRel    
    PATHS ${BUILD_DIR}
    REQUIRED
  )
  #message("MAP: ${MAP}")

  # read symbols from the map file into given list
  # Note: For each symbol X, ${X_NAME} is set to the demangled name

  read_map_file(EXPORTS ${MAP})

  if(ABIUPDATE)

    # Update ABI definition with symbols read from the map file.
    # The headers with version info are read from the original ABI definition 
    # -- this version info might need to be updated manually (e.g., connector 
    # version)
    
    file(STRINGS "${ABIDEF}" HEADERS
      LIMIT_COUNT 2
    )
    abi_update(${ABIDEF} ${HEADERS} ${EXPORTS})
    return()
  endif()

  message(STATUS "comparing with ABI definition in: ${ABIDEF}")

  # Read ABI symbols from ABI definition file
  file(STRINGS ${ABIDEF} SYMBOLS)

  # Skip 3 header lines
  list(REMOVE_AT SYMBOLS 0 1 2)

  # Read symobls from each line, mangled name is separated from 
  # demangled version by " __==__ " (see abi_update())

  unset(ABI)
  foreach(S IN LISTS SYMBOLS)
    string(REPLACE " __==__ " ";" S ${S})
    list(GET S 0 X)
    list(GET S 1 ${X}_NAME)
    #message(STATUS "ABI: ${X} (${${X}_NAME})")
    list(APPEND ABI ${X})
  endforeach()

  # Compare symbols in ABI list with ones in EXPORTS

  foreach(SYM IN LISTS EXPORTS)

    list(FIND ABI ${SYM} found)

    if(found LESS 0)
      message("!! new symbol: ${${SYM}_NAME}")
      set(fail TRUE)
    else()
      list(REMOVE_AT ABI ${found})
    endif()

  endforeach()

  # Note: All symbols from ABI should be removed now.

  foreach(miss IN LISTS ABI)
    message("!!! ABI symbol not found: ${${miss}_NAME}")
    set(fail TRUE)
  endforeach()

  if(fail)
    message(FATAL_ERROR "ABI changes detected!"
      " If ABI symbols are missing, code needs to be fixed."
      " If only new symbols are added, the build is still ABI compatible"
      " with previous versions."
      " If the intention is to extend the ABI, the ABI definition file"
      " should be updated (after careful review...)."
    )
  endif()

endmacro(main)


# 
# read_map_file(LIST MAP)
#
# Read mangled and demangled symbol names from a .map file ${MAP}. Each mangled 
# name is appended to the list variable named by LIST argument. For each 
# mangled name ${X} added to the output list, variable ${X}_NAME is set to its 
# demangled name.
#
# Example contents of a .map file
#
# 2AC 00000000 UNDEF  notype       External     | ?$TSS0@?1??get@Settings_impl@mysqlx_2_0@@QEBAAEBVValue@3@H@Z@4HA (int `public: class mysqlx_2_0::Value const & __cdecl mysqlx_2_0::Settings_impl::get(int)const '::`2'::$TSS0)
# 2AD 00000000 UNDEF  notype       External     | ??0?$Row_result_detail@VColumns@mysqlx@@@internal@mysqlx@@IEAA@AEAVResult_init@mysqlx_2_0@@@Z (protected: __cdecl mysqlx::internal::Row_result_detail<class mysqlx::Columns>::Row_result_detail<class mysqlx::Columns>(class mysqlx_2_0::Result_init &))
# 2AE 00000000 UNDEF  notype       External     | ??0Client_detail@internal@mysqlx@@IEAA@$$QEAU012@@Z (protected: __cdecl mysqlx::internal::Client_detail::Client_detail(struct mysqlx::internal::Client_detail &&))
# 2AF 00000000 UNDEF  notype       External     | ??0Client_detail@internal@mysqlx@@QEAA@AEAVSettings_impl@mysqlx_2_0@@@Z (public: __cdecl mysqlx::internal::Client_detail::Client_detail(class mysqlx_2_0::Settings_impl &))
# 2B0 00000000 UNDEF  notype       External     | ??0Collection_detail@internal@mysqlx@@IEAA@AEBV?$shared_ptr@VSession_impl@mysqlx_2_0@@@std@@AEBVstring@2@@Z (protected: __cdecl mysqlx::internal::Collection_detail::Collection_detail(class std::shared_ptr<class mysqlx_2_0::Session_impl> const &,class mysqlx::string const &))

function(read_map_file LIST MAP)

  file(STRINGS "${MAP}" LINES 
      REGEX "[ \t]+External[ \t]+[|]"
      #LIMIT_COUNT 20
  )

  # Read exported symbols and their demangled names. Symbols are stored
  # in LIST. For each symbol X its demangled name is stored in X_NAME
  # variable.

  foreach(X IN LISTS LINES)

    # remove prefix
    string(REGEX REPLACE "^.*[ \t]+External[| \t]+" "" X ${X})
    # demangled name comes in parentheses after the symbol name
    string(REGEX REPLACE "^([^ ]+) +[(](.*)[)]" "\\1;\\2" X ${X})

    #message(": {${X}}")
    list(GET X 0 symbol)
    list(REMOVE_AT X 0)

    list(APPEND exports ${symbol})

    # For plain C symbols there is no demangled name in the .map file
    # and X should be empty now. Otherwise X contains demangled name.

    if(X)
      set(name ${X})
    else()
      set(name ${symbol})
    endif()

    # Note: Demangled names could be set when reading other map files
    # and they should be identical!

    if(DEFINED ${symbol}_NAME AND NOT ${symbol}_NAME STREQUAL name)
      message(FATAL_ERROR
        "Different demangled names for the same symbol ${symbol}: "
        "(${${symbol_NAME}) vs (${name})"
      )
    endif()

    set(${symbol}_NAME ${name} PARENT_SCOPE)

  endforeach()

  set(${LIST} ${exports} PARENT_SCOPE)

endfunction(read_map_file)


#
# abi_update(ABIDEF HDR1 HDR2 X1 X2 ... XN)
#
# Update ABI definition file ABIDEF with ABI symbols X1,.., XN
# (read from a .map file). Each Xi is a mangled symbol name and ${Xi_NAME} 
# should be the corresponding demangled name.  HDR1 and HDR2 are two header
# lines from the .map file that contain information about ABI and connector
# versions.
#
# The format of ABI definition file: 3 header lines followed by lines defining 
# ABI symbols, each line containing mangled and demangled name separated
# by " __==__ " (separator is choosen so that it should not appear either in 
# mangled or demangled name)
#
# Example:
#
# ABI version: 2.0 (64bit)
# Note: Generated at connector version 2.0.28
#
# ?$TSS0@?1??get@Settings_impl@common@r0@abi2@mysqlx@@QEBAAEBVValue@3456@H@Z@4HA __==__ int `public: class mysqlx::abi2::r0::common::Value const & __cdecl mysqlx::abi2::r0::common::Settings_impl::get(int)const '::`2'::$TSS0
#

function(abi_update ABIDEF HDR1 HDR2)

  message(STATUS "updating ABI definition in: ${ABIDEF}")

  file(WRITE ${ABIDEF} "${HDR1}\n${HDR2}\n\n")

  foreach(X IN LISTS ARGN)
    #message(STATUS "${X}; ${${X}_NAME}")
    file(APPEND ${ABIDEF} "${X} __==__ ${${X}_NAME}\n")
  endforeach()

endfunction(abi_update)


main()
message(STATUS "No ABI changes detected")