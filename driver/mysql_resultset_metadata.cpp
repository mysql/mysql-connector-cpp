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

#ifndef _WIN32
#include <stdlib.h>
#endif	//	_WIN32

#include "mysql_resultset.h"
#include "mysql_resultset_metadata.h"

#include "mysql_debug.h"
#include "mysql_util.h"

#define MAX_LEN_PER_CHAR 4

namespace sql
{
namespace mysql
{


/* {{{ MySQL_ResultSetMetaData::MySQL_ResultSetMetaData -I- */
MySQL_ResultSetMetaData::MySQL_ResultSetMetaData(MYSQL_RES_Wrapper * res, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l)
  : result(res), logger(l? l->getReference():NULL)
{
	CPP_ENTER("MySQL_ResultSetMetaData::MySQL_ResultSetMetaData");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::~MySQL_ResultSetMetaData -I- */
MySQL_ResultSetMetaData::~MySQL_ResultSetMetaData()
{
	/* Don't remove the block or we can get into problems with logger */
	{
		CPP_ENTER("MySQL_ResultSetMetaData::~MySQL_ResultSetMetaData");
		result->deleteReference();
	}
	logger->freeReference();
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getCatalogName -I- */
std::string
MySQL_ResultSetMetaData::getCatalogName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getCatalogName");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return "";
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnCount -I- */
unsigned int
MySQL_ResultSetMetaData::getColumnCount()
{
	CPP_ENTER("MySQL_ResultSetMetaData::getColumnCount");
	if (result->isValid()) {
		return mysql_num_fields(result->get());
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnDisplaySize -I- */
unsigned int
MySQL_ResultSetMetaData::getColumnDisplaySize(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getColumnDisplaySize");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}

		int ret = mysql_fetch_field_direct(result->get(), columnIndex - 1)->length;
		CPP_INFO_FMT("column=%u name=%s display_size=%d", columnIndex, mysql_fetch_field_direct(result->get(), columnIndex - 1)->name, ret);
		return ret;
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnLabel -I- */
std::string
MySQL_ResultSetMetaData::getColumnLabel(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getColumnLabel");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return mysql_fetch_field_direct(result->get(), columnIndex - 1)->name;
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnName -I- */
std::string
MySQL_ResultSetMetaData::getColumnName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getColumnName");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return mysql_fetch_field_direct(result->get(), columnIndex - 1)->name;
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnType -I- */
int
MySQL_ResultSetMetaData::getColumnType(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getColumnType");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return mysql_fetch_field_direct(result->get(), columnIndex - 1)->type;
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnTypeName -I- */
std::string
MySQL_ResultSetMetaData::getColumnTypeName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getColumnTypeName");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		bool isUnsigned = ( mysql_fetch_field_direct(result->get(), columnIndex - 1)->flags & UNSIGNED_FLAG ) != 0;
		switch (mysql_fetch_field_direct(result->get(), columnIndex - 1)->type) {
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
				return isUnsigned ? "INT UNSIGNED" : "INT";
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
				if (mysql_fetch_field_direct(result->get(), columnIndex - 1)->flags & BINARY_FLAG) {
					return "BLOB";
				}
				return "TEXT";
			case MYSQL_TYPE_VARCHAR:
				return "VARCHAR";
			case MYSQL_TYPE_VAR_STRING:
				if (mysql_fetch_field_direct(result->get(), columnIndex - 1)->flags & BINARY_FLAG) {
					return "VARBINARY";
				}
				return "VARCHAR";
			case MYSQL_TYPE_STRING:
				if (mysql_fetch_field_direct(result->get(), columnIndex - 1)->flags & BINARY_FLAG) {
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

		return mysql_fetch_field_direct(result->get(), columnIndex - 1)->name;
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getPrecision -I- */
unsigned int
MySQL_ResultSetMetaData::getPrecision(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getPrecision");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		int scale = getScale(columnIndex);
		int ret = mysql_fetch_field_direct(result->get(), columnIndex - 1)->length;
		if (scale) {
			ret -= scale + 1;
		}
		CPP_INFO_FMT("column=%u precision=%d", columnIndex, ret);
		return ret;		
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getScale -I- */
unsigned int
MySQL_ResultSetMetaData::getScale(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getScale");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		int ret = mysql_fetch_field_direct(result->get(), columnIndex - 1)->decimals;
		CPP_INFO_FMT("column=%u scale=%d", columnIndex, ret);
		return ret;		
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getSchemaName -I- */
std::string
MySQL_ResultSetMetaData::getSchemaName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getSchemaName");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		char *db = mysql_fetch_field_direct(result->get(), columnIndex - 1)->db;
		return db ? db : "";
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getTableName -I- */
std::string
MySQL_ResultSetMetaData::getTableName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getTableName");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return mysql_fetch_field_direct(result->get(), columnIndex - 1)->org_table;
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isAutoIncrement -I- */
bool
MySQL_ResultSetMetaData::isAutoIncrement(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isAutoIncrement");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return (mysql_fetch_field_direct(result->get(), columnIndex - 1)->flags & AUTO_INCREMENT_FLAG ) != 0;
	}

	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isCaseSensitive -I- */
bool
MySQL_ResultSetMetaData::isCaseSensitive(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isCaseSensitive");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return (mysql_fetch_field_direct(result->get(), columnIndex - 1)->flags & BINARY_FLAG) != 0;
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isCurrency -I- */
bool
MySQL_ResultSetMetaData::isCurrency(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isCurrency");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return false;
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isDefinitelyWritable -I- */
bool
MySQL_ResultSetMetaData::isDefinitelyWritable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isDefinitelyWritable");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return isWritable(columnIndex);
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isNullable -I- */
int
MySQL_ResultSetMetaData::isNullable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isNullable");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return mysql_fetch_field_direct(result->get(), columnIndex - 1)->flags & NOT_NULL_FLAG? columnNoNulls:columnNullable;
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isReadOnly -I- */
bool
MySQL_ResultSetMetaData::isReadOnly(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isReadOnly");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		/* We consider we connect to >= 40100 - else, we can't say */
		char * orgColumnName = mysql_fetch_field_direct(result->get(), columnIndex - 1)->org_name;
		unsigned int orgColumnNameLen = mysql_fetch_field_direct(result->get(), columnIndex - 1)->org_name_length;
		char * orgTableName = mysql_fetch_field_direct(result->get(), columnIndex - 1)->org_table;
		unsigned int orgTableNameLen = mysql_fetch_field_direct(result->get(), columnIndex - 1)->org_table_length;

		return !(orgColumnName != NULL && orgColumnNameLen > 0 && orgTableName != NULL && orgTableNameLen > 0);
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isSearchable -I- */
bool
MySQL_ResultSetMetaData::isSearchable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isSearchable");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return true;
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isSigned -I- */
bool
MySQL_ResultSetMetaData::isSigned(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isSigned");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return !(mysql_fetch_field_direct(result->get(), columnIndex - 1)->flags & UNSIGNED_FLAG);
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isWritable -I- */
bool
MySQL_ResultSetMetaData::isWritable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isWritable");
	if (result->isValid()) {
		if (columnIndex == 0 || columnIndex > mysql_num_fields(result->get())) {
			throw sql::InvalidArgumentException("Invalid value for columnIndex");
		}
		return !isReadOnly(columnIndex);
	}
	throw sql::InvalidArgumentException("ResultSet is not valid anymore");
}
/* }}} */


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
