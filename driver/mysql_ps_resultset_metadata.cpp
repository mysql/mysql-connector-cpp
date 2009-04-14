/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/


#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

#include <cppconn/exception.h>
#include "mysql_util.h"
#include "mysql_ps_resultset.h"
#include "mysql_ps_resultset_metadata.h"

#include "mysql_debug.h"

#define MAX_LEN_PER_CHAR 4

namespace sql
{
namespace mysql
{


/* {{{ MySQL_Prepared_ResultSetMetaData::MySQL_Prepared_ResultSetMetaData -I- */
MySQL_Prepared_ResultSetMetaData::MySQL_Prepared_ResultSetMetaData(MYSQL_STMT * s, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger> * l)
  :logger(l? l->getReference():NULL), result_meta(mysql_stmt_result_metadata(s)), num_fields(mysql_stmt_field_count(s))
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::MySQL_Prepared_ResultSetMetaData");
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::~MySQL_Prepared_ResultSetMetaData -I- */
MySQL_Prepared_ResultSetMetaData::~MySQL_Prepared_ResultSetMetaData()
{
	/* Don't remove the block or we can get into problems with logger */
	{
		CPP_ENTER("MySQL_Prepared_ResultSetMetaData::~MySQL_Prepared_ResultSetMetaData");
		CPP_INFO_FMT("this=%p", this);
		mysql_free_result(result_meta);
	}
	logger->freeReference();
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::checkColumnIndex -I- */
void
MySQL_Prepared_ResultSetMetaData::checkColumnIndex(unsigned int columnIndex) const
{
	if (columnIndex == 0 || columnIndex > num_fields) {
		throw sql::InvalidArgumentException("Invalid value for columnIndex");
	}
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::getCatalogName -I- */
std::string
MySQL_Prepared_ResultSetMetaData::getCatalogName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::getCatalogName");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return getFieldMeta(columnIndex)->catalog;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::getColumnCount -I- */
unsigned int
MySQL_Prepared_ResultSetMetaData::getColumnCount()
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::getColumnCount");
	CPP_INFO_FMT("this=%p", this);
	return num_fields;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::getColumnDisplaySize -I- */
unsigned int
MySQL_Prepared_ResultSetMetaData::getColumnDisplaySize(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::getColumnDisplaySize");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	int ret = getFieldMeta(columnIndex)->length;
	CPP_INFO_FMT("column=%u display_size=%d", columnIndex, ret);
	return ret;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::getColumnLabel -I- */
std::string
MySQL_Prepared_ResultSetMetaData::getColumnLabel(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::getColumnLabel");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return getFieldMeta(columnIndex)->name;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::getColumnName -I- */
std::string
MySQL_Prepared_ResultSetMetaData::getColumnName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::getColumnName");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return getFieldMeta(columnIndex)->name;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::getColumnType -I- */
int
MySQL_Prepared_ResultSetMetaData::getColumnType(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::getColumnType");
	CPP_INFO_FMT("this=%p", this);
	CPP_INFO_FMT("column=%u", columnIndex);
	checkColumnIndex(columnIndex);
	int mysql_type = getFieldMeta(columnIndex)->type;
	CPP_INFO_FMT("type=%d", mysql_type);
	int ret = sql::mysql::util::mysql_type_to_datatype(
					getFieldMeta(columnIndex)
				);
	CPP_INFO_FMT("our type is %d", ret);
	return ret;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::getColumnTypeName -I- */
std::string
MySQL_Prepared_ResultSetMetaData::getColumnTypeName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::getColumnTypeName");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return sql::mysql::util::mysql_type_to_string(
				getFieldMeta(columnIndex)
			);
}
/* }}} */


// Scale - Number of digits right of the decimal point
/* {{{ MySQL_Prepared_ResultSetMetaData::getPrecision -I- */
unsigned int
MySQL_Prepared_ResultSetMetaData::getPrecision(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::getPrecision");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	unsigned int ret = getFieldMeta(columnIndex)->decimals;
	CPP_INFO_FMT("column=%u scale=%d", columnIndex, ret);
	return ret;
}
/* }}} */


// Precision - total number of digits
/* {{{ MySQL_Prepared_ResultSetMetaData::getScale -I- */
unsigned int
MySQL_Prepared_ResultSetMetaData::getScale(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::getScale");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	unsigned int precision = getPrecision(columnIndex);
	unsigned int ret = getFieldMeta(columnIndex)->length;
	if (precision) {
		ret = precision - ret;
	}
	CPP_INFO_FMT("column=%u precision=%d", columnIndex, ret);
	return ret;
}
/* }}} */



/* {{{ MySQL_Prepared_ResultSetMetaData::getSchemaName -I- */
std::string
MySQL_Prepared_ResultSetMetaData::getSchemaName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::getSchemaName");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	const char * const db = getFieldMeta(columnIndex)->db;
	return db ? db : "";
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::getTableName -I- */
std::string
MySQL_Prepared_ResultSetMetaData::getTableName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::getTableName");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return getFieldMeta(columnIndex)->org_table;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::isAutoIncrement -I- */
bool
MySQL_Prepared_ResultSetMetaData::isAutoIncrement(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::isAutoIncrement");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return (getFieldMeta(columnIndex)->flags & AUTO_INCREMENT_FLAG ) != 0;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::isCaseSensitive -I- */
bool
MySQL_Prepared_ResultSetMetaData::isCaseSensitive(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::isCaseSensitive");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	const MYSQL_FIELD * const field = getFieldMeta(columnIndex);
	if (field->flags & NUM_FLAG || field->type == MYSQL_TYPE_NEWDECIMAL || field->type == MYSQL_TYPE_DECIMAL) {
		return false;
	}
	const sql::mysql::util::OUR_CHARSET * const cs = sql::mysql::util::find_charset(field->charsetnr);
	if (!cs) {
		std::ostringstream msg("Server sent uknown charsetnr (");
		msg << field->charsetnr << ") . Please report";
		throw SQLException(msg.str());
	}
	return NULL == strstr(cs->collation, "_ci");
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::isCurrency -I- */
bool
MySQL_Prepared_ResultSetMetaData::isCurrency(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::isCurrency");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return false;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::isDefinitelyWritable -I- */
bool
MySQL_Prepared_ResultSetMetaData::isDefinitelyWritable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::isDefinitelyWritable");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return isWritable(columnIndex);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::isNullable -I- */
int
MySQL_Prepared_ResultSetMetaData::isNullable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::isNullable");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return getFieldMeta(columnIndex)->flags & NOT_NULL_FLAG? columnNoNulls : columnNullable;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::isReadOnly -I- */
bool
MySQL_Prepared_ResultSetMetaData::isReadOnly(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::isReadOnly");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	/* Seems for Views, where the value is generated DB is empty everything else is set */
	const char * const db = getFieldMeta(columnIndex)->db;
	return !(db && strlen(db));
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::isSearchable -I- */
bool
MySQL_Prepared_ResultSetMetaData::isSearchable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::isSearchable");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return true;
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::isSigned -I- */
bool
MySQL_Prepared_ResultSetMetaData::isSigned(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::isSigned");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	if (getFieldMeta(columnIndex)->type == FIELD_TYPE_YEAR) {
		return false;
	}
	return !(getFieldMeta(columnIndex)->flags & UNSIGNED_FLAG);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::isWritable -I- */
bool
MySQL_Prepared_ResultSetMetaData::isWritable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::isWritable");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return !isReadOnly(columnIndex);
}
/* }}} */


/* {{{ MySQL_Prepared_ResultSetMetaData::isZerofill -I- */
bool
MySQL_Prepared_ResultSetMetaData::isZerofill(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_Prepared_ResultSetMetaData::isZerofill");
	CPP_INFO_FMT("this=%p", this);
	checkColumnIndex(columnIndex);
	return (getFieldMeta(columnIndex)->flags & ZEROFILL_FLAG) != 0;
}
/* }}} */


} /* namespace mysql */
} /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
