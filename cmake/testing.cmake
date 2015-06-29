# Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#
# Infrastructure for defining unit tests for the project
# ======================================================
#
# We use gtest framework. Add source files defining unit tests using:
#
#   ADD_NG_TESTS(<sources>)
#
# If unit tests require a library, add it using:
#
#   ADD_TEST_LIBRARIES(<list of libraries>)
#
# Additional include paths required by unit tests should be specified with:
#
#   ADD_TEST_INCLUDES(<list of include paths>)
#
# Macro ADD_TEST_TARGET() should be called after all unit tests are registered
# with above macros. It generates run_unit_tests target which will run all the
# tests and update_test_groups target which will generate file with ctest
# definitions used to integrate with ctest framework.
#

IF(NOT DEFINED WITH_TESTS)
  OPTION(WITH_TESTS "Build project's unit tests" ON)
ENDIF()

IF(NOT DEFINED WITH_COVERAGE)
  OPTION(WITH_COVERAGE "Enable coverage support for gcc" OFF)
ENDIF()

IF(WITH_TESTS)
  IF(WITH_COVERAGE)
    MESSAGE("Building CDK tests (with coverage, if supported)")
  ELSE()
    MESSAGE("Building CDK tests")
  ENDIF()
ENDIF()

MACRO(ADD_GCOV target)
IF (WITH_TESTS)
  IF(WITH_COVERAGE)
    MESSAGE(STATUS "Enabling coverage support for gcc")

    SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS "-g -O0 -Wall -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated -Woverloaded-virtual -Wwrite-strings -fprofile-arcs -ftest-coverage ")
    SET_TARGET_PROPERTIES(${target} PROPERTIES LINK_FLAGS "-fprofile-arcs -ftest-coverage")

  ENDIF(WITH_COVERAGE)
ENDIF (WITH_TESTS)

ENDMACRO(ADD_GCOV)

MACRO(SETUP_TESTING)
IF(WITH_TESTS)
  INCLUDE(CTest)
  INCLUDE(gtest)
  SETUP_GTEST()
ENDIF()
ENDMACRO(SETUP_TESTING)

#
# Determine location of files accompanying this testing.cmake module which are
# in testing/ sub-folder of the location where the module is located
#

GET_FILENAME_COMPONENT(tests_dir ${CMAKE_CURRENT_LIST_FILE} PATH)
SET(tests_dir "${tests_dir}/testing" CACHE INTERNAL
    "Location of testing.cmake support files" FORCE)

#
# Global variable test_sources collects all source files with tests
#

MACRO(test_source_add src)
  LIST(APPEND test_sources ${src})
  SET(test_sources ${test_sources} CACHE INTERNAL
      "Unit tests defined in the project" FORCE)
ENDMACRO(test_source_add)

# Reset test_sources to be initially empty

SET(test_sources "")
test_source_add("")  # to clear cache entry

#
# Global variable test_libs collects all libraries required by
# unit tests
#

MACRO(test_lib_add lib)
  LIST(APPEND test_libs ${lib})
  SET(test_libs ${test_libs} CACHE INTERNAL
      "Libraries used by unit tests" FORCE)
ENDMACRO(test_lib_add)

# Reset test_libs to be initially empty

SET(test_libs "")
test_lib_add("")  # to clear cache entry

#
# Global variable test_includes collects all include paths required by
# unit tests
#

MACRO(test_includes_add path)
  LIST(APPEND test_includes ${path})
  SET(test_includes ${test_includes} CACHE INTERNAL
      "Include paths used by unit tests" FORCE)
ENDMACRO(test_includes_add)

# Reset test_includes to be initially empty

SET(test_includes "")
test_includes_add("")  # to clear cache entry


#
# Global variable test_environment collects all include paths required by
# unit tests
#

MACRO(test_environment_add env_var)
  LIST(APPEND test_environment ${env_var})
  SET(test_environment ${test_environment} CACHE INTERNAL
      "Environment Vars used by unit tests" FORCE)
ENDMACRO(test_environment_add)

# Reset test_includes to be initially empty

SET(test_environment "")
test_environment_add("")  # to clear cache entry

MACRO(ADD_NG_TESTS)

IF(WITH_TESTS)

  FOREACH(src ${ARGN})

    GET_FILENAME_COMPONENT(path ${src} ABSOLUTE)
    test_source_add(${path})
    MESSAGE(STATUS "Added unit tests: ${src}")

  ENDFOREACH(src)

ENDIF()

ENDMACRO(ADD_NG_TESTS)


MACRO(ADD_TEST_LIBRARIES)

IF(WITH_TESTS)

  FOREACH(lib ${ARGN})

    test_lib_add(${lib})
    MESSAGE(STATUS "Added test library: ${lib}")

  ENDFOREACH(lib)

ENDIF()

ENDMACRO(ADD_TEST_LIBRARIES)


MACRO(ADD_TEST_INCLUDES)

IF(WITH_TESTS)

  FOREACH(path ${ARGN})

    test_includes_add(${path})
    MESSAGE(STATUS "Added test include path: ${path}")

  ENDFOREACH(path)

ENDIF()

ENDMACRO(ADD_TEST_INCLUDES)

MACRO(ADD_TEST_ENVIRONMENT)

IF(WITH_TESTS)

  FOREACH(env_var ${ARGN})

    test_environment_add(${env_var})
    MESSAGE(STATUS "Added environment var: ${env_var}")

  ENDFOREACH(env_var)

ENDIF()

ENDMACRO(ADD_TEST_ENVIRONMENT)

#
# Define run_unit_ests and update_test_groups targets
#

MACRO(ADD_TEST_TARGET)

IF(WITH_TESTS)

  #MESSAGE("Adding run test target for unit tests from: ${test_sources}")
  #MESSAGE("Test libraries: ${test_libs}")

  #
  # Generate main() function for run_unit_tests
  #
  CONFIGURE_FILE(${tests_dir}/test_main.in ${CMAKE_CURRENT_BINARY_DIR}/tests_main.cc @ONLY)

  #
  # Define run_unit_tests target
  #
  SET(target_run_unit_tests ${cdk_target_prefix}run_unit_tests
    CACHE INTERNAL "CDK unit test target")

  ADD_EXECUTABLE(${target_run_unit_tests}
                 ${CMAKE_CURRENT_BINARY_DIR}/tests_main.cc
                 ${test_sources}
                 ${CMAKE_SOURCE_DIR}/common/process_launcher/process_launcher.cc
                 ${CMAKE_SOURCE_DIR}/common/common/exception.cc
                )
  INCLUDE_DIRECTORIES(${test_includes})
  INCLUDE_DIRECTORIES(
                      ${CMAKE_SOURCE_DIR}/common/process_launcher
                      ${CMAKE_SOURCE_DIR}/common/common
                     )
  TARGET_LINK_LIBRARIES(${target_run_unit_tests} gtest)
  ADD_GCOV(${target_run_unit_tests})
  ADD_BOOST(${target_run_unit_tests})

  #
  # Link with libraries required by unit tests
  #
  FOREACH(tlib ${test_libs})
    TARGET_LINK_LIBRARIES(${target_run_unit_tests} ${tlib})
  ENDFOREACH()

  #
  #  Add ctest definitions for each gtest group
  #

  SET(test_group_defs ${CMAKE_CURRENT_BINARY_DIR}/TestGroups.cmake)

  set(TEST_ENV ${test_environment})

  IF (NOT EXISTS ${test_group_defs})
    FILE(WRITE ${test_group_defs} "")
  ENDIF()

  INCLUDE(${test_group_defs})


  ADD_CUSTOM_TARGET(${cdk_target_prefix}update_test_groups
     run_unit_tests --generate_test_groups=${test_group_defs}
     SOURCES ${tests_dir}/test_main.in
  )

ENDIF()

ENDMACRO(ADD_TEST_TARGET)


