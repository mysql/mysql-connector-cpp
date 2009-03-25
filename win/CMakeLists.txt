CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0 FATAL_ERROR)

# This is Windows only, so just set one of two possible
IF(CMAKE_SIZEOF_VOID_P MATCHES 8)
  SET(CONNECTOR_PLATFORM "winx64") 
  SET(CONNECTOR_DEFAULT_DEST "ProgramFiles64Folder")
ELSE(CMAKE_SIZEOF_VOID_P MATCHES 8)
  SET(CONNECTOR_PLATFORM "win32") 
  SET(CONNECTOR_DEFAULT_DEST "ProgramFilesFolder")
ENDIF(CMAKE_SIZEOF_VOID_P MATCHES 8)

INCLUDE("version.cmake")


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
	# MESSAGE("${arg}")
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

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/mysql-conncpp-msi-template.xml.in
               ${CMAKE_CURRENT_SOURCE_DIR}/mysql-conncpp-msi-template.xml @ONLY)

SET(WIXOUT "mysql-connector-c++-${CONNECTOR_PRODUCT_VERSION}-${CONNECTOR_PLATFORM}")

MESSAGE(STATUS "${WIXOUT}")
       
ADD_CUSTOM_COMMAND(OUTPUT "${WIXOUT}.wixobj"
			COMMAND ${CONNECTOR_WIX_DIR}/candle -out ${WIXOUT}.wixobj mysql-conncpp-msi-template.xml)

ADD_CUSTOM_COMMAND(OUTPUT ${WIXOUT}.msi
			COMMAND ${CONNECTOR_WIX_DIR}/light -o ${WIXOUT}.msi ${WIXOUT}.wixobj ${CONNECTOR_WIX_DIR}/WixUI.wixlib -loc ${CONNECTOR_WIX_DIR}/WixUI_en-us.wxl
			DEPENDS "${WIXOUT}.wixobj")

ADD_CUSTOM_TARGET(MSI ALL DEPENDS "${WIXOUT}.msi")

IF(CONNECTOR_SIGNED)
  ADD_CUSTOM_COMMAND(OUTPUT "${WIXOUT}.msi.md5"
			COMMAND signtool sign -a -d "MySQL Connector C++ ${CONECTOR_PRODUCT_VERSION}" -du "http://www.mysql.com" -t "http://timestamp.verisign.com/scripts/timestamp.dll" ${WIXOUT}.msi
			COMMAND md5sum ${WIXOUT}.msi > ${WIXOUT}.msi.md5)
			ADD_CUSTOM_TARGET(MSI_SIGNED ALL DEPENDS "${WIXOUT}.msi.md5")
ENDIF(CONNECTOR_SIGNED)

