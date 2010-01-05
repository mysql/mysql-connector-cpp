#   Copyright 2007 - 2008 MySQL AB, 2008 - 2010 Sun Microsystems, Inc.  All rights reserved.
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

SET(perf_statement_sources
	../unit_fixture.cpp
	../main.cpp
	perf_statement.cpp)

IF(WIN32)
	SET(perf_statement_sources
    		${perf_statement_sources}
	../unit_fixture.h
	perf_statement.h)
ENDIF(WIN32)

ADD_EXECUTABLE(perf_statement ${perf_statement_sources})
SET_TARGET_PROPERTIES(perf_statement PROPERTIES
					OUTPUT_NAME "perf_statement"
					LINK_FLAGS "${MYSQLCPPCONN_LINK_FLAGS_ENV} ${MYSQL_LINK_FLAGS}"
					COMPILE_FLAGS "${MYSQLCPPCONN_COMPILE_FLAGS_ENV} ${MYSQL_CXXFLAGS}")
TARGET_LINK_LIBRARIES(perf_statement ${MY_TARGET_LINK_LIBRARIES} ${MY_GCOV_LINK_LIBRARIES})

MESSAGE(STATUS "Configuring performance test - statement")
