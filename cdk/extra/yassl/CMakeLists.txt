# Copyright (c) 2006, 2014, Oracle and/or its affiliates. All rights reserved.
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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

INCLUDE(install_macros)
INCLUDE(msvc)

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/taocrypt/include
  ${CMAKE_CURRENT_SOURCE_DIR}/taocrypt/mySTL)

ADD_DEFINITIONS(${SSL_DEFINES})

# Speed up build process excluding unused header files
ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN -DNOGDI)

# We want to use std::min/std::max, not the windows.h macros
ADD_DEFINITIONS(-DNOMINMAX)


#MY_CHECK_CXX_COMPILER_FLAG("-Wno-unused-const-variable" HAVE_NO_UNUSED_CONST_VAR)

IF(CMAKE_GENERATOR MATCHES "Xcode" AND HAVE_NO_UNUSED_CONST_VAR)
  INCLUDE(${MYSQL_CMAKE_SCRIPT_DIR}/compile_flags.cmake)
  ADD_COMPILE_FLAGS(
    src/yassl_int.cpp
    src/socket_wrapper.cpp
    COMPILE_FLAGS -Wno-unused-const-variable)
ENDIF()

INCLUDE (CheckTypeSize)

CHECK_TYPE_SIZE("long"      SIZEOF_LONG)
CHECK_TYPE_SIZE("long long" SIZEOF_LONG_LONG)

ADD_DEFINITIONS(-DSIZEOF_LONG=${SIZEOF_LONG} -DSIZEOF_LONG_LONG=${SIZEOF_LONG_LONG})

SET(YASSL_SOURCES  src/buffer.cpp src/cert_wrapper.cpp src/crypto_wrapper.cpp src/handshake.cpp src/lock.cpp
        src/log.cpp src/socket_wrapper.cpp src/ssl.cpp src/timer.cpp src/yassl_error.cpp
        src/yassl_imp.cpp src/yassl_int.cpp)

ADD_LIBRARY(yassl STATIC ${YASSL_SOURCES})
#RESTRICT_SYMBOL_EXPORTS(yassl)

ADD_SUBDIRECTORY(taocrypt)
TARGET_LINK_LIBRARIES(yassl taocrypt)

IF(MSVC)
  TARGET_COMPILE_OPTIONS(yassl PRIVATE /wd4267 /wd4334)
ELSE()
  TARGET_COMPILE_OPTIONS(yassl PRIVATE -Wno-unused-but-set-variable)
ENDIF()

#if(WIN32)
#  if(NOT WINDOWS_RUNTIME_MD)
#    CHANGE_MD_2_MT()
#  endif()
#endif()
