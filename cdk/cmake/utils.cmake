# Copyright (c) 2019, Oracle and/or its affiliates. All rights reserved.
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

if(COMMAND abort)
  return()
endif()


function(abort)
  message(FATAL_ERROR "!!! " ${ARGN})
endfunction(abort)


# Check that given variables are defined

macro(check_vars)
  foreach(var ${ARGN})      
  if(NOT DEFINED ${var})
    abort("Required variable not defined: ${var}")
  endif()
  endforeach(var ${ARGN})
endmacro()


#
# Add test fixture named NAME defined by tests NAME_setup/cleanup (if they 
# exists). Additional setup/cleanup tests with names of the form NAME_foo can 
# be added by listing additional test name suffixes. For example the following 
# will add fixture `FIX` with setup tests  FIX_setup (if exists) FIX_foo 
# FIX_bar  and cleanup tests  FIX_cleanup (if exists) FIX_baz.
#
#  add_fixture(FIX foo bar CLEANUP baz)
#

function(add_fixture NAME)

  set(setup_tests)
  set(cleanup_tests)

  if(TEST "${NAME}_setup")
    list(APPEND setup_tests "${NAME}_setup")
  endif()

  if(TEST "${NAME}_cleanup")
    list(APPEND cleanup_tests "${NAME}_cleanup")
  endif()

  while(ARGN)
    list(POP_FRONT ARGN test)
    if(test STREQUAL "SETUP")
      continue()
    endif()
    if(test STREQUAL "CLEANUP")
      break()
    endif()
    list(APPEND setup_tests "${NAME}_${test}")
  endwhile()

  while(ARGN)
    list(POP_FRONT ARGN test)
    list(APPEND cleanup_tests "${NAME}_${test}")
  endwhile()

  message(STATUS "Adding fixture ${NAME}: ${setup_tests} / ${cleanup_tests}")

  if(setup_tests)
    set_property(TEST ${setup_tests} PROPERTY FIXTURES_SETUP ${NAME})
  endif()
  if(cleanup_tests)
    set_property(TEST ${cleanup_tests} PROPERTY FIXTURES_CLEANUP ${NAME})
  endif()

endfunction()


#
# Compatibility layer for OS operations
#

function(check_execute MSG)

  execute_process(
    COMMAND ${ARGN}
    RESULT_VARIABLE res
  )

  if(res)
    message(FATAL_ERROR "!!! ${MSG}")
  endif()

endfunction()

# Note: Relative to the binary dir!

function(get_path VAR PATH)

  if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.19)
    file(REAL_PATH "${PATH}" path BASE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
  else()
    # TODO: remove this when we can work with at least cmake 3.19
    get_filename_component(path
      "${PATH}"
      ABSOLUTE
      BASE_DIR ${CMAKE_CURRENT_BINARY_DIR}
    )
  endif()

  set(${VAR} "${path}" PARENT_SCOPE)
  
endfunction()


function(make_dir DIR)
  get_path(DIR "${DIR}")
  dir_remove("${DIR}")
  file(MAKE_DIRECTORY ${DIR})
endfunction()

function(make_symlink OLD NEW)
  # TODO: use file(CREATE_LINK ...) for cmake version 3.14+
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${OLD} ${NEW}
  )
endfunction()

function(file_copy SRC DEST)
  configure_file(${SRC} ${DEST} COPYONLY)
endfunction()

function(dir_copy SRC DST)
  file(COPY ${SRC}/ DESTINATION ${DST})
endfunction()

function(dir_remove DIR)
  #message(STATUS "removing dir: ${DIR}")
  file(REMOVE_RECURSE ${DIR})
endfunction()
