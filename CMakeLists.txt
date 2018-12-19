# Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
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


CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
CMAKE_POLICY(VERSION 2.8.12)
cmake_policy(SET CMP0022 NEW)
cmake_policy(SET CMP0023 OLD)

SET(BUILDTYPE_DOCSTRING
 "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or
 CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel")

#
# If cmake is invoked with -DCMAKE_BUILD_TYPE,
# respect user wishes and do not (re)define CMAKE_BUILD_TYPE. Otherwise,
# use Relwithdebinfo.
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



##########################################################################

PROJECT(MySQL_CONCPP)

message("Building on system: ${CMAKE_SYSTEM} (${CMAKE_SYSTEM_PROCESSOR})")
message("Using cmake generator: ${CMAKE_GENERATOR}")
#message("Compiler identification: ${CMAKE_CXX_COMPILER_ID}")

if(CMAKE_SYSTEM_NAME MATCHES "SunOS")
  set(SUNOS ON CACHE INTERNAL "")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "SunPro")
  set(SUNPRO ON CACHE INTERNAL "")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.15")
    message(WARNING "Connector/C++ requires SunPro version 5.15 or later.")
  endif()
  add_compile_options(-m64)
endif()

if((CMAKE_SIZEOF_VOID_P EQUAL 8) OR SUNPRO)
  set(IS64BIT TRUE)
  message("Using 64bit generator")
else()
  set(IS64BIT FALSE)
  message("Using 32bit genereator")
endif()


#
# On Windows, set VS to "vsNN" where NN is the MSVC version.
#

unset(VS)
if(WIN32)
  if(MSVC12)
    set(VS "vs12")
  elseif(MSVC14)
    set(VS "vs14")
  endif()
endif()


include(version.cmake)

# Base name for the connector libraries.

set(LIB_NAME_BASE "mysqlcppconn${CONCPP_VERSION_MAJOR}")
set(JDBC_LIB_NAME_BASE "mysqlcppconn")

#
# Set up cmake module search patch
# ================================
#

# First, use modules that are local to this project

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Then use modules from CDK sub-project

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cdk/cmake)

#
# Library utilities such as add_library_ex()
#

include(libutils)

#
# Install settings
# ================
#

include(install_layout.cmake)

set(
  BUNDLE_DEPENDENCIES OFF CACHE BOOLEAN
  "If enabled, external libraries used by the connector, such as openSSL,
   will be installed together with the connector library"
)

#
#  Default install location
#

if(NOT CMAKE_INSTALL_PREFIX)

  if(WIN32)

    if(DEFINED ENV{HOMEPATH})
      file(TO_CMAKE_PATH "$ENV{HOMEDRIVE}$ENV{HOMEPATH}" install_home)
    else()
      set(install_home "C:/Program Files (x86)")
    endif()
    set(CMAKE_INSTALL_PREFIX "${install_home}/MySQL/MySQL Connector C++ ${CONCPP_PACKAGE_BASE_VERSION}")

  else()

    set(CMAKE_INSTALL_PREFIX "/usr/local/mysql/connector-c++-${CONCPP_PACKAGE_BASE_VERSION}")

  endif()

endif()


#
# Compiler settings
# =================
#

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(SUNPRO)

  set(options
    -m64
    -mt
    -xatomic=studio
    -xtarget=generic     # more portable code
    -xdebuginfo=no%decl  # faster build times
    # optimization options
    -xbuiltin=%all
    -xlibmil
  )

  if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "sparc")
    list(APPEND options -nofstore)
  endif()

  string(REPLACE ";" " " options "${options}")

  set(CMAKE_CXX_FLAGS "${options} ${CMAKE_CXX_FLAGS}")

  #foreach(type EXE STATIC SHARED MODULE)
  #  set(CMAKE_${type}_LINKER_FLAGS "${options}")
  #endforeach()

  set(CMAKE_CXX_FLAGS_DEBUG "-g ${CMAKE_CXX_FLAGS_DEBUG}")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
    "-g -xO2 ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
  )
  set(CMAKE_CXX_FLAGS_RELEASE "-g0 -xO3 ${CMAKE_CXX_FLAGS_RELEASE}")
  set(CMAKE_CXX_FLAGS_MINSIZEREL
    "-g0 -xO3 -xsize ${CMAKE_CXX_FLAGS_MINSIZEREL}"
  )

  add_definitions(
    -D_POSIX_PTHREAD_SEMANTICS
    -D_REENTRANT
  )

endif()

#
# Enable C++11
#

set(CPP11FLAGS)

if (
  CMAKE_COMPILER_IS_GNUCXX OR SUNPRO
  OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"
)
  list(APPEND CPP11FLAGS -std=c++11)
endif()

# With SunPro use GNU libstdc++ implementation of c++ std library. This
# is the default for -std=c++11, but we set it explicitly to be on the safe
# side.

if(SUNPRO)
  list(APPEND CPP11FLAGS -library=stdcpp)
endif()


#
# If Clang is used and deployment target is not specified
# with MACOSX_DEPLOYMENT_TARGET environment variable, make
# sure that clang's native implementation of C++ std
# libarary (libc++) is used. Otherwise clang defaults to
# GNU version (libstdc++) which is outdated and does
# not handle C++11 well.
#
# TODO: Add option to use the default runtime if user wishes
# so.
#

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang"
   AND NOT DEFINED ENV{MACOSX_DEPLOYMENT_TARGET}
   AND NOT CMAKE_SYSTEM_NAME MATCHES "Linux")

  list(APPEND CPP11FLAGS -stdlib=libc++)

endif()

string(REPLACE ";" " " CPP11FLAGS "${CPP11FLAGS}")
set(CMAKE_CXX_FLAGS "${CPP11FLAGS} ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_REQUIRED_FLAGS "${CPP11FLAGS} ${CMAKE_REQUIRED_FLAGS}")

#
# Tell cmake to not use compiler specific extensions
#

INCLUDE(compiler_features)
SET(CMAKE_CXX_EXTENSIONS no)

#
# Configure static runtime library on Windows if requested
#

option(STATIC_MSVCRT "Use static MSVC runtime library" OFF)

if(WIN32 AND STATIC_MSVCRT)

message("Using static runtime library")

foreach(LANG C CXX)
  set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} /MT")
  foreach(TYPE RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_${LANG}_FLAGS_${TYPE} "${CMAKE_${LANG}_FLAGS_${TYPE}} /MT")
  endforeach()
  set(CMAKE_${LANG}_FLAGS_DEBUG "${CMAKE_${LANG}_FLAGS_DEBUG} /MTd")
endforeach(LANG)

endif()

#
# Enable MSVC Debug Format Information
# Produces an .obj file containing full symbolic debugging information for use
# with the debugger.
#

if(WIN32)
  foreach(LANG C CXX)
    set(CMAKE_${LANG}_FLAGS_DEBUG "${CMAKE_${LANG}_FLAGS_DEBUG} /Z7")
  endforeach()
endif()

#
# Disable MSVC unreachable code warnings unless requested.
#

option(UNREACHABLE_CODE_WARNINGS "Enable unreachable code compiler warnings")
mark_as_advanced(UNREACHABLE_CODE_WARNINGS)

if(WIN32 AND NOT UNREACHABLE_CODE_WARNINGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4702")
endif()

#
# Disable MSVC warning C4297 as it seems to be buggy.
# Seehttps://connect.microsoft.com/VisualStudio/feedback/details/897611/incorrect-warning-of-c4297-for-destructor-with-try-catch-statement
#

if(WIN32 AND NOT UNREACHABLE_CODE_WARNINGS)
  add_compile_options(/wd4297)
endif()

#
# Disable unsecure warnings as our current code uses std library methods
# that trigger them (and we know what we are doing!)
#

if(WIN32)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS)
endif()


if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
  add_definitions(-DNOMINMAX)
endif()


#
# Gcov support (Linux only)
#

include(gcov)
#message("WITH_COVERAGE: ${WITH_COVERAGE}")

#message("flags: ${CMAKE_C_FLAGS}")
#message("c++ flags: ${CMAKE_CXX_FLAGS}")
#foreach(TYPE DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
#  message("${TYPE} flags: ${CMAKE_C_FLAGS_${TYPE}}")
#  message("c++ ${TYPE} flags: ${CMAKE_CXX_FLAGS_${TYPE}}")
#endforeach()


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

option(BUILD_STATIC "Build static version of connector library" OFF)

if(BUILD_STATIC)
  message("Building static connector library")
  set(LIB_TYPE STATIC)

  set_property(
    DIRECTORY .
    APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<NOT:$<CONFIG:Static>>:CONCPP_BUILD_STATIC>
  )

else()
  message("Building shared connector library")

  if(WIN32 AND STATIC_MSVCRT)
    message(SEND_ERROR "Shared library should not use static runtime.")
  endif()

  set(LIB_TYPE SHARED)

  set_property(
    DIRECTORY .
    APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<NOT:$<CONFIG:Static>>:CONCPP_BUILD_SHARED>
  )

  #TODO: do not set options below when building Static configuration?

  set(CMAKE_POSITION_INDEPENDENT_CODE ON)

  # Hide all symbols that are not explicitly exported.
  # Note: setting target property CXX_VISIBILITY did not work for
  # object libraries that we use to build the connector.

  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-fvisibility-ms-compat)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "SunPro")
    add_compile_options(-xldscope=symbolic)
  elseif(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-fvisibility=hidden)
  endif()

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

OPTION(WITH_TESTS "Build project's unit tests" 0)

#
# Note: if building static connector on windows, the type of
# runtime library used by tests must match that used by the connector
# (which is statically linked with tests). This must also match the
# runtime type used by gtest library (which we also link statically to).
#

if(WITH_TESTS AND BUILD_STATIC)
  set(STATIC_TESTS_MSVCRT ${STATIC_MSVCRT})
endif()


OPTION(THROW_AS_ASSERT
  "Turn THROW() statements in the code into asserts for easier debugging"
  0)

mark_as_advanced(THROW_AS_ASSERT)

if(THROW_AS_ASSERT)
  add_definitions(-DTHROW_AS_ASSERT)
endif()


#
# We use UUID generator from common sub-module.
#

set(WITH_UUID "${PROJECT_SOURCE_DIR}/cdk/extra/uuid")

if(NOT EXISTS "${WITH_UUID}/src/CMakeLists.txt")
  message(FATAL_ERROR "Could not find UUID generator sources at: ${WITH_UUID}")
endif()


#
# CDK
#

SET(WITH_CDK_DOC 0)
OPTION(WITH_CDK_TESTS "cdk tests" OFF)
set(WITH_PIC ON)

ADD_SUBDIRECTORY(cdk)
INCLUDE_DIRECTORIES(${CDK_INCLUDE_DIR})
INCLUDE_DIRECTORIES(cdk/parser)


#
# Unit tests framework
#

INCLUDE(testing)
SETUP_TESTING()

ADD_TEST_INCLUDES(${PROJECT_SOURCE_DIR}/testing)
ADD_TEST_LIBRARIES(connector)

#
# Project's public headers
#

ADD_SUBDIRECTORY(include)
INCLUDE_DIRECTORIES(include)

#
# Set higher warning level for the main connector code.
#

if(WIN32)

  # 4127 = conditional expression is constant (needed for do {...} while(false))
  # 4512 = assignment operator could not be generated
  #
  # Note: 4512 is disabled because according to C++11 standard the situations
  # that triggers this warning should be handled automatically by the compiler
  # (and this is the case for MSVC 2015).
  # See: http://en.cppreference.com/w/cpp/language/copy_assignment

  add_compile_options(/W4 /wd4512 /wd4127)

else()

  set(TEST_COMPILE_FLAGS "-Wno-sign-compare")

  if(CMAKE_CXX_COMPILER_ID MATCHES "SunPro")
    add_compile_options(
      -errtags=yes
    )
  else()
    add_compile_options(-Wextra)

  endif()

endif()


#
# Connector/C++ components
#

option(WITH_JDBC
 "Whether to build a variant of connector library which implements legacy JDBC API"
 OFF
)

if(WITH_JDBC)
  include(jdbc.cmake)
endif()

ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(common)
ADD_SUBDIRECTORY(xapi)
ADD_SUBDIRECTORY(devapi)

#
# Packaging
#
ADD_SUBDIRECTORY(packaging)

#
#  Target which builds the final connector library
#  ===============================================
#

# Generate the main connector library.

add_library_ex(connector ${LIB_TYPE}
    ${WITH_UUID}/src/uuid_gen.cc
    OBJECTS xapi devapi common
    LIBS    cdk
)

target_include_directories(connector PRIVATE "${WITH_UUID}/include")


if(MAINTAINER_MODE)

add_library_ex(connector-static STATIC EXCLUDE_FROM_ALL
    ${WITH_UUID}/src/uuid_gen.cc
    OBJECTS xapi devapi common
    LIBS    cdk
)

target_include_directories(connector-static PRIVATE "${WITH_UUID}/include")

endif()

# Disable warnings in code which is not ours.

if(MSVC)
  set_source_files_properties(${WITH_UUID}/src/uuid_gen.cc
    PROPERTIES COMPILE_FLAGS "/wd4244 /wd4311 /wd4302"
  )
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
# Pick output library name
# ------------------------
#
# The library name base is mysqlcppconnX where X is the major version
# of Connector/C++ product.
#
# Static library has -static suffix added to the base name.
#
# On Windows we add major ABI version to the shared library name, so that
# different ABI versions of the library can be installed next to each other.
# Also, on Windows we distinguish the MSVC version used to build the library
# (as this determines the runtime version). The shared libraries use
# -vsNN suffix, the import library does not have the suffix but is installed
# to a vsNN/ subfolder of the library install location (see install layout
# below). For static libraries, we add -mt suffix if it is linked with
# static runtime.
#

set(LIB_NAME_STATIC "${LIB_NAME_BASE}-static")
if(WIN32 AND STATIC_MSVCRT)
  set(LIB_NAME_STATIC "${LIB_NAME}-mt")
endif()

if(BUILD_STATIC)

  set(LIB_NAME ${LIB_NAME_STATIC})

else()

  set(LIB_NAME "${LIB_NAME_BASE}")
  if(WIN32)
    set(LIB_NAME "${LIB_NAME}-${ABI_VERSION_MAJOR}")
  endif()
  if(VS)
    set(LIB_NAME "${LIB_NAME}-${VS}")
  endif()

endif()


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

if(MAINTAINER_MODE)

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

ADD_EXECUTABLE(try EXCLUDE_FROM_ALL try.cc)
TARGET_LINK_LIBRARIES(try connector)
SET_INTERFACE_OPTIONS(try devapi)
#  ADD_GCOV(try)

if(WITH_JDBC)

  add_executable(try_jdbc EXCLUDE_FROM_ALL try_jdbc.cc)
  target_link_libraries(try_jdbc ${JDBC_LIBS})
  add_dependencies(try_jdbc build_jdbc)
  target_include_directories(try_jdbc PRIVATE ${JDBC_INCLUDES})

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

add_dependencies(link_test connector)
if(WITH_JDBC)
  add_dependencies(link_test build_jdbc)
endif()

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
  include(PackageSpecs.cmake)
  ADD_SUBDIRECTORY(packaging/WiX)
endif()

