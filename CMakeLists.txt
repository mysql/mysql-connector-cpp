# Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
#
# The MySQL Connector/C++ is licensed under the terms of the GPLv2
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
# MySQL Connectors. There are special exceptions to the terms and
# conditions of the GPLv2 as it is applied to this software, see the
# FLOSS License Exception
# <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published
# by the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA


CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
CMAKE_POLICY(VERSION 2.8.12)
cmake_policy(SET CMP0022 NEW)

#
# Prevent cmake from setting its default value of CMAKE_INSTALL_PREFIX
# inside PROJECT() command.
# Thanks to this, we can detect if user has set CMAKE_INSTALL_PREFIX
# or not, and then set our own default in the latter case.
#
set(CMAKE_INSTALL_PREFIX "" CACHE PATH "Install location")

PROJECT(MySQL_CONCPP)

message("Building on system: ${CMAKE_SYSTEM} (${CMAKE_SYSTEM_PROCESSOR})")
message("Using cmake generator: ${CMAKE_GENERATOR}")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(IS64BIT 1)
  message("Using 64bit generator")
else()
  message("Using 32bit genereator")
endif()

include(version.cmake)


# Base name for the connector libraries.

#set(LIB_NAME_BASE "mysqlcppconn2")
set(LIB_NAME_BASE "mysqlcppconn${CONCPP_VERSION_MAJOR}")

#
# Set up cmake module search patch
# ================================
#

# First, use modules that are local to this project

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Then use modules from CDK sub-project

LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cdk/cmake)

include(libutils)

#
# Compiler settings
# =================
#

#
# Tell cmake to not use compiler specific extensions
#

INCLUDE(compiler_features)
SET(CMAKE_CXX_EXTENSIONS no)

#
# If Clang is used and deployment target is not specified
# with MACOSX_DEPLOYMENT_TARGET environment variable, make
# sure that clang's native implementation of C++ runtime
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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4297")
endif()


if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
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
# Main project and its dependencies
# =================================
#

option(BUILD_STATIC "Build static version of connector library" OFF)

if(BUILD_STATIC)
  message("Building static connector library")
  set(LIB_TYPE STATIC)
  add_definitions(-DCONCPP_BUILD_STATIC)
else()
  message("Building shared connector library")

  if(WIN32 AND STATIC_MSVCRT)
    message(SEND_ERROR "Shared library should not use static runtime.")
  endif()

  set(LIB_TYPE SHARED)
  add_definitions(-DCONCPP_BUILD_SHARED)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)

  # Note: setting target property CXX_VISIBILITY did not work for
  # object libraries that we use to build the connector.

  if(CMAKE_COMPILER_IS_GNUCXX)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
      add_compile_options(-fvisibility-ms-compat)
    elseif()
      add_compile_options(-fvisibility=hidden)
    endif()
  endif()

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
# Add Boost
#

ADD_DEFINITIONS(-DBOOST_NO_AUTO_PTR=1)
INCLUDE(boost)
SETUP_BOOST()

#
# CDK
#

SET(WITH_CDK_DOC 0)
OPTION(WITH_CDK_TESTS "cdk tests" OFF)
set(WITH_PIC ${CMAKE_POSITION_INDEPENDENT_CODE})


if(NOT DEFINED WITH_SSL)
SET(WITH_SSL "bundled" CACHE STRING "")
endif()

IF(WITH_SSL)
  IF(WITH_SSL STREQUAL "bundled")
    ADD_DEFINITIONS(-DWITH_SSL_YASSL)
  ENDIF()
  ADD_DEFINITIONS(-DWITH_SSL)
ENDIF()



ADD_SUBDIRECTORY(cdk)
INCLUDE_DIRECTORIES(${CDK_INCLUDE_DIR})
INCLUDE_DIRECTORIES(cdk/parser)

#
# Unit tests framework
#

INCLUDE(testing)
SETUP_TESTING()

ADD_TEST_INCLUDES(${PROJECT_SOURCE_DIR}/testing)
ADD_TEST_LIBRARIES(libconcpp)

#
# Project's public headers
#

ADD_SUBDIRECTORY(include)
INCLUDE_DIRECTORIES(include)

#
# Set higher warning level for the main connector code.
#

foreach(LANG C CXX)

if(WIN32)

  # 4127 = conditional expression is constant (needed for do {...} while(false))
  # 4512 = assignment operator could not be generated
  #
  # Note: 4512 is disabled because according to C++11 standard the situations
  # that triggers this warning should be handled automatically by the compiler
  # (and this is the case for MSVC 2015).
  # See: http://en.cppreference.com/w/cpp/language/copy_assignment

  set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} /W4 /wd4512 /wd4127")

else()

  set(CMAKE_${LANG}_FLAGS "${CMAKE_${LANG}_FLAGS} -Wextra")
  set(TEST_COMPILE_FLAGS "-Wno-sign-compare")

endif()

endforeach(LANG)


#
# Connector/C++ components
#

ADD_SUBDIRECTORY(doc)
ADD_SUBDIRECTORY(xapi)
ADD_SUBDIRECTORY(devapi)


#
#  Target which builds the final connector library
#  ===============================================
#

# Generate the main connector library.

add_library_ex(libconcpp ${LIB_TYPE}
    ${WITH_UUID}/src/uuid_gen.cc
    OBJECTS devapi xapi
    LIBS    cdk
)

target_include_directories(libconcpp PRIVATE "${WITH_UUID}/include")

# Disable warnings in code which is not ours.

if(MSVC)
  set_source_files_properties(${WITH_UUID}/src/uuid_gen.cc
    PROPERTIES COMPILE_FLAGS "/wd4244 /wd4311 /wd4302"
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
# On Windows we distinguish the MSVC version used to build the library
# (as this determines the runtime version). The shared libraries use
# -vsNN suffix, the import library does not have the suffix but is installed
# to a vsNN/ subfolder of the library install location (see install layout
# below). For static libraries, we add -mt suffix if it is linked with
# static runtime.
#

set(VS)
if(WIN32)
  if(MSVC12)
    set(VS "vs12")
  elseif(MSVC14)
    set(VS "vs14")
  endif()
endif()

if(BUILD_STATIC)

  set(LIB_NAME "${LIB_NAME_BASE}-static")
  if(WIN32 AND STATIC_MSVCRT)
    set(LIB_NAME "${LIB_NAME}-mt")
  endif()

else()

  set(LIB_NAME "${LIB_NAME_BASE}")
  if(VS)
    set(LIB_NAME "${LIB_NAME}-${VS}")
  endif()

endif()


set_property(TARGET libconcpp PROPERTY OUTPUT_NAME ${LIB_NAME})
message("Connector library name: ${LIB_NAME}")

if(NOT BUILD_STATIC)
  set_property(TARGET libconcpp PROPERTY ARCHIVE_OUTPUT_NAME ${LIB_NAME_BASE})
endif()

set_target_properties(libconcpp PROPERTIES
  VERSION "${API_VERSION}"
  SOVERSION "${API_VERSION_MAJOR}"
)

#
# Pick install location for the main library
# ------------------------------------------
#
# On Windows the install layout is as follows, where NN is the MSVC version
# used to build the connector:
#
#  {lib,lib64}/mysqlcppconnX-vsNN.dll              <-- shared library
#  {lib,lib64}/vsNN/mysqlcppconnX-static.lib       <-- static with /MD
#  {lib,lib64}/vsNN/mysqlcppconnX-static-mt.lib    <-- static with /MT
#  {lib,lib64}/vsNN/mysqlcppconnX.lib              <-- import library for DLL
#
# On Linux it is as follows, where A.B is the API version number
#
#  {lib,lib64}/libmysqlcppconnX.so.A.B             <-- shared library
#  {lib,lib64}/libmysqlcppconnX.so.A               <-- soname link
#  {lib,lib64}/libmysqlcppconnX.so                 <-- development link
#  {lib,lib64}/libmysqlcppconnX-static.a           <-- static library
#
# Additionally, if connector is built in debug mode, the libraries are installed
# in debug/ subfolder of {lib,lib64}/ or {lib,lib64}/vsNN/.
#

set(LIB_DIR "" CACHE PATH "Library install location (relative to install root)")

if(NOT LIB_DIR)
  if(IS64BIT)
    set(LIB_DIR "lib64")
  else()
    set(LIB_DIR "lib")
  endif()
endif()

message("Connector library will be installed at: ${LIB_DIR}")

set(LIB_DIR_STATIC "${LIB_DIR}")
if(VS)
  set(LIB_DIR_STATIC "${LIB_DIR_STATIC}/${VS}")
endif()


install(TARGETS libconcpp
  CONFIGURATIONS Release RelWithDebInfo
  ARCHIVE DESTINATION "${LIB_DIR_STATIC}"
  RUNTIME DESTINATION "${LIB_DIR}"
  LIBRARY DESTINATION "${LIB_DIR}"
)

install(TARGETS libconcpp
  CONFIGURATIONS Debug
  ARCHIVE DESTINATION "${LIB_DIR_STATIC}/debug"
  RUNTIME DESTINATION "${LIB_DIR}/debug"
  LIBRARY DESTINATION "${LIB_DIR}/debug"
)


#
# Tests
# =====
#

#
# Enable C++11 for test code that can use DevAPI
#

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  # Use C++11 also for CHECK_CXX_SOURCE_COMPILES()
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++11")
endif()

remove_definitions(-DCONCPP_BUILD_STATIC)
remove_definitions(-DCONCPP_BUILD_SHARED)

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

ADD_EXECUTABLE(try try.cc)
TARGET_LINK_LIBRARIES(try libconcpp)
SET_INTERFACE_OPTIONS(try devapi)
#  ADD_GCOV(try)

#
# Show dynamic library dependencies for try program.
#
# TODO: Use a cmake module for that
# TODO: Do it also for the shared library
#

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
endif()

#
# Linking test
#
# This test compiles test application using internall installation of built
# connector. It is important to work with installed files because otherwise
# cmake does its magic to resolve missing dependencies when buiding test code.
# We don't want this to happen to make sure that test code can be built with
# connector library only, as we distribute it.
#
# Note: internall installation into <binary_dir>/install is done by directly
# executing cmake_install.cmake script which is generated by cmake.
#
# TODO: Also test dynamic linking (requires adopting test app project)
#

file(REMOVE_RECURSE ${PROJECT_BINARY_DIR}/link_test)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/link_test)

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
    -D BUILD_STATIC=${BUILD_STATIC}
    -D STATIC_MSVCRT=${STATIC_MSVCRT}
    ${PROJECT_SOURCE_DIR}/testapp
  COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIGURATION>  --clean-first
  DEPENDS libconcpp
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/link_test
  VERBATIM
)

add_test(NAME Link_test
  COMMAND ${CMAKE_COMMAND} --build . --target link_test --config $<CONFIGURATION>
  WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
)


#
# Packaging specifications
# ========================
#

if(NOT CMAKE_INSTALL_PREFIX)

  if(WIN32)

    if(DEFINED ENV{HOMEPATH})
      string(REPLACE "\\" "/" install_prefix "$ENV{HOMEDRIVE}$ENV{HOMEPATH}")
    else()
      set(install_prefix "C:/Program Files (x86)")
    endif()
    set(CMAKE_INSTALL_PREFIX "${install_prefix}/MySQL/MySQL Connector C++ 2.0")

  else()

    set(CMAKE_INSTALL_PREFIX "/usr/local/mysql/connector-c++-2.0")

  endif()

endif()

message("Install location: ${CMAKE_INSTALL_PREFIX}")

option(WITH_PACKAGES "Configure for building binary/source packages" OFF)
if(WITH_PACKAGES)
  include(PackageSpecs.cmake)
endif()

