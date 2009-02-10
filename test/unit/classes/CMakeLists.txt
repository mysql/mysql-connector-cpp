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

SET(classes_sources
	../unit_fixture.cpp
	../main.cpp
	connection.cpp
	resultsetmetadata.cpp
	connectionmetadata.cpp)

IF(WIN32)
	SET(classes_sources
    		${classes_sources}
	../unit_fixture.h
	connection.h
	resultsetmetadata.h
	connectionmetadata.h)
ENDIF(WIN32)

ADD_EXECUTABLE(classes ${classes_sources})
TARGET_LINK_LIBRARIES(classes ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

#
# End of the instructions for building binary classes from classes/*.cpp|h
#

MESSAGE(STATUS "Configuring unit tests - classes")
