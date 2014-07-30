#   Copyright (c) 2008, 2014, Oracle and/or its affiliates. All rights reserved.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPLv2
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPLv2 as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published
#   by the Free Software Foundation; version 2 of the License.
#
#   This program is distributed in the hope that it will be useful, but
#   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
#   or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
#   for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program; if not, write to the Free Software Foundation, Inc.,
#   51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

##########################################################################

MACRO(_MYSQL_CONFIG VAR _regex _opt)
   EXECUTE_PROCESS(COMMAND ${MYSQL_CONFIG_EXECUTABLE} ${_opt}
       OUTPUT_VARIABLE _mysql_config_output
       )
   SET(_var ${_mysql_config_output})
   STRING(REGEX MATCHALL "${_regex}([^ ]+)" _mysql_config_output "${_mysql_config_output}")
   STRING(REGEX REPLACE "^[ \t]+" "" _mysql_config_output "${_mysql_config_output}")
   STRING(REGEX REPLACE "[\r\n]$" "" _mysql_config_output "${_mysql_config_output}")
   STRING(REGEX REPLACE "${_regex}" "" _mysql_config_output "${_mysql_config_output}")
   SEPARATE_ARGUMENTS(_mysql_config_output)
   SET(${VAR} ${_mysql_config_output})
ENDMACRO(_MYSQL_CONFIG _regex _opt)


MACRO(_MYSQL_CONFIG_REPLACE VAR _regex1 _replace _regex2 _opt)
   EXECUTE_PROCESS(COMMAND ${MYSQL_CONFIG_EXECUTABLE} ${_opt}
       OUTPUT_VARIABLE _mysql_config_output1
       )
   SET(_var ${_mysql_config_output1})
   STRING(REGEX MATCHALL "${_regex2}([^ ]+)" _mysql_config_output1 "${_mysql_config_output1}")
   STRING(REGEX REPLACE "^[ \t]+" "" _mysql_config_output1 "${_mysql_config_output1}")
   STRING(REGEX REPLACE "[\r\n]$" "" _mysql_config_output1 "${_mysql_config_output1}")
   STRING(REGEX REPLACE "${_regex2}" "" _mysql_config_output1 "${_mysql_config_output1}")
   STRING(REGEX REPLACE "${_regex1}" "${_replace}" _mysql_config_output1 "${_mysql_config_output1}")
   SET(${VAR} ${_mysql_config_output1})
ENDMACRO(_MYSQL_CONFIG_REPLACE _regex1 _replace _regex2 _opt)


IF (MYSQLCLIENT_LIB_NAME)
  IF (${MYSQLCLIENT_LIB_NAME} MATCHES "libmysqlclient_r.a" 
       OR ${MYSQLCLIENT_LIB_NAME} MATCHES "libmysqlclient.a" 
       OR ${MYSQLCLIENT_LIB_NAME} MATCHES "mysqlclient*")
    SET(MYSQLCLIENT_STATIC_LINKING 1)
  ELSEIF (${MYSQLCLIENT_LIB_NAME} MATCHES "libmysqlclient_r.so" 
           OR ${MYSQLCLIENT_LIB_NAME} MATCHES "libmysqlclient.so" 
           OR ${MYSQLCLIENT_LIB_NAME} MATCHES "libmysql.*")
    SET(MYSQLCLIENT_STATIC_LINKING 0)
  ENDIF (${MYSQLCLIENT_LIB_NAME} MATCHES "libmysqlclient_r.a" 
       OR ${MYSQLCLIENT_LIB_NAME} MATCHES "libmysqlclient.a" 
       OR ${MYSQLCLIENT_LIB_NAME} MATCHES "mysqlclient*")
ELSE (MYSQLCLIENT_LIB_NAME)
  IF (WIN32)
    SET(MYSQLCLIENT_LIB_NAME "mysqlclient.lib")
  ELSE (WIN32)
    SET(MYSQLCLIENT_LIB_NAME "libmysqlclient.a libmysqlclient_r.a")
  ENDIF (WIN32)
  SET(MYSQLCLIENT_STATIC_LINKING 1)
ENDIF (MYSQLCLIENT_LIB_NAME)
SEPARATE_ARGUMENTS(MYSQLCLIENT_LIB_NAME)


IF (NOT MYSQL_CONFIG_EXECUTABLE AND NOT WIN32)
  IF (EXISTS "$ENV{MYSQL_DIR}/bin/mysql_config")
    SET(MYSQL_CONFIG_EXECUTABLE "$ENV{MYSQL_DIR}/bin/mysql_config")
  ELSE (EXISTS "$ENV{MYSQL_DIR}/bin/mysql_config")
    FIND_PROGRAM(MYSQL_CONFIG_EXECUTABLE
      NAMES mysql_config
      DOC "full path of mysql_config"
      PATHS /usr/bin
        /usr/local/bin
        /opt/mysql/mysql/bin
        /usr/local/mysql/bin
    )
  ENDIF (EXISTS "$ENV{MYSQL_DIR}/bin/mysql_config")
ENDIF (NOT MYSQL_CONFIG_EXECUTABLE AND NOT WIN32)
 

IF(MYSQL_CONFIG_EXECUTABLE AND NOT WIN32)
  MESSAGE(STATUS "mysql_config was found ${MYSQL_CONFIG_EXECUTABLE}")

  EXECUTE_PROCESS(COMMAND ${MYSQL_CONFIG_EXECUTABLE} "--version"
          OUTPUT_VARIABLE __MYSQL_VERSION
          )

  # Clean up so only numeric, in case of "-alpha" or similar
  STRING(REGEX MATCHALL "([0-9]+.[0-9]+.[0-9]+)" MYSQL_VERSION "${__MYSQL_VERSION}")
  # To create a fully numeric version, first normalize so N.NN.NN
  STRING(REGEX REPLACE "[.]([0-9])[.]" ".0\\1." MYSQL_NUM_VERSION "${MYSQL_VERSION}")
  STRING(REGEX REPLACE "[.]([0-9])$"   ".0\\1"  MYSQL_NUM_VERSION "${MYSQL_NUM_VERSION}")
  # Finally remove the dot
  STRING(REGEX REPLACE "[.]" "" MYSQL_NUM_VERSION "${MYSQL_NUM_VERSION}")

  IF(MYSQL_CXX_LINKAGE OR MYSQL_NUM_VERSION GREATER 50603)
    EXECUTE_PROCESS(COMMAND ${MYSQL_CONFIG_EXECUTABLE} "--cxxflags"
            OUTPUT_VARIABLE _mysql_config_output
            )
    SET(MYSQL_CXX_LINKAGE 1)
  ELSE (MYSQL_CXX_LINKAGE OR MYSQL_NUM_VERSION GREATER 50603)
    EXECUTE_PROCESS(COMMAND ${MYSQL_CONFIG_EXECUTABLE} "--cflags"
            OUTPUT_VARIABLE _mysql_config_output
            )
  ENDIF(MYSQL_CXX_LINKAGE OR MYSQL_NUM_VERSION GREATER 50603)

  IF (NOT MYSQL_LINK_FLAGS)
    STRING(REGEX MATCHALL "-m([^\r\n]+)" MYSQL_LINK_FLAGS "${_mysql_config_output}")
  ENDIF (NOT MYSQL_LINK_FLAGS)
  IF (NOT MYSQL_CXXFLAGS)
    STRING(REGEX REPLACE "[\r\n]$" "" MYSQL_CXXFLAGS "${_mysql_config_output}")
  ENDIF (NOT MYSQL_CXXFLAGS)
#  ADD_DEFINITIONS("${MYSQL_CXXFLAGS}")
ENDIF (MYSQL_CONFIG_EXECUTABLE AND NOT WIN32)


#----------------- FIND MYSQL_INCLUDE_DIR -------------------
IF(NOT MYSQL_INCLUDE_DIR AND MYSQL_CONFIG_EXECUTABLE AND NOT WIN32)
  _MYSQL_CONFIG(MYSQL_INCLUDE_DIR "(^| )-I" "--include")
ELSE(NOT MYSQL_INCLUDE_DIR AND MYSQL_CONFIG_EXECUTABLE AND NOT WIN32)
  MESSAGE(STATUS "ENV{MYSQL_DIR} = $ENV{MYSQL_DIR}")
  FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
    ${MYSQL_INCLUDE_DIR}
    $ENV{MYSQL_INCLUDE_DIR}
    $ENV{MYSQL_DIR}/include
    /usr/include/mysql
    /usr/local/include/mysql
    /opt/mysql/mysql/include
    /opt/mysql/mysql/include/mysql
    /usr/local/mysql/include
    /usr/local/mysql/include/mysql
    $ENV{ProgramFiles}/MySQL/*/include
    $ENV{SystemDrive}/MySQL/*/include)
ENDIF(NOT MYSQL_INCLUDE_DIR AND MYSQL_CONFIG_EXECUTABLE AND NOT WIN32)


#----------------- FIND MYSQL_LIB_DIR -------------------
IF (WIN32)
	# Set lib path suffixes
	# dist = for mysql binary distributions
	# build = for custom built tree
	IF (CMAKE_BUILD_TYPE STREQUAL Debug)
		SET(libsuffixDist debug)
		SET(libsuffixBuild Debug)
	ELSE (CMAKE_BUILD_TYPE STREQUAL Debug)
		SET(libsuffixDist opt)
		SET(libsuffixBuild Release)
		ADD_DEFINITIONS(-DDBUG_OFF)
	ENDIF (CMAKE_BUILD_TYPE STREQUAL Debug)

  FIND_LIBRARY(MYSQL_LIB 
    NAMES 
      ${MYSQLCLIENT_LIB_NAME}
    PATHS
      ${MYSQL_LIB_DIR}
      ${MYSQL_LIB_DIR}/lib/${libsuffixDist}
      ${MYSQL_LIB_DIR}/lib  #mysqlclient may be in lib for some c/c distros
      ${MYSQL_LIB_DIR}/libmysql/${libsuffixBuild}
      ${MYSQL_LIB_DIR}/client/${libsuffixBuild}
      $ENV{MYSQL_LIB_DIR}
      $ENV{MYSQL_LIB_DIR}/lib/${libsuffixDist}
      $ENV{MYSQL_LIB_DIR}/lib  #mysqlclient may be in lib for some c/c distros
      $ENV{MYSQL_LIB_DIR}/libmysql/${libsuffixBuild}
      $ENV{MYSQL_LIB_DIR}/client/${libsuffixBuild}
      $ENV{MYSQL_DIR}/lib/${libsuffixDist}
      $ENV{MYSQL_DIR}/lib  #mysqlclient may be in lib for some c/c distros
      $ENV{MYSQL_DIR}/libmysql/${libsuffixBuild}
      $ENV{MYSQL_DIR}/client/${libsuffixBuild}
      $ENV{ProgramFiles}/MySQL/*/lib/${libsuffixDist}
      $ENV{ProgramFiles}/MySQL/*/lib
      $ENV{SystemDrive}/MySQL/*/lib/${libsuffixDist}
      $ENV{SystemDrive}/MySQL/*/lib)

  SET(MYSQL_LIBRARIES ${MYSQL_LIB}) 
  IF(MYSQL_LIB)
    GET_FILENAME_COMPONENT(MYSQL_LIB_DIR ${MYSQL_LIB} PATH)
  ENDIF(MYSQL_LIB)
ELSE (WIN32)
  IF (NOT MYSQL_LIB_DIR AND MYSQL_CONFIG_EXECUTABLE)
    # In MySQL Server 5.6.10 "mysql_config" outputs -lssl
                 # -lcrypto dependencies even if (as it is with the
    # Oracle built binaries) OpenSSL is linked statically
    # to the client library and no reference is needed.
    # We remove the OpenSSL libraries if -DREMOVE_OPENSSL_DEP
    # is given
    IF (REMOVE_OPENSSL_DEP)
       LIST(REMOVE_ITEM MYSQL_LIBRARIES "ssl" "crypto")
    ENDIF (REMOVE_OPENSSL_DEP)
 
    FIND_LIBRARY(MYSQL_LIB 
      NAMES 
        ${MYSQLCLIENT_LIB_NAME}
      PATHS
        ${MYSQL_LIB_DIR}
        ${MYSQL_LIB_DIR}/libmysql_r/.libs
        ${MYSQL_LIB_DIR}/lib
        ${MYSQL_LIB_DIR}/lib/mysql
        $ENV{MYSQL_LIB_DIR}
        $ENV{MYSQL_LIB_DIR}/libmysql_r/.libs
        $ENV{MYSQL_LIB_DIR}/lib
        $ENV{MYSQL_LIB_DIR}/lib/mysql
        $ENV{MYSQL_DIR}/libmysql_r/.libs
        $ENV{MYSQL_DIR}/lib
        $ENV{MYSQL_DIR}/lib/mysql
        ${MYSQL_CLIB_DIR}
        ${MYSQL_CLIB_DIR}/libmysql_r/.libs
        ${MYSQL_CLIB_DIR}/lib
        ${MYSQL_CLIB_DIR}/lib/mysql
        /usr/lib/mysql
        /usr/local/lib/mysql
        /usr/local/mysql/lib
        /usr/local/mysql/lib/mysql
        /opt/mysql/mysql/lib
        /opt/mysql/mysql/lib/mysql)
   
    IF (MYSQLCLIENT_STATIC_LINKING)
      _MYSQL_CONFIG_REPLACE(MYSQL_LIBRARIES 
             "(mysqlclient|mysqlclient_r)" "${MYSQL_LIB}" "(^| )-l" "--libs")
    ELSE (MYSQLCLIENT_STATIC_LINKING)
      _MYSQL_CONFIG(MYSQL_LIBRARIES "(^| )-l" "--libs")
    ENDIF (MYSQLCLIENT_STATIC_LINKING)

    _MYSQL_CONFIG(MYSQL_CLIB_DIR "(^| )-L" "--libs")

    IF (NOT MYSQL_LIB_DIR)
      SET(MYSQL_LIB_DIR ${MYSQL_CLIB_DIR}) 
    ENDIF (NOT MYSQL_LIB_DIR)
  ELSE (NOT MYSQL_LIB_DIR AND MYSQL_CONFIG_EXECUTABLE)
    FIND_LIBRARY(MYSQL_LIB 
      NAMES 
        ${MYSQLCLIENT_LIB_NAME}
      PATHS
        ${MYSQL_LIB_DIR}
        ${MYSQL_LIB_DIR}/libmysql_r/.libs
        ${MYSQL_LIB_DIR}/lib
        ${MYSQL_LIB_DIR}/lib/mysql
        $ENV{MYSQL_LIB_DIR}
        $ENV{MYSQL_LIB_DIR}/libmysql_r/.libs
        $ENV{MYSQL_LIB_DIR}/lib
        $ENV{MYSQL_LIB_DIR}/lib/mysql
        $ENV{MYSQL_DIR}/libmysql_r/.libs
        $ENV{MYSQL_DIR}/lib
        $ENV{MYSQL_DIR}/lib/mysql
        /usr/lib/mysql
        /usr/local/lib/mysql
        /usr/local/mysql/lib
        /usr/local/mysql/lib/mysql
        /opt/mysql/mysql/lib
        /opt/mysql/mysql/lib/mysql)

    SET(MYSQL_LIBRARIES ${MYSQL_LIB})
    IF(MYSQL_LIB)
      GET_FILENAME_COMPONENT(MYSQL_LIB_DIR ${MYSQL_LIB} PATH)
    ENDIF(MYSQL_LIB)
  ENDIF (NOT MYSQL_LIB_DIR AND MYSQL_CONFIG_EXECUTABLE)

  IF (MYSQLCLIENT_STATIC_LINKING AND
      NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    LIST(APPEND MYSQL_LIBRARIES "rt")
  ENDIF ()
ENDIF (WIN32)

# Optionally add more libraries from the command line
# using -DEXTRA_MYSQL_DEP=...
IF (EXTRA_MYSQL_DEP)
   LIST(APPEND MYSQL_LIBRARIES ${EXTRA_MYSQL_DEP})
ENDIF (EXTRA_MYSQL_DEP)

SET(MYSQL_CLIENT_LIBS ${MYSQL_LIBRARIES})

SET(VERBOSE 1)
IF (MYSQL_INCLUDE_DIR AND MYSQL_LIB_DIR)
	SET(MYSQL_FOUND TRUE)

  MESSAGE(STATUS "MySQL Include dir: ${MYSQL_INCLUDE_DIR}")
  MESSAGE(STATUS "MySQL Library    : ${MYSQL_LIBRARIES}")
  MESSAGE(STATUS "MySQL Library dir: ${MYSQL_LIB_DIR}")
  MESSAGE(STATUS "MySQL CXXFLAGS: ${MYSQL_CXXFLAGS}")
  MESSAGE(STATUS "MySQL Link flags: ${MYSQL_LINK_FLAGS}")
  
  IF (MYSQL_VERSION)
    MESSAGE(STATUS "MySQL Version: ${MYSQL_VERSION}")
  ELSE (MYSQL_VERSION)
    IF(WIN32)
      #SET(CMAKE_REQUIRED_LIBRARIES ${MYSQL_LIB})
    ELSE(WIN32)
      INCLUDE(CheckFunctionExists)
      INCLUDE(CheckLibraryExists)
      # For now this works only on *nix
      SET(CMAKE_REQUIRED_LIBRARIES ${MYSQL_LIBRARIES})
      SET(CMAKE_REQUIRED_INCLUDES ${MYSQL_INCLUDE_DIR})
      CHECK_FUNCTION_EXISTS("mysql_set_character_set" HAVE_SET_CHARSET)
      CHECK_LIBRARY_EXISTS(mysqlclient "mysql_set_character_set" ${MYSQL_LIB_DIR} HAVE_SET_CHARSET_IN_LIB)
      SET(CMAKE_REQUIRED_LIBRARIES)
      SET(CMAKE_REQUIRED_INCLUDES)
      IF (HAVE_SET_CHARSET OR HAVE_SET_CHARSET_IN_LIB)
        MESSAGE(STATUS "libmysql version - ok")
      ELSE (HAVE_SET_CHARSET OR HAVE_SET_CHARSET_IN_LIB)
        MESSAGE(FATAL_ERROR "Versions < 4.1.13 (for MySQL 4.1.x) and < 5.0.7 for (MySQL 5.0.x) are not supported. Please update your libraries.")
      ENDIF (HAVE_SET_CHARSET OR HAVE_SET_CHARSET_IN_LIB)
    ENDIF(WIN32)
  ENDIF(MYSQL_VERSION)

  MESSAGE(STATUS "MySQL Include dir: ${MYSQL_INCLUDE_DIR}")
  MESSAGE(STATUS "MySQL Library dir: ${MYSQL_LIB_DIR}")
  MESSAGE(STATUS "MySQL CXXFLAGS: ${MYSQL_CXXFLAGS}")
  MESSAGE(STATUS "MySQL Link flags: ${MYSQL_LINK_FLAGS}")

	INCLUDE_DIRECTORIES(${MYSQL_INCLUDE_DIR})
	LINK_DIRECTORIES(${MYSQL_LIB_DIR})

ELSE (MYSQL_INCLUDE_DIR AND MYSQL_LIB_DIR)
  IF(NOT WIN32)
    MESSAGE(SEND_ERROR "mysql_config wasn't found, -DMYSQL_CONFIG_EXECUTABLE=...")
  ENDIF(NOT WIN32)
  MESSAGE(FATAL_ERROR "Cannot find MySQL. Include dir: ${MYSQL_INCLUDE_DIR}  library dir: ${MYSQL_LIB_DIR} cxxflags: ${MYSQL_CXXFLAGS}")
ENDIF (MYSQL_INCLUDE_DIR AND MYSQL_LIB_DIR)

