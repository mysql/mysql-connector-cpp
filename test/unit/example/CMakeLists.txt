#   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.
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

SET(example_sources
	../unit_fixture.cpp
	../main.cpp
	example.cpp)

IF(WIN32)
	SET(example_sources
    		${example_sources}
	../unit_fixture.h
	example.h)
ENDIF(WIN32)

ADD_EXECUTABLE(example ${example_sources})
TARGET_LINK_LIBRARIES(example ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

#
# End of the instructions for building binary example from example.cpp|h
#

MESSAGE(STATUS "Configuring unit tests - examples")
