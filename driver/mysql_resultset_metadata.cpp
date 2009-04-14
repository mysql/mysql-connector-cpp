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

#include <cppconn/datatype.h>
#include <cppconn/exception.h>
#include "mysql_util.h"
#include "mysql_resultset.h"
#include "mysql_resultset_metadata.h"

#include "mysql_debug.h"

namespace sql
{
namespace mysql
{


/* {{{ MySQL_ResultSetMetaData::MySQL_ResultSetMetaData -I- */
MySQL_ResultSetMetaData::MySQL_ResultSetMetaData(MYSQL_RES_Wrapper * res, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l)
  : result(res), logger(l? l->getReference():NULL), num_fields(mysql_num_fields(result->get()))
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


/* {{{ MySQL_ResultSetMetaData::checkColumnIndex -I- */
void
MySQL_ResultSetMetaData::checkColumnIndex(unsigned int columnIndex) const
{
	if (columnIndex == 0 || columnIndex > num_fields) {
		throw sql::InvalidArgumentException("Invalid value for columnIndex");
	}
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::checkValid -I- */
void
MySQL_ResultSetMetaData::checkValid() const
{
	if (!result->isValid()) {
		throw sql::InvalidArgumentException("ResultSet is not valid anymore");
	}
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getCatalogName -I- */
std::string
MySQL_ResultSetMetaData::getCatalogName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getCatalogName");
	checkValid();
	checkColumnIndex(columnIndex);
	return "";
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnCount -I- */
unsigned int
MySQL_ResultSetMetaData::getColumnCount()
{
	CPP_ENTER("MySQL_ResultSetMetaData::getColumnCount");
	checkValid();
	return num_fields;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnDisplaySize -I- */
unsigned int
MySQL_ResultSetMetaData::getColumnDisplaySize(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getColumnDisplaySize");
	checkValid();
	checkColumnIndex(columnIndex);

	int ret = getFieldMeta(columnIndex)->length;
	CPP_INFO_FMT("column=%u name=%s display_size=%d", columnIndex, getFieldMeta(columnIndex)->name, ret);
	return ret;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnLabel -I- */
std::string
MySQL_ResultSetMetaData::getColumnLabel(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getColumnLabel");
	checkValid();
	checkColumnIndex(columnIndex);

	return getFieldMeta(columnIndex)->name;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnName -I- */
std::string
MySQL_ResultSetMetaData::getColumnName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getColumnName");
	checkValid();
	checkColumnIndex(columnIndex);

	return getFieldMeta(columnIndex)->name;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnType -I- */
int
MySQL_ResultSetMetaData::getColumnType(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getColumnType");
	checkValid();
	checkColumnIndex(columnIndex);

	return sql::mysql::util::mysql_type_to_datatype(getFieldMeta(columnIndex));
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnTypeName -I- */
std::string
MySQL_ResultSetMetaData::getColumnTypeName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getColumnTypeName");
	checkValid();
	checkColumnIndex(columnIndex);

	return sql::mysql::util::mysql_type_to_string(getFieldMeta(columnIndex));
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getScale -I- */
unsigned int
MySQL_ResultSetMetaData::getScale(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getScale");
	checkValid();
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


/* {{{ MySQL_ResultSetMetaData::getPrecision -I- */
unsigned int
MySQL_ResultSetMetaData::getPrecision(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getPrecision");
	checkValid();
	checkColumnIndex(columnIndex);

	unsigned int ret = getFieldMeta(columnIndex)->decimals;
	CPP_INFO_FMT("column=%u scale=%d", columnIndex, ret);
	return ret;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getSchemaName -I- */
std::string
MySQL_ResultSetMetaData::getSchemaName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getSchemaName");
	checkValid();
	checkColumnIndex(columnIndex);

	const char * const db = getFieldMeta(columnIndex)->db;
	return db ? db : "";
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getTableName -I- */
std::string
MySQL_ResultSetMetaData::getTableName(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::getTableName");
	checkValid();
	checkColumnIndex(columnIndex);

	return getFieldMeta(columnIndex)->org_table;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isAutoIncrement -I- */
bool
MySQL_ResultSetMetaData::isAutoIncrement(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isAutoIncrement");
	checkValid();
	checkColumnIndex(columnIndex);

	return (getFieldMeta(columnIndex)->flags & AUTO_INCREMENT_FLAG ) != 0;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isCaseSensitive -I- */
bool
MySQL_ResultSetMetaData::isCaseSensitive(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isCaseSensitive");
	checkValid();
	checkColumnIndex(columnIndex);

	const MYSQL_FIELD * const field = getFieldMeta(columnIndex);
	if (field->flags & NUM_FLAG || field->type == MYSQL_TYPE_NEWDECIMAL || field->type == MYSQL_TYPE_DECIMAL) {
		return false;
	}
	const sql::mysql::util::OUR_CHARSET * const cs = sql::mysql::util::find_charset(field->charsetnr);
	if (!cs) {
		std::ostringstream msg;
		msg << "Server sent uknown charsetnr (" << field->charsetnr << ") . Please report";
		throw SQLException(msg.str());
	}
	return NULL == strstr(cs->collation, "_ci");
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isCurrency -I- */
bool
MySQL_ResultSetMetaData::isCurrency(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isCurrency");
	checkValid();
	checkColumnIndex(columnIndex);

	return false;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isDefinitelyWritable -I- */
bool
MySQL_ResultSetMetaData::isDefinitelyWritable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isDefinitelyWritable");
	checkValid();
	checkColumnIndex(columnIndex);

	return isWritable(columnIndex);
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isNullable -I- */
int
MySQL_ResultSetMetaData::isNullable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isNullable");
	checkValid();
	checkColumnIndex(columnIndex);

	return getFieldMeta(columnIndex)->flags & NOT_NULL_FLAG? columnNoNulls:columnNullable;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isReadOnly -I- */
bool
MySQL_ResultSetMetaData::isReadOnly(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isReadOnly");
	checkValid();
	checkColumnIndex(columnIndex);

	const char * const db = getFieldMeta(columnIndex)->db;
	return !(db && strlen(db));
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isSearchable -I- */
bool
MySQL_ResultSetMetaData::isSearchable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isSearchable");
	checkValid();
	checkColumnIndex(columnIndex);

	return true;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isSigned -I- */
bool
MySQL_ResultSetMetaData::isSigned(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isSigned");
	checkValid();
	checkColumnIndex(columnIndex);

	if (getFieldMeta(columnIndex)->type == FIELD_TYPE_YEAR) {
		return false;
	}
	return !(getFieldMeta(columnIndex)->flags & UNSIGNED_FLAG);
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isWritable -I- */
bool
MySQL_ResultSetMetaData::isWritable(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isWritable");
	checkValid();
	checkColumnIndex(columnIndex);

	return !isReadOnly(columnIndex);
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::isZerofill -I- */
bool
MySQL_ResultSetMetaData::isZerofill(unsigned int columnIndex)
{
	CPP_ENTER("MySQL_ResultSetMetaData::isZerofill");
	checkValid();
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
