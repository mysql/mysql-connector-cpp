# Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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

PROJECT(MSI)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

# This is Windows only, so just set one of two possible

SET(CONNECTOR_PLATFORM "Intel")
SET(CONNECTOR_PLATFORM_SUFFIX "win32")
SET(CONNECTOR_DEFAULT_DEST "ProgramFilesFolder")
IF(CMAKE_SIZEOF_VOID_P MATCHES 8)
  SET(CONNECTOR_PLATFORM "x64")
  SET(CONNECTOR_PLATFORM_SUFFIX "winx64")
  SET(CONNECTOR_DEFAULT_DEST "ProgramFiles64Folder")
ENDIF(CMAKE_SIZEOF_VOID_P MATCHES 8)

INCLUDE(${CMAKE_BINARY_DIR}/config.cmake)

SET(CONNECTOR_PRODUCT_DEST "MySQL Connector C++ ${CONNECTOR_PRODUCT_VERSION}")

# Generate GUID
EXECUTE_PROCESS(COMMAND uuidgen
                OUTPUT_VARIABLE CONNECTOR_PKG_ID1)
STRING(STRIP ${CONNECTOR_PKG_ID1} CONNECTOR_PKG_ID1)
EXECUTE_PROCESS(COMMAND uuidgen
                OUTPUT_VARIABLE CONNECTOR_PKG_ID2)
STRING(STRIP ${CONNECTOR_PKG_ID2} CONNECTOR_PKG_ID2)

# check if wix is in current path #
FIND_PATH(CONNECTOR_WIX_DIR candle.exe
        $ENV{CONNECTOR_WIX_DIR}
        $ENV{WIX_DIR}
        $ENV{WIX_DIR}/bin
        $ENV{ProgramFiles}/wix/bin
        $ENV{ProgramFiles}/Windows Installer */bin)

IF(NOT CONNECTOR_WIX_DIR)
  MESSAGE(ERROR "Wix not found. Please change your environment variable PATH or specify WIX_DIR")
ENDIF(NOT CONNECTOR_WIX_DIR)

IF (CONNECTOR_SIGNED)
  FIND_PROGRAM(HAVE_MD5SUM NAMES md5sum
  PATH ENV PATH)
  IF(NOT HAVE_MD5SUM)
    MESSAGE(ERROR "Can't find md5sum")
  ENDIF(NOT HAVE_MD5SUM)

  FIND_PROGRAM(HAVE_SIGNTOOL NAMES signtool
  PATH ENV PATH)
  IF(NOT HAVE_SIGNTOOL)
    MESSAGE(ERROR "Can't find signtool")
  ENDIF(NOT HAVE_SIGNTOOL)
ENDIF (CONNECTOR_SIGNED)

IF(NOT EXTRA_NAME_SUFFIX)
  SET(EXTRA_NAME_SUFFIX "")
ENDIF(NOT EXTRA_NAME_SUFFIX)

SET(WIXOUT "mysql-connector-c++${EXTRA_NAME_SUFFIX}-${CONNECTOR_PRODUCT_VERSION}${CONNECTOR_PRODUCT_LEVEL}-${CONNECTOR_PLATFORM_SUFFIX}")

SET(ProductURN "MSQZP-100-ZZZZ")

#-----------------------------------------------------

IF(EXISTS "${CMAKE_SOURCE_DIR}/../LICENSE.mysql")
  SET(LIC_SHORT_NAME "LICENSE.txt")
  SET(LIC_NAME       "LICENSE.mysql.txt")
  SET(LICENSE_FILE "${CMAKE_SOURCE_DIR}/../LICENSE.mysql")
  SET(LICENSE_RTF  "${CMAKE_BINARY_DIR}/License.rtf")
ELSE()
  SET(LIC_SHORT_NAME "LICENSE.txt")
  SET(LIC_NAME       "LICENSE.txt")
  SET(LICENSE_FILE "${CMAKE_SOURCE_DIR}/../LICENSE")
  SET(LICENSE_RTF  "${CMAKE_BINARY_DIR}/License.rtf")
ENDIF()
FILE(READ ${LICENSE_FILE} CONTENTS)
STRING(REGEX REPLACE "\n" "\\\\par\n" CONTENTS "${CONTENTS}")
STRING(REGEX REPLACE "\t" "\\\\tab" CONTENTS "${CONTENTS}")
FILE(WRITE "${LICENSE_RTF}"
     "{\\rtf1\\ansi\\deff0{\\fonttbl{\\f0\\fnil\\fcharset0 Courier New;}}\\viewkind4\\uc1\\pard\\lang1031\\f0\\fs15")
FILE(APPEND "${LICENSE_RTF}" "${CONTENTS}")
FILE(APPEND "${LICENSE_RTF}" "\n}\n")

#-----------------------------------------------------

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/mysql-conncpp-msi-template.xml.in
               ${CMAKE_BINARY_DIR}/mysql-conncpp-msi-template.${CONNECTOR_PLATFORM}.xml @ONLY)

MESSAGE(STATUS "${WIXOUT}")

SET(XML1                        mysql-conncpp-msi-ui.xml)
SET(WIXOBJ1 ${CMAKE_BINARY_DIR}/mysql-conncpp-msi-ui.wixobj)
SET(XML2    ${CMAKE_BINARY_DIR}/mysql-conncpp-msi-template.${CONNECTOR_PLATFORM}.xml)
SET(WIXOBJ2 ${CMAKE_BINARY_DIR}/mysql-conncpp-msi-template.${CONNECTOR_PLATFORM}.wixobj)

ADD_CUSTOM_COMMAND(OUTPUT "${WIXOBJ1}"
                   COMMAND "${CONNECTOR_WIX_DIR}/candle"
                     "-dBuildDir=${CMAKE_BINARY_DIR}"
                     "-I${CMAKE_SOURCE_DIR}"
                     -o "${WIXOBJ1}"
                     "${XML1}"
                   WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

ADD_CUSTOM_COMMAND(OUTPUT "${WIXOBJ2}"
                   COMMAND "${CONNECTOR_WIX_DIR}/candle"
                     "-dBuildDir=${CMAKE_BINARY_DIR}"
                     "-I${CMAKE_SOURCE_DIR}"
                     -o "${WIXOBJ2}"
                     "${XML2}"
                   WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

ADD_CUSTOM_COMMAND(OUTPUT "${CMAKE_BINARY_DIR}/${WIXOUT}.msi"
                   COMMAND "${CONNECTOR_WIX_DIR}/light"
                     -o "${CMAKE_BINARY_DIR}/${WIXOUT}.msi"
                     "${WIXOBJ1}"
                     "${WIXOBJ2}"
                     "${CONNECTOR_WIX_DIR}/WixUI.wixlib"
                     "${CONNECTOR_WIX_DIR}/wixca.wixlib"
                     -loc "${CONNECTOR_WIX_DIR}/WixUI_en-us.wxl"
                   WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                   DEPENDS "${WIXOBJ1}" "${WIXOBJ2}")

ADD_CUSTOM_TARGET(MSI ALL DEPENDS "${CMAKE_BINARY_DIR}/${WIXOUT}.msi")

IF(CONNECTOR_SIGNED)
  ADD_CUSTOM_COMMAND(OUTPUT "${WIXOUT}.msi.md5"
                     COMMAND signtool sign -a -d "MySQL Connector C++ ${CONECTOR_PRODUCT_VERSION}" -du "http://www.mysql.com" -t "http://timestamp.verisign.com/scripts/timestamp.dll" ${WIXOUT}.msi
                     COMMAND md5sum ${WIXOUT}.msi > ${WIXOUT}.msi.md5)
  ADD_CUSTOM_TARGET(MSI_SIGNED ALL DEPENDS "${WIXOUT}.msi.md5")
ENDIF(CONNECTOR_SIGNED)
