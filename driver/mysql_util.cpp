
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
#include <string>
#include <memory>
#include "mysql_util.h"

namespace sql {
namespace mysql {
namespace util {

/* {{{ mysql_to_datatype() -I- */
int
mysql_type_to_datatype(const MYSQL_FIELD * const field)
{
	switch (field->type) {
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
			return sql::DataType::REAL;
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
			return sql::DataType::DATE;
		case MYSQL_TYPE_DATETIME:
			return sql::DataType::TIMESTAMP;
		case MYSQL_TYPE_TINY_BLOB:
			if (field->flags & BINARY_FLAG) {
				return sql::DataType::VARBINARY;
			}
			return sql::DataType::VARCHAR;
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
			if (field->flags & BINARY_FLAG) {
				return sql::DataType::LONGVARBINARY;
			}
			return sql::DataType::LONGVARCHAR;
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if (field->flags & BINARY_FLAG) {
				return sql::DataType::VARBINARY;
			}
			return sql::DataType::VARCHAR;
		case MYSQL_TYPE_STRING:
			if (field->flags & BINARY_FLAG) {
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
int
mysql_string_type_to_datatype(const std::string & name)
{
	/*
	  I_S.COLUMNS is buggy, because it deflivers (float|double) unsigned
	  but not (tinyint|smallint|mediumint|int|bigint) unsigned
	*/
	if (!name.compare("bit")) {
		return sql::DataType::BIT;
	} else if (!name.compare("decimal") || !name.compare("decimal unsigned")) {
		return sql::DataType::DECIMAL;
	} else if (!name.compare("tinyint") || !name.compare("tinyint unsigned")) {
		return sql::DataType::TINYINT;
	} else if (!name.compare("smallint") || !name.compare("smallint unsigned")) {
		return sql::DataType::SMALLINT;
	} else if (!name.compare("mediumint") || !name.compare("mediumint unsigned")) {
		return sql::DataType::MEDIUMINT;
	} else if (!name.compare("int") || !name.compare("int unsigned")) {
		return sql::DataType::INTEGER;
	} else if (!name.compare("bigint") || !name.compare("bigint unsigned")) {
		return sql::DataType::BIGINT;
	} else if (!name.compare("float") || !name.compare("float unsigned")) {
		return sql::DataType::REAL;
	} else if (!name.compare("double") || !name.compare("double unsigned")) {
		return sql::DataType::DOUBLE;
	} else if (!name.compare("timestamp")) {
		return sql::DataType::TIMESTAMP;
	} else if (!name.compare("date")) {
		return sql::DataType::DATE;
	} else if (!name.compare("time")) {
		return sql::DataType::TIME;
	} else if (!name.compare("year")) {
		return sql::DataType::DATE;
	} else if (!name.compare("datetime")) {
		return sql::DataType::TIMESTAMP;
	} else if (!name.compare("tinytext")) {
		return sql::DataType::VARCHAR;
	} else if (!name.compare("mediumtext") || !name.compare("text") || !name.compare("longtext")) {
		return sql::DataType::LONGVARCHAR;
	} else if (!name.compare("tinyblob")) {
		return sql::DataType::VARBINARY;
	} else if (!name.compare("mediumblob") || !name.compare("blob") || !name.compare("longblob")) {
		return sql::DataType::LONGVARBINARY;
	} else if (!name.compare("char")) {
		return sql::DataType::CHAR;
	} else if (!name.compare("binary")) {
		return sql::DataType::BINARY;
	} else if (!name.compare("varchar")) {
		return sql::DataType::VARCHAR;
	} else if (!name.compare("varbinary")) {
		return sql::DataType::VARBINARY;
	} else if (!name.compare("enum")) {
		return sql::DataType::CHAR;
	} else if (!name.compare("set")) {
		return sql::DataType::CHAR;
	} else if (!name.compare("geometry")) {
		return sql::DataType::GEOMETRY;
	} else {
		return sql::DataType::UNKNOWN;
	}
}
/* }}} */


/* {{{ mysql_to_datatype() -I- */
const char *
mysql_type_to_string(const MYSQL_FIELD * const field)
{
	bool isUnsigned = (field->flags & UNSIGNED_FLAG) != 0;
	bool isZerofill = (field->flags & ZEROFILL_FLAG) != 0;
	switch (field->type) {
		case MYSQL_TYPE_BIT:
			return "BIT";
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
			return isUnsigned ? (isZerofill? "DECIMAL UNSIGNED ZEROFILL" : "DECIMAL UNSIGNED"): "DECIMAL";
		case MYSQL_TYPE_TINY:
			return isUnsigned ? (isZerofill? "TINYINT UNSIGNED ZEROFILL" : "TINYINT UNSIGNED"): "TINYINT";
		case MYSQL_TYPE_SHORT:
			return isUnsigned ? (isZerofill? "SMALLINT UNSIGNED ZEROFILL" : "SMALLINT UNSIGNED"): "SMALLINT";
		case MYSQL_TYPE_LONG:
			return isUnsigned ? (isZerofill? "INT UNSIGNED ZEROFILL" : "INT UNSIGNED"): "INT";
		case MYSQL_TYPE_FLOAT:
			return isUnsigned ? (isZerofill? "FLOAT UNSIGNED ZEROFILL" : "FLOAT UNSIGNED"): "FLOAT";
		case MYSQL_TYPE_DOUBLE:
			return isUnsigned ? (isZerofill? "DOUBLE UNSIGNED ZEROFILL" : "DOUBLE UNSIGNED"): "DOUBLE";
		case MYSQL_TYPE_NULL:
			return "NULL";
		case MYSQL_TYPE_TIMESTAMP:
			return "TIMESTAMP";
		case MYSQL_TYPE_LONGLONG:
			return isUnsigned ? (isZerofill? "BIGINT UNSIGNED ZEROFILL" : "BIGINT UNSIGNED") : "BIGINT";
		case MYSQL_TYPE_INT24:
			return isUnsigned ? (isZerofill? "MEDIUMINT UNSIGNED ZEROFILL" : "MEDIUMINT UNSIGNED") : "MEDIUMINT";
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
			if (field->flags & BINARY_FLAG) {
				return "BLOB";
			}
			return "TEXT";
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if (field->flags & BINARY_FLAG) {
				return "VARBINARY";
			}
			return "VARCHAR";
		case MYSQL_TYPE_STRING:
			if (field->flags & ENUM_FLAG) {
				return "ENUM";
			}
			if (field->flags & SET_FLAG) {
				return "SET";
			}
			if (field->flags & BINARY_FLAG) {
				return "BINARY";
			}
			return "CHAR";
		case MYSQL_TYPE_ENUM:
			return "ENUM";
		case MYSQL_TYPE_YEAR:
			return "DATE";
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
