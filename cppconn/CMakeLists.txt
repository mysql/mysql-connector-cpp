#   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.
#
#   The MySQL Connector/C++ is licensed under the terms of the GPL
#   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
#   MySQL Connectors. There are special exceptions to the terms and
#   conditions of the GPL as it is applied to this software, see the
#   FLOSS License Exception
#   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)

SET(MYSQLCPPCONN_INSTALL_HEADERS
		connection.h
		datatype.h
		driver.h
		exception.h
		metadata.h
		parameter_metadata.h
		prepared_statement.h
		resultset.h
		resultset_metadata.h
		statement.h
		warning.h)

INSTALL(FILES ${MYSQLCPPCONN_INSTALL_HEADERS} DESTINATION include/cppconn)
