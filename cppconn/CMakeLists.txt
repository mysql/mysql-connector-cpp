#
# Copyright (c) 2008, 2011, Oracle and/or its affiliates. All rights reserved.
#
# The MySQL Connector/C++ is licensed under the terms of the GPLv2
# <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
# MySQL Connectors. There are special exceptions to the terms and
# conditions of the GPLv2 as it is applied to this software, see the
# FLOSS License Exception
# <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published
# by the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
# for more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
#


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


CHECK_FUNCTION_EXISTS(strtold HAVE_FUNCTION_STRTOLD)
CHECK_FUNCTION_EXISTS(strtol HAVE_FUNCTION_STRTOL)
CHECK_FUNCTION_EXISTS(strtoll HAVE_FUNCTION_STRTOLL)
CHECK_FUNCTION_EXISTS(strtoul HAVE_FUNCTION_STRTOUL)
CHECK_FUNCTION_EXISTS(strtoull HAVE_FUNCTION_STRTOULL)
CHECK_FUNCTION_EXISTS(strtoimax HAVE_FUNCTION_STRTOIMAX)
CHECK_FUNCTION_EXISTS(strtoumax HAVE_FUNCTION_STRTOUMAX)


CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cm ${CMAKE_CURRENT_BINARY_DIR}/config.h)


SET(MYSQLCPPCONN_INSTALL_HEADERS
		build_config.h
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
		sqlstring.h
		warning.h)

INSTALL(FILES ${MYSQLCPPCONN_INSTALL_HEADERS} DESTINATION include/cppconn)
