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

CHECK_INCLUDE_FILES(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILES(inttypes.h HAVE_INTTYPES_H)

CHECK_TYPE_SIZE("int8_t"   HAVE_INT8_T)
CHECK_TYPE_SIZE("uint8_t"  HAVE_UINT8_T)
CHECK_TYPE_SIZE("int16_t"  HAVE_INT16_T)
CHECK_TYPE_SIZE("uint16_t" HAVE_UINT16_T)
CHECK_TYPE_SIZE("int32_t"  HAVE_INT32_T)
CHECK_TYPE_SIZE("uint32_t" HAVE_UINT32_T)
CHECK_TYPE_SIZE("int64_t"  HAVE_INT64_T)
CHECK_TYPE_SIZE("uint64_t" HAVE_UINT64_T)
CHECK_TYPE_SIZE("__int8"			HAVE_MS_INT8)
CHECK_TYPE_SIZE("unsigned __int8" 	HAVE_MS_UINT8)
CHECK_TYPE_SIZE("__int16"			HAVE_MS_INT16)
CHECK_TYPE_SIZE("unsigned __int16"	HAVE_MS_UINT16)
CHECK_TYPE_SIZE("__int32"			HAVE_MS_INT32)
CHECK_TYPE_SIZE("unsigned __int32"	HAVE_MS_UINT32)
CHECK_TYPE_SIZE("__int64"			HAVE_MS_INT64)
CHECK_TYPE_SIZE("unsigned __int64"	HAVE_MS_UINT64)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config.h)


SET(MYSQLCPPCONN_INSTALL_HEADERS
		config.h
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
