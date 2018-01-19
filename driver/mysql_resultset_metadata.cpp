/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */




#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <iostream>
#include <sstream>
#include <boost/shared_ptr.hpp>

#include <cppconn/datatype.h>
#include <cppconn/exception.h>
#include "mysql_util.h"
#include "mysql_resultset.h"
#include "mysql_resultset_metadata.h"

#include "nativeapi/native_resultset_wrapper.h"

#include "mysql_debug.h"

namespace sql
{
namespace mysql
{


/* {{{ MySQL_ResultSetMetaData::MySQL_ResultSetMetaData -I- */
MySQL_ResultSetMetaData::MySQL_ResultSetMetaData(boost::shared_ptr< NativeAPI::NativeResultsetWrapper > res, boost::shared_ptr< MySQL_DebugLogger > & l)
  : result(res), logger(l)
{
  CPP_ENTER("MySQL_ResultSetMetaData::MySQL_ResultSetMetaData");
  boost::shared_ptr< NativeAPI::NativeResultsetWrapper > result_p = result.lock();
  if (result_p) {
    num_fields = result_p->num_fields();
  }
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::~MySQL_ResultSetMetaData -I- */
MySQL_ResultSetMetaData::~MySQL_ResultSetMetaData()
{
  CPP_ENTER("MySQL_ResultSetMetaData::~MySQL_ResultSetMetaData");
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
  CPP_ENTER("MySQL_ResultSetMetaData::checkValid");
  boost::shared_ptr< NativeAPI::NativeResultsetWrapper > result_p = result.lock();
  if (!result_p) {
    throw sql::InvalidArgumentException("ResultSet is not valid anymore");
  }
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getCatalogName -I- */
SQLString
MySQL_ResultSetMetaData::getCatalogName(unsigned int columnIndex)
{
  CPP_ENTER("MySQL_ResultSetMetaData::getCatalogName");
  checkValid();
  checkColumnIndex(columnIndex);
  return getFieldMeta(columnIndex)->catalog;
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
  const MYSQL_FIELD * const field = getFieldMeta(columnIndex);
  const sql::mysql::util::OUR_CHARSET * const cs = sql::mysql::util::find_charset(field->charsetnr);
  if (!cs) {
    std::ostringstream msg("Server sent unknown charsetnr (");
    msg << field->charsetnr << ") . Please report";
    throw SQLException(msg.str());
  }
  int ret = field->length / cs->char_maxlen;

  CPP_INFO_FMT("column=%u name=%s display_size=%d", columnIndex, getFieldMeta(columnIndex)->name, ret);
  return ret;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnLabel -I- */
SQLString
MySQL_ResultSetMetaData::getColumnLabel(unsigned int columnIndex)
{
  CPP_ENTER("MySQL_ResultSetMetaData::getColumnLabel");
  checkValid();
  checkColumnIndex(columnIndex);

  return getFieldMeta(columnIndex)->name;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnName -I- */
SQLString
MySQL_ResultSetMetaData::getColumnName(unsigned int columnIndex)
{
  CPP_ENTER("MySQL_ResultSetMetaData::getColumnName");
  checkValid();
  checkColumnIndex(columnIndex);

  return getFieldMeta(columnIndex)->org_name;
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
SQLString
MySQL_ResultSetMetaData::getColumnTypeName(unsigned int columnIndex)
{
  CPP_ENTER("MySQL_ResultSetMetaData::getColumnTypeName");
  checkValid();
  checkColumnIndex(columnIndex);

  return sql::mysql::util::mysql_type_to_string(getFieldMeta(columnIndex), this->logger);
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnCharset -I- */
SQLString
MySQL_ResultSetMetaData::getColumnCharset(unsigned int columnIndex)
{
  CPP_ENTER("MySQL_ResultSetMetaData::getColumnCharset");
  checkValid();
  checkColumnIndex(columnIndex);

  const MYSQL_FIELD * const field = getFieldMeta(columnIndex);
  const sql::mysql::util::OUR_CHARSET * const cs = sql::mysql::util::find_charset(field->charsetnr);
  if (!cs) {
    std::ostringstream msg;
    msg << "Server sent unknown charsetnr (" << field->charsetnr << ") . Please report";
    throw SQLException(msg.str());
  }
  return cs->name;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getColumnCollation -I- */
SQLString
MySQL_ResultSetMetaData::getColumnCollation(unsigned int columnIndex)
{
  CPP_ENTER("MySQL_ResultSetMetaData::getColumnCollation");
  checkValid();
  checkColumnIndex(columnIndex);

  const MYSQL_FIELD * const field = getFieldMeta(columnIndex);
  const sql::mysql::util::OUR_CHARSET * const cs = sql::mysql::util::find_charset(field->charsetnr);
  if (!cs) {
    std::ostringstream msg;
    msg << "Server sent unknown charsetnr (" << field->charsetnr << ") . Please report";
    throw SQLException(msg.str());
  }
  return cs->collation;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getFieldMeta -I- */
MYSQL_FIELD *
MySQL_ResultSetMetaData::getFieldMeta(unsigned int columnIndex) const
{
  boost::shared_ptr< NativeAPI::NativeResultsetWrapper > result_p = result.lock();
  return result_p->fetch_field_direct(columnIndex - 1);
}
/* }}} */


// Precision - total number of digits
/* {{{ MySQL_ResultSetMetaData::getPrecision -I- */
unsigned int
MySQL_ResultSetMetaData::getPrecision(unsigned int columnIndex)
{
  CPP_ENTER("MySQL_ResultSetMetaData::getPrecision");
  checkValid();
  checkColumnIndex(columnIndex);

  unsigned int ret = getFieldMeta(columnIndex)->max_length - getScale(columnIndex);
  CPP_INFO_FMT("column=%u precision=%d", columnIndex, ret);
  return ret;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getScale -I- */
unsigned int
MySQL_ResultSetMetaData::getScale(unsigned int columnIndex)
{
  CPP_ENTER("MySQL_ResultSetMetaData::getScale");
  checkValid();
  checkColumnIndex(columnIndex);

  unsigned int ret = getFieldMeta(columnIndex)->decimals;
  CPP_INFO_FMT("column=%u scale=%d", columnIndex, ret);
  return ret;
}
/* }}} */


/* {{{ MySQL_ResultSetMetaData::getSchemaName -I- */
SQLString
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
SQLString
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
    msg << "Server sent unknown charsetnr (" << field->charsetnr << ") . Please report";
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


/* {{{ MySQL_ResultSetMetaData::isNumeric -I- */
bool
MySQL_ResultSetMetaData::isNumeric(unsigned int columnIndex)
{
  CPP_ENTER("MySQL_ResultSetMetaData::isNumeric");
  checkValid();
  checkColumnIndex(columnIndex);

  return (getFieldMeta(columnIndex)->flags & NUM_FLAG) != 0;
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
