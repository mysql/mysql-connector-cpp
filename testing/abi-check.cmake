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

  read_map_file(ABI ${ABIDEF})
  read_map_file(EXPORTS ${MAP})

  message(STATUS "comparing with ABI definition in: ${ABIDEF}")

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
      "If ABI symbols are missing, code needs to be fixed."
      " If only new symbols are added, the build is still ABI compatible"
      " with previous versions."
      " If the intention is to extend the ABI, the ABI definition file"
      " should be updated (after careful review...)."
    )
  endif()

endmacro(main)


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

main()
message(STATUS "No ABI changes detected")