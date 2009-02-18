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

SET(connection_sources
	../unit_fixture.cpp
	../main.cpp
	connection.cpp)

IF(WIN32)
	SET(connection_sources
    		${connection_sources}
	../unit_fixture.h
	connection.h)
ENDIF(WIN32)

ADD_EXECUTABLE(connection ${connection_sources})
TARGET_LINK_LIBRARIES(connection ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring unit tests - connection")

SET(resultsetmetadata_sources
	../unit_fixture.cpp
	../main.cpp
	resultsetmetadata.cpp)

IF(WIN32)
	SET(resultsetmetadata_sources
    		${resultsetmetadata_sources}
	../unit_fixture.h
	resultsetmetadata.h)
ENDIF(WIN32)

ADD_EXECUTABLE(resultsetmetadata ${resultsetmetadata_sources})
TARGET_LINK_LIBRARIES(resultsetmetadata ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring unit tests - resultsetmetadata")

SET(resultset_sources
	../unit_fixture.cpp
	../main.cpp
	resultset.cpp)

IF(WIN32)
	SET(resultset_sources
    		${resultset_sources}
	../unit_fixture.h
	connectionmetadata.h)
ENDIF(WIN32)

ADD_EXECUTABLE(resultset ${resultset_sources})
TARGET_LINK_LIBRARIES(resultset ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring unit tests - resultset")
