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


LINK_DIRECTORIES(${MYSQLCPPCONN_BOOST_LIBRARY_DIRS})


INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR})
INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR}/cppconn)
INCLUDE_DIRECTORIES(${MYSQLCPPCONN_BOOST_INCLUDE_DIRS})
MESSAGE(STATUS "BOOST_INCLUDE_DIRS=${MYSQLCPPCONN_BOOST_INCLUDE_DIRS}")


SET(MYSQLCPPCONN_TRACE_ENABLE 0 CACHE BOOL "trace-enabled")

IF(MYSQLCPPCONN_TRACE_ENABLE)
	MESSAGE(STATUS "Building with tracing functionality")
	# Visual Studio 8 and up understand Variadic macros
	IF(WIN32)
		IF(CMAKE_GENERATOR MATCHES "Visual Studio 8"
			OR CMAKE_GENERATOR MATCHES "Visual Studio 9"
			OR CMAKE_GENERATOR MATCHES "NMake Makefiles")

			ADD_DEFINITIONS("-DCPPCONN_TRACE_ENABLED")
		ELSE(CMAKE_GENERATOR MATCHES "Visual Studio 8"
				OR CMAKE_GENERATOR MATCHES "Visual Studio 9"
				OR CMAKE_GENERATOR MATCHES "NMake Makefiles")
			MESSAGE(STATUS "Tracing will not be available in this build. VC8 or VC9 is needed.")
		ENDIF(CMAKE_GENERATOR MATCHES "Visual Studio 8"
				OR CMAKE_GENERATOR MATCHES "Visual Studio 9"
				OR CMAKE_GENERATOR MATCHES "NMake Makefiles")
	ELSE(WIN32)
		ADD_DEFINITIONS("-DCPPCONN_TRACE_ENABLED")
	ENDIF(WIN32)
ENDIF(MYSQLCPPCONN_TRACE_ENABLE)

SET(MYSQLCLIENT_STATIC_BINDING 1 CACHE BOOL "enable static binding")

IF(MYSQLCLIENT_STATIC_BINDING)
	MESSAGE(STATUS "Using static libmysql binding")
ELSE(MYSQLCLIENT_STATIC_BINDING)
	MESSAGE(STATUS "Using dynamic libmysql binding")
ENDIF(MYSQLCLIENT_STATIC_BINDING)

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
# CPPFLAGS, CXXFLAGS and LDFLAGS from the environment
SET(MYSQLCPPCONN_DT_RPATH "" CACHE STRING "For dynamic libmysql binding: DT_RPATH to set (default: unset)")

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

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/nativeapi/binding_config.h.cm ${CMAKE_CURRENT_BINARY_DIR}/nativeapi/binding_config.h)

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


ADD_LIBRARY(mysqlcppconn SHARED ${MYSQLCPPCONN_SOURCES})
ADD_LIBRARY(mysqlcppconn-static STATIC ${MYSQLCPPCONN_SOURCES})

IF(WIN32)

	INCLUDE_DIRECTORIES(${MYSQL_DIR}/include)
	IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
		LINK_DIRECTORIES(${MYSQL_DIR}/lib/debug)
	ELSEIF(CMAKE_BUILD_TYPE STREQUAL "")
		LINK_DIRECTORIES(${MYSQL_DIR}/lib/opt)
	ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

	SET(MYSQLCPPCONN_SOURCES ${MYSQLCPPCONN_SOURCES} mysqlcppconn.def)


	ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")


	#Disables bunch of pretty useless warnings on win
	ADD_DEFINITIONS("-D_SCL_SECURE_NO_WARNINGS")

	IF(MYSQLCLIENT_STATIC_BINDING)
		SET(MY_TARGET_LINK_LIBRARIES_DYNAMIC libmysql ws2_32)
	ELSE(MYSQLCLIENT_STATIC_BINDING)
		SET(MY_TARGET_LINK_LIBRARIES_DYNAMIC ws2_32)
	ENDIF(MYSQLCLIENT_STATIC_BINDING)

	SET(MY_TARGET_LINK_LIBRARIES_STATIC mysqlclient ws2_32)
ELSEIF(NOT WIN32)

	IF(MYSQLCLIENT_STATIC_BINDING)
	  SET(MY_TARGET_LINK_LIBRARIES_DYNAMIC ${MYSQL_LIBRARIES} ${MYSQL_LINK_FLAGS})
		SET(MY_TARGET_LINK_LIBRARIES_STATIC ${MYSQL_LIBRARIES} ${MYSQL_LINK_FLAGS})
	ELSE(MYSQLCLIENT_STATIC_BINDING)
		FOREACH(_lib ${MYSQL_LIBRARIES})
			IF(NOT _lib MATCHES "libmysqlclient.*" AND NOT _lib MATCHES "mysqlclient.*")
				SET(MY_TARGET_LINK_LIBRARIES_DYNAMIC ${MY_TARGET_LINK_LIBRARIES_DYNAMIC} ${_lib})
				SET(MY_TARGET_LINK_LIBRARIES_STATIC ${MY_TARGET_LINK_LIBRARIES_STATIC} ${_lib})
			ENDIF(NOT _lib MATCHES "libmysqlclient.*" AND NOT _lib MATCHES "mysqlclient.*")
		ENDFOREACH(_lib ${MYSQL_LIBRARIES})
		SET(MY_TARGET_LINK_LIBRARIES_DYNAMIC ${MY_TARGET_LINK_LIBRARIES_DYNAMIC} ${MYSQL_LINK_FLAGS} ${CMAKE_DL_LIBS})
		SET(MY_TARGET_LINK_LIBRARIES_STATIC ${MY_TARGET_LINK_LIBRARIES_STATIC} ${MYSQL_LINK_FLAGS} ${CMAKE_DL_LIBS})
	ENDIF(MYSQLCLIENT_STATIC_BINDING)
ENDIF(WIN32)


IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
	SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)


TARGET_LINK_LIBRARIES(mysqlcppconn	${MY_TARGET_LINK_LIBRARIES_DYNAMIC}
									${MY_GCOV_LINK_LIBRARIES}
									${MYSQLCPPCONN_STLPORT_LIB}
									${MYSQLCPPCONN_BOOST_SYSTEM_LIBS}
									${MYSQLCPPCONN_BOOST_THREAD_LIBS}
									${MYSQLCPPCONN_ICU_LIBRARY})

# Need to find way it's defined for mysqlcppconn-static only
#IF(WIN32)
#	ADD_DEFINITIONS("-DCPPCONN_PUBLIC_FUNC=\"\"")
#ENDIF(WIN32)


TARGET_LINK_LIBRARIES(mysqlcppconn-static	${MY_TARGET_LINK_LIBRARIES_STATIC}
											${MY_GCOV_LINK_LIBRARIES}
											${MYSQLCPPCONN_STLPORT_LIB}
											${MYSQLCPPCONN_BOOST_SYSTEM_LIBS}
											${MYSQLCPPCONN_BOOST_THREAD_LIBS}
											${MYSQLCPPCONN_ICU_LIBRARY})

MESSAGE("$ENV{CPPFLAGS} ${MYSQL_CXXFLAGS} $ENV{CXXFLAGS}")

SET_TARGET_PROPERTIES(mysqlcppconn-static PROPERTIES
					LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV} ${MYSQL_LINK_FLAGS}"
					COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV} ${MYSQL_CXXFLAGS}"
					COMPILE_DEFINITIONS CPPCONN_LIB_BUILD="")


SET_TARGET_PROPERTIES(mysqlcppconn
					PROPERTIES
					SOVERSION "${MYSQLCPPCONN_SOVERSION}"
					VERSION "${MYSQLCPPCONN_SOVERSION}.${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}"
					LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV} ${MYSQL_LINK_FLAGS}"
					COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV} ${MYSQL_CXXFLAGS}"
					COMPILE_DEFINITIONS CPPCONN_SO_BUILD="")

IF(WIN32)
	INSTALL(TARGETS mysqlcppconn mysqlcppconn-static
		RUNTIME DESTINATION lib
		ARCHIVE DESTINATION lib
	)
ELSE(WIN32)
	INSTALL(TARGETS mysqlcppconn mysqlcppconn-static
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
	)
ENDIF(WIN32)


# Install some MySQL specific headers
SET(MYSQLCPPCONN_SPECIFIC_INSTALL_HEADERS
		mysql_connection.h
		mysql_driver.h)

INSTALL(FILES ${MYSQLCPPCONN_SPECIFIC_INSTALL_HEADERS} DESTINATION include)


MESSAGE(STATUS "Configuring driver")
