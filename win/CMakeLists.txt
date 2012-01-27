#
# Copyright (c) 2008, 2011, Oracle and/or its affiliates. All rights reserved.
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
#


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


INCLUDE("config.cmake")


#####################################
# Looking for ms redistributable libs

SET(THE_MACHINE "x86")
SET(CRT_VERSION "90")

SET(MSVCRTX_REDIST_DIR "$ENV{VCINSTALLDIR}\\redist")

EXECUTE_PROCESS(COMMAND "dumpbin" "/DEPENDENTS" "${CMAKE_SOURCE_DIR}/../driver/RelWithDebInfo/mysqlcppconn.dll"
				OUTPUT_VARIABLE CPPLIB_DEPENDENCIES
				)

IF(${CPPLIB_DEPENDENCIES} MATCHES ".*MSVC.([0-9]+)[.]dll.*")
	STRING(REGEX REPLACE ".*MSVC.([0-9]+)[.]dll.*" "\\1" CRT_VERSION "${CPPLIB_DEPENDENCIES}")
ENDIF(${CPPLIB_DEPENDENCIES} MATCHES ".*MSVC.([0-9]+)[.]dll.*")

STRING(REGEX MATCHALL "(MSVC.[0-9][0-9][.]dll)" DEPENDENTS "${CPPLIB_DEPENDENCIES}")


# MESSAGE("${CRT_VERSION} ${MSVCRTX_REDIST_DIR} ${DEPENDENTS} ${MACHINE}")

FOREACH(_dir $ENV{LIB})
	# MESSAGE("${_dir}")
	# $ENV{VCINSTALLDIR}
	IF(${_dir} MATCHES ".+[\\]VC[\\]LIB[\\]([^\\]+)$")
		STRING(REGEX REPLACE ".+[\\]LIB[\\]([^\\]+)$" "\\1" THE_MACHINE "${_dir}")
		#MESSAGE("======= Have match for machine")
	ENDIF(${_dir} MATCHES ".+[\\]VC[\\]LIB[\\]([^\\]+)$")
ENDFOREACH(_dir $ENV{LIB})

# MESSAGE("-=${THE_MACHINE}=-")

SET(REQUIRED_REDISTRIBUTABLE_LIBS "")
SET(counter "1")
FOREACH(arg ${DEPENDENTS})
	MESSAGE(STATUS "Adding found dependency lib: ${THE_MACHINE}\\${arg}")
	SET(REQUIRED_REDISTRIBUTABLE_LIBS "${REQUIRED_REDISTRIBUTABLE_LIBS}
<File Id=\"file016${counter}\" Name=\"${arg}\" LongName=\"${arg}\" Source=\"${MSVCRTX_REDIST_DIR}\\${THE_MACHINE}\\Microsoft.VC${CRT_VERSION}.CRT\\${arg}\" />")

	MATH( EXPR counter "${counter} + 1" )
ENDFOREACH(arg ${DEPENDENTS})

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

IF(EXISTS "../COPYING")
  SET(LIC_SHORT_NAME "COPYING")
  SET(LIC_NAME       "COPYING")
ELSE(EXISTS "../COPYING")
  SET(LIC_SHORT_NAME "LICENSE.mys")
  SET(LIC_NAME       "LICENSE.mysql")
ENDIF(EXISTS "../COPYING")

#-----------------------------------------------------

IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE.mysql")
  SET(LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE.mysql")
  SET(LICENSE_RTF  "${CMAKE_CURRENT_BINARY_DIR}/License.rtf")
ELSE()
  SET(LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../COPYING")
  SET(LICENSE_RTF  "${CMAKE_CURRENT_BINARY_DIR}/License.rtf")
ENDIF()
FILE(READ ${LICENSE_FILE} CONTENTS)
STRING(REGEX REPLACE "\n" "\\\\par\n" CONTENTS "${CONTENTS}")
STRING(REGEX REPLACE "\t" "\\\\tab" CONTENTS "${CONTENTS}")
FILE(WRITE "${LICENSE_RTF}"
     "{\\rtf1\\ansi\\deff0{\\fonttbl{\\f0\\fnil\\fcharset0 Courier New;}}\\viewkind4\\uc1\\pard\\lang1031\\f0\\fs15")
FILE(APPEND "${LICENSE_RTF}" "${CONTENTS}")
FILE(APPEND "${LICENSE_RTF}" "\n}\n")

#-----------------------------------------------------

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/mysql-conncpp-msi-template.xml.in
               ${CMAKE_CURRENT_SOURCE_DIR}/mysql-conncpp-msi-template.${CONNECTOR_PLATFORM}.xml @ONLY)

MESSAGE(STATUS "${WIXOUT}")

ADD_CUSTOM_COMMAND(OUTPUT "${WIXOUT}.wixobj"
                   COMMAND ${CONNECTOR_WIX_DIR}/candle mysql-conncpp-msi-template.${CONNECTOR_PLATFORM}.xml mysql-conncpp-msi-ui.xml)

ADD_CUSTOM_COMMAND(OUTPUT ${WIXOUT}.msi
                   COMMAND ${CONNECTOR_WIX_DIR}/light -o ${WIXOUT}.msi mysql-conncpp-msi-template.${CONNECTOR_PLATFORM}.wixobj mysql-conncpp-msi-ui.wixobj ${CONNECTOR_WIX_DIR}/WixUI.wixlib ${CONNECTOR_WIX_DIR}/wixca.wixlib -loc ${CONNECTOR_WIX_DIR}/WixUI_en-us.wxl
                   DEPENDS "${WIXOUT}.wixobj")

ADD_CUSTOM_TARGET(MSI ALL DEPENDS "${WIXOUT}.msi")

IF(CONNECTOR_SIGNED)
  ADD_CUSTOM_COMMAND(OUTPUT "${WIXOUT}.msi.md5"
                     COMMAND signtool sign -a -d "MySQL Connector C++ ${CONECTOR_PRODUCT_VERSION}" -du "http://www.mysql.com" -t "http://timestamp.verisign.com/scripts/timestamp.dll" ${WIXOUT}.msi
                     COMMAND md5sum ${WIXOUT}.msi > ${WIXOUT}.msi.md5)
  ADD_CUSTOM_TARGET(MSI_SIGNED ALL DEPENDS "${WIXOUT}.msi.md5")
ENDIF(CONNECTOR_SIGNED)
