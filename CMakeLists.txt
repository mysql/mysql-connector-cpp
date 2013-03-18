#
# Copyright (c) 2008, 2013, Oracle and/or its affiliates. All rights reserved.
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


PROJECT(MYSQLCPPCONN)

# Bump this every time we change the API/ABI
SET(MYSQLCPPCONN_SOVERSION "7")


IF(WIN32)
# We need this for mysqlcppconn_EXPORTS needed for the static build
	cmake_minimum_required(VERSION 2.6.2)
ELSE(WIN32)
	cmake_minimum_required(VERSION 2.6)
ENDIF(WIN32)

if(COMMAND cmake_policy AND POLICY CMP0015)
	cmake_policy(SET CMP0015 NEW)
endif(COMMAND cmake_policy)

#-----------------
# CPPFLAGS, CXXFLAGS and LDFLAGS from the environment
SET(MYSQLCPPCONN_COMPILE_FLAGS_ENV "$ENV{CPPFLAGS} ${MYSQL_CXXFLAGS} $ENV{CXXFLAGS}")
MESSAGE(STATUS "Environment compile flags: ${MYSQLCPPCONN_COMPILE_FLAGS_ENV}")

SET(MYSQLCPPCONN_LINK_FLAGS_ENV "$ENV{LDFLAGS}")
MESSAGE(STATUS "Environment link flags: ${MYSQLCPPCONN_LINK_FLAGS_ENV}")

#-----------------
# ICU

SET(MYSQLCPPCONN_ICU_ENABLE 0 CACHE BOOL "development only: search icu although we do not make use of it yet.")

IF(MYSQLCPPCONN_ICU_ENABLE)
	SET(MYSQLCPPCONN_ICU_CONFIG_EXECUTABLE)
	SET(MYSQLCPPCONN_ICU_LIBRARY)
	SET(MYSQLCPPCONN_ICU_INCLUDE)

	FIND_PROGRAM (MYSQLCPPCONN_ICU_CONFIG_EXECUTABLE icu-config)

	IF (NOT MYSQLCPPCONN_ICU_CONFIG_EXECUTABLE)
		SET (MYSQLCPPCONN_ICU_FOUND FALSE)
			MESSAGE(STATUS "icu-config not found")
	ELSE (NOT MYSQLCPPCONN_ICU_CONFIG_EXECUTABLE)

		EXEC_PROGRAM ("${MYSQLCPPCONN_ICU_CONFIG_EXECUTABLE} --ldflags-libsonly"
		OUTPUT_VARIABLE MYSQLCPPCONN_ICU_LIBRARY
		RETURN_VALUE ERROR_CODE
		)

		EXEC_PROGRAM ("${MYSQLCPPCONN_ICU_CONFIG_EXECUTABLE} --cppflags-searchpath|sed s/^-I//"
		OUTPUT_VARIABLE MYSQLCPPCONN_ICU_INCLUDE
		RETURN_VALUE ERROR_CODE
		)

		EXEC_PROGRAM ("${MYSQLCPPCONN_ICU_CONFIG_EXECUTABLE} --ldflags-searchpath|sed s/^-L//"
		OUTPUT_VARIABLE MYSQLCPPCONN_ICU_LDLIB
		RETURN_VALUE ERROR_CODE
		)

		IF (MYSQLCPPCONN_ICU_LIBRARY)
			SET (MYSQLCPPCONN_ICU_FOUND TRUE)
		ENDIF (MYSQLCPPCONN_ICU_LIBRARY)

		MESSAGE(STATUS "ICU::CONFIG: ${MYSQLCPPCONN_ICU_CONFIG_EXECUTABLE}")
		MESSAGE(STATUS "ICU::LIBRARY: ${MYSQLCPPCONN_ICU_LIBRARY}")
		MESSAGE(STATUS "ICU::INCLUDE: ${MYSQLCPPCONN_ICU_INCLUDE}")
		MESSAGE(STATUS "ICU::LDLIB: ${MYSQLCPPCONN_ICU_LDLIB}")

		INCLUDE_DIRECTORIES(${MYSQLCPPCONN_ICU_INCLUDE})
		LINK_DIRECTORIES(${MYSQLCPPCONN_ICU_LDLIB})
	ENDIF (NOT MYSQLCPPCONN_ICU_CONFIG_EXECUTABLE)
ENDIF(MYSQLCPPCONN_ICU_ENABLE)
#-----------------

#-----------------
# BOOST
SET(ENV_BOOST $ENV{BOOST_ROOT})
IF(NOT BOOST_ROOT AND ENV_BOOST)
	SET(BOOST_ROOT ${ENV_BOOST})
ENDIF(NOT BOOST_ROOT AND ENV_BOOST)

IF(NOT BOOST_ROOT AND WIN32)
        SET(BOOST_ROOT "C:/Program Files/Boost")
ENDIF(NOT BOOST_ROOT AND WIN32)

# Prefer static linking in all cases
SET(Boost_ADDITIONAL_VERSIONS "1.37.0" "1.37" "1.38.0" "1.38" "1.39.0" "1.39")
SET(MYSQLCPPCONN_BOOST_COMPONENTS thread date_time)
SET(Boost_USE_STATIC_LIBS TRUE)
#FIND_PACKAGE(Boost COMPONENTS ${MYSQLCPPCONN_BOOST_COMPONENTS})
FIND_PACKAGE(Boost)
IF(NOT Boost_FOUND)
	# Try dynamic
	set(Boost_USE_STATIC_LIBS FALSE)
	FIND_PACKAGE(Boost COMPONENTS ${MYSQLCPPCONN_BOOST_COMPONENTS})
	IF(NOT Boost_FOUND)
		MESSAGE(FATAL_ERROR "Boost or some of its libraries found. If not in standard place please set -DBOOST_ROOT:STRING=")
	ENDIF(NOT Boost_FOUND)
ENDIF(NOT Boost_FOUND)

SET(MYSQLCPPCONN_BOOST_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
SET(MYSQLCPPCONN_BOOST_SYSTEM_LIBS ${Boost_SYSTEM_LIBRARY})
SET(MYSQLCPPCONN_BOOST_THREAD_LIBS ${Boost_THREAD_LIBRARY})
SET(MYSQLCPPCONN_BOOST_LIBRARY_DIRS ${Boost_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(${MYSQLCPPCONN_BOOST_INCLUDE_DIRS})
MESSAGE(STATUS "BOOST_INCLUDE_DIRS=${MYSQLCPPCONN_BOOST_INCLUDE_DIRS}")


#-----------------


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
		MESSAGE(FATAL_ERROR "STLPORT not found. Please set \$ENV{STLPORT_INCLUDE_DIR} if installed in non-standard location")
	ENDIF(STLPORT_INCLUDE_DIR)
ELSE(MYSQLCPPCONN_STLPORT_ENABLE)
	SET(MYSQLCPPCONN_STLPORT_LIB "")
ENDIF(MYSQLCPPCONN_STLPORT_ENABLE)
#
# STLPORT - END
#----------------------

# Make tests cover methods that throw not implemented to detect API changes?
SET(MYSQLCPPCONN_TEST_NOT_IMPLEMENTED 0 CACHE BOOL "HEAD/trunk QA: invoke methods that should return not implemented to detect API changes")

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

IF(EXTRA_NAME_SUFFIX)
  SET(CPACK_PACKAGE_NAME "mysql-connector-c++${EXTRA_NAME_SUFFIX}")
ELSE(EXTRA_NAME_SUFFIX)
  SET(CPACK_PACKAGE_NAME "mysql-connector-c++")
ENDIF(EXTRA_NAME_SUFFIX)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY
    "Connector/C++, a library for connecting to MySQL servers.")
SET(CPACK_PACKAGE_VERSION_MAJOR 1)
SET(CPACK_PACKAGE_VERSION_MINOR 1)
SET(CPACK_PACKAGE_VERSION_PATCH 3)
SET(CPACK_PACKAGE_RELEASE_TYPE "")
SET(CPACK_PACKAGE_VENDOR "Oracle and/or its affiliates")
SET(CPACK_RPM_PACKAGE_DESCRIPTION
"The MySQL Connector/C++ is a MySQL database connector for C++. The
MySQL Driver for C++ can be used to connect to the MySQL Server from
C++ applications. The MySQL Driver for C++ mimics the JDBC 4.0 API. It
is recommended to use the connector with MySQL 5.1 or later. Note -
its full functionality is not available when connecting to MySQL 5.0.
The MySQL Driver for C++ cannot connect to MySQL 4.1 or earlier. MySQL
is a trademark of ${CPACK_PACKAGE_VENDOR}

The MySQL software has Dual Licensing, which means you can use the MySQL
software free of charge under the GNU General Public License
(http://www.gnu.org/licenses/). You can also purchase commercial MySQL
licenses from ${CPACK_PACKAGE_VENDOR} if you do not wish to be
bound by the terms of the GPL. See the chapter "Licensing and Support"
in the manual for further info.")

IF(EXISTS "${CMAKE_SOURCE_DIR}/COPYING")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING")
ELSE(EXISTS "${CMAKE_SOURCE_DIR}/COPYING")
  SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.mysql")
ENDIF(EXISTS "${CMAKE_SOURCE_DIR}/COPYING")
SET(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_INSTALL "${CMAKE_SOURCE_DIR}/INSTALL")
SET(MYSQLCPPCONN_NUMERIC_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
IF(EXTRA_VERSION)
  SET(MYSQLCPPCONN_VERSION "${MYSQLCPPCONN_NUMERIC_VERSION}${EXTRA_VERSION}${CPACK_PACKAGE_RELEASE_TYPE}")
ELSE(EXTRA_VERSION)
  SET(MYSQLCPPCONN_VERSION "${MYSQLCPPCONN_NUMERIC_VERSION}${CPACK_PACKAGE_RELEASE_TYPE}")
ENDIF(EXTRA_VERSION)
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${MYSQLCPPCONN_VERSION}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}-${MYSQLCPPCONN_VERSION}-${CONNECTOR_PLATFORM}")
IF(WIN32)
  SET(CPACK_GENERATOR "ZIP")
  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-noinstall-${MYSQLCPPCONN_VERSION}-${CONNECTOR_PLATFORM}")
ELSE(WIN32)
  SET(CPACK_GENERATOR "TGZ")
  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
ENDIF(WIN32)

IF(WIN32)
  FILE(WRITE "${CMAKE_SOURCE_DIR}/win/config.cmake" "SET(CONNECTOR_PRODUCT_VERSION ${MYSQLCPPCONN_VERSION})
SET( LICENSE_FILENAME "${CPACK_RESOURCE_FILE_LICENSE}")")
ENDIF(WIN32)

INSTALL(FILES
	${CPACK_RESOURCE_FILE_README}
	${CPACK_RESOURCE_FILE_INSTALL}
	${CPACK_RESOURCE_FILE_LICENSE}
	"${CMAKE_SOURCE_DIR}/Licenses_for_Third-Party_Components.txt"
	"${CMAKE_SOURCE_DIR}/ANNOUNCEMENT"
	DESTINATION "." OPTIONAL)

SET(COMMON_IGNORE_FILES "/CMakeFiles/" "/Testing/" "/.bzr/" "_CPack_Packages/"
    ".cmake$" "~" ".swp" ".log" ".gz" ".directory$" "CMakeCache.txt" "Makefile"
	"install_manifest.txt")
SET(PRJ_COMMON_IGNORE_FILES ${COMMON_IGNORE_FILES} "ANNOUNCEMENT_102_ALPHA" "ANNOUNCEMENT_103_ALPHA" "ANNOUNCEMENT_104_BETA" "ANNOUNCEMENT_105_GA" "ANNOUNCEMENT_110_GA" "ANNOUNCEMENT_111_GA" "ANNOUNCEMENT_DRAFT" )

SET(CPACK_SOURCE_IGNORE_FILES ${PRJ_COMMON_IGNORE_FILES}  "./cppconn/config.h$" "./driver/nativeapi/binding_config.h$")
SET(CPACK_PACKAGE_IGNORE_FILES ${PRJ_COMMON_IGNORE_FILES} "something_there" )
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

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/cppconn)
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/driver/nativeapi)

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
