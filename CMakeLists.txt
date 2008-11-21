#   Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; version 2 of the License.
#
#   There are special exceptions to the terms and conditions of the GPL
#   as it is applied to this software. View the full text of the
#   exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
#   software distribution.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the Free Software
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

PROJECT(MYSQLCPPCONN)

cmake_minimum_required(VERSION 2.4.2)
if(COMMAND cmake_policy)
        cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckTypeSize)

INCLUDE(${CMAKE_SOURCE_DIR}/FindMySQL.cmake)

# ----------------------------------------------------------------------
# Create package script
# ----------------------------------------------------------------------

# Defaults to "win32", other option is "winx64"
#IF(NOT ARCH)
#  SET(ARCH win32)
#ENDIF(NOT ARCH)

# Get the part of the package name for this product
IF(MYSQL_SERVER_SUFFIX STREQUAL "-community")
  SET(CPACK_SERVER_SUFFIX "")
ELSE(MYSQL_SERVER_SUFFIX STREQUAL "-community")
  SET(CPACK_SERVER_SUFFIX ${MYSQL_SERVER_SUFFIX})
ENDIF(MYSQL_SERVER_SUFFIX STREQUAL "-community")

SET(CPACK_GENERATOR "TGZ")
SET(CPACK_PACKAGE_VERSION_MAJOR 1)
SET(CPACK_PACKAGE_VERSION_MINOR 0)
SET(CPACK_PACKAGE_VERSION_PATCH 1)
SET(CPACK_PACKAGE_VENDOR "MySQL")
SET(CPACK_RESOURCE_FILE_LICENSE "COPYING")
SET(CPACK_RESOURCE_FILE_README "README")
IF(WIN32)
	SET(CPACK_PACKAGE_FILE_NAME
		"mysql-connector-c++-noinstall-version-arch")
ELSE(WIN32)
	SET(CPACK_PACKAGE_FILE_NAME
		"mysql-connector-c++-version-arch")
ENDIF(WIN32)
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "mysql-connector-c++-version-arch")
SET(COMMON_IGNORE_FILES "/CMakeFiles/" "/Testing/" "/.bzr/" "_CPack_Packages/" 
    ".cmake$" "~" ".swp" ".log" ".gz" ".directory$" "CMakeCache.txt" "Makefile")
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


# ----------------------------------------------------------------------

ADD_SUBDIRECTORY(cppconn)
ADD_SUBDIRECTORY(driver)
ADD_SUBDIRECTORY(test)
ADD_SUBDIRECTORY(test/CJUnitTestsPort)
ADD_SUBDIRECTORY(examples)

IF(DEFINED CMAKE_SYSTEM_NAME AND ${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")
	# see also README
	IF(${CMAKE_C_COMPILER} MATCHES "gcc$")
		message("NOTE")
		message("You seem to be using GCC on SunOS.")
		message("This is not recommended. Compilation")
		message("might fail due to compile flags")
		message("returned by the MySQL tool mysql_config.")
		message("In case of an error, try commenting out:")
		message("# ADD_DEFINITIONS(\"\${MYSQL_CFLAGS}\")")
		message("in the file FindMySQL.cmake")
	ENDIF(${CMAKE_C_COMPILER} MATCHES "gcc$")

	IF(${CMAKE_C_COMPILER} MATCHES "gcc$")
		IF(${CMAKE_CXX_COMPILER} MATCHES "CC$")
			message("NOTE")
			message("You seem to be using the GCC C-compiler")
			message("together with the Sun CC C++-compiler.")
			message("Linking of the resulting binaries")
			message("migh fail. In case of an error,")
			message("try using Sun compilers only")
		ENDIF(${CMAKE_CXX_COMPILER} MATCHES "CC$")
	ELSEIF(${CMAKE_C_COMPILER} MATCHES "cc$" AND ${CMAKE_CXX_COMPILER} MATCHES "[c|g]\\+\\+$")
		message("NOTE")
		message("You seem to be using the GCC C-compiler")
		message("together with the Sun CC C++-compiler.")
		message("Linking of the resulting binaries")
		message("migh fail. In case of an error,")
		message("try using Sun compilers only")
	ENDIF(${CMAKE_C_COMPILER} MATCHES "gcc$")
ENDIF(DEFINED CMAKE_SYSTEM_NAME AND ${CMAKE_SYSTEM_NAME} STREQUAL "SunOS")
