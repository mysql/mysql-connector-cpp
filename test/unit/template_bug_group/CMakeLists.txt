#   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPL
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPL as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

#
# Instructions for compiling bug123.cpp|h into the binary bug123
# Use Copy&Paste and Search&Replace to add new tests to the build
#

SET(bug123_sources
	../unit_fixture.cpp
	../main.cpp
	bug123.cpp)

IF(WIN32)
	SET(bug123_sources
    		${bug123_sources}
	../unit_fixture.h
	bug123.h)
ENDIF(WIN32)

ADD_EXECUTABLE(bug123 ${bug123_sources})
TARGET_LINK_LIBRARIES(bug123 ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

#
# End of the instructions for building binary bug123 from bug123.cpp|h
#

#
# Instructions for compiling bug456.cpp|h into the binary bug456
#

SET(bug456_sources
	../unit_fixture.cpp
	../main.cpp
	bug456.cpp)

IF(WIN32)
	SET(bug456_sources
    		${bug456_sources}
	../unit_fixture.h
	bug456.h)
ENDIF(WIN32)

ADD_EXECUTABLE(bug456 ${bug456_sources})
TARGET_LINK_LIBRARIES(bug456 ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring unit tests - group template_bug")
