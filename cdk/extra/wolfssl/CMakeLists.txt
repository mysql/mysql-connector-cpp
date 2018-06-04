# Copyright (c) 2018, Oracle and/or its affiliates. All rights reserved.
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

include(CheckTypeSize)

INCLUDE_DIRECTORIES(${SSL_INCLUDES})

ADD_DEFINITIONS(${SSL_DEFINES})

CHECK_TYPE_SIZE("long"      SIZEOF_LONG)
CHECK_TYPE_SIZE("long long" SIZEOF_LONG_LONG)
ADD_DEFINITIONS(-DSIZEOF_LONG=${SIZEOF_LONG} -DSIZEOF_LONG_LONG=${SIZEOF_LONG_LONG})
# rename get_tty_password to avoid collisions with the main binary
ADD_DEFINITIONS(-Dget_tty_password_ext=wolfssl_mysql_get_tty_password_ext)
ADD_DEFINITIONS(-Dget_tty_password=wolfssl_mysql_get_tty_password)
ADD_DEFINITIONS(
  -DBUILDING_WOLFSSL
  -DHAVE_ECC
  -DHAVE_HASHDRBG
  -DKEEP_OUR_CERT
  -DMULTI_THREADED
  -DOPENSSL_EXTRA
  -DSESSION_CERT
  -DWC_NO_HARDEN
  -DWOLFSSL_AES_DIRECT
  -DWOLFSSL_ALLOW_TLSV10
  -DWOLFSSL_CERT_EXT
  -DWOLFSSL_MYSQL_COMPATIBLE
  -DWOLFSSL_SHA224
  -DWOLFSSL_SHA384
  -DWOLFSSL_SHA512
  -DWOLFSSL_STATIC_RSA
  -DWOLFSSL_CERT_GEN
  )

SET(WOLFSSL_SOURCES ${WOLFSSL_SOURCE_DIR}/src/crl.c
                    ${WOLFSSL_SOURCE_DIR}/src/internal.c
                    ${WOLFSSL_SOURCE_DIR}/src/keys.c
                    ${WOLFSSL_SOURCE_DIR}/src/sniffer.c
                    ${WOLFSSL_SOURCE_DIR}/src/tls.c
                    ${WOLFSSL_SOURCE_DIR}/src/wolfio.c
                    ${WOLFSSL_SOURCE_DIR}/src/ocsp.c
                    ${WOLFSSL_SOURCE_DIR}/src/ssl.c )

ADD_LIBRARY(wolfssl ${WOLFSSL_SOURCES})

SET(WOLFCRYPT_SOURCE_DIR "${WOLFSSL_SOURCE_DIR}/wolfcrypt")

SET(WOLFCRYPT_SOURCES ${WOLFCRYPT_SOURCE_DIR}/src/aes.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/arc4.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/asn.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/blake2b.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/camellia.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/chacha.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/coding.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/compress.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/des3.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/dh.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/dsa.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/ecc.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/error.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/hc128.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/hmac.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/integer.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/logging.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/md2.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/md4.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/md5.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/memory.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/pkcs7.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/pkcs12.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/poly1305.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/pwdbased.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/rabbit.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/random.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/ripemd.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/rsa.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/sha.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/sha256.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/sha512.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/tfm.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/wc_port.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/wc_encrypt.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/hash.c
                      ${WOLFCRYPT_SOURCE_DIR}/src/wolfmath.c
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/aes.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/arc4.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/asn.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/blake2.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/camellia.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/chacha.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/coding.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/compress.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/des3.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/dh.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/dsa.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/ecc.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/error-crypt.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/hc128.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/hmac.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/integer.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/logging.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/md2.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/md4.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/md5.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/memory.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/misc.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/pkcs7.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/poly1305.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/pwdbased.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/rabbit.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/random.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/ripemd.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/rsa.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/sha.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/sha256.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/sha512.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/tfm.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/wc_port.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/wc_encrypt.h
                      ${WOLFSSL_SOURCE_DIR}/wolfssl/wolfcrypt/hash.h
   )
# misc.c is not compiled in since using INLINE

ADD_LIBRARY(wolfcrypt ${WOLFCRYPT_SOURCES})

IF(MSVC)
   INSTALL_DEBUG_TARGET(wolfcrypt DESTINATION ${INSTALL_LIBDIR}/debug)
   INSTALL_DEBUG_TARGET(wolfssl DESTINATION ${INSTALL_LIBDIR}/debug)
ENDIF()
