#   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPL
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPL as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

#
# Instructions for compiling example.cpp|h into the binary example
# Use Copy&Paste and Search&Replace to add new tests to the build
#

SET(test_connection_sources
	../unit_fixture.cpp
	../main.cpp
	connection.cpp)

IF(WIN32)
	SET(test_connection_sources
    		${test_connection_sources}
	../unit_fixture.h
	connection.h)
ENDIF(WIN32)

ADD_EXECUTABLE(test_connection ${test_connection_sources})
SET_TARGET_PROPERTIES(test_connection PROPERTIES OUTPUT_NAME "connection")
TARGET_LINK_LIBRARIES(test_connection ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring unit tests - connection")

SET(test_databasemetadata_sources
	../unit_fixture.cpp
	../main.cpp
	connectionmetadata.cpp)

IF(WIN32)
	SET(test_databasemetadata_sources
    		${test_databasemetadata_sources}
	../unit_fixture.h
	connectionmetadata.h)
ENDIF(WIN32)

ADD_EXECUTABLE(test_databasemetadata ${test_databasemetadata_sources})
SET_TARGET_PROPERTIES(test_databasemetadata PROPERTIES OUTPUT_NAME "databasemetadata")
TARGET_LINK_LIBRARIES(test_databasemetadata ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring unit tests - databasemetadata")


SET(test_resultsetmetadata_sources
	../unit_fixture.cpp
	../main.cpp
	resultsetmetadata.cpp)

IF(WIN32)
	SET(test_resultsetmetadata_sources
    		${test_resultsetmetadata_sources}
	../unit_fixture.h
	resultsetmetadata.h)
ENDIF(WIN32)

ADD_EXECUTABLE(test_resultsetmetadata ${test_resultsetmetadata_sources})
SET_TARGET_PROPERTIES(test_resultsetmetadata PROPERTIES OUTPUT_NAME "resultsetmetadata")
TARGET_LINK_LIBRARIES(test_resultsetmetadata ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring unit tests - resultsetmetadata")

SET(test_resultset_sources
	../unit_fixture.cpp
	../main.cpp
	resultset.cpp)

IF(WIN32)
	SET(test_resultset_sources
    		${test_resultset_sources}
	../unit_fixture.h
	resultset.h)
ENDIF(WIN32)

ADD_EXECUTABLE(test_resultset ${test_resultset_sources})
SET_TARGET_PROPERTIES(test_resultset PROPERTIES OUTPUT_NAME "resultset")
TARGET_LINK_LIBRARIES(test_resultset ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring unit tests - resultset")

SET(test_savepoint_sources
	../unit_fixture.cpp
	../main.cpp
	savepoint.cpp)

IF(WIN32)
	SET(test_savepoint_sources
    		${test_savepoint_sources}
	../unit_fixture.h
	savepoint.h)
ENDIF(WIN32)

ADD_EXECUTABLE(test_savepoint ${test_savepoint_sources})
SET_TARGET_PROPERTIES(test_savepoint PROPERTIES OUTPUT_NAME "savepoint")
TARGET_LINK_LIBRARIES(test_savepoint ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring unit tests - savepoint")


SET(test_art_resultset_sources
	../unit_fixture.cpp
	../main.cpp
	art_resultset.cpp)

IF(WIN32)
	SET(test_art_resultset_sources
    		${test_art_resultset_sources}
	../unit_fixture.h
	art_resultset.h)

	ADD_DEFINITIONS("-DCPPCONN_PUBLIC_FUNC=")
ENDIF(WIN32)

ADD_EXECUTABLE(test_art_resultset ${test_art_resultset_sources})
SET_TARGET_PROPERTIES(test_art_resultset PROPERTIES OUTPUT_NAME "art_resultset")

IF(WIN32)
	MESSAGE("${MY_TARGET_LINK_LIBRARIES_STATIC_CPPLIB}")
	TARGET_LINK_LIBRARIES(test_art_resultset ${MY_TARGET_LINK_LIBRARIES_STATIC_CPPLIB} ${MY_GCOV_LINK_LIBRARIES})
ELSE(WIN32)
	TARGET_LINK_LIBRARIES(test_art_resultset ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})
ENDIF(WIN32)

MESSAGE(STATUS "Configuring unit tests - art_resultset")
