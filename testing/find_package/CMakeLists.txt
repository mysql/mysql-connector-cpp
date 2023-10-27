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
# This project defines tests to verify that cmake package configuration file
# included in our packages works as expected. See README.txt for more details.
#

cmake_minimum_required(VERSION 3.15)

set(HOME_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE PATH INTERNAL)
set(SRC_DIR "${HOME_DIR}/../..")
set(CDK_DIR "${SRC_DIR}/cdk" CACHE PATH INTERNAL)

project(CPP_cmake_config NONE)

include("${CDK_DIR}/cmake/setup.cmake")
include(utils)
include(config_options)

# TODO: Option aliases

add_config_option(WITH_CONCPP PATH
  "Location of Con/C++ installation."
)
# option_alias(CONCPP_DIR WITH_CONCPP)

add_config_option(WITH_SSL PATH
  "Custom OpenSSL install location for the corresponding test scenario."
)

add_config_option(WITH_JDBC BOOL DEFAULT 1 "Whether to cover JDBC connector variant.")

add_config_option(VERSION STRING
  "Version of the Con/C++ installation being tested (in case it could not be determined automatically)."
)

add_config_option(BUILD_STATIC BOOL DEFAULT 1
  "If disabled then test scenarios where static connector library is used will not be generated. This is needed when testing packages that do not ship static libraries."
)

add_config_option(WITH_DEBUG BOOL DEFAULT 0
  "Whether to add test scenarios on Windows that test building with debug variants of the connector libraries. This should be enabled only on Windows and only if the debug package was installed."
)

add_config_option(SSL_DEFAULT BOOL ADVANCED DEFAULT 1
  "Whether to add test scenarios that expect OpenSSL to be installed system-wide or bundled in the package."
)


if(WITH_DEBUG AND NOT WIN32)

  message("Warning: "
    "Option WITH_DEBUG works only on Windows, ignoring it."
  )
  set(WITH_EBUG 0)

endif()


#
# Testing 'REQUIRE debug' clause of `find_package()`.
#

macro(add_debug_tests)

  if(WITH_CONCPP)
    set(test_opts "-DMYSQL_CONCPP_DIR=${WITH_CONCPP}")
  endif()

  add_test(NAME debug_require
    COMMAND ${CMAKE_COMMAND}
    "-DCDK_DIR=${CDK_DIR}"
    "-DCONCPP_SRC_DIR=${HOME_DIR}/../.."
    -DACTION=debug
    ${test_opts}
    -P "${HOME_DIR}/run_test.cmake"
  )

  if(NOT WITH_DEBUG)
    set_property(TEST debug_require PROPERTY WILL_FAIL 1)
  endif()

  message(STATUS "# added test: debug_require")

endmacro(add_debug_tests)


#
#  Testing 'X.Y.Z' version clause of `find_package()`.
#

macro(add_version_tests)

  # Determine installed version (if not explicitly given)

  if(NOT VERSION)

    if(WITH_CONCPP)

      find_file(INFO_SRC 
        NAME INFO_SRC
        PATHS ${WITH_CONCPP}
        NO_DEFAULT_PATH
        NO_CACHE
      )

    else()

      file(GLOB find
        "C:/Program Files*/MySQL/MySQL Connector C++ */INFO_SRC"
        "/usr/local/mysql/connector-c++-*/INFO_SRC"
      )

      if(find)
        list(GET find 0 INFO_SRC)
      endif()

    endif()

    #message(STATUS "INFO_SRC: ${INFO_SRC}")

    if(INFO_SRC)

      file(STRINGS "${INFO_SRC}" line REGEX " *version  *:" )
      string(REGEX REPLACE " *version *:" "" line ${line})
      #message(STATUS "info line: ${line}")
      string(STRIP "${line}" VERSION)

    endif()

  endif()

  if(VERSION)

    message(STATUS "Conector/C++ version: ${VERSION}")

  else()

    message(
      "Note: Connector/C++ version could not be determined,"
      " version tests will not be configured."
      " You can still add these tests by setting VERSION config option."
    )

    return()

  endif()


  set(VERSION_exact ${VERSION})
  set(VERSION_different "5.0.0")

  string(REPLACE "." ";" ver ${VERSION})
  list(GET ver 0 MAJOR)
  list(GET ver 1 MINOR)
  math(EXPR NEWER "${MINOR}+1")
  set(VERSION_newer "${MAJOR}.${NEWER}.0")

  if(MINOR GREATER 0)
    math(EXPR OLDER "${MINOR}-1")
    set(VERSION_older "${MAJOR}.${OLDER}.0")
  endif()

  foreach(A older newer exact different)
  foreach(B no yes)

    set(variant "${A}:${B}")
    set(name "version_A${A}_B${B}")

    if(NOT DEFINED VERSION_${A})
      continue()
    endif()

    # Determine if test should fail in this scenario

    set(fails 0)
    if(B STREQUAL "yes" AND NOT A STREQUAL "exact")
      set(fails 1)
    endif()
    if(A MATCHES "newer|different")
      set(fails 1)
    endif()

    # Prepare options for run_test.cmake

    set(test_opts 
      "-DVERSION=${VERSION_${A}}"
      "-DWITH_DEBUG=${WITH_DEBUG}"
    )

    if(B)
      list(APPEND test_opts "-DEXACT=1")
    endif()

    # Do extra checks in scenarios where test should be successful

    if(NOT fails)
      list(APPEND test_opts "-DEXTRA=1")
    endif()

    if(WITH_CONCPP)
      list(APPEND test_opts "-DMYSQL_CONCPP_DIR=${WITH_CONCPP}")
    endif()

    add_test(NAME "${name}"
      COMMAND ${CMAKE_COMMAND}
        "-DCDK_DIR=${CDK_DIR}"
        "-DCONCPP_SRC_DIR=${HOME_DIR}/../.."
        -DACTION=vertest
        ${test_opts}
        -P "${HOME_DIR}/run_test.cmake"
    )

    set_property(TEST ${name} PROPERTY WILL_FAIL ${fails})

    message(STATUS "# added test: ${name}")

  endforeach(B)
  endforeach(A)

endmacro(add_version_tests)


#
#  Main test scenarios.
#

macro(add_tests)

  if(WITH_SSL)

    find_library(openssl_lib
      NAMES ssl libssl
      PATHS ${WITH_SSL}
      PATH_SUFFIXES lib
      NO_DEFAULT_PATH
      NO_CACHE
    )

  endif()

  if(NOT openssl_lib)
    message("Note: D=custom scenarios are skipped because WITH_SSL is not set or OpenSSL libraries were not found there.")
  else()
    get_filename_component(WITH_SSL "${openssl_lib}" DIRECTORY)
    message(STATUS "For D=custom scenarios using OpenSSL library at: ${WITH_SSL}")
  endif()

  foreach(A dynamic static)
  foreach(B Release Debug)
  foreach(C default custom)   # OpenSSL dependency handling

    set(name_suffix "A${A}_B${B}_C${C}")
    set(variant "${A}:${B}:${C}")

    if(NOT BUILD_STATIC AND A STREQUAL "static")
      continue()
    endif()

    # Note: On Windows scenarios which build in Debug mode with static
    # connector library are added only if debug package is available.

    if(WIN32 
      AND A STREQUAL "static" 
      AND B STREQUAL "Debug"
      AND NOT WITH_DEBUG
    )
      continue()
    endif()

    # Do not add scenarios that expect OpenSSL at default locations
    # if SSL_DEFAULT option disables them.

    if(NOT SSL_DEFAULT AND C STREQUAL "default")
      continue()
    endif()

    # Prepare options for run_test.cmake

    set(test_opts)

    foreach(opt WITH_CONCPP WITH_JDBC)
      if(DEFINED ${opt})
        list(APPEND test_opts "-D${opt}=${${opt}}")
      endif()
    endforeach()

    list(APPEND test_opts "-DCONFIG=${B}")

    if(A STREQUAL "static")
      list(APPEND test_opts "-DBUILD_STATIC=1")
    endif()

    if(C STREQUAL "custom")
      if(NOT WITH_SSL)
        continue()
      endif()
      list(APPEND test_opts "-DWITH_SSL=${WITH_SSL}")
    endif()

    # Add tests:
    #   config_...  - configure project that builds test app
    #   build_...   - build test application
    #   check_...   - check the executable
    #
    # They are implemented by run_test.cmake -- see there.

    add_test("config_${name_suffix}" 
      ${CMAKE_COMMAND}
      "-DCDK_DIR=${CDK_DIR}"
      "-DCONCPP_SRC_DIR=${HOME_DIR}/../.."
      -DACTION=config
      ${test_opts}
      -P "${HOME_DIR}/run_test.cmake"
    )

    message(STATUS "# added test: config_${name_suffix}")

    set_property(TEST "config_${name_suffix}" PROPERTY
      FIXTURES_SETUP "${name_suffix}_configured"
    )

    add_test("build_${name_suffix}" 
      ${CMAKE_COMMAND}
      "-DCDK_DIR=${CDK_DIR}"
      "-DCONCPP_SRC_DIR=${HOME_DIR}/../.."
      -DACTION=build
      ${test_opts}
      -P "${HOME_DIR}/run_test.cmake"
    )

    set_property(TEST "build_${name_suffix}" PROPERTY
      FIXTURES_SETUP "${name_suffix}_built"
    )
    set_property(TEST "build_${name_suffix}" PROPERTY
      FIXTURES_REQUIRED "${name_suffix}_configured"
    )

    message(STATUS "# added test: build_${name_suffix}")

    add_test("check_${name_suffix}" 
      ${CMAKE_COMMAND}
      "-DCDK_DIR=${CDK_DIR}"
      "-DCONCPP_SRC_DIR=${HOME_DIR}/../.."
      -DACTION=check
      ${test_opts}
      -P "${HOME_DIR}/run_test.cmake"
    )

    set_property(TEST "check_${name_suffix}" PROPERTY
      FIXTURES_REQUIRED "${name_suffix}_built"
    )

    message(STATUS "# added test: check_${name_suffix}")

  endforeach(C)
  endforeach(B)
  endforeach(A)

endmacro(add_tests)

include(CTest)

add_tests()
add_version_tests()
add_debug_tests()

show_config_options()