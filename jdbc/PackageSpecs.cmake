# Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
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

#
# Specifications for Connector/C++ binary and source packages
#
# TODO
# - Generate HTML docs with Doxygen and include in bin packages
#
# Note: CPACK_XXX variables must be set before include(CPack)
#

SET(CPACK_PACKAGE_VERSION_MAJOR ${CONNECTOR_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${CONNECTOR_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${CONNECTOR_PATCH})
SET(CPACK_PACKAGE_RELEASE_TYPE "")

# Needed for CPACK configuraiton, used for file name construction. Thus leaving it as is
IF(EXTRA_VERSION)
  SET(MYSQLCPPCONN_VERSION "${CONNECTOR_NUMERIC_VERSION}${EXTRA_VERSION}${CPACK_PACKAGE_RELEASE_TYPE}")
ELSE(EXTRA_VERSION)
  SET(MYSQLCPPCONN_VERSION "${CONNECTOR_NUMERIC_VERSION}${CPACK_PACKAGE_RELEASE_TYPE}")
ENDIF(EXTRA_VERSION)


# This is needed by windows installer and for CPACK
IF(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE.mysql")
  SET(LICENSE_FILENAME "${CMAKE_SOURCE_DIR}/LICENSE.mysql")
ELSE(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE.mysql")
  SET(LICENSE_FILENAME "${CMAKE_SOURCE_DIR}/LICENSE")
ENDIF(EXISTS "${CMAKE_SOURCE_DIR}/LICENSE.mysql")

# Creating file with version info that will be used for installer
# We have to do this trick because of license filename that is looked in this CMAKE_SOURCE_DIR (installer's cmake is run separately)
IF(WIN32)
  FILE(WRITE "${CMAKE_BINARY_DIR}/win/config.cmake" "SET(CONNECTOR_PRODUCT_VERSION ${CONNECTOR_VERSION})\n"
                                                    "SET(LICENSE_FILENAME \"${LICENSE_FILENAME}\")\n")
ENDIF(WIN32)


#
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

#CPACK version variables are initialized in VersionInfo.cmake
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
    QLCPPCONN_VERSION
    in the manual for further info.")

SET(CPACK_RESOURCE_FILE_LICENSE "${LICENSE_FILENAME}")
SET(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_INSTALL "${CMAKE_SOURCE_DIR}/INSTALL")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${MYSQLCPPCONN_VERSION}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CPACK_PACKAGE_NAME}-${MYSQLCPPCONN_VERSION}-${CONNECTOR_PLATFORM}")
IF(WIN32)
  SET(CPACK_GENERATOR "ZIP")
  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-noinstall-${MYSQLCPPCONN_VERSION}-${CONNECTOR_PLATFORM}")
ELSE(WIN32)
  SET(CPACK_GENERATOR "TGZ")
  SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY}")
ENDIF(WIN32)

SET(DOC_DESTINATION ".")
IF(RPM_LAYOUT)
  SET(DOC_DESTINATION "share/doc/${CPACK_PACKAGE_NAME}-${MYSQLCPPCONN_VERSION}")
ENDIF()

INSTALL(FILES
  ${CPACK_RESOURCE_FILE_README}
  ${CPACK_RESOURCE_FILE_INSTALL}
  ${CPACK_RESOURCE_FILE_LICENSE}
  "${CMAKE_SOURCE_DIR}/ANNOUNCEMENT"
  DESTINATION ${DOC_DESTINATION} OPTIONAL COMPONENT Readme)

SET(COMMON_IGNORE_FILES
        "/CMakeFiles/"
        "/Testing/"
        "/.bzr/"
        "_CPack_Packages/"
        "~"
        ".swp"
        ".log"
        ".gz"
        ".directory$"
        "CMakeCache.txt"
        "Makefile"
  "install_manifest.txt"
)
SET(PRJ_COMMON_IGNORE_FILES
        ${COMMON_IGNORE_FILES}
  "ANNOUNCEMENT_102_ALPHA"
  "ANNOUNCEMENT_103_ALPHA"
  "ANNOUNCEMENT_104_BETA"
  "ANNOUNCEMENT_105_GA"
  "ANNOUNCEMENT_110_GA"
  "ANNOUNCEMENT_111_GA"
  "ANNOUNCEMENT_DRAFT"
)

SET(CPACK_SOURCE_IGNORE_FILES
  ${PRJ_COMMON_IGNORE_FILES}
  "cppconn/config.h$"
  "cppconn/version_info.h$"
  "driver/nativeapi/binding_config.h$"
  "driver/version_info.h$"
)
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
