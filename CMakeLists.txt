# Copyright (c) 2015, 2019, Oracle and/or its affiliates. All rights reserved.
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


CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

CMAKE_POLICY(VERSION 3.1)
cmake_policy(SET CMP0022 NEW)


SET(BUILDTYPE_DOCSTRING
 "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or
 CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel")

#
# If cmake is invoked with -DCMAKE_BUILD_TYPE,
# respect user wishes and do not (re)define CMAKE_BUILD_TYPE. Otherwise,
# use Debug by default.
#
IF(NOT DEFINED CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Debug" CACHE STRING
  ${BUILDTYPE_DOCSTRING} FORCE)
ENDIF()

#
# Prevent cmake from setting its default value of CMAKE_INSTALL_PREFIX
# inside PROJECT() command.
# Thanks to this, we can detect if user has set CMAKE_INSTALL_PREFIX
# or not, and then set our own default in the latter case.
#
set(CMAKE_INSTALL_PREFIX "" CACHE PATH "Install location")

#
# Enable grouping targets into folders for IDE tools
#
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


##########################################################################

PROJECT(MySQL_CONCPP)

include(version.cmake)

# Load cmake modules

include(cdk/cmake/setup.cmake)
include(cmake/setup.cmake)

include(platform)
include(dependency)      # find_dependency()
include(config_options)  # add_config_option()
include(libutils)        # merge_libraries()

message("Building on system: ${CMAKE_SYSTEM} (${CMAKE_SYSTEM_PROCESSOR})")
message("Using cmake generator: ${CMAKE_GENERATOR}")
#message("Compiler identification: ${CMAKE_CXX_COMPILER_ID}")
if(DEFINED CMAKE_GENERATOR_TOOLSET)
  message("Using toolset: ${CMAKE_GENERATOR_TOOLSET}")
endif()


if(SUNPRO)
  if(SUNPRO VERSION_LESS "5.15")
    message(WARNING "Connector/C++ requires SunPro version 5.15 or later.")
  endif()
  set_arch_m64()
endif()

#
# Note: On Solaris cmake's built-in checks for bit size use plain compiler
# options which imply 32-bit architecture. Above we force 64-bit but this
# is not reflected in IS64BIT flag.
#

if(IS64BIT OR SUNPRO)
  message("Building 64bit code")
else()
  message("Building 32bit code")
endif()


#
# Install settings
# ================
#

include(install_layout.cmake)

add_config_option(BUNDLE_DEPENDENCIES BOOL ADVANCED DEFAULT OFF
  "If enabled, external libraries used by the connector, such as openSSL,"
  " will be installed together with the connector library"
)


#
# Compiler settings
# =================
#

enable_pic()
enable_cxx11()

#
# Configure static runtime library on Windows if requested
#

add_config_option(STATIC_MSVCRT BOOL ADVANCED DEFAULT OFF
  "Use static MSVC runtime library"
)

if(WIN32 AND STATIC_MSVCRT)
  message("Using static runtime library")
  set_msvcrt(STATIC)
endif()

# Other MSVC settings

if(MSVC)
  add_compile_options(/bigobj)
  add_definitions(-DNOMINMAX)
endif()


#
# Gcov support (Linux only)
#

find_dependency(Coverage)  # defines add_coverage() command
#message("WITH_COVERAGE: ${WITH_COVERAGE}")


if(0)
message("flags: ${CMAKE_C_FLAGS}")
message("c++ flags: ${CMAKE_CXX_FLAGS}")
foreach(TYPE DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
  message("${TYPE} flags: ${CMAKE_C_FLAGS_${TYPE}}")
  message("c++ ${TYPE} flags: ${CMAKE_CXX_FLAGS_${TYPE}}")
endforeach()
endif()


#
# Linker settings
# ===============
#

#
# Produce rpath dependent libraries on MacOS
# see: <https://developer.apple.com/library/content/documentation/DeveloperTools/Conceptual/DynamicLibraries/100-Articles/RunpathDependentLibraries.html#//apple_ref/doc/uid/TP40008306-SW1>
# and: <https://cmake.org/cmake/help/v3.0/prop_tgt/MACOSX_RPATH.html>
#

if(APPLE)
  set(CMAKE_MACOSX_RPATH ON)
endif()


#
# Main project and its dependencies
# =================================
#

add_config_option(BUILD_STATIC BOOLEAN DEFAULT OFF
  "Build static version of connector library"
)

if(BUILD_STATIC)

  message("Building static connector library")
  set(BUILD_SHARED_LIBS OFF)

  set_property(
    DIRECTORY .
    APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<NOT:$<CONFIG:Static>>:CONCPP_BUILD_STATIC>
  )

else()

  message("Building shared connector library")
  set(BUILD_SHARED_LIBS ON)

  if(WIN32 AND STATIC_MSVCRT)
    message(SEND_ERROR "Shared library should not use static runtime.")
  endif()

  if(WITH_COVERAGE)
    message(WARNING "To get good coverage results use static build of the"
            " connector (BUILD_STATIC)")
  endif()

  # Note: Using set_propery() instead of add_compile_options() because we
  # want to remove these defs later, when building test code.

  set_property(
    DIRECTORY .
    APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<NOT:$<CONFIG:Static>>:CONCPP_BUILD_SHARED>
  )
  #add_compile_options($<$<NOT:$<CONFIG:Static>>:-DCONCPP_BUILD_SHARED>)

  # Hide all symbols that are not explicitly exported.

  set_visibility(hidden)

endif()

if(MAINTAINER_MODE)

  list(APPEND CMAKE_CONFIGURATION_TYPES Static)

  set(CMAKE_CXX_FLAGS_STATIC ${CMAKE_CXX_FLAGS_RELEASE})
  set(CMAKE_C_FLAGS_STATIC ${CMAKE_C_FLAGS_RELEASE})
  set(CMAKE_SHARED_LINKER_FLAGS_STATIC ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
  set(CMAKE_EXE_LINKER_FLAGS_STATIC ${CMAKE_EXE_LINKER_FLAGS_RELEASE})

  set_property(
    DIRECTORY .
    APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<CONFIG:Static>:CONCPP_BUILD_STATIC>
  )

endif()


add_config_option(WITH_TESTS BOOL ADVANCED DEFAULT OFF
  "Build project's unit tests"
)

#
# Note: if building static connector on windows, the type of
# runtime library used by tests must match that used by the connector
# (which is statically linked with tests). This must also match the
# runtime type used by gtest library (which we also link statically to).
#

if(WITH_TESTS AND BUILD_STATIC)
  set(STATIC_TESTS_MSVCRT ${STATIC_MSVCRT})
endif()


add_config_option(THROW_AS_ASSERT BOOL ADVANCED DEFAULT OFF
  "Turn THROW() statements in the code into asserts for easier debugging"
)

if(THROW_AS_ASSERT)
  add_definitions(-DTHROW_AS_ASSERT)
endif()

#
#  Warnings
#
# TODO: Fix these warnings.
#

if(MSVC)

  add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

  # Disable MSVC unreachable code warnings unless requested.
  add_compile_options(/wd4702)

  # Disable MSVC warning C4297 as it seems to be buggy.
  # Seehttps://connect.microsoft.com/VisualStudio/feedback/details/897611/incorrect-warning-of-c4297-for-destructor-with-try-catch-statement

  add_compile_options(/wd4297)

endif()


if(SUNPRO)

  add_compile_options(
    -errtags=yes -erroff=hidevf,wvarhidemem
  )

endif()


#
# CDK
#

add_subdirectory(cdk)

foreach(T cdk cdk_foundation cdk_mysqlx cdk_proto_mysqlx cdk_parser)
  set_target_properties(${T} PROPERTIES FOLDER "CDK")
endforeach()

#
# Unit tests framework
#

INCLUDE(testing)
SETUP_TESTING()

ADD_TEST_INCLUDES(${PROJECT_SOURCE_DIR}/testing)
ADD_TEST_LIBRARIES(connector)

if(MSVC)
else()
  set(TEST_COMPILE_FLAGS "-Wno-sign-compare")
endif()


#
# Project's public headers
#

ADD_SUBDIRECTORY(include)
INCLUDE_DIRECTORIES(include)

#
# The legacy connector, if selected.
# Note: it is included before setting higher warning levels.
#

add_config_option(WITH_JDBC BOOL DEFAULT OFF
 "Whether to build a variant of connector library which implements legacy JDBC API"
)

if(WITH_JDBC)
  add_subdirectory(jdbc)
endif()


#
# Set higher warning level for the main connector code.
#

if(MAINTAINER_MODE)

  if(MSVC)

    # 4127 = conditional expression is constant (needed for do {...} while(false))
    # 4512 = assignment operator could not be generated
    #
    # Note: 4512 is disabled because according to C++11 standard the situations
    # that triggers this warning should be handled automatically by the compiler
    # (and this is the case for MSVC 2015).
    # See: http://en.cppreference.com/w/cpp/language/copy_assignment

    add_compile_options(/W4 /wd4512 /wd4127)

  elseif(SUNPRO)
  else()

      add_compile_options(-Wextra)

  endif()

endif(MAINTAINER_MODE)


#
# Connector/C++ components
#


add_subdirectory(doc)
add_subdirectory(common)
add_subdirectory(xapi)
add_subdirectory(devapi)


#
#  Target which builds the final connector library
#  ===============================================
#

# Generate the main connector library.

merge_libraries(connector xapi devapi)
target_include_directories(connector PUBLIC "${PROJECT_SOURCE_DIR}/include")

if(MAINTAINER_MODE)

  merge_libraries(connector-static STATIC xapi devapi)
  target_include_directories(connector-static PUBLIC "${PROJECT_SOURCE_DIR}/include")

endif()


#
# Embed rpath information in the connector library.
#

set_property(TARGET connector PROPERTY BUILD_WITH_INSTALL_RPATH ON)

# The $ORIGIN/@loader_path entry tells to look for dependent libraries in the
# location where our connector library is stored.

if(APPLE)
  set_property(TARGET connector APPEND PROPERTY INSTALL_RPATH "@loader_path")
elseif(NOT WIN32)
  set_property(TARGET connector APPEND PROPERTY INSTALL_RPATH "$ORIGIN")
endif()


if(0)
#
#  Add command to show rpath information
#

if(APPLE)
  set(list_rpath_cmd otool -l $<TARGET_FILE:libconcpp> "|" grep RPATH -A2)
elseif(NOT WIN32)
  set(list_rpath_cmd objdump -x $<TARGET_FILE:libconcpp> "|" grep RPATH -A2)
endif()

add_custom_command(TARGET libconcpp POST_BUILD
  COMMAND ${list_rpath_cmd}
  COMMENT "RPATH setting for: $<TARGET_FILE_NAME:mysqlcppconn>"
)
endif()


#
# Install specifications
# ----------------------
#
# Note: Locations and names are configured in install_layout.cmake
#


set_property(TARGET connector PROPERTY OUTPUT_NAME ${LIB_NAME})
message("Connector library name: ${LIB_NAME}")

if(NOT BUILD_STATIC)
  set_property(TARGET connector PROPERTY ARCHIVE_OUTPUT_NAME ${LIB_NAME_BASE})
endif()

set_target_properties(connector PROPERTIES
  VERSION "${ABI_VERSION_MAJOR}.${CONCPP_VERSION}"
  SOVERSION "${ABI_VERSION_MAJOR}"
)


install(TARGETS connector
  CONFIGURATIONS Release RelWithDebInfo
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR_STATIC}" COMPONENT XDevAPIDev
  RUNTIME DESTINATION "${INSTALL_LIB_DIR}" COMPONENT XDevAPIDll
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT XDevAPIDll
)

install(TARGETS connector
  CONFIGURATIONS Debug
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR_STATIC}/debug" COMPONENT XDevAPIDev
  RUNTIME DESTINATION "${INSTALL_LIB_DIR}/debug" COMPONENT XDevAPIDll
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}/debug" COMPONENT XDevAPIDll
)

if(MSVC AND NOT BUILD_STATIC)

  install(FILES $<TARGET_PDB_FILE:connector>
    CONFIGURATIONS RelWithDebInfo
    DESTINATION "${INSTALL_LIB_DIR}"
    COMPONENT Debuginfo
  )

  install(FILES $<TARGET_PDB_FILE:connector>
    CONFIGURATIONS Debug
    DESTINATION "${INSTALL_LIB_DIR}/debug"
    COMPONENT Debuginfo
  )

endif()


if(TARGET connector-static)

  set_property(TARGET connector-static PROPERTY OUTPUT_NAME ${LIB_NAME_STATIC})

  set_target_properties(connector-static PROPERTIES
    VERSION "${ABI_VERSION_MAJOR}.${CONCPP_VERSION}"
    SOVERSION "${ABI_VERSION_MAJOR}"
  )

  install(TARGETS connector-static
    CONFIGURATIONS Static
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR_STATIC}" COMPONENT XDevAPIDev
    RUNTIME DESTINATION "${INSTALL_LIB_DIR}" COMPONENT XDevAPIDll
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT XDevAPIDll
  )

  if(TARGET connector-jdbc-static)
    add_dependencies(connector-static connector-jdbc-static)
  endif()

endif()


#
# Tests
# =====
#

#
# Note: We must clear compile flags - the ones used to build the connector
# are not good for building client code that uses the connector.
#

set_property(
  DIRECTORY .
  PROPERTY COMPILE_DEFINITIONS ""
)

if(BUILD_STATIC)
  add_definitions(-DSTATIC_CONCPP)
endif()


# Auto-generated test targets

IF (WITH_TESTS)
  # Unit tests declared with ADD_NG_TEST() (see cdk/cmake/testing.cmake)
  ADD_TEST_TARGET()

  # Test for public headers declared with ADD_HEADERS()
  # (see cdk/cmake/headers.cmake)
  ADD_HEADERS_TEST()
ENDIF (WITH_TESTS)

#
# Sample code to try things out
#

add_executable(try EXCLUDE_FROM_ALL try.cc)
target_link_libraries(try connector)


if(WITH_JDBC)

  add_executable(try_jdbc EXCLUDE_FROM_ALL try_jdbc.cc)
  target_link_libraries(try_jdbc connector-jdbc)
  target_include_directories(try_jdbc
    PRIVATE "${PROJECT_BINARY_DIR}/include/mysql"
    PRIVATE "${PROJECT_BINARY_DIR}/include/jdbc/cppconn"
  )

endif()

#
# Show dynamic library dependencies for try program.
#
# TODO: Use a cmake module for that
# TODO: Do it also for the shared library
#

if(NOT WIN32)

find_program(LDD ldd)
if(NOT LDD)
  find_program(LDD otool)
  if(LDD)
    set(LDD_OPTS "-L" CACHE INTERNAL "options for linker debugger")
  endif()
endif()

if(LDD)

  add_custom_command(TARGET try POST_BUILD
    COMMAND ${LDD} ${LDD_OPTS} $<TARGET_FILE:try>
    COMMENT "Checking dynamic library dependencies:"
  )

  if(WITH_JDBC)
    add_custom_command(TARGET try_jdbc POST_BUILD
      COMMAND ${LDD} ${LDD_OPTS} $<TARGET_FILE:try_jdbc>
      COMMENT "Checking dynamic library dependencies:"
    )
  endif()

endif()

endif()

#
# Linking test
# ------------
#
# This test compiles test application using internal installation of built
# connector. It is important to work with installed files because otherwise
# cmake does its magic to resolve missing dependencies when building test code.
# We don't want this to happen to make sure that test code can be built with
# connector library only, as we distribute it.
#
# Note: internal installation into <binary_dir>/install is done by directly
# executing cmake_install.cmake script which is generated by cmake.
#
# TODO: Also test dynamic linking (requires adopting test app project)
#

file(REMOVE_RECURSE ${PROJECT_BINARY_DIR}/link_test)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/link_test)

unset(jdbc_options)
if(WITH_JDBC)
  #message("-- BOOST_ROOT: ${BOOST_ROOT}")
  list(APPEND jdbc_options -DWITH_JDBC=ON -DWITH_BOOST=${BOOST_ROOT})
endif()

add_custom_target(link_test
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_BINARY_DIR}/install
  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/install
  COMMAND ${CMAKE_COMMAND}
    -D CMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/install
    -D CMAKE_INSTALL_CONFIG_NAME=$<$<BOOL:$<CONFIGURATION>>:$<CONFIGURATION>>$<$<NOT:$<BOOL:$<CONFIGURATION>>>:Release>
    -P ${PROJECT_BINARY_DIR}/cmake_install.cmake
  COMMAND ${CMAKE_COMMAND} -E remove -f ${PROJECT_BINARY_DIR}/link_test/CMakeCache.txt
  COMMAND ${CMAKE_COMMAND}
    -G "${CMAKE_GENERATOR}"
    -D WITH_CONCPP=${PROJECT_BINARY_DIR}/install
    -D WITH_SSL="${WITH_SSL}"
    -D BUILD_STATIC=${BUILD_STATIC}
    -D STATIC_MSVCRT=${STATIC_MSVCRT}
    ${jdbc_options}
    ${PROJECT_SOURCE_DIR}/testapp
  COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIGURATION>  --clean-first
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/link_test
  VERBATIM
)

set_target_properties(link_test PROPERTIES FOLDER "Tests")

add_dependencies(link_test connector)
#if(TARGET connector-merge)
#  add_dependencies(link_test connector-merge)
#endif()
#if(WITH_JDBC)
#  add_dependencies(link_test build_jdbc)
#endif()

# TDOD: Use ${CMAKE_COMMAND}, but evaluated at test time, not here.

add_test(NAME Link_test
  COMMAND cmake --build . --target link_test --config $<CONFIGURATION>
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)

#
# Create the INFO_SRC and INFO_BIN files
# =========================
#

include(buildinfo.cmake)


#
# Packaging specifications
# ========================
#

message("Install location: ${CMAKE_INSTALL_PREFIX}")
message("Connector libraries will be installed at: ${INSTALL_LIB_DIR}")

option(WITH_PACKAGES "Configure for building binary/source packages" OFF)

if(WITH_PACKAGES)
  ADD_SUBDIRECTORY(packaging)
endif()


show_config_options()
