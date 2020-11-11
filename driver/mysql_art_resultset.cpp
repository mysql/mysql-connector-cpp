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



#include <sstream>
#include <math.h>
#include <stdio.h>

#include "mysql_util.h"
#include "mysql_art_resultset.h"
#include "mysql_art_rset_metadata.h"
#include "mysql_debug.h"

#include <cppconn/exception.h>


namespace sql
{
namespace mysql
{

/* {{{ MyVal::MyVal() -I- */
MyVal::MyVal(const sql::SQLString & s)
  : val_type(typeString)
{
  val.str = new sql::SQLString(s);
}

/* }}} */


/* {{{ MyVal::MyVal() -I- */
MyVal::MyVal(const char * const s)
  : val_type(typeString)
{
  // Init it clearly
  val.str = new sql::SQLString(s);
}
/* }}} */


/* {{{ MyVal::getString() -I- */
sql::SQLString
MyVal::getString()
{
  switch (val_type) {
    case typeString:
      return *val.str;
    case typeDouble:
    {
      char buf[31];
      size_t len = snprintf(buf, sizeof(buf) - 1, "%14.14Lf", val.dval);
      return sql::SQLString(buf, len);
    }
    case typeInt:
    {
      char buf[20];
      size_t len = snprintf(buf, sizeof(buf) - 1, "%lld", (long long int)val.lval);
      return sql::SQLString(buf, len);
    }
    case typeUInt:
    {
      char buf[20];
      size_t len = snprintf(buf, sizeof(buf) - 1, "%llu", (long long unsigned int)val.ulval);
      return sql::SQLString(buf, len);
    }
    case typeBool:
    {
      char buf[3];
      size_t len = snprintf(buf, sizeof(buf) - 1, "%d", val.bval);
      return sql::SQLString(buf, len);
    }
    case typePtr:
      return "";
  }
  throw std::runtime_error("impossible");
}
/* }}} */


/* {{{ MyVal::getDouble() -I- */
long double
MyVal::getDouble()
{
  switch (val_type) {
    case typeString:
      return sql::mysql::util::strtold(val.str->c_str(), NULL);
    case typePtr:
      return .0;
    case typeDouble:
      return val.dval;
    case typeInt:
      return static_cast<long double>(val.lval);
    case typeUInt:
      return static_cast<long double>(val.ulval);
    case typeBool:
      return val.bval ? 1.0 : 0.0;

  }
  throw std::runtime_error("impossible");
}
/* }}} */


/* {{{ MyVal::getInt64() -I- */
int64_t
MyVal::getInt64()
{
  switch (val_type) {
    case typeString:
      return strtoll(val.str->c_str(), NULL, 10);
    case typePtr:
      return 0;
    case typeDouble:
      return static_cast<int64_t>(val.dval);
    case typeBool:
      return val.bval ? 1LL : 0LL;
    case typeInt:
    case typeUInt:
      return val.lval;

  }
  throw std::runtime_error("impossible");
}
/* }}} */


/* {{{ MyVal::getUInt64() -I- */
uint64_t
MyVal::getUInt64()
{
  switch (val_type) {
    case typeString:
      return strtoull(val.str->c_str(), NULL, 10);
    case typePtr:
      return 0;
    case typeDouble:
      return static_cast<uint64_t>(val.dval);
    case typeBool:
      return val.bval ? UL64(1) : UL64(0);
    case typeInt:
    case typeUInt:
      return val.ulval;

  }
  throw std::runtime_error("impossible");
}
/* }}} */


/* {{{ MyVal::getBool() -I- */
bool
MyVal::getBool()
{
  static const double delta= 0.000001;

  switch (val_type) {
    case typeString:
      return getInt64() != 0;
    case typeDouble:
      return !(val.dval < delta && val.dval > -delta);
    case typeInt:
    case typeUInt:
      return val.lval != 0;
    case typeBool:
      return val.bval;
    case typePtr:
      return val.pval != NULL;

  }
  throw std::runtime_error("impossible");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::MySQL_ArtResultSet() -I- */
MySQL_ArtResultSet::MySQL_ArtResultSet(const StringList& fn, boost::shared_ptr< rset_t > &rs, boost::shared_ptr< MySQL_DebugLogger > & l)
  : num_fields(static_cast<int>(fn.size())), rset(rs), current_record(rset->begin()),
    started(false), field_index_to_name_map(new sql::SQLString[num_fields]),
  num_rows(rset->size()), row_position(0), is_closed(false), logger(l)
{
  CPP_ENTER("MySQL_ArtResultSet::MySQL_ArtResultSet");
  CPP_INFO_FMT("metadata.size=%d resultset.size=%d", fn.size(), rset->size());

//	field_index_to_name_map = new sql::SQLString[num_fields];

  unsigned int idx = 0;
  for (StringList::const_iterator it = fn.begin(), e = fn.end(); it != e; ++it, ++idx) {
    field_name_to_index_map[*it] = idx;
    field_index_to_name_map[idx] = *it;
  }

  meta.reset(new MySQL_ArtResultSetMetaData(this, logger));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::~MySQL_ArtResultSet() -I- */
MySQL_ArtResultSet::~MySQL_ArtResultSet()
{
  CPP_ENTER("MySQL_ArtResultSet::~MySQL_ArtResultSet");
  if (!isClosed()) {
    close();
  }
}
/* }}} */


/* {{{ MySQL_ArtResultSet::seek() -I- */
void MySQL_ArtResultSet::seek()
{
  CPP_ENTER("MySQL_ArtResultSet::seek");
  current_record = rset->begin();
  /* i should be signed, or when row_position is 0 `i` will overflow */
  for (long long i = row_position - 1; i > 0; --i) {
    ++current_record;
  }
}
/* }}} */


/* {{{ MySQL_ArtResultSet::isBeforeFirstOrAfterLast() -I- */
bool
MySQL_ArtResultSet::isBeforeFirstOrAfterLast() const
{
  CPP_ENTER("MySQL_ArtResultSet::isBeforeFirstOrAfterLast");
  checkValid();
  return (row_position == 0) || (row_position == num_rows + 1);
}
/* }}} */


/* {{{ MySQL_ArtResultSet::absolute() -I- */
bool
MySQL_ArtResultSet::absolute(const int row)
{
  CPP_ENTER("MySQL_ArtResultSet::absolute");
  checkValid();
  if (row > 0) {
    if (row > (int) num_rows) {
      afterLast();
    } else {
      row_position = row;
      seek();
      return true;
    }
  } else if (row < 0) {
    if ((-row) > (int) num_rows) {
      beforeFirst();
    } else {
      row_position = num_rows - (-row)  + 1;
      seek();
      return true;
    }
  } else {
    /* According to the JDBC book, absolute(0) means before the result set */
    beforeFirst();
  }
  return (row_position > 0 && row_position < (num_rows + 1));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::afterLast() -I- */
void
MySQL_ArtResultSet::afterLast()
{
  CPP_ENTER("MySQL_ArtResultSet::afterLast");
  checkValid();
  row_position = num_rows + 1;
  seek();
}
/* }}} */


/* {{{ MySQL_ArtResultSet::beforeFirst() -I- */
void
MySQL_ArtResultSet::beforeFirst()
{
  CPP_ENTER("MySQL_ArtResultSet::beforeFirst");
  checkValid();
  row_position = 0;
  seek();
}
/* }}} */


/* {{{ MySQL_ArtResultSet::cancelRowUpdates() -U- */
void
MySQL_ArtResultSet::cancelRowUpdates()
{
  CPP_ENTER("MySQL_ArtResultSet::cancelRowUpdates");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::cancelRowUpdates()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::checkValid() -I- */
void
MySQL_ArtResultSet::checkValid() const
{
  CPP_ENTER("MySQL_ArtResultSet::checkValid");
  CPP_INFO_FMT("this=%p", this);
  if (isClosed()) {
    throw sql::InvalidInstanceException("ResultSet has been closed");
  }
}
/* }}} */


/* {{{ MySQL_ArtResultSet::clearWarnings() -U- */
void
MySQL_ArtResultSet::clearWarnings()
{
  CPP_ENTER("MySQL_ArtResultSet::clearWarnings");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::clearWarnings()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::close() -I- */
void
MySQL_ArtResultSet::close()
{
  CPP_ENTER("MySQL_ArtResultSet::close");
  checkValid();
//	delete [] field_index_to_name_map;
  is_closed = true;
}
/* }}} */


/* {{{ MySQL_ArtResultSet::findColumn() -I- */
uint32_t
MySQL_ArtResultSet::findColumn(const sql::SQLString& columnLabel) const
{
  CPP_ENTER("MySQL_ArtResultSet::columnLabel");
  checkValid();

  FieldNameIndexMap::const_iterator iter = field_name_to_index_map.find(columnLabel);

  if (iter == field_name_to_index_map.end()) {
    return 0;
  }
  /* findColumn returns 1-based indexes */
  return iter->second + 1;
}
/* }}} */


/* {{{ MySQL_ArtResultSet::first() -I- */
bool
MySQL_ArtResultSet::first()
{
  CPP_ENTER("MySQL_ArtResultSet::first");
  checkValid();
  if (num_rows) {
    row_position = 1;
    seek();
  }
  return num_rows != 0;
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getBlob() -I- */
std::istream *
MySQL_ArtResultSet::getBlob(const uint32_t columnIndex) const
{
  CPP_ENTER("MySQL_ArtResultSet::getBlob(int)");

  /* isBeforeFirst checks for validity */
  if (isBeforeFirstOrAfterLast()) {
    throw sql::InvalidArgumentException("MySQL_ArtResultSet::getString: can't fetch because not on result set");
  }
  return new std::istringstream(getString(columnIndex));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getBlob() -I- */
std::istream *
MySQL_ArtResultSet::getBlob(const sql::SQLString& columnLabel) const
{
  CPP_ENTER("MySQL_ArtResultSet::getBlob(string)");
  return new std::istringstream(getString(columnLabel));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getBoolean() -I- */
bool
MySQL_ArtResultSet::getBoolean(const uint32_t columnIndex) const
{
  CPP_ENTER("MySQL_ArtResultSet::getBoolean(int)");

  /* isBeforeFirst checks for validity */
  if (isBeforeFirstOrAfterLast()) {
    throw sql::InvalidArgumentException("MySQL_ArtResultSet::getString: can't fetch because not on result set");
  }
  return getInt(columnIndex) != 0;
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getBoolean() -I- */
bool
MySQL_ArtResultSet::getBoolean(const sql::SQLString& columnLabel) const
{
  CPP_ENTER("MySQL_ArtResultSet::getBoolean(string)");
  return getInt(columnLabel) != 0;
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getConcurrency() -U- */
int
MySQL_ArtResultSet::getConcurrency()
{
  CPP_ENTER("MySQL_ArtResultSet::getConcurrency");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getConcurrency()");
  return 0; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getCursorName() -U- */
SQLString
MySQL_ArtResultSet::getCursorName()
{
  CPP_ENTER("MySQL_ArtResultSet::getCursorName");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getCursorName()");
  return 0; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getDouble() -I- */
long double
MySQL_ArtResultSet::getDouble(uint32_t columnIndex) const
{
  CPP_ENTER("MySQL_ArtResultSet::getDouble(int)");

  /* isBeforeFirst checks for validity */
  if (isBeforeFirstOrAfterLast()) {
    throw sql::InvalidArgumentException("MySQL_ArtResultSet::getDouble: can't fetch because not on result set");
  }

  if (columnIndex > num_fields || columnIndex == 0) {
    throw sql::InvalidArgumentException("MySQL_ArtResultSet::getDouble: invalid value of 'columnIndex'");
  }

  return (*current_record)[columnIndex - 1].getDouble();
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getDouble() -I- */
long double
MySQL_ArtResultSet::getDouble(const sql::SQLString& columnLabel) const
{
  CPP_ENTER("MySQL_ArtResultSet::getDouble(string)");
  return getDouble(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getFetchDirection() -U- */
int
MySQL_ArtResultSet::getFetchDirection()
{
  CPP_ENTER("MySQL_ArtResultSet::getFetchDirection");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getFetchDirection()");
  return 0; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getFetchSize() -U- */
size_t
MySQL_ArtResultSet::getFetchSize()
{
  CPP_ENTER("MySQL_ArtResultSet::getFetchSize");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getFetchSize()");
  return 0; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getHoldability() -U- */
int
MySQL_ArtResultSet::getHoldability()
{
  CPP_ENTER("MySQL_ArtResultSet::getHoldability");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getHoldability()");
  return 0; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getInt() -I- */
int32_t
MySQL_ArtResultSet::getInt(uint32_t columnIndex) const
{
  CPP_ENTER("MySQL_ArtResultSet::getInt(int)");
  return static_cast<int32_t>(getInt64(columnIndex));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getInt() -I- */
int32_t
MySQL_ArtResultSet::getInt(const sql::SQLString& columnLabel) const
{
  CPP_ENTER("MySQL_ArtResultSet::getInt(string)");
  return getInt(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getUInt() -I- */
uint32_t
MySQL_ArtResultSet::getUInt(uint32_t columnIndex) const
{
  CPP_ENTER("MySQL_ArtResultSet::getUInt(int)");
  return static_cast<uint32_t>(getUInt64(columnIndex));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getUInt() -I- */
uint32_t
MySQL_ArtResultSet::getUInt(const sql::SQLString& columnLabel) const
{
  CPP_ENTER("MySQL_ArtResultSet::getUInt(string)");
  return getUInt(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getInt64() -I- */
int64_t
MySQL_ArtResultSet::getInt64(const uint32_t columnIndex) const
{
  CPP_ENTER("MySQL_ArtResultSet::getInt64(int)");

  /* isBeforeFirst checks for validity */
  if (isBeforeFirstOrAfterLast()) {
    throw sql::InvalidArgumentException("MySQL_ArtResultSet::getInt64: can't fetch because not on result set");
  }

  if (columnIndex > num_fields || columnIndex == 0) {
    throw sql::InvalidArgumentException("MySQL_ArtResultSet::getInt64: invalid value of 'columnIndex'");
  }

  return (*current_record)[columnIndex - 1].getInt64();
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getInt64() -I- */
int64_t
MySQL_ArtResultSet::getInt64(const sql::SQLString& columnLabel) const
{
  CPP_ENTER("MySQL_ArtResultSet::getInt64(string)");
  return getInt64(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getUInt64() -I- */
uint64_t
MySQL_ArtResultSet::getUInt64(const uint32_t columnIndex) const
{
  CPP_ENTER("MySQL_ArtResultSet::getUInt64(int)");

  /* isBeforeFirst checks for validity */
  if (isBeforeFirstOrAfterLast()) {
    throw sql::InvalidArgumentException("MySQL_ArtResultSet::getUInt64: can't fetch because not on result set");
  }

  if (columnIndex > num_fields || columnIndex == 0) {
    throw sql::InvalidArgumentException("MySQL_ArtResultSet::getUInt64: invalid value of 'columnIndex'");
  }

  return (*current_record)[columnIndex - 1].getUInt64();
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getUInt64() -I- */
uint64_t
MySQL_ArtResultSet::getUInt64(const sql::SQLString& columnLabel) const
{
  CPP_ENTER("MySQL_ArtResultSet::getUInt64(string)");
  return getUInt64(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getMetaData() -I- */
sql::ResultSetMetaData *
MySQL_ArtResultSet::getMetaData() const
{
  CPP_ENTER("MySQL_ArtResultSet::getMetaData");
  checkValid();
  return meta.get();
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getRow() -I- */
size_t
MySQL_ArtResultSet::getRow() const
{
  CPP_ENTER("MySQL_ArtResultSet::getRow");
  checkValid();
  /* row_position is 0 based */
  return static_cast<size_t> (row_position);
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getRowId() -U- */
sql::RowID *
MySQL_ArtResultSet::getRowId(uint32_t)
{
  CPP_ENTER("MySQL_ArtResultSet::getRowId");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getRowId()");
  return NULL; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getRowId() -U- */
sql::RowID *
MySQL_ArtResultSet::getRowId(const sql::SQLString &)
{
  CPP_ENTER("MySQL_ArtResultSet::getRowId");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getRowId()");
  return NULL; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getStatement() -I- */
const Statement *
MySQL_ArtResultSet::getStatement() const
{
  CPP_ENTER("MySQL_ArtResultSet::getStatement");
  checkValid();
  return NULL; /* This is a constructed result set - no statement -> NULL */
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getString() -I- */
SQLString
MySQL_ArtResultSet::getString(uint32_t columnIndex) const
{
  CPP_ENTER("MySQL_ArtResultSet::getString(int)");
  CPP_INFO_FMT("this=%p column=%u", this, columnIndex);

  /* isBeforeFirst checks for validity */
  if (isBeforeFirstOrAfterLast()) {
    throw sql::InvalidArgumentException("MySQL_ArtResultSet::getString: can't fetch because not on result set");
  }

  if (columnIndex > num_fields || columnIndex == 0) {
    throw sql::InvalidArgumentException("MySQL_ArtResultSet::getString: invalid value of 'columnIndex'");
  }

  return (*current_record)[columnIndex - 1].getString();
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getString() -I- */
SQLString
MySQL_ArtResultSet::getString(const sql::SQLString& columnLabel) const
{
  CPP_ENTER("MySQL_ArtResultSet::getString(string)");
  return getString(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getType() -I- */
sql::ResultSet::enum_type
MySQL_ArtResultSet::getType() const
{
  CPP_ENTER("MySQL_ArtResultSet::getType");
  checkValid();
  return sql::ResultSet::TYPE_SCROLL_INSENSITIVE;
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getWarnings() -U- */
void
MySQL_ArtResultSet::getWarnings()
{
  CPP_ENTER("MySQL_ArtResultSet::getWarnings");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getWarnings()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::insertRow() -U- */
void
MySQL_ArtResultSet::insertRow()
{
  CPP_ENTER("MySQL_ArtResultSet::insertRow");
  checkValid();
   /* TODO - We don't support inserting anyway */
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::insertRow()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::isAfterLast() -I- */
bool
MySQL_ArtResultSet::isAfterLast() const
{
  CPP_ENTER("MySQL_ArtResultSet::isAfterLast");
  checkValid();
  return (row_position == num_rows + 1);
}
/* }}} */


/* {{{ MySQL_ArtResultSet::isBeforeFirst() -I- */
bool
MySQL_ArtResultSet::isBeforeFirst() const
{
  CPP_ENTER("MySQL_ArtResultSet::isBeforeFirst");
  checkValid();
  return (row_position == 0);
}
/* }}} */


/* {{{ MySQL_ArtResultSet::isClosed() -I- */
bool
MySQL_ArtResultSet::isClosed() const
{
  CPP_ENTER("MySQL_ArtResultSet::isClosed");
  return is_closed;
}
/* }}} */


/* {{{ MySQL_ArtResultSet::isFirst() -I- */
bool
MySQL_ArtResultSet::isFirst() const
{
  CPP_ENTER("MySQL_ArtResultSet::isFirst");
  checkValid();
  /* OR current_record == rs.begin() */
  return (row_position == 1);
}
/* }}} */


/* {{{ MySQL_ArtResultSet::isLast() -I- */
bool
MySQL_ArtResultSet::isLast() const
{
  CPP_ENTER("MySQL_ArtResultSet::isLast");
  checkValid();
  /* OR current_record == rs.end() */
  return (row_position == num_rows);
}
/* }}} */


/* {{{ MySQL_ArtResultSet::isNull() -I- */
bool
MySQL_ArtResultSet::isNull(const uint32_t columnIndex) const
{
  CPP_ENTER("MySQL_ArtResultSet::isNull(int)");
  checkValid();
  if (columnIndex > num_fields || columnIndex == 0) {
    throw sql::InvalidArgumentException("MySQL_ArtResultSet::isNull: invalid value of 'columnIndex'");
  }
  /* isBeforeFirst checks for validity */
  if (isBeforeFirstOrAfterLast()) {
    throw sql::InvalidArgumentException("MySQL_ArtResultSet::getString: can't fetch because not on result set");
  }
  return false;
}
/* }}} */


/* {{{ MySQL_ArtResultSet::isNull() -I- */
bool
MySQL_ArtResultSet::isNull(const sql::SQLString& columnLabel) const
{
  CPP_ENTER("MySQL_ArtResultSet::isNull(string)");
  return isNull(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::last() -I- */
bool
MySQL_ArtResultSet::last()
{
  CPP_ENTER("MySQL_ArtResultSet::last");
  checkValid();
  if (num_rows) {
    row_position = num_rows;
    seek();
  }
  return num_rows != 0;
}
/* }}} */


/* {{{ MySQL_ArtResultSet::moveToCurrentRow() -U- */
void
MySQL_ArtResultSet::moveToCurrentRow()
{
  CPP_ENTER("MySQL_ArtResultSet::moveToCurrentRow");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::moveToCurrentRow()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::moveToInsertRow() -U- */
void
MySQL_ArtResultSet::moveToInsertRow()
{
  CPP_ENTER("MySQL_ArtResultSet::moveToInsertRow");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::moveToInsertRow()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::next() -I- */
bool
MySQL_ArtResultSet::next()
{
  CPP_ENTER("MySQL_ArtResultSet::next");
  checkValid();
  bool ret = false;
  if (isLast()) {
    afterLast();
  } else if (row_position == 0) {
    first();
    ret = true;
  } else if (row_position > 0 && row_position < num_rows) {
    ++current_record;
    ++row_position;
    ret = true;
  }
  CPP_INFO_FMT("row_position=%llu num_rows=%llu", row_position, num_rows);
  return ret;
}
/* }}} */


/* {{{ MySQL_ArtResultSet::previous() -I- */
bool
MySQL_ArtResultSet::previous()
{
  CPP_ENTER("MySQL_ArtResultSet::previous");

  /* isBeforeFirst checks for validity */
  if (isBeforeFirst()) {
    return false;
  } else if (isFirst()) {
    beforeFirst();
    return false;
  } else if (row_position > 1) {
    --row_position;
    --current_record;
    return true;
  }
  throw sql::SQLException("Impossible");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::refreshRow() -U- */
void
MySQL_ArtResultSet::refreshRow()
{
  CPP_ENTER("MySQL_ArtResultSet::refreshRow");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ResultSet::refreshRow()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::relative() -I- */
bool
MySQL_ArtResultSet::relative(const int rows)
{
  CPP_ENTER("MySQL_ArtResultSet::relative");
  checkValid();
  if (rows != 0) {
    if ((row_position + rows) > num_rows || (row_position + rows) < 1) {
      rows > 0? afterLast(): beforeFirst(); /* after last or before first */
    } else {
      row_position += rows;
      seek();
    }
  }

  return (row_position > 0 && row_position <= num_rows);
}
/* }}} */


/* {{{ MySQL_ArtResultSet::rowDeleted() -U- */
bool
MySQL_ArtResultSet::rowDeleted()
{
  CPP_ENTER("MySQL_ArtResultSet::rowDeleted");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::rowDeleted()");
  return false; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSet::rowInserted() -U- */
bool
MySQL_ArtResultSet::rowInserted()
{
  CPP_ENTER("MySQL_ArtResultSet::rowInserted");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::rowInserted()");
  return false; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSet::rowUpdated() -U- */
bool
MySQL_ArtResultSet::rowUpdated()
{
  CPP_ENTER("MySQL_ArtResultSet::rowUpdated");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ArtResultSet::rowUpdated()");
  return false; // This will shut up compilers
}
/* }}} */


/* {{{ MySQL_ArtResultSet::rowsCount() -I- */
size_t
MySQL_ArtResultSet::rowsCount() const
{
  CPP_ENTER("MySQL_ArtResultSet::rowsCount");
  checkValid();
  return static_cast<size_t>( num_rows );
}
/* }}} */


/* {{{ MySQL_ArtResultSet::setFetchSize() -U- */
void
MySQL_ArtResultSet::setFetchSize(size_t /* rows */)
{
  CPP_ENTER("MySQL_ArtResultSet::setFetchSize");
  checkValid();
  throw sql::MethodNotImplementedException("MySQL_ResultSet::rowDeleted()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::wasNull() -I- */
bool
MySQL_ArtResultSet::wasNull() const
{
  CPP_ENTER("MySQL_ArtResultSet::wasNull");
  checkValid();
  /* isBeforeFirst checks for validity */
  if (isBeforeFirstOrAfterLast()) {
    throw sql::InvalidArgumentException("MySQL_ResultSet::wasNull: can't fetch because not on result set");
  }
  return false;
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
