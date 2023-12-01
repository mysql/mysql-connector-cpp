# Copyright (c) 2023, Oracle and/or its affiliates. All rights reserved.
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
# This cmake script is used to run tests from the "find_package" test suite. It
# is invoked by tests created by the test suite project.
#

cmake_policy(SET CMP0012 NEW)

include("${CDK_DIR}/cmake/setup.cmake")
include(utils)
include(config_options)

add_config_option(CONFIG STRING "Build configuratoin (Release or Debug)")
add_config_option(BUILD_STATIC BOOL "Whether to link to static connector library")
add_config_option(CONCPP_SRC_DIR PATH "Location of Con/CPP sources")
add_config_option(WITH_SSL PATH "Custom OpenSSL install location")
add_config_option(WITH_JDBC BOOL "Whether to cover JDBC connector variant")

add_config_option(VERSION STRING "Version to test in version test")
add_config_option(EXACT BOOL "Whether exact version should be requested by version test")
add_config_option(EXTRA BOOL "Whether extra checks should be done by version test")

check_vars(CONCPP_SRC_DIR)


if(NOT EXISTS "${CONCPP_SRC_DIR}/testapp")
  show_config_options()
  abort("Could not find test application project at CONCPP_SRC_DIR")
endif()

macro(main)

  if(ACTION STREQUAL "debug")
    action_debug()
  elseif(ACTION STREQUAL "vertest")
    action_vertest()
  elseif(ACTION STREQUAL "config")
    action_config()
  elseif(ACTION STREQUAL "build")
    action_build()
  elseif(ACTION STREQUAL "check")
    action_check()
  endif()

endmacro(main)


function(action_debug)
  set(COMPONENTS "debug")
  set(VERSION "")
  action_vertest()
endfunction()


function(action_vertest)

  check_vars(VERSION)

  make_dir(ver_test)

  configure_file(
    "${CONCPP_SRC_DIR}/testing/find_package/version_test.cmake.in"
    ver_test/CMakeLists.txt
    @ONLY
  )

  set(config_opts "-DVERSION=${VERSION}" "-DWITH_DEBUG=${WITH_DEBUG}")

  if(EXACT)
    list(APPEND config_opts "-DEXACT=1")
  endif()

  if(EXTRA)
    list(APPEND config_opts "-DEXTRA=1")
  endif()

  if(COMPONENTS)
    list(APPEND config_opts "-DCOMPONENTS=${COMPONENTS}")
  endif()

  if(DEFINED MYSQL_CONCPP_DIR)
    list(APPEND config_opts "-Dmysql-concpp_DIR=${MYSQL_CONCPP_DIR}")
  endif()

  check_execute("Configuring test project"
    ${CMAKE_COMMAND} . ${config_opts}
    WORKING_DIRECTORY ver_test
  )

endfunction(action_vertest)



function(action_config)

  check_vars(CONFIG)

  message(STATUS "Build configuration: ${CONFIG}")

  make_dir(config_test)

  set(config_opts)

  if(DEFINED MYSQL_CONCPP_DIR)
    list(APPEND config_opts "-Dmysql-concpp_DIR=${MYSQL_CONCPP_DIR}")
  endif()


  list(APPEND config_opts "-DCMAKE_BUILD_TYPE=${CONFIG}")

  foreach(opt BUILD_STATIC WITH_SSL WITH_JDBC)
    if(DEFINED ${opt})
      list(APPEND config_opts "-D${opt}=${${opt}}")
    endif()
  endforeach()

  message("\n==== Configuring test application project =====\n")
  message("using config options: ${config_opts}")
  check_execute("Configuration failed"
    ${CMAKE_COMMAND} -B config_test -S "${CONCPP_SRC_DIR}/testapp"
    # Note: CMAKE_BUILD_TYPE is used or not depending on the generator
    --no-warn-unused-cli
    -Dmysql-concpp_FIND_VERBOSE=1
    ${config_opts}
  #  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )

endfunction(action_config)


function(action_build)

  message("\n==== Building test application =====\n")
  check_execute("Build failed"
    ${CMAKE_COMMAND} --build config_test --config ${CONFIG} --verbose
  #  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
  )

endfunction(action_build)


function(action_check)

  if(CONFIG MATCHES "Debug")
    set(out_dir "run/debug")
  else()
    set(out_dir "run")
  endif()

  message("\n==== Check the binary ====")

  find_program(LDD_COMMAND ldd)

  foreach(exec devapi_test xapi_test jdbc_test)

    set(exec_path)
    find_program(exec_path 
      NAMES ${exec}
      PATHS "config_test/${out_dir}"
      NO_DEFAULT_PATH
      NO_CACHE
    )

    if(NOT EXISTS "${exec_path}")
      continue()
    endif()

    if(LDD_COMMAND)

      message("\n== dependencies of: ${exec_path}\n")
      execute_process(
        COMMAND ${LDD_COMMAND} "${exec_path}"
        OUTPUT_VARIABLE foo
        ERROR_VARIABLE foo
      #  ECHO_OUTPUT_VARIABLE  # note: not available in 3.15
      )

      if(foo MATCHES "not found")
        abort("Unresolved dependencies")
      endif()

    elseif(WIN32)

      # Note: We run the app which must fail but in case of missing dependencies it produces error 0xc0000135

      message(STATUS "Checking: ${exec_path}")

      execute_process(
        COMMAND "${exec_path}" --help
        OUTPUT_QUIET  ERROR_QUIET
        RESULT_VARIABLE res
      )

      if("${res}" MATCHES "0xc")
        abort("Unresolved dependencies")
      endif()

    endif()

  endforeach()

endfunction(action_check)

main()
message("Done!")