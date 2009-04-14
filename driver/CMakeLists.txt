
#   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPL
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPL as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

ADD_DEFINITIONS("-DCPPCONN_LIB_BUILD")

INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR})
INCLUDE_DIRECTORIES(${MYSQLCPPCONN_SOURCE_DIR}/cppconn)


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
	mysql_res_wrapper.cpp
	mysql_statement.cpp
	mysql_util.cpp
	mysql_warning.cpp)

IF(WIN32)
	# adding headers to sources, so they will be included to VS projects

	SET(MYSQLCPPCONN_SOURCES
		${MYSQLCPPCONN_SOURCES}
		mysql_art_resultset.h
		mysql_art_rset_metadata.h
		mysql_connection.h
		mysql_debug.h
		mysql_driver.h
		mysql_metadata.h
		mysql_parameter_metadata.h
		mysql_prepared_statement.h
		mysql_ps_resultset.h
		mysql_ps_resultset_metadata.h
		mysql_resultbind.h
		mysql_resultset.h
		mysql_resultset_metadata.h
		mysql_res_wrapper.h
		mysql_statement.h
		mysql_util.h
		mysql_warning.h)
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


	SET(MY_TARGET_LINK_LIBRARIES_DYNAMIC libmysql ws2_32)
	SET(MY_TARGET_LINK_LIBRARIES_STATIC mysqlclient ws2_32)
ELSEIF(NOT WIN32)

	SET(MY_TARGET_LINK_LIBRARIES_DYNAMIC ${MYSQL_LIBRARIES} ${MYSQL_LINK_FLAGS})
	SET(MY_TARGET_LINK_LIBRARIES_STATIC ${MYSQL_LIBRARIES} ${MYSQL_LINK_FLAGS})
ENDIF(WIN32)

IF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)
	SET(MY_GCOV_LINK_LIBRARIES gcov)
ENDIF(CMAKE_COMPILER_IS_GNUCC AND MYSQLCPPCONN_GCOV_ENABLE)


TARGET_LINK_LIBRARIES(mysqlcppconn ${MY_TARGET_LINK_LIBRARIES_DYNAMIC} ${MY_GCOV_LINK_LIBRARIES} ${MYSQLCPPCONN_STLPORT_LIB})

# Need to find way it's defined for mysqlcppconn-static only
#IF(WIN32)
#	ADD_DEFINITIONS("-DCPPCONN_PUBLIC_FUNC=\"\"")
#ENDIF(WIN32)


TARGET_LINK_LIBRARIES(mysqlcppconn-static ${MY_TARGET_LINK_LIBRARIES_STATIC} ${MY_GCOV_LINK_LIBRARIES} ${MYSQLCPPCONN_STLPORT_LIB})


SET_TARGET_PROPERTIES(mysqlcppconn-static PROPERTIES
					LINK_FLAGS "${MYSQL_LINK_FLAGS} $ENV{LDFLAGS}"
					COMPILE_FLAGS "$ENV{CPPFLAGS} ${MYSQL_CXXFLAGS} $ENV{CXXFLAGS}")


SET_TARGET_PROPERTIES(mysqlcppconn
					PROPERTIES
					SOVERSION "${CPACK_PACKAGE_VERSION_MAJOR}"
					VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}"
					LINK_FLAGS "${MYSQL_LINK_FLAGS} $ENV{LDFLAGS}"
					COMPILE_FLAGS "$ENV{CPPFLAGS} ${MYSQL_CXXFLAGS} $ENV{CXXFLAGS}")


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
