# Copyright (c) 2008, 2020, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License, version 2.0, as
# published by the Free Software Foundation.
#
# This program is also distributed with certain software (including
# but not limited to OpenSSL) that is licensed under separate terms,
# as designated in a particular file or component or in included license
# documentation. The authors of MySQL hereby grant you an
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
# Input:
#
#  WITH_MYSQL | MYSQL_DIR | MYSQL_INCLUDE_DIR + MYSQL_LIB_DIR
#  MYSQL_CONFIG_EXECUTABLE
#  MYSQL_LIB_STATIC - Determines meaning of MySQL::client target
#
# Output:
#
#  MySQL::client-shared   - target for shared library (not implemented yet)
#  MySQL::client-static   - target for static library
#  MySQL::client          - points at shared library, or static library if
#                           MYSQL_LIB_STATIC is set
#
#  MYSQL_VERSION, MYSQL_VERSION_ID
#  MYSQL_INCLUDE_DIR
#  MYSQL_LIB_DIR
#  MYSQL_PLUGIN_DIR
#  MYSQL_EXTERNAL_DEPENDENCIES
#  MYSQL_EXTERNAL_SEARCHPATH
#
##########################################################################


add_config_option(MYSQL_CONFIG_EXECUTABLE PATH ADVANCED
  "Location of mysql_config program."
)

add_config_option(WITH_MYSQL PATH
  "Base location of (monolithic) MySQL installation."
)

add_config_option(MYSQL_INCLUDE_DIR PATH ADVANCED "Path to MYSQL headers.")
add_config_option(MYSQL_LIB_DIR PATH ADVANCED "Path to MYSQL libraries.")
add_config_option(MYSQL_PLUGIN_DIR PATH ADVANCED "Path to MYSQL plugin libraries.")


function(main)

  if(TARGET MySQL::client)
    return()
  endif()

  message("Looking for MySQL Client library:")

  if(MYSQL_CONFIG_EXECUTABLE)

    use_mysql_config()

  else()

    # Note: legacy behavior (use MYSQL_DIR env. variable if set)

    if(NOT DEFINED MYSQL_DIR AND DEFINED ENV{MYSQL_DIR})
      set(MYSQL_DIR "$ENV{MYSQL_DIR}")
    endif()

    if(DEFINED WITH_MYSQL)
      set(MYSQL_DIR "${WITH_MYSQL}")
    endif()

    if(NOT DEFINED MYSQL_INCLUDE_DIR OR NOT DEFINED MYSQL_LIB_DIR)

      # Try using mysql_config, if available

      if(MYSQL_DIR)

        find_program(MYSQL_CONFIG_EXECUTABLE
          NAMES mysql_config
          PATHS ${MYSQL_DIR}/bin
          NO_DEFAULT_PATH
        )

      else()

        find_program(MYSQL_CONFIG_EXECUTABLE
          NAMES mysql_config
        )

      endif()

      if(MYSQL_CONFIG_EXECUTABLE)
        use_mysql_config()
      endif()

      # If paths still not set, try default locations

      if(NOT DEFINED MYSQL_INCLUDE_DIR AND MYSQL_DIR)
        set(MYSQL_INCLUDE_DIR "${MYSQL_DIR}/include")
      endif()

      if(NOT DEFINED MYSQL_LIB_DIR AND MYSQL_DIR)
        set(MYSQL_LIB_DIR "${MYSQL_DIR}/lib")
      endif()

      if(NOT DEFINED MYSQL_PLUGIN_DIR AND MYSQL_LIB_DIR)
        set(MYSQL_PLUGIN_DIR "${MYSQL_LIB_DIR}/plugin")
      endif()

    endif()

  endif(MYSQL_CONFIG_EXECUTABLE)


  if(NOT MYSQL_INCLUDE_DIR OR NOT EXISTS "${MYSQL_INCLUDE_DIR}/mysql.h")
    message(FATAL_ERROR
     "Could not find MySQL headers at: ${MYSQL_INCLUDE_DIR}\n"
     "Point at MySQL client library location using WITH_MYSQL or"
     " MYSQL_INCLUDE_DIR, MYSQL_LIB_DIR settings."
    )
  endif()

  #
  # Update cached values to the computed ones
  #

  set(MYSQL_INCLUDE_DIR "${MYSQL_INCLUDE_DIR}"
    CACHE PATH "Path to MYSQL headers (computed)."
    FORCE
  )

  set(MYSQL_LIB_DIR "${MYSQL_LIB_DIR}"
    CACHE PATH "Path to MYSQL libraries (computed)."
    FORCE
  )

  set(MYSQL_PLUGIN_DIR "${MYSQL_PLUGIN_DIR}"
    CACHE PATH "Path to MYSQL plugin libraries (computed)."
    FORCE
  )


  #
  # Searching for library.
  #
  # TODO: lib64/?
  # TODO: Handle both static and dynamic library

  find_library(MYSQL_LIB
    NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}mysqlclient${CMAKE_STATIC_LIBRARY_SUFFIX}
    PATHS ${MYSQL_LIB_DIR}
    NO_DEFAULT_PATH
  )

  find_library(MYSQL_LIB_DEBUG
    NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}mysqlclient${CMAKE_STATIC_LIBRARY_SUFFIX}
    PATHS "${MYSQL_LIB_DIR}/debug"
    NO_DEFAULT_PATH
  )

  if(NOT WIN32)

    find_library(MYSQL_DLL
      NAMES ${CMAKE_DYNAMIC_LIBRARY_PREFIX}mysqlclient${CMAKE_DYNAMIC_LIBRARY_SUFFIX}
      PATHS ${MYSQL_LIB_DIR}
      NO_DEFAULT_PATH
      )

    find_library(MYSQL_DLL_DEBUG
      NAMES ${CMAKE_DYNAMIC_LIBRARY_PREFIX}mysqlclient${CMAKE_DYNAMIC_LIBRARY_SUFFIX}
      PATHS "${MYSQL_LIB_DIR}/debug"
      NO_DEFAULT_PATH
      )

  else() #WIN32

    find_library(MYSQL_DLL
      NAMES libmysql
      PATHS ${MYSQL_LIB_DIR}
      NO_DEFAULT_PATH
      )

    find_library(MYSQL_DLL_DEBUG
      NAMES libmysql
      PATHS "${MYSQL_LIB_DIR}/debug"
      NO_DEFAULT_PATH
      )

    find_library(MYSQL_DLL_IMP
      NAMES libmysql.lib
      PATHS ${MYSQL_LIB_DIR}
      NO_DEFAULT_PATH
    )

    find_library(MYSQL_DLL_IMP_DEBUG
      NAMES libmysql.lib
      PATHS "${MYSQL_LIB_DIR}/debug"
      NO_DEFAULT_PATH
    )
  endif()

  #message("-- static lib: ${MYSQL_LIB}")
  #message("-- debug lib: ${MYSQL_LIB_DEBUG}")

  #
  # Unless we are on Win, we try using optimized lib also for debug builds if
  # debug variant was not found, and the other way around.
  #
  # On Windows this is not going to work. If correct variant of the client lib
  # is not available, build will fail.
  #

  if(NOT WIN32)

    if (NOT MYSQL_LIB_DEBUG)
      set(MYSQL_LIB_DEBUG "${MYSQL_LIB}")
    endif()

    if (NOT MYSQL_LIB)
      set(MYSQL_LIB "${MYSQL_LIB_DEBUG}")
    endif()

    if (NOT MYSQL_DLL_DEBUG)
      set(MYSQL_DLL_DEBUG "${MYSQL_DLL}")
    endif()

    if (NOT MYSQL_DLL)
      set(MYSQL_DLL "${MYSQL_DLL_DEBUG}")
    endif()

  endif()


  #
  # Define import target for the client library (currently only static)
  #

  add_library(MySQL::client-static STATIC IMPORTED GLOBAL)

  set_target_properties(MySQL::client-static PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${MYSQL_INCLUDE_DIR}"
    IMPORTED_LOCATION "${MYSQL_LIB}"
    IMPORTED_LOCATION_DEBUG "${MYSQL_LIB_DEBUG}"
  )

  add_library(MySQL::client-shared SHARED IMPORTED GLOBAL)

  set_target_properties(MySQL::client-shared PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${MYSQL_INCLUDE_DIR}"
    IMPORTED_LOCATION "${MYSQL_DLL}"
    IMPORTED_LOCATION_DEBUG "${MYSQL_DLL_DEBUG}"
  )

  if (WIN32)
    if(MYSQL_DLL_IMP)
      set_target_properties(MySQL::client-shared PROPERTIES
        IMPORTED_IMPLIB "${MYSQL_DLL_IMP}")
    endif()
    if(MYSQL_DLL_IMP_DEBUG)
      set_target_properties(MySQL::client-shared PROPERTIES
        IMPORTED_IMPLIB_DEBUG "${MYSQL_DLL_IMP_DEBUG}")
    endif()
  endif()


  # Define alias MySQL::client pointing to -static or -shared library,
  # depending on MYSQL_LIB_STATIC setting.
  #
  # Note: In older cmake versions it is not possible to make an alias
  # to imported target. For that reason we create alias to interface target
  # that depends on the final imported library.

  add_library(mysql-client-if INTERFACE)

  set_target_properties(mysql-client-if PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${MYSQL_INCLUDE_DIR}"
      )

  if(MYSQL_LIB_STATIC)
    target_link_libraries(mysql-client-if INTERFACE MySQL::client-static)
  else()
    target_link_libraries(mysql-client-if INTERFACE MySQL::client-shared)
  endif()

  add_library(MySQL::client ALIAS mysql-client-if)

  #
  # Determine library version
  #

  get_version()

  if(NOT MYSQL_VERSION)
    message(FATAL_ERROR
      "Could not determine the MySQL client library version."
    )
  endif()

  set_property(TARGET MySQL::client-static
    PROPERTY VERSION ${MYSQL_VERSION}
  )

  set_property(TARGET MySQL::client-shared
    PROPERTY VERSION ${MYSQL_VERSION}
  )

  #
  # Detect shared libraries on which MySQL client lib depends (if possible).
  # Stores result in MYSQL_EXTERNAL_DEPENDENCIES.
  #

  get_dependencies()

  message("  version: ${MYSQL_VERSION}")
  get_target_property(dummy MySQL::client-shared INTERFACE_INCLUDE_DIRECTORIES)
  message("  include path: ${dummy}")
  get_target_property(dummy MySQL::client-shared IMPORTED_LOCATION)
  message("  library location: ${dummy}")

  # Using INTERFACE_LINK_LIBRARIES we ensure that -L option will appear
  # in the link line of the consumer of this library. Adding this -L option
  # is needed to resolve dependencies, such as -lssl, to the libraries that
  # are bundled with the server.
  #
  # Note: This must be done before populating INTERFACE_LINK_LIBRARIES with
  # the dependencies themselves.

  if(NOT MSVC)

    set(searchpath "")
    list(APPEND searchpath "-L${MYSQL_LIB_DIR}"
                           "-L${MYSQL_LIB_DIR}/private")

    if(MYSQL_EXTERNAL_SEARCHPATH)

      foreach(search ${MYSQL_EXTERNAL_SEARCHPATH})
        list(APPEND searchpath "-L${search}")
      endforeach()

    endif()

    string(REPLACE ";" " " searchpath "${searchpath}")
    message("  dependencies search path: ${searchpath}")


    set_property(TARGET MySQL::client-static
      PROPERTY INTERFACE_LINK_LIBRARIES
      "${searchpath}"
    )
    set_property(TARGET MySQL::client-shared
      PROPERTY INTERFACE_LINK_LIBRARIES
      "${searchpath}"
    )
  endif()

  if(MYSQL_EXTERNAL_DEPENDENCIES)
    string(REPLACE ";" " " deps "${MYSQL_EXTERNAL_DEPENDENCIES}")
    message("  dependencies: ${deps}")

    #
    # If external dependencies were found, add them to the static target
    # as any code that liks to static library should also link with the
    # external dependencies.
    #

    target_link_libraries(MySQL::client-static INTERFACE ${MYSQL_EXTERNAL_DEPENDENCIES})

  endif()


endfunction(main)


##################################################################


function(get_version)

  # MYSQL_VERSION could have been already determined in use_mysql_config()

  if(NOT MYSQL_VERSION)

    #
    # If mysql_config could not be used, we compile small program that takes
    # version information from the client library headers.
    #

    set(GETMYSQLVERSION_SOURCEFILE
      "${CMAKE_CURRENT_BINARY_DIR}/getmysqlversion.c"
    )

    file(WRITE "${GETMYSQLVERSION_SOURCEFILE}"
         "#include <mysql.h>\n"
         "#include <stdio.h>\n"
         "int main() {\n"
         "  printf(\"%s\", MYSQL_SERVER_VERSION);\n"
         "}\n"
    )

    # Compile and run the created executable, store output in MYSQL_VERSION

    try_run(_run_result _compile_result
      "${CMAKE_BINARY_DIR}"
      "${GETMYSQLVERSION_SOURCEFILE}"
      CMAKE_FLAGS "-DINCLUDE_DIRECTORIES:STRING=${MYSQL_INCLUDE_DIR}"
      RUN_OUTPUT_VARIABLE MYSQL_VERSION
      COMPILE_OUTPUT_VARIABLE _compile_out
    )

    #message("DBG: Running \"getmysqlversion\": (compile: ${_compile_result}) (run: ${_run_result})")

    #if (NOT _compile_result)
    #  message(FATAL_ERROR "compilation failed:\n\n${_compile_out}")
    #endif()

  endif()

  # Clean up so only numeric, in case of "-alpha" or similar. Compute
  # MYSQL_VERSION_ID and MYSQL_NUM_VERSION

  string(REGEX MATCHALL "([0-9]+.[0-9]+.[0-9]+)" MYSQL_VERSION "${MYSQL_VERSION}")

  # To create a fully numeric version, first normalize so N.NN.NN

  string(REGEX REPLACE "[.]([0-9])[.]" ".0\\1." MYSQL_VERSION_ID "${MYSQL_VERSION}")
  string(REGEX REPLACE "[.]([0-9])$"   ".0\\1"  MYSQL_VERSION_ID "${MYSQL_VERSION_ID}")
  # Finally remove the dot
  string(REGEX REPLACE "[.]" "" MYSQL_VERSION_ID "${MYSQL_VERSION_ID}")

  set(MYSQL_VERSION ${MYSQL_VERSION} CACHE INTERNAL "MySQL client library version")
  set(MYSQL_VERSION_ID ${MYSQL_VERSION_ID} CACHE INTERNAL "MySQL client library version")
  set(MYSQL_NUM_VERSION ${MYSQL_VERSION_ID} CACHE INTERNAL "MySQL client library version")

endfunction(get_version)


##################################################################


function(get_dependencies)

  # Currently the only way of determining dependencies is
  # via mysql_config, which should happen in use_mysql_config()
  #
  # TODO: Find a way of extracting MYSQL_EXTERNAL_DEPENDENCIES also
  # on Windows

  set(MYSQL_EXTERNAL_DEPENDENCIES "${MYSQL_EXTERNAL_DEPENDENCIES}"
    CACHE INTERNAL
    "List of external libraries on which MySQL client library depends."
  )

  set(MYSQL_EXTERNAL_SEARCHPATH "${MYSQL_EXTERNAL_SEARCHPATH}"
    CACHE INTERNAL
    "List of locations of external libraries on which MySQL client library depends as reported by mysql_config."
  )

endfunction(get_dependencies)


##################################################################


function(use_mysql_config)

  if(NOT EXISTS "${MYSQL_CONFIG_EXECUTABLE}")
    #message("-- mysql_config not fount")
    return()
  endif()

  # Install location

  _mysql_conf(MYSQL_INCLUDE_DIR --variable=pkgincludedir)
  _mysql_conf(MYSQL_LIB_DIR     --variable=pkglibdir)
  _mysql_conf(MYSQL_PLUGIN_DIR     --variable=plugindir)

  file(TO_CMAKE_PATH "${MYSQL_INCLUDE_DIR}" MYSQL_INCLUDE_DIR)
  file(TO_CMAKE_PATH "${MYSQL_LIB_DIR}" MYSQL_LIB_DIR)
  file(TO_CMAKE_PATH "${MYSQL_PLUGIN_DIR}" MYSQL_PLUGIN_DIR)

  set(MYSQL_INCLUDE_DIR "${MYSQL_INCLUDE_DIR}" PARENT_SCOPE)
  set(MYSQL_LIB_DIR "${MYSQL_LIB_DIR}" PARENT_SCOPE)
  set(MYSQL_PLUGIN_DIR "${MYSQL_PLUGIN_DIR}" PARENT_SCOPE)

  # client library version (note: it will be cleaned up in get_version())

  _mysql_conf(MYSQL_VERSION --version)

  set(MYSQL_VERSION "${MYSQL_VERSION}" PARENT_SCOPE)

  # Find external dependencies

  set(MYSQL_EXTERNAL_DEPENDENCIES "")

  _mysql_conf(config_libs_paths --libs)
  string(REGEX MATCHALL " -l[^ ]+" config_libs ${config_libs_paths})
  message("-- libs: ${config_libs}")


  foreach(lib ${config_libs})

    string(REGEX REPLACE " -l([^ ]+)" "\\1" lib ${lib})
    #message("-- checking lib: ${lib}")

    # Libraries that are known to be internal compiler ones are not set as
    # explicit dependencies.

    if(NOT lib MATCHES
        "(mysqlclient|libmysql|^stdc|^gcc|^CrunG3|^c$|^statomic)"
      )

      list(APPEND MYSQL_EXTERNAL_DEPENDENCIES ${lib})

    else()
      #message("-- skipping it")
    endif()

  endforeach()

  set(MYSQL_EXTERNAL_DEPENDENCIES "${MYSQL_EXTERNAL_DEPENDENCIES}" PARENT_SCOPE)

  # check if --libs have libs search path
  set(MYSQL_EXTERNAL_SEARCHPATH "")

  string(REGEX MATCHALL " -L[^ ]+" config_lib_search_path ${config_libs_paths})
  message("-- libs search path: ${config_lib_search_path}")

  foreach(search ${config_lib_search_path})

    string(REGEX REPLACE " -L([^ ]+)" "\\1" search ${search})
    message("-- checking search path: ${search}")


    list(APPEND MYSQL_EXTERNAL_SEARCHPATH ${search})
  endforeach()

  set(MYSQL_EXTERNAL_SEARCHPATH "${MYSQL_EXTERNAL_SEARCHPATH}" PARENT_SCOPE)

endfunction(use_mysql_config)


##################################################################
# mysql_config helpers


# ----------------------------------------------------------------------
#
# Macro that runs "mysql_config ${_opt}" and return the line after
# trimming away ending space/newline.
#
# _mysql_conf(
#   _var    - output variable name, will contain a ';' separated list
#   _opt    - the flag to give to mysql_config
#
# ----------------------------------------------------------------------

macro(_mysql_conf _var _opt)
  execute_process(
    COMMAND ${MYSQL_CONFIG_EXECUTABLE} ${_opt}
    OUTPUT_VARIABLE ${_var}
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
endmacro()

# ----------------------------------------------------------------------
#
# Macro that runs "mysql_config ${_opt}", selects output args using a
# regex, and clean it up a bit removing space/tab/newline before
# setting it to a variable.
#
# _mysql_config(
#   _var    - output variable name, will contain a ';' separated list
#   _regex  - regular expression matching the prefix of args to select
#   _opt    - the flag to give to mysql_config
#
# ----------------------------------------------------------------------

macro(_mysql_config _var _regex _opt)
  _mysql_conf(_mysql_config_output ${_opt})
  string(REGEX MATCHALL "${_regex}([^ ]+)" _mysql_config_output "${_mysql_config_output}")
  string(REGEX REPLACE "^[ \t]+" "" _mysql_config_output "${_mysql_config_output}")
  IF(CMAKE_SYSTEM_NAME MATCHES "SunOS")
    string(REGEX REPLACE " -latomic" "" _mysql_config_output "${_mysql_config_output}")
  ENDIF()
  string(REGEX REPLACE "${_regex}" "" _mysql_config_output "${_mysql_config_output}")
  separate_arguments(_mysql_config_output)
  set(${_var} ${_mysql_config_output})
endmacro()


######################################################################

main()
return()
