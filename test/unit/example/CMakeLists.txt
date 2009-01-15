#   Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.
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

SET(example_sources
	../unit_fixture.cpp
	../main.cpp
	../../CJUnitTestsPort/framework/test_suite.cpp
	../../CJUnitTestsPort/framework/test_factory.cpp
	../../CJUnitTestsPort/framework/test_listener.cpp
	../../CJUnitTestsPort/framework/test_asserts.cpp
	../../CJUnitTestsPort/framework/test_tapOutputter.cpp
	../../CJUnitTestsPort/framework/test_runner.cpp
	../../../common/stringutils.cpp
	example.cpp)

IF(WIN32)
	SET(example_sources
    		${example_sources}
	unit_fixture.h
	../../CJUnitTestsPort/framework/test_runner.h
	../../CJUnitTestsPort/framework/test_suite.h
	../../CJUnitTestsPort/framework/test_case.h
	../../CJUnitTestsPort/framework/test_factory.h
	../../CJUnitTestsPort/framework/test_listener.h
	../../CJUnitTestsPort/framework/test_asserts.h
	../../CJUnitTestsPort/framework/test_outputter.h
	../../CJUnitTestsPort/framework/test_tapOutputter.h
	../../../common/stringutils.h
	example.h)
ENDIF(WIN32)

ADD_EXECUTABLE(example ${example_sources})
TARGET_LINK_LIBRARIES(example ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

#
# End of the instructions for building binary example from example.cpp|h
#

MESSAGE(STATUS "Configuring unit tests - examples")
