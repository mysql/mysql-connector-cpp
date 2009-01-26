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

/* {{{ mysql_to_datatype() -I- */
int
mysql_type_to_datatype(int mysql_type, int flags)
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
/* }}} */


/* {{{ mysql_to_datatype() -I- */
const char *
mysql_type_to_string(const int cppconn_type, const int flags)
{
	bool isUnsigned = (flags & UNSIGNED_FLAG) != 0;
	switch (cppconn_type) {
		case MYSQL_TYPE_BIT:
			return "BIT";
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
			return isUnsigned ? "DECIMAL UNSIGNED" : "DECIMAL";
		case MYSQL_TYPE_TINY:
			return isUnsigned ? "TINYINT UNSIGNED" : "TINYINT";
		case MYSQL_TYPE_SHORT:
			return isUnsigned ? "SMALLINT UNSIGNED" : "SMALLINT";
		case MYSQL_TYPE_LONG:
			return isUnsigned ? "INTEGER UNSIGNED" : "INT";
		case MYSQL_TYPE_FLOAT:
			return isUnsigned ? "FLOAT UNSIGNED" : "FLOAT";
		case MYSQL_TYPE_DOUBLE:
			return isUnsigned ? "DOUBLE UNSIGNED" : "DOUBLE";
		case MYSQL_TYPE_NULL:
			return "NULL";
		case MYSQL_TYPE_TIMESTAMP:
			return "TIMESTAMP";
		case MYSQL_TYPE_LONGLONG:
			return isUnsigned ? "BIGINT UNSIGNED" : "BIGINT";
		case MYSQL_TYPE_INT24:
			return isUnsigned ? "MEDIUMINT UNSIGNED" : "MEDIUMINT";
		case MYSQL_TYPE_DATE:
			return "DATE";
		case MYSQL_TYPE_TIME:
			return "TIME";
		case MYSQL_TYPE_DATETIME:
			return "DATETIME";
		case MYSQL_TYPE_TINY_BLOB:
			return "TINYBLOB";
		case MYSQL_TYPE_MEDIUM_BLOB:
			return "MEDIUMBLOB";
		case MYSQL_TYPE_LONG_BLOB:
			return "LONGBLOB";
		case MYSQL_TYPE_BLOB:
			if (flags & BINARY_FLAG) {
				return "BLOB";
			}
			return "TEXT";
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if (flags & BINARY_FLAG) {
				return "VARBINARY";
			}
			return "VARCHAR";
		case MYSQL_TYPE_STRING:
			if (flags & ENUM_FLAG) {
				return "ENUM";
			}
			if (flags & BINARY_FLAG) {
				return "BINARY";
			}
			return "CHAR";
		case MYSQL_TYPE_ENUM:
			return "ENUM";
		case MYSQL_TYPE_YEAR:
			return "YEAR";
		case MYSQL_TYPE_SET:
			return "SET";
		case MYSQL_TYPE_GEOMETRY:
			return "GEOMETRY";
		default:
			return "UNKNOWN";
	}
}
/* }}} */


}; /* namespace util */
}; /* namespace mysql */
}; /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
