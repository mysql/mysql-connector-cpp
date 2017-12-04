# Copyright (c) 2009, 2017, Oracle and/or its affiliates. All rights reserved.
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

# We support different versions of SSL:
# - "bundled" uses source code in <source dir>/extra/yassl
# - "system"  (typically) uses headers/libraries in /usr/lib and /usr/lib64
# - a custom installation of openssl can be used like this
#     - cmake -DCMAKE_PREFIX_PATH=</path/to/custom/openssl> -DWITH_SSL="system"
#   or
#     - cmake -DWITH_SSL=</path/to/custom/openssl>
#
# The default value for WITH_SSL is "bundled"
#
# WITH_SSL="system" means: use the SSL library that comes with the operating
# system. This typically means you have to do 'yum install openssl-devel'
# or something similar.
#
# For Windows or OsX, WITH_SSL="system" is handled a bit differently:
# We assume you have installed
#     https://slproweb.com/products/Win32OpenSSL.html
#     find_package(OpenSSL) will locate it
# or
#     http://brewformulas.org/Openssl
#     we give a hint /usr/local/opt/openssl to find_package(OpenSSL)
# When the package has been located, we treat it as if cmake had been
# invoked with  -DWITH_SSL=</path/to/custom/openssl>


SET(WITH_SSL_DOC "bundled (use yassl)")
SET(WITH_SSL_DOC
  "${WITH_SSL_DOC}, yes (prefer os library if present, otherwise use bundled)")
SET(WITH_SSL_DOC
  "${WITH_SSL_DOC}, system (use os library)")
SET(WITH_SSL_DOC
  "${WITH_SSL_DOC}, </path/to/custom/installation>")

MACRO (CHANGE_SSL_SETTINGS string)
  SET(WITH_SSL ${string} CACHE STRING ${WITH_SSL_DOC} FORCE)
ENDMACRO()

MACRO (SET_SSL_DEFINES string)
  SET(SSL_DEFINES ${string} CACHE STRING INTERNAL FORCE)
  MESSAGE(STATUS "SSL_DEFINES=${SSL_DEFINES}")
ENDMACRO(SET_SSL_DEFINES)

MACRO (SET_SSL_LIBRARIES string)
  SET(SSL_LIBRARIES ${string} CACHE STRING INTERNAL FORCE)
  MESSAGE(STATUS "SSL_LIBRARIES=${SSL_LIBRARIES}")
ENDMACRO(SET_SSL_LIBRARIES)

MACRO (MYSQL_USE_BUNDLED_SSL)
  SET(INC_DIRS
    ${CMAKE_SOURCE_DIR}/extra/yassl/include
    ${CMAKE_SOURCE_DIR}/extra/yassl/taocrypt/include
  )
  SET_SSL_LIBRARIES("yassl;taocrypt")
  INCLUDE_DIRECTORIES(SYSTEM ${INC_DIRS})
  SET(SSL_INTERNAL_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/extra/yassl/taocrypt/mySTL)
  SET_SSL_DEFINES("-DWITH_SSL -DWITH_SSL_YASSL -DYASSL_PREFIX -DMULTI_THREADED")
  CHANGE_SSL_SETTINGS("bundled")

  ADD_SUBDIRECTORY(extra/yassl)
ENDMACRO()

MACRO(RESET_SSL_VARIABLES)
  UNSET(WITH_SSL_PATH)
  UNSET(WITH_SSL_PATH CACHE)
  UNSET(OPENSSL_ROOT_DIR)
  UNSET(OPENSSL_ROOT_DIR CACHE)
  UNSET(OPENSSL_INCLUDE_DIR)
  UNSET(OPENSSL_INCLUDE_DIR CACHE)
  UNSET(OPENSSL_APPLINK_C)
  UNSET(OPENSSL_APPLINK_C CACHE)
  UNSET(OPENSSL_LIBRARY)
  UNSET(OPENSSL_LIBRARY CACHE)
  UNSET(CRYPTO_LIBRARY)
  UNSET(CRYPTO_LIBRARY CACHE)
  UNSET(HAVE_SHA512_DIGEST_LENGTH)
  UNSET(HAVE_SHA512_DIGEST_LENGTH CACHE)
ENDMACRO()

# MYSQL_CHECK_SSL
#
# Provides the following configure options:
# WITH_SSL=[yes|bundled|system|<path/to/custom/installation>]
MACRO (MYSQL_CHECK_SSL)
  IF(NOT WITH_SSL)
    CHANGE_SSL_SETTINGS("bundled")
  ENDIF()

  # See if WITH_SSL is of the form </path/to/custom/installation>
  FILE(GLOB WITH_SSL_HEADER ${WITH_SSL}/include/openssl/ssl.h)
  IF (WITH_SSL_HEADER)
    SET(WITH_SSL_PATH ${WITH_SSL} CACHE PATH "path to custom SSL installation")
  ENDIF()

  IF(WITH_SSL STREQUAL "bundled")
    RESET_SSL_VARIABLES()
    MYSQL_USE_BUNDLED_SSL()
  ELSEIF(WITH_SSL STREQUAL "system" OR
      WITH_SSL STREQUAL "yes" OR
      WITH_SSL_PATH
      )
    # Treat "system" the same way as -DWITH_SSL=</path/to/custom/openssl>
    IF((APPLE OR WIN32) AND WITH_SSL STREQUAL "system")
      # FindOpenSSL.cmake knows about
      # http://www.slproweb.com/products/Win32OpenSSL.html
      # and will look for "C:/OpenSSL-Win64/" (and others)
      # For APPLE we set the hint /usr/local/opt/openssl
      IF(STATIC_MSVCRT)
        SET(OPENSSL_MSVC_STATIC_RT ON)
      ENDIF()
      IF(APPLE AND NOT OPENSSL_ROOT_DIR)
        SET(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
      ENDIF()
      FIND_PACKAGE(OpenSSL)
      IF(OPENSSL_FOUND)
        GET_FILENAME_COMPONENT(OPENSSL_ROOT_DIR ${OPENSSL_INCLUDE_DIR} PATH)
        MESSAGE(STATUS "system OpenSSL has root ${OPENSSL_ROOT_DIR}")
        SET(WITH_SSL_PATH "${OPENSSL_ROOT_DIR}" CACHE PATH "Path to system SSL")
      ELSE()
        RESET_SSL_VARIABLES()
        MESSAGE(SEND_ERROR "Could not find system OpenSSL")
      ENDIF()
    ENDIF()

    # First search in WITH_SSL_PATH.
    FIND_PATH(OPENSSL_ROOT_DIR
      NAMES include/openssl/ssl.h
      NO_CMAKE_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      HINTS ${WITH_SSL_PATH}
    )
    # Then search in standard places (if not found above).
    FIND_PATH(OPENSSL_ROOT_DIR
      NAMES include/openssl/ssl.h
    )

    FIND_PATH(OPENSSL_INCLUDE_DIR
      NAMES openssl/ssl.h
      HINTS ${OPENSSL_ROOT_DIR}/include
    )

    IF (WIN32)
      FIND_FILE(OPENSSL_APPLINK_C
        NAMES openssl/applink.c
        HINTS ${OPENSSL_ROOT_DIR}/include
      )
      MESSAGE(STATUS "OPENSSL_APPLINK_C ${OPENSSL_APPLINK_C}")
    ENDIF()


    # On mac this list is <.dylib;.so;.a>
    # On most platforms we still prefer static libraries, so we revert it here.
    IF (WITH_SSL_PATH AND NOT APPLE)
      LIST(REVERSE CMAKE_FIND_LIBRARY_SUFFIXES)
      MESSAGE(STATUS "suffixes <${CMAKE_FIND_LIBRARY_SUFFIXES}>")
    ENDIF()
    FIND_LIBRARY(OPENSSL_LIBRARY
                 NAMES ssl ssleay32 ssleay32MD
                 HINTS ${OPENSSL_ROOT_DIR}/lib)
    FIND_LIBRARY(CRYPTO_LIBRARY
                 NAMES crypto libeay32
                 HINTS ${OPENSSL_ROOT_DIR}/lib)
    IF (WITH_SSL_PATH AND NOT APPLE AND NOT LINUX_STANDALONE)
      LIST(REVERSE CMAKE_FIND_LIBRARY_SUFFIXES)
    ENDIF()

    # Verify version number. Version information looks like:
    #   #define OPENSSL_VERSION_NUMBER 0x1000103fL
    # Encoded as MNNFFPPS: major minor fix patch status
    FILE(STRINGS "${OPENSSL_INCLUDE_DIR}/openssl/opensslv.h"
      OPENSSL_VERSION_NUMBER
      REGEX "^#[ ]*define[\t ]+OPENSSL_VERSION_NUMBER[\t ]+0x[0-9].*"
    )
    STRING(REGEX REPLACE
      "^.*OPENSSL_VERSION_NUMBER[\t ]+0x([0-9]).*$" "\\1"
      OPENSSL_MAJOR_VERSION "${OPENSSL_VERSION_NUMBER}"
    )

    IF(OPENSSL_INCLUDE_DIR AND
       OPENSSL_LIBRARY   AND
       CRYPTO_LIBRARY      AND
       OPENSSL_MAJOR_VERSION STREQUAL "1"
      )
      SET(OPENSSL_FOUND TRUE)
    ELSE()
      SET(OPENSSL_FOUND FALSE)
    ENDIF()

    # If we are invoked with -DWITH_SSL=/path/to/custom/openssl
    # and we have found static libraries, then link them statically
    # into our executables and libraries.
    # Adding IMPORTED_LOCATION allows MERGE_STATIC_LIBS
    # to merge imported libraries as well as our own libraries.
    SET(MY_CRYPTO_LIBRARY "${CRYPTO_LIBRARY}")
    SET(MY_OPENSSL_LIBRARY "${OPENSSL_LIBRARY}")
    IF (WITH_SSL_PATH)
      GET_FILENAME_COMPONENT(CRYPTO_EXT "${CRYPTO_LIBRARY}" EXT)
      GET_FILENAME_COMPONENT(OPENSSL_EXT "${OPENSSL_LIBRARY}" EXT)
      IF (CRYPTO_EXT STREQUAL ".a" OR OPENSSL_EXT STREQUAL ".lib")
        SET(MY_CRYPTO_LIBRARY imported_crypto)
        ADD_IMPORTED_LIBRARY(imported_crypto "${CRYPTO_LIBRARY}")
      ENDIF()
      IF (OPENSSL_EXT STREQUAL ".a" OR OPENSSL_EXT STREQUAL ".lib")
        SET(MY_OPENSSL_LIBRARY imported_openssl)
        ADD_IMPORTED_LIBRARY(imported_openssl "${OPENSSL_LIBRARY}")
      ENDIF()
    ENDIF()

    MESSAGE(STATUS "OPENSSL_INCLUDE_DIR = ${OPENSSL_INCLUDE_DIR}")
    MESSAGE(STATUS "OPENSSL_LIBRARY = ${OPENSSL_LIBRARY}")
    MESSAGE(STATUS "CRYPTO_LIBRARY = ${CRYPTO_LIBRARY}")
    MESSAGE(STATUS "OPENSSL_MAJOR_VERSION = ${OPENSSL_MAJOR_VERSION}")

    INCLUDE(CheckSymbolExists)
    SET(CMAKE_REQUIRED_INCLUDES ${OPENSSL_INCLUDE_DIR})
    CHECK_SYMBOL_EXISTS(SHA512_DIGEST_LENGTH "openssl/sha.h"
                        HAVE_SHA512_DIGEST_LENGTH)
    IF(OPENSSL_FOUND AND HAVE_SHA512_DIGEST_LENGTH)
      SET_SSL_LIBRARIES("${MY_OPENSSL_LIBRARY};${MY_CRYPTO_LIBRARY}")
      INCLUDE_DIRECTORIES(SYSTEM ${OPENSSL_INCLUDE_DIR})
      SET(SSL_INTERNAL_INCLUDE_DIRS "")
      SET_SSL_DEFINES("-DWITH_SSL")
    ELSE()

      RESET_SSL_VARIABLES()

      MESSAGE(SEND_ERROR
        "Cannot find appropriate system libraries for SSL. "
        "Make sure you've specified a supported SSL version. "
        "Consult the documentation for WITH_SSL alternatives")
    ENDIF()
  ELSE()
    MESSAGE(SEND_ERROR
      "Wrong option or path for WITH_SSL. "
      "Valid options are : ${WITH_SSL_DOC}")
  ENDIF()

ENDMACRO()

MACRO(MYSQL_OPENSSL_SPACKAGE INSTALL_LIBDIR INSTALL_LIBDIR_DEBUG)
if(WIN32 OR APPLE)

  if (OPENSSL_LIBRARY OR CRYPTO_LIBRARY)
    # Install/bundle the OpenSSL libraries including the soft links
    install(FILES
      ${OPENSSL_LIBRARY}
      ${CRYPTO_LIBRARY}
      CONFIGURATIONS Release RelWithDebInfo
      DESTINATION ${INSTALL_LIBDIR}
      COMPONENT main
      )

    install(FILES
      ${OPENSSL_LIBRARY}
      ${CRYPTO_LIBRARY}
      CONFIGURATIONS Debug
      DESTINATION ${INSTALL_LIBDIR_DEBUG}
      COMPONENT main
      )
  endif()
endif()
ENDMACRO(MYSQL_OPENSSL_SPACKAGE)
