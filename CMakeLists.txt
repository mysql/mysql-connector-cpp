#   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPL
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPL as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

PROJECT(MYSQLCPPCONN)

IF(WIN32)
	# We need this for mysqlcppconn_EXPORTS needed for the static build
	cmake_minimum_required(VERSION 2.6.2)
ELSE(WIN32)
	cmake_minimum_required(VERSION 2.4.2)
ENDIF(WIN32)

if(COMMAND cmake_policy)
        cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckTypeSize)


#----------------------
# STLPORT - BEGIN
#
SET(MYSQLCPPCONN_STLPORT_ENABLE 0 CACHE BOOL "stlport-enabled")
IF(MYSQLCPPCONN_STLPORT_ENABLE)
	MESSAGE(STATUS "Looking for STLPort")
	FIND_PATH(STLPORT_INCLUDE_DIR stl_user_config.h
			$ENV{STLPORT_INCLUDE_DIR}
			/usr/include/stlport
			/usr/local/include/stlport)
	IF(STLPORT_INCLUDE_DIR)
		MESSAGE(STATUS "Using STLPort from : ${STLPORT_INCLUDE_DIR}")

		INCLUDE_DIRECTORIES(STLPORT_INCLUDE_DIR)
		SET(MYSQLCPPCONN_STLPORT_LIB "stlport")
	ELSE(STLPORT_INCLUDE_DIR)
		MESSAGE(SEND_ERROR "STLPORT not found. Please set \$ENV{STLPORT_INCLUDE_DIR} if installed in non-standard location")
	ENDIF(STLPORT_INCLUDE_DIR)
ELSE(MYSQLCPPCONN_STLPORT_ENABLE)
	SET(MYSQLCPPCONN_STLPORT_LIB "")
ENDIF(MYSQLCPPCONN_STLPORT_ENABLE)
#
# STLPORT - END
#----------------------



INCLUDE(${CMAKE_SOURCE_DIR}/FindMySQL.cm)

# ----------------------------------------------------------------------
# Create package script
# ----------------------------------------------------------------------

IF(NOT CONNECTOR_PLATFORM)
  IF(WIN32)
    IF(CMAKE_SIZEOF_VOID_P MATCHES 8)
      SET(CONNECTOR_PLATFORM "winx64") 
    ELSE(CMAKE_SIZEOF_VOID_P MATCHES 8)
      SET(CONNECTOR_PLATFORM "win32") 
    ENDIF(CMAKE_SIZEOF_VOID_P MATCHES 8)
  ELSE(WIN32)
    SET(CONNECTOR_PLATFORM "unknown")
  ENDIF(WIN32)
ENDIF(NOT CONNECTOR_PLATFORM)

# Get the part of the package name for this product
IF(MYSQL_SERVER_SUFFIX STREQUAL "-community")
  SET(CPACK_SERVER_SUFFIX "")
ELSE(MYSQL_SERVER_SUFFIX STREQUAL "-community")
  SET(CPACK_SERVER_SUFFIX ${MYSQL_SERVER_SUFFIX})
ENDIF(MYSQL_SERVER_SUFFIX STREQUAL "-community")

SET(CPACK_PACKAGE_NAME "mysql-connector-c++")
SET(CPACK_PACKAGE_VERSION_MAJOR 1)
SET(CPACK_PACKAGE_VERSION_MINOR 0)
SET(CPACK_PACKAGE_VERSION_PATCH 5)
SET(CPACK_PACKAGE_RELEASE_TYPE "")
SET(CPACK_PACKAGE_VENDOR "Sun Microsystems, Inc.")
SET(CPACK_RESOURCE_FILE_LICENSE "COPYING")
SET(CPACK_RESOURCE_FILE_README "README")
SET(MYSQLCPPCONN_NUMERIC_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
IF(EXTRA_VERSION)
  SET(MYSQLCPPCONN_VERSION "${MYSQLCPPCONN_NUMERIC_VERSION}${EXTRA_VERSION}${CPACK_PACKAGE_RELEASE_TYPE}")
ELSE(EXTRA_VERSION)
  SET(MYSQLCPPCONN_VERSION "${MYSQLCPPCONN_NUMERIC_VERSION}${CPACK_PACKAGE_RELEASE_TYPE}")
ENDIF(EXTRA_VERSION)
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "mysql-connector-c++-${MYSQLCPPCONN_VERSION}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "mysql-connector-c++-${MYSQLCPPCONN_VERSION}-${CONNECTOR_PLATFORM}")
IF(WIN32)
  SET(CPACK_GENERATOR "ZIP")
  SET(CPACK_PACKAGE_FILE_NAME "mysql-connector-c++-noinstall-${MYSQLCPPCONN_VERSION}-${CONNECTOR_PLATFORM}")
ELSE(WIN32)
  SET(CPACK_GENERATOR "TGZ")
  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
ENDIF(WIN32)

IF(WIN32)
  FILE(WRITE "${CMAKE_SOURCE_DIR}/win/version.cmake" "SET(CONNECTOR_PRODUCT_VERSION ${MYSQLCPPCONN_NUMERIC_VERSION}.0)")
ENDIF(WIN32)

INSTALL(FILES "README" "COPYING" DESTINATION ".")

SET(COMMON_IGNORE_FILES "/CMakeFiles/" "/Testing/" "/.bzr/" "_CPack_Packages/"
    ".cmake$" "~" ".swp" ".log" ".gz" ".directory$" "CMakeCache.txt" "Makefile"
	"install_manifest.txt")
SET(PRJ_COMMON_IGNORE_FILES ${COMMON_IGNORE_FILES} "something_here" )

SET(CPACK_SOURCE_IGNORE_FILES ${PRJ_COMMON_IGNORE_FILES})
SET(CPACK_PACKAGE_IGNORE_FILES ${PRJ_COMMON_IGNORE_FILES} "none_so_far" )
SET(CPACK_SOURCE_GENERATOR "TGZ")

SET(MYSQLCPPCONN_GCOV_ENABLE 0 CACHE BOOL "gcov-enabled")
IF(CMAKE_COMPILER_IS_GNUCC)
	ADD_DEFINITIONS("-Wall -fPIC -Woverloaded-virtual")
	IF (MYSQLCPPCONN_GCOV_ENABLE)
		ADD_DEFINITIONS("-fprofile-arcs -ftest-coverage")
	ENDIF (MYSQLCPPCONN_GCOV_ENABLE)
ENDIF(CMAKE_COMPILER_IS_GNUCC)

# SET(CPACK_*) before the INCLUDE(CPack)
INCLUDE(CPack)

IF(WIN32)
	STRING(REGEX REPLACE "MYSQLCPPCONN" "MySQL/ConnectorCPP" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")
ENDIF(WIN32)
MESSAGE(STATUS "Installation path is: ${CMAKE_INSTALL_PREFIX}   (overwrite with -DCMAKE_INSTALL_PREFIX=/your/path)")

# thread module build is disabled at the moment
#ADD_SUBDIRECTORY(thread)

ADD_SUBDIRECTORY(cppconn)
ADD_SUBDIRECTORY(driver)
ADD_SUBDIRECTORY(examples)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(test/framework)
ADD_SUBDIRECTORY(test/CJUnitTestsPort)
ADD_SUBDIRECTORY(test/unit)

IF(DEFINED CMAKE_SYSTEM_NAME AND ${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")
	# see also README
	IF(${CMAKE_C_COMPILER} MATCHES "gcc$")
		message("NOTE")
		message("You seem to be using GCC on SunOS.")
		message("This is not recommended. Compilation")
		message("might fail due to compile flags")
		message("returned by the MySQL tool mysql_config.")
		message("In case of an error, try commenting out:")
		message("# ADD_DEFINITIONS(\"\${MYSQL_CXXFLAGS}\")")
		message("in the file FindMySQL.cm")
	ENDIF(${CMAKE_C_COMPILER} MATCHES "gcc$")

	IF(${CMAKE_C_COMPILER} MATCHES "gcc$")
		IF(${CMAKE_CXX_COMPILER} MATCHES "CC$")
			message("NOTE")
			message("You seem to be using the GCC C-compiler")
			message("together with the Sun CC C++-compiler.")
			message("Linking of the resulting binaries")
			message("might fail. In case of an error,")
			message("try using Sun compilers only")
		ENDIF(${CMAKE_CXX_COMPILER} MATCHES "CC$")
	ELSEIF(${CMAKE_C_COMPILER} MATCHES "cc$" AND ${CMAKE_CXX_COMPILER} MATCHES "[c|g]\\+\\+$")
		message("NOTE")
		message("You seem to be using the GCC C-compiler")
		message("together with the Sun CC C++-compiler.")
		message("Linking of the resulting binaries")
		message("might fail. In case of an error,")
		message("try using Sun compilers only")
	ENDIF(${CMAKE_C_COMPILER} MATCHES "gcc$")
ENDIF(DEFINED CMAKE_SYSTEM_NAME AND ${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")

INSTALL(FILES "BUILDINFO" DESTINATION . OPTIONAL)
