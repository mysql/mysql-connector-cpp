/* Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   There are special exceptions to the terms and conditions of the GPL 
   as it is applied to this software. View the full text of the 
   exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this 
   software distribution.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include <cppconn/datatype.h>
#include <memory>
#include "mysql_private_iface.h"

namespace sql {
namespace mysql {
namespace util {
int mysql_to_datatype(int mysql_type, int flags)
{
	switch (mysql_type) {
		case MYSQL_TYPE_BIT:
			return sql::DataType::BIT;
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
			return sql::DataType::DECIMAL;
		case MYSQL_TYPE_TINY:
			return sql::DataType::TINYINT;
		case MYSQL_TYPE_SHORT:
			return sql::DataType::SMALLINT;
		case MYSQL_TYPE_INT24:
			return sql::DataType::MEDIUMINT;
		case MYSQL_TYPE_LONG:
			return sql::DataType::INTEGER;
		case MYSQL_TYPE_LONGLONG:
			return sql::DataType::BIGINT;
		case MYSQL_TYPE_FLOAT:
			return sql::DataType::FLOAT;
		case MYSQL_TYPE_DOUBLE:
			return sql::DataType::DOUBLE;
		case MYSQL_TYPE_NULL:
			return sql::DataType::SQLNULL;
		case MYSQL_TYPE_TIMESTAMP:
			return sql::DataType::TIMESTAMP;
		case MYSQL_TYPE_DATE:
			return sql::DataType::DATE;
		case MYSQL_TYPE_TIME:
			return sql::DataType::TIME;
		case MYSQL_TYPE_YEAR:
			return sql::DataType::YEAR;
		case MYSQL_TYPE_DATETIME:
			return sql::DataType::DATETIME;
		case MYSQL_TYPE_TINY_BLOB:
			if (flags & BINARY_FLAG) {
				return sql::DataType::VARBINARY;
			}
			return sql::DataType::VARCHAR;
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
			if (flags & BINARY_FLAG) {
				return sql::DataType::LONGVARBINARY;
			}
			return sql::DataType::LONGVARCHAR;
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if (flags & BINARY_FLAG) {
				return sql::DataType::VARBINARY;
			}
			return sql::DataType::VARCHAR;
		case MYSQL_TYPE_STRING:
			if (flags & BINARY_FLAG) {
				return sql::DataType::BINARY;
			}
			return sql::DataType::CHAR;
		case MYSQL_TYPE_ENUM:
			return sql::DataType::CHAR;
		case MYSQL_TYPE_SET:
			return sql::DataType::CHAR;
		case MYSQL_TYPE_GEOMETRY:
			return sql::DataType::GEOMETRY;
		default:
			return sql::DataType::UNKNOWN;
	}
}


}; /* namespace util */
}; /* namespace mysql */
}; /* namespace sql */
