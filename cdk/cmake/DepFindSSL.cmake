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

##############################################################################
#
# SSL::ssl    - main library
# SSL::crypto
#
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

if(TARGET SSL::ssl)
  return()
endif()

include(CheckSymbolExists)

add_config_option(WITH_SSL STRING DEFAULT system
  "Path to custom SSL library or 'system' to..."
)

# TODO: use_wolfssl()

function(main)

  message(STATUS "Looking for SSL library.")

  if(NOT WITH_SSL MATCHES "^(system|yes)$")

    if(EXISTS ${WITH_SSL}/wolfssl/openssl/ssl.h)
      message(STATUS "Using WolfSSL implementation of SSL")
      use_wolfssl()
      return()
    endif()

    if(EXISTS ${WITH_SSL}/include/openssl/ssl.h)
      set(OPENSSL_ROOT_DIR "${WITH_SSL}")
    endif()

  endif()


  # TODO: Is it needed for anything?
  #IF(STATIC_MSVCRT)
  #  SET(OPENSSL_MSVC_STATIC_RT ON)
  #ENDIF()


  #
  # Note: FindOpenSSL is broken on earlier versions of cmake. We use
  # our simplified replacement in that case.
  #
  # Note: I got strange results on Win even with cmake 3.8
  #

  find_openssl()
  #find_package(OpenSSL)

  if(NOT TARGET SSL::ssl)

    message(SEND_ERROR
      "Cannot find appropriate system libraries for SSL. "
      "Make sure you've specified a supported SSL version. "
      "Consult the documentation for WITH_SSL alternatives"
    )

    return()

  endif()

  set(OPENSSL_LIB_DIR "${OPENSSL_LIB_DIR}" CACHE INTERNAL "")

  if(NOT OPENSSL_VERSION_MAJOR EQUAL 1)
    message(SEND_ERROR "OpenSSL version 1.x is required but version ${OPENSSL_VERSION} was found")
  else()
    message(STATUS "Using OpenSSL version: ${OPENSSL_VERSION}")
  endif()

  #message(STATUS "OPENSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
  #message(STATUS "OPENSSL_LIBRARIES: ${OPENSSL_LIBRARIES}")


  set(CMAKE_REQUIRED_INCLUDES "${OPENSSL_INCLUDE_DIR}")
  CHECK_SYMBOL_EXISTS(SHA512_DIGEST_LENGTH "openssl/sha.h"
                      HAVE_SHA512_DIGEST_LENGTH)

  if(NOT HAVE_SHA512_DIGEST_LENGTH)

    message(SEND_ERROR "Could not find SHA512_DIGEST_LENGTH symbol in sha.h header of OpenSSL library")

  endif()

  if(WIN32 AND EXISTS "${OPENSSL_INCLUDE_DIR}/openssl/applink.c")

    message("-- Handling applink.c")

    add_library(openssl-applink STATIC "${OPENSSL_INCLUDE_DIR}/openssl/applink.c")
    target_link_libraries(SSL::ssl INTERFACE openssl-applink)

    # Remove warnings from openssl applink.c
    target_compile_options(openssl-applink PRIVATE /wd4152 /wd4996)

  endif()


  if(BUNDLE_DEPENDENCIES)
    bundle_ssl_libs()
  endif()

endfunction(main)


#
# output:
#   OPENSSL_INCLUDE_DIR
#   OPENSSL_LIB_DIR
#   OPENSSL_VERSION
#   OPENSSL_VERSION_MAJOR
#

function(find_openssl)

  set(hints)
  if(OPENSSL_ROOT_DIR)
    set(hints HINTS ${OPENSSL_ROOT_DIR} NO_DEFAULT_PATH)
  endif()

  find_path(OPENSSL_INCLUDE_DIR
    NAMES openssl/ssl.h
    PATH_SUFFIXES include
    ${hints}
  )

  if(NOT OPENSSL_INCLUDE_DIR)
    return()
  endif()

  set(OPENSSL_INCLUDE_DIR "${OPENSSL_INCLUDE_DIR}" PARENT_SCOPE)
  message("-- found OpenSSL headers at: ${OPENSSL_INCLUDE_DIR}")


  # Verify version number. Version information looks like:
  #   #define OPENSSL_VERSION_NUMBER 0x1000103fL
  # Encoded as MNNFFPPS: major minor fix patch status

  FILE(STRINGS "${OPENSSL_INCLUDE_DIR}/openssl/opensslv.h"
    OPENSSL_VERSION_NUMBER
    REGEX "^#[ ]*define[\t ]+OPENSSL_VERSION_NUMBER[\t ]+0x[0-9].*"
  )

  #message("== OPENSSL_VERSION_NUMBER: ${OPENSSL_VERSION_NUMBER}")
  STRING(REGEX REPLACE
    "^.*OPENSSL_VERSION_NUMBER[\t ]+0x(.)(..)(..)(..)(.).*$"
    "\\1;\\2;\\3;\\4;\\5"
    version_list "${OPENSSL_VERSION_NUMBER}"
  )
  #message("-- OPENSSL_VERSION: ${version_list}")

  list(GET version_list 0 OPENSSL_VERSION_MAJOR)
  math(EXPR OPENSSL_VERSION_MAJOR ${OPENSSL_VERSION_MAJOR})

  list(GET version_list 1 OPENSSL_VERSION_MINOR)
  math(EXPR OPENSSL_VERSION_MINOR ${OPENSSL_VERSION_MINOR})

  list(GET version_list 2 OPENSSL_VERSION_FIX)
  math(EXPR OPENSSL_VERSION_FIX ${OPENSSL_VERSION_FIX})

  list(GET version_list 3 OPENSSL_VERSION_PATCH)
  math(EXPR OPENSSL_VERSION_PATCH ${OPENSSL_VERSION_PATCH})

  list(GET version_list 4 OPENSSL_VERSION_STATUS)

  #
  # Calculate patch letter as: 1 - 'a', 2 - 'b' , ...
  # Note: ascii 'a' is 97
  #

  if(OPENSSL_VERSION_PATCH GREATER 0)
    math(EXPR letter_code "97 + ${OPENSSL_VERSION_PATCH} - 1")
    string(ASCII ${letter_code} patch_letter)
  endif()

  set(OPENSSL_VERSION
    "${OPENSSL_VERSION_MAJOR}.${OPENSSL_VERSION_MINOR}.${OPENSSL_VERSION_FIX}${patch_letter}"
    PARENT_SCOPE
  )
  set(OPENSSL_VERSION_MAJOR ${OPENSSL_VERSION_MAJOR} PARENT_SCOPE)


  find_library(OPENSSL_LIBRARY
    NAMES ssl ssleay32 ssleay32MD
    PATH_SUFFIXES lib
    ${hints}
  )

  find_library(CRYPTO_LIBRARY
    NAMES crypto libeay32 libeay32MD
    PATH_SUFFIXES lib
    ${hints}
  )

  if(NOT OPENSSL_LIBRARY OR NOT CRYPTO_LIBRARY)
    return()
  endif()

  message("-- OpenSSL library: ${OPENSSL_LIBRARY}")
  message("-- OpenSSL crypto library: ${CRYPTO_LIBRARY}")

  # Note: apparently UNKNOWN library type does not work
  # https://stackoverflow.com/questions/39346679/cmake-imported-unknown-global-target

  add_library(SSL::ssl SHARED IMPORTED GLOBAL)
  set_target_properties(SSL::ssl PROPERTIES
    IMPORTED_LOCATION "${OPENSSL_LIBRARY}"
    IMPORTED_IMPLIB "${OPENSSL_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}"
  )

  add_library(SSL::crypto SHARED IMPORTED GLOBAL)
  set_target_properties(SSL::crypto PROPERTIES
    IMPORTED_LOCATION "${CRYPTO_LIBRARY}"
    IMPORTED_IMPLIB "${CRYPTO_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${OPENSSL_INCLUDE_DIR}"
  )

  set_property(TARGET SSL::ssl PROPERTY
    INTERFACE_LINK_LIBRARIES SSL::crypto
  )

  get_filename_component(OPENSSL_LIB_DIR "${OPENSSL_LIBRARY}" PATH CACHE)

  set(OPENSSL_FOUND TRUE PARENT_SCOPE)

endfunction(find_openssl)


function(bundle_ssl_libs)

  if(NOT OPENSSL_LIB_DIR)
    return()
  endif()

  # Very simplistic approach

  file(GLOB glob1
    "${OPENSSL_LIB_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
  )

  file(GLOB glob2
    "${OPENSSL_LIB_DIR}/../bin/*${CMAKE_SHARED_LIBRARY_SUFFIX}*"
  )

  foreach(lib ${glob1} ${glob2})

    message("-- bundling OpenSSL library: ${lib}")

    install(FILES ${lib}
      DESTINATION ${INSTALL_LIB_DIR}
      COMPONENT OpenSSLDll
    )

  endforeach()

  # For Windows we also need static import libraries

  if(WIN32)

    file(GLOB glob
      "${OPENSSL_LIB_DIR}/*.lib"
    )

    foreach(lib ${glob})

      message("-- bundling OpenSSL library: ${lib}")

      install(FILES ${lib}
        DESTINATION ${INSTALL_LIB_DIR_STATIC}
        COMPONENT OpenSSLDev
      )

    endforeach()

  endif()

endfunction(bundle_ssl_libs)


main()
return()

##########################################################################

