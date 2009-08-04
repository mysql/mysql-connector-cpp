
#   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPL
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPL as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

LINK_DIRECTORIES(${MYSQLCPPCONN_BOOST_LIBRARY_DIRS})

ADD_DEFINITIONS("-DCPPCONN_LIB_BUILD")


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


IF(MYSQLCLIENT_STATIC_BINDING)
  ADD_DEFINITIONS(-DMYSQLCLIENT_STATIC_BINDING)
ELSE(MYSQLCLIENT_STATIC_BINDING)
	IF(NOT WIN32)
		CHECK_INCLUDE_FILES(dlfcn.h HAVE_DLFCN_H)
		# We are not on windows and don't have dlfcn.h - so we need static binding
		IF(HAVE_DLFCN_H)
			ADD_DEFINITIONS(-DHAVE_DLFCN_H)
		ELSE(HAVE_DLFCN_H)
			ADD_DEFINITIONS(-DMYSQLCLIENT_STATIC_BINDING)
		ENDIF(HAVE_DLFCN_H)
	ENDIF(NOT WIN32)
ENDIF(MYSQLCLIENT_STATIC_BINDING)


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
  mysql_client_api.cpp
  mysql_resultset_data.cpp)

IF(MYSQLCLIENT_STATIC_BINDING)
	SET(MYSQLCPPCONN_SOURCES
		${MYSQLCPPCONN_SOURCES}
		mysql_client_static.cpp
	)
ELSE(MYSQLCLIENT_STATIC_BINDING)
	SET(MYSQLCPPCONN_SOURCES
		${MYSQLCPPCONN_SOURCES}
		mysql_lib_loader.cpp
		mysql_client_loader.cpp
	)

ENDIF(MYSQLCLIENT_STATIC_BINDING)

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
		mysql_statement.h
		mysql_util.h
		mysql_warning.h
		mysql_client_api.h
    mysql_resultset_data.h)

	IF(MYSQLCLIENT_STATIC_BINDING)
	        SET(MYSQLCPPCONN_SOURCES
                ${MYSQLCPPCONN_SOURCES}
                mysql_client_static.h
        	)
	ELSE(MYSQLCLIENT_STATIC_BINDING)
        	SET(MYSQLCPPCONN_SOURCES
                	${MYSQLCPPCONN_SOURCES}
			mysql_lib_loader.h
	                mysql_client_loader.h
        	)

	ENDIF(MYSQLCLIENT_STATIC_BINDING)
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

	SET(MY_TARGET_LINK_LIBRARIES_DYNAMIC ${MYSQL_LIBRARIES} ${MYSQL_LINK_FLAGS})

	SET(MY_TARGET_LINK_LIBRARIES_STATIC ${MYSQL_LIBRARIES} ${MYSQL_LINK_FLAGS})

  IF(NOT MYSQLCLIENT_STATIC_BINDING)
    SET(MY_TARGET_LINK_LIBRARIES_DYNAMIC ${MY_TARGET_LINK_LIBRARIES_DYNAMIC} "-ldl")
    SET(MY_TARGET_LINK_LIBRARIES_STATIC ${MY_TARGET_LINK_LIBRARIES_STATIC} "-ldl")
  ENDIF(NOT MYSQLCLIENT_STATIC_BINDING)
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
					VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}"
					LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV} ${MYSQL_LINK_FLAGS}"
					COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV} ${MYSQL_CXXFLAGS}")


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
