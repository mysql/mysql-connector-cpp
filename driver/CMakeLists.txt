# Copyright (c) 2015, 2020, Oracle and/or its affiliates. All rights reserved.
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


include_directories(${PROJECT_SOURCE_DIR})  # <cppconn/*.h>
include_directories(${PROJECT_BINARY_DIR})  # generated <cppconn/config.h>
include_directories(${PROJECT_BINARY_DIR}/cppconn)  # generated "config.h"


# There is a generated "version_info.h" in "cppconn/" as well,
# specify that we want the one built from this directory

configure_file(version_info.h.cmake version_info.h @ONLY)
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

#-------------------

if(MYSQLCLIENT_STATIC_BINDING)
  message(STATUS "Using static libmysql binding")
  add_definitions(-DMYSQLCLIENT_STATIC_BINDING)
else(MYSQLCLIENT_STATIC_BINDING)
  message(STATUS "Using dynamic libmysql binding")
endif(MYSQLCLIENT_STATIC_BINDING)

IF(NOT MYSQLCLIENT_STATIC_BINDING)
  IF(NOT WIN32)
    CHECK_INCLUDE_FILES(dlfcn.h HAVE_DLFCN_H)
    # We are not on windows and don't have dlfcn.h - so we need static binding
    IF(NOT HAVE_DLFCN_H)
      MESSAGE(FATAL_ERROR "Dynamic binding has been requested but we cannot find dlfcn.h!")
      SET(MYSQLCLIENT_STATIC_BINDING 1)
    ENDIF(NOT HAVE_DLFCN_H)
  ENDIF(NOT WIN32)
ENDIF(NOT MYSQLCLIENT_STATIC_BINDING)

#-----------------

if(0)

if(NOT DEFINED MYSQLCPPCONN_DT_RPATH)
  if(APPLE)
    set(rpath "@loader_path")
  elseif(NOT WIN32)
    set(rpath "$ORIGIN")
  endif()
endif()

SET(MYSQLCPPCONN_DT_RPATH ${MYSQLCPPCONN_DT_RPATH} CACHE STRING "For dynamic libmysql binding: DT_RPATH to set (default: unset)")

IF(MYSQLCPPCONN_DT_RPATH)
  SET(CMAKE_SKIP_BUILD_RPATH  TRUE)
  SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
  SET(CMAKE_INSTALL_RPATH ${MYSQLCPPCONN_DT_RPATH})
  SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
  MESSAGE(STATUS "DT_RPATH set to ${CMAKE_INSTALL_RPATH}")
ELSE(MYSQLCPPCONN_DT_RPATH)
  SET(CMAKE_SKIP_RPATH TRUE)
  MESSAGE(STATUS "DT_RPATH will not be set")
ENDIF(MYSQLCPPCONN_DT_RPATH)

endif()

#-----------------

configure_file(nativeapi/binding_config.h.cm
               nativeapi/binding_config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/nativeapi)

SET(MYSQLCPPCONN_SOURCES
  mysql_art_resultset.cpp
  mysql_art_rset_metadata.cpp
  mysql_connection.cpp
  mysql_debug.cpp
  mysql_driver.cpp
  mysql_metadata.cpp
  mysql_parameter_metadata.cpp
  mysql_prepared_statement.cpp
  mysql_ps_resultset.cpp
  mysql_ps_resultset_metadata.cpp
  mysql_resultbind.cpp
  mysql_resultset.cpp
  mysql_resultset_metadata.cpp
  mysql_statement.cpp
  mysql_util.cpp
  mysql_warning.cpp
  mysql_uri.cpp
  nativeapi/mysql_client_api.cpp
  nativeapi/library_loader.cpp
  nativeapi/mysql_native_driver_wrapper.cpp
  nativeapi/mysql_native_connection_wrapper.cpp
  nativeapi/mysql_native_statement_wrapper.cpp
  nativeapi/mysql_native_resultset_wrapper.cpp
)


IF(WIN32)
# adding headers to sources, so they will be included to VS projects

  #file(GLOB "*.h|nativeapi/*.h|cppconn/*.h")
  SET(MYSQLCPPCONN_SOURCES
    ${MYSQLCPPCONN_SOURCES}
    mysql_art_resultset.h
    mysql_art_rset_metadata.h
    mysql_connection.h
    mysql_connection_options.h
    mysql_debug.h
    mysql_driver.h
    mysql_error.h
    mysql_metadata.h
    mysql_parameter_metadata.h
    mysql_prepared_statement.h
    mysql_ps_resultset.h
    mysql_ps_resultset_metadata.h
    mysql_resultbind.h
    mysql_resultset.h
    mysql_resultset_metadata.h
    mysql_statement.h
    mysql_util.h
    mysql_warning.h
    mysql_uri.h
    ${CMAKE_CURRENT_BINARY_DIR}/version_info.h
    nativeapi/mysql_client_api.h
    nativeapi/mysql_native_driver_wrapper.h
    nativeapi/native_driver_wrapper.h
    nativeapi/mysql_native_connection_wrapper.h
    nativeapi/native_connection_wrapper.h
    nativeapi/mysql_native_statement_wrapper.h
    nativeapi/native_statement_wrapper.h
    nativeapi/mysql_native_resultset_wrapper.h
    nativeapi/native_resultset_wrapper.h
    ../cppconn/warning.h
    ../cppconn/statement.h
    ../cppconn/sqlstring.h
    ../cppconn/resultset_metadata.h
    ../cppconn/resultset.h
    ../cppconn/prepared_statement.h
    ../cppconn/parameter_metadata.h
    ../cppconn/metadata.h
    ../cppconn/exception.h
    ../cppconn/driver.h
    ../cppconn/datatype.h
    ../cppconn/variant.h
    ../cppconn/connection.h
    ../cppconn/config.h
    ../cppconn/build_config.h
  )

  IF(NOT MYSQLCLIENT_STATIC_BINDING)
    SET(MYSQLCPPCONN_SOURCES ${MYSQLCPPCONN_SOURCES}
        nativeapi/library_loader.h)
  ENDIF(NOT MYSQLCLIENT_STATIC_BINDING)

  # Used to create source filter in projects in VS
  SOURCE_GROUP(NativeApi nativeapi/.+)
  SOURCE_GROUP(API ../cppconn/.+)

ENDIF(WIN32)


#-----------------


add_library(jdbc STATIC ${MYSQLCPPCONN_SOURCES})

target_link_libraries(jdbc PUBLIC Boost::boost)

IF(WIN32)
  target_link_libraries(jdbc PRIVATE Dnsapi)
ELSEIF(NOT CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
  target_link_libraries(jdbc PRIVATE resolv)
ENDIF()

if (MYSQLCLIENT_STATIC_BINDING)
  target_link_libraries(jdbc PRIVATE MySQL::client)
else()
  target_include_directories(jdbc PRIVATE ${MYSQL_INCLUDE_DIR})
  if(NOT WIN32)
    target_link_libraries(jdbc PRIVATE dl)
  ENDIF(NOT WIN32)
endif()

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(jdbc PRIVATE -DCPPCONN_LIB_BUILD)
else()
  target_compile_definitions(jdbc PRIVATE -Dconnector_jdbc_EXPORTS)
endif()


return()

#-----------------


SET(MYSQL_LIB_OBJECTS "${MYSQL_LIBRARIES}")

IF(WIN32)

  #? SET(MYSQLCPPCONN_SOURCES ${MYSQLCPPCONN_SOURCES} mysqlcppconn.def)

ENDIF(WIN32)
