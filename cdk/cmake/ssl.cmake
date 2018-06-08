# Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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

# We support different versions of SSL:
# - "system"  (typically) uses headers/libraries in /usr/lib and /usr/lib64
# - a custom installation of openssl can be used like this
#     - cmake -DCMAKE_PREFIX_PATH=</path/to/custom/openssl> -DWITH_SSL="system"
#   or
#     - cmake -DWITH_SSL=</path/to/custom/openssl>
#   or
#     - cmake -DWITH_SSL=</path/to/custom/wolfssl>
#
# The default value for WITH_SSL is "system"
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


SET(WITH_SSL_DOC "system (use the OS openssl library)")
SET(WITH_SSL_DOC
  "${WITH_SSL_DOC}, </path/to/ssl/installation> (Can be OpenSSL or WolfSSL)")

if(NOT WIN32)
  find_program(READLINK readlink)
  find_program(READELF readelf)
endif()


#
# Set up the given target for using SSL libraries. This uses global SSL_XXX
# variables which are set by mysql_check_sql() macro defined below, which must
# be called first.
#

function(add_ssl TARGET)

  if(NOT WITH_SSL)
    return()
  endif()

  lib_link_libraries(${TARGET} ${SSL_LIBRARIES})
  target_include_directories(${TARGET} PRIVATE ${SSL_INCLUDES})

  if(SSL_DEFINES)
    #message("setting SSL defines for ${TARGET}: ${SSL_DEFINES}")
    target_compile_definitions(${TARGET} PRIVATE ${SSL_DEFINES})
  endif()

  #
  # On MacOS, set RPTAH_MANGLE property on the target to ensure that link names
  # of openSSL libraries stored in the final executable/library start with
  # @rptah/. See libutils.cmake for description of rpath managling on MacOS.
  #

  if(APPLE AND SSL_LINK_NAMES)
    #message("setting RPATH_MANGLE for ${TARGET}: ${SSL_LINK_NAMES}")
    set_property(TARGET ${TARGET} PROPERTY RPATH_MANGLE ${SSL_LINK_NAMES})
  endif()

endfunction(add_ssl)



MACRO (CHANGE_SSL_SETTINGS string)
  SET(WITH_SSL ${string} CACHE STRING ${WITH_SSL_DOC} FORCE)
ENDMACRO()

function(set_ssl_defines)
  set(SSL_DEFINES ${ARGV} CACHE INTERNAL "")
  #message(STATUS "SSL macros: ${SSL_DEFINES}")
endfunction(set_ssl_defines)
unset(SSL_DEFINES CACHE)

function(set_ssl_libraries)
  set(SSL_LIBRARIES ${ARGV} CACHE INTERNAL "")
  #message(STATUS "SSL libraries: ${SSL_LIBRARIES}")
endfunction(set_ssl_libraries)
unset(SSL_LIBRARIES CACHE)

function(set_ssl_includes)
  set(SSL_INCLUDES ${ARGV} CACHE INTERNAL "")
  #message(STATUS "SSL include path(s): ${SSL_INCLUDES}")
endfunction(set_ssl_includes)
unset(SSL_INCLUDES CACHE)



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


MACRO (MYSQL_USE_WOLFSSL)

  SET(WOLFSSL_SOURCE_DIR "${WITH_SSL_PATH}")
  MESSAGE(STATUS "WOLFSSL_SOURCE_DIR = ${WOLFSSL_SOURCE_DIR}")

  set_ssl_includes(
    "${WOLFSSL_SOURCE_DIR}/include"
    "${WOLFSSL_SOURCE_DIR}"
    "${WOLFSSL_SOURCE_DIR}/wolfssl"
  )

  set(SSL_LIBRARIES wolfssl wolfcrypt)
  IF(CMAKE_SYSTEM_NAME MATCHES "SunOS")
    list(APPEND SSL_LIBRARIES ${LIBSOCKET})
  ENDIF()
  set_ssl_libraries(${SSL_LIBRARIES})  # to put it in the cache

  SET(SSL_INTERNAL_INCLUDE_DIRS ${WOLFSSL_SOURCE_DIR})
  ADD_DEFINITIONS(
    -DHAVE_ECC
    -DKEEP_OUR_CERT
    -DOPENSSL_EXTRA
    -DWC_NO_HARDEN
    -DWOLFSSL_MYSQL_COMPATIBLE
    )
  CHANGE_SSL_SETTINGS("${WOLFSSL_SOURCE_DIR}")
  ADD_SUBDIRECTORY("${PROJECT_SOURCE_DIR}/extra/wolfssl")
  SET(SSL_SOURCES ${WOLFSSL_SOURCES} ${WOLFCRYPT_SOURCES})
  SET(WITH_SSL_WOLFSSL ON CACHE INTERNAL "Tells whether WolfSSL implementation is used")

ENDMACRO()


# MYSQL_CHECK_SSL
#
# Configure build system to use SSL libraries based on WITH_SSL option which
# can have values: system|<path/to/custom/installation>
#
# Optional Boolean argument tells whether we are building on a big-endian
# platform. If not given, little-endian is assumed.
#

function(MYSQL_CHECK_SSL)

  reset_ssl_variables()

  IF(NOT WITH_SSL)
    CHANGE_SSL_SETTINGS("system")
  ENDIF()

  set(SSL_BIG_ENDIAN 0)
  if(${ARGV0})
    set(SSL_BIG_ENDIAN ${ARGV0})
  endif()

  # See if WITH_SSL is of the form </path/to/custom/installation>)
  IF(EXISTS ${WITH_SSL}/wolfssl/openssl/ssl.h)
    SET(WITH_SSL_PATH ${WITH_SSL} CACHE PATH "path to custom SSL installation")
    MYSQL_USE_WOLFSSL()
    message(STATUS "Using WolfSSL implementation of SSL")
    return()
  ENDIF()

  IF (EXISTS ${WITH_SSL}/include/openssl/ssl.h)
    SET(WITH_SSL_PATH ${WITH_SSL} CACHE PATH "path to custom SSL installation")
  ENDIF()

  IF(
    WITH_SSL STREQUAL "system" OR
    WITH_SSL STREQUAL "yes" OR
    WITH_SSL_PATH
  )

    # TODO: Is it needed for anything?
    #IF(STATIC_MSVCRT)
    #  SET(OPENSSL_MSVC_STATIC_RT ON)
    #ENDIF()

    # FindOpenSSL.cmake knows about
    # http://www.slproweb.com/products/Win32OpenSSL.html
    # and will look for "C:/OpenSSL-Win64/" (and others)
    # For APPLE we set the hint /usr/local/opt/openssl

    if(WITH_SSL_PATH)
      set(OPENSSL_ROOT_DIR "${WITH_SSL_PATH}")
    elseif(APPLE AND NOT OPENSSL_ROOT_DIR)
      set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
    endif()

    #message("OPENSSL_ROOT_DIR: ${OPENSSL_ROOT_DIR}")

    #
    # Note: FindOpenSSL is broken on earlier versions of cmake. We use
    # our simplified replacement in that case.
    #

    if(CMAKE_VERSION VERSION_LESS "3.0")
      find_openssl()
    else()
      FIND_PACKAGE(OpenSSL)
    endif()

    if(NOT OPENSSL_FOUND)

      MESSAGE(SEND_ERROR
        "Cannot find appropriate system libraries for SSL. "
        "Make sure you've specified a supported SSL version. "
        "Consult the documentation for WITH_SSL alternatives")

      return()
    endif()

    if(NOT OPENSSL_VERSION_MAJOR EQUAL 1)
      message(SEND_ERROR "OpenSSL version 1.x is required but version ${OPENSSL_VERSION} was found")
    else()
      message(STATUS "Using OpenSSL version: ${OPENSSL_VERSION}")
    endif()

    #
    # We assume that FindOpenSSL finds  2 libraries in this order.
    #

    list(GET OPENSSL_LIBRARIES 0 OPENSSL_LIBRARY)
    list(GET OPENSSL_LIBRARIES 1 CRYPTO_LIBRARY)

    MESSAGE(STATUS "OPENSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
    MESSAGE(STATUS "OPENSSL_LIBRARY: ${OPENSSL_LIBRARY}")
    MESSAGE(STATUS "CRYPTO_LIBRARY: ${CRYPTO_LIBRARY}")

    IF (WIN32)
      FIND_FILE(OPENSSL_APPLINK_C
        NAMES openssl/applink.c
        HINTS "${OPENSSL_INCLUDE_DIR}"
      )
      MESSAGE(STATUS "OPENSSL_APPLINK_C: ${OPENSSL_APPLINK_C}")
    ENDIF()

    #
    # TODO: Support for linking with static openSSL libs?
    #

    INCLUDE(CheckSymbolExists)

    SET(CMAKE_REQUIRED_INCLUDES ${OPENSSL_INCLUDE_DIR})
    CHECK_SYMBOL_EXISTS(SHA512_DIGEST_LENGTH "openssl/sha.h"
                        HAVE_SHA512_DIGEST_LENGTH)

    if(NOT HAVE_SHA512_DIGEST_LENGTH)

      message(SEND_ERROR "Could not find SHA512_DIGEST_LENGTH symbol in sha.h header of OpenSSL library")

    endif()

    set_ssl_libraries(${OPENSSL_LIBRARIES})
    set_ssl_includes(${OPENSSL_INCLUDE_DIR})

    # Note: We set WITH_SSL in config.h. To avoid compiler warnings,
    # remove it form command-line defines

    string(REPLACE "-DWITH_SSL" "" SSL_DEFINES "${SSL_DEFINES}")
    set_ssl_defines(${SSL_DEFINES})

    # On MacOS put link names of openSSL libraries into SSL_LINK_NAMES.
    # This is used by add_ssl() to set up rpath mangling.

    set_link_names()

    if(BUNDLE_DEPENDENCIES)
      message(STATUS "Bundling OpenSSL libraries")
      bundle_ssl_libs()
    endif()

  ELSE()

    MESSAGE(SEND_ERROR
      "Could not find OpenSSL at a default location. "
      "Ensure that OpenSSL is installed on your system "
      "and/or set WITH_SSL to the location where it is installed. "
      "You can also build connector with WolfSSL, in that case "
      "set WITH_SSL to the location of WolfSSL sources.")

  ENDIF()

endfunction()


#
# On MacOS, read link names of openSSL libraries and store them in
# SSL_LINK_NAMES global variable.
#

function(set_link_names)

  unset(SSL_LINK_NAMES CACHE)

  if(NOT APPLE)
    return()
  endif()

  # Below we use otool to find exact link name of openSSL libraries

  if(NOT OTOOL)
    message(FATAL_ERROR
      "Tool otool required to build Connector/C++ is not available"
    )
  endif()

  set(SSL_LINK_NAMES)

  foreach(lib ${OPENSSL_LIBRARIES})

    execute_process(
      COMMAND otool -D "${lib}"
      OUTPUT_VARIABLE lib_link_name
    )

    # The output of otool -D has 2 lines, 1st line ends in ":",
    # 2nd line contains link name.

    string(REGEX REPLACE "^.*:\n" "" lib_link_name "${lib_link_name}")
    string(REGEX REPLACE "\n.*$" "" lib_link_name "${lib_link_name}")

    #message("== SSL link name: ${lib_link_name}")
    list(APPEND SSL_LINK_NAMES "${lib_link_name}")

  endforeach()

  set(SSL_LINK_NAMES "${SSL_LINK_NAMES}" CACHE INTERNAL "")

endfunction()


#
# Get locations of the openSSL libraries (resolving links on Unix) and
# store them in OPENSSL_FILES global variable.
#

function(bundle_ssl_libs)

  foreach(lib ${OPENSSL_LIBRARIES})

      if(WIN32)

        #
        # Note: the libraries listed in OPENSSL_LIBRARIES are import libraries.
        # below we find the DLLs that need to be bundled.
        #

        get_filename_component(lib_name "${lib}" NAME_WE)
        get_filename_component(lib_path "${lib}" DIRECTORY)

        #message("Looking for DLL for: ${lib_name}")

        unset(lib_dll CACHE)
        find_file(lib_dll
          NAMES "${lib_name}.dll"
          PATHS "${lib_path}"
          PATH_SUFFIXES "../bin" "." ".."
          NO_DEFAULT_PATH
        )

        #message("result: ${lib_dll}")

        message("bundling: ${lib}")
        install(FILES "${lib}" DESTINATION "${INSTALL_LIB_DIR_STATIC}" COMPONENT OpenSSLDev)

        if(lib_dll)
          message("bundling: ${lib_dll}")
          install(FILES "${lib_dll}" DESTINATION "${INSTALL_LIB_DIR}" COMPONENT OpenSSLDll)
        else()

          message(WARNING "Could not find DLL library for openSSL library ${lib_name} at ${lib_path}. OpenSSL libraries can not be bundled with the connector.")

        endif()

      else()

        get_lib_file(lib_file soname "${lib}")

        if(NOT lib_file)

          message(WARNING "Could not resolve location of openSSL library and therefore it will not be installed together with the connector (library: ${lib})")

        else()

          message("bundling: ${lib_file}")
          install(FILES "${lib_file}" DESTINATION "${INSTALL_LIB_DIR_STATIC}" COMPONENT OpenSSLDev)
          get_filename_component(lib_dir "${lib_file}" DIRECTORY)
          get_filename_component(lib_name "${lib_file}" NAME)

          # Create symlink used at link time

          string(REGEX MATCH "[^.]+" link_name "${lib_name}")
          set(link_name "${link_name}${CMAKE_SHARED_LIBRARY_SUFFIX}")

          message("bundling link: ${link_name} -> ${lib_name}")
          install(CODE "execute_process(
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                    ${lib_name} ${link_name}
            WORKING_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR_STATIC}\"
          )")

          # Create soname link

          if(soname)

            message("bundling soname link: ${soname} -> ${lib_name}")
            install(CODE "execute_process(
               COMMAND ${CMAKE_COMMAND} -E create_symlink
                       ${lib_name} ${soname}
               WORKING_DIRECTORY \"\${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR_STATIC}\"
            )")

          endif()

        endif()

      endif()

  endforeach()

endfunction()


#
# Given a compile time library ${LIB} set output variables:
#
#  LIB_VAR    - run-time library file (DLL for Windows)
#  SONAME_VAR - soname of the library
#
# Returned paths are absolute.
#

function(get_lib_file LIB_VAR SONAME_VAR LIB)

  if(WIN32)
    # TODO: get the path to DLL
    set(${LIB_VAR} "${LIB}")
    return()
  endif()

  set(${LIB_VAR} NOTFOUND)
  set(${SONAME_VAR} NOTFOUND)

  # resolve symbolic links
  get_filename_component(lib_path "${LIB}" REALPATH)
  get_filename_component(lib_name "${lib_path}" NAME)

  if(NOT lib_name)
    return()
  endif()

  get_filename_component(lib_dir "${lib_path}" DIRECTORY)

  #message("== Looking for ${lib_name} in: ${lib_dir}")

  unset(lib_file CACHE)
  find_file(lib_file "${lib_name}" PATHS "${lib_dir}")
  #message("== lib_file: ${lib_file}")

  set(${LIB_VAR} "${lib_file}" PARENT_SCOPE)

  if (NOT WIN32 AND NOT APPLE AND NOT SUNOS)

    # TODO: How to get soname on SunOS?

    if(NOT READELF)
      message(WARNING "Tool readelf required to find library soname is not available")
    endif()

    execute_process(
      COMMAND readelf -d "${LIB}"
      COMMAND grep SONAME
      OUTPUT_VARIABLE readelf_out
    )

    # example output:
    # 0x000000000000000e (SONAME)             Library soname: [libssl.so.10]

    string(REGEX MATCH "\\[.*\\]" readelf_out "${readelf_out}")
    string(REGEX REPLACE "^\\[|\\]$" "" lib_soname "${readelf_out}")

    #message("== lib_soname: ${lib_soname}")

    set(${SONAME_VAR} "${lib_soname}" PARENT_SCOPE)

  endif()

endfunction()


macro(find_openssl)

    # First search in WITH_SSL_PATH.
    FIND_PATH(OPENSSL_ROOT_DIR
      NAMES include/openssl/ssl.h
      NO_CMAKE_PATH
      NO_CMAKE_ENVIRONMENT_PATH
      HINTS "${OPENSSL_ROOT_DIR}"
    )
    # Then search in standard places (if not found above).
    FIND_PATH(OPENSSL_ROOT_DIR
      NAMES include/openssl/ssl.h
    )

    FIND_PATH(OPENSSL_INCLUDE_DIR
      NAMES openssl/ssl.h
      HINTS ${OPENSSL_ROOT_DIR}/include
    )


    FIND_LIBRARY(OPENSSL_LIBRARY
                 NAMES ssl ssleay32 ssleay32MD
                 HINTS ${OPENSSL_ROOT_DIR}/lib)
    FIND_LIBRARY(CRYPTO_LIBRARY
                 NAMES crypto libeay32
                 HINTS ${OPENSSL_ROOT_DIR}/lib)


    # Verify version number. Version information looks like:
    #   #define OPENSSL_VERSION_NUMBER 0x1000103fL
    # Encoded as MNNFFPPS: major minor fix patch status

    FILE(STRINGS "${OPENSSL_INCLUDE_DIR}/openssl/opensslv.h"
      OPENSSL_VERSION_NUMBER
      REGEX "^#[ ]*define[\t ]+OPENSSL_VERSION_NUMBER[\t ]+0x[0-9].*"
    )
    message("== OPENSSL_VERSION_NUMBER: ${OPENSSL_VERSION_NUMBER}")
    STRING(REGEX REPLACE
      "^.*OPENSSL_VERSION_NUMBER[\t ]+0x([0-9]).*$" "\\1"
      OPENSSL_VERSION_MAJOR "${OPENSSL_VERSION_NUMBER}"
    )
    message("== OPENSSL_VERSION_MAJOR: ${OPENSSL_VERSION_MAJOR}")

    IF(
      OPENSSL_INCLUDE_DIR AND
      OPENSSL_LIBRARY   AND
      CRYPTO_LIBRARY
    )

      set(OPENSSL_FOUND TRUE)
      set(OPENSSL_VERSION "${OPENSSL_VERSION_MAJOR}.?.?")
      set(OPENSSL_LIBRARIES "${OPENSSL_LIBRARY}" "${CRYPTO_LIBRARY}")

    ELSE()

      set(OPENSSL_FOUND FALSE)

    ENDIF()

endmacro()
