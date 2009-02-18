#   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.
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
	connectionmetadata.h)
ENDIF(WIN32)

ADD_EXECUTABLE(test_resultset ${test_resultset_sources})
SET_TARGET_PROPERTIES(test_resultset PROPERTIES OUTPUT_NAME "resultset")
TARGET_LINK_LIBRARIES(test_resultset ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring unit tests - resultset")
