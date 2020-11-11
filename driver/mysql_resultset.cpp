/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates.
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



#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <limits>
#include <boost/scoped_array.hpp>

#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/warning.h>
#include "mysql_util.h"
#include "mysql_resultset.h"
#include "mysql_resultset_metadata.h"
#include "mysql_statement.h"

#include "nativeapi/native_connection_wrapper.h"
#include "nativeapi/native_resultset_wrapper.h"

#include "mysql_debug.h"

//Prevent windows min() macro because of std::numeric_limits<int>::min()
#undef min

namespace sql
{
namespace mysql
{



/* {{{ MySQL_ResultSet::MySQL_ResultSet() -I- */
MySQL_ResultSet::MySQL_ResultSet(boost::shared_ptr< NativeAPI::NativeResultsetWrapper > res, std::weak_ptr< NativeAPI::NativeConnectionWrapper > _proxy, sql::ResultSet::enum_type rset_type,
            MySQL_Statement * par, boost::shared_ptr< MySQL_DebugLogger > & l
        )
    : row(NULL), result(res), proxy(_proxy), row_position(0), was_null(false),
      last_queried_column(-1), parent(par), logger(l), resultset_type(rset_type)
{
    CPP_ENTER("MySQL_ResultSet::MySQL_ResultSet");
    num_rows = result->num_rows();

    num_fields = result->num_fields();
    for (unsigned int i = 0; i < num_fields; ++i) {
#if A0
        std::cout << "Elements=" << field_name_to_index_map.size() << "\n";
#endif
        field_name_to_index_map[getFieldMeta(i + 1)->name] = i;
    }
#if A0
    std::cout << "Elements=" << field_name_to_index_map.size() << "\n";
#endif
    rs_meta.reset(new MySQL_ResultSetMetaData(result, logger));
}
/* }}} */


/* {{{ MySQL_ResultSet::~MySQL_ResultSet() -I- */
MySQL_ResultSet::~MySQL_ResultSet()
{
    CPP_ENTER("MySQL_ResultSet::~MySQL_ResultSet");
}
/* }}} */


/* {{{ MySQL_ResultSet::absolute() -I- */
bool
MySQL_ResultSet::absolute(const int new_pos)
{
    CPP_ENTER("MySQL_ResultSet::absolute");
    checkValid();
    checkScrollable();
    if (new_pos > 0) {
        if (new_pos > (int) num_rows) {
            row_position = num_rows + 1; /* after last row */
        } else {
            row_position = (my_ulonglong) new_pos; /* the cast is inspected and is valid */
            seek();
            return true;
        }
    } else if (new_pos < 0) {
        if ((-new_pos) > (int) num_rows || (new_pos == std::numeric_limits<int>::min())) {
            row_position = 0; /* before first new_pos */
        } else {
            row_position = num_rows - (-new_pos)  + 1;
            seek();
            return true;
        }
    } else {
        /* According to the JDBC book, absolute(0) means before the result set */
        row_position = 0;
        /* no seek() here, as we are not on data*/
        result->data_seek(0);
    }
    return (row_position > 0 && row_position < (num_rows + 1));
}
/* }}} */


/* {{{ MySQL_ResultSet::afterLast() -I- */
void
MySQL_ResultSet::afterLast()
{
    CPP_ENTER("MySQL_ResultSet::afterLast");
    checkValid();
    row_position = num_rows + 1;
}
/* }}} */


/* {{{ MySQL_ResultSet::beforeFirst() -I- */
void
MySQL_ResultSet::beforeFirst()
{
    CPP_ENTER("MySQL_ResultSet::beforeFirst");
    checkValid();
    checkScrollable();
    result->data_seek(0);
    row_position = 0;
}
/* }}} */


/* {{{ MySQL_ResultSet::cancelRowUpdates() -U- */
void
MySQL_ResultSet::cancelRowUpdates()
{
    CPP_ENTER("MySQL_ResultSet::cancelRowUpdates");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::cancelRowUpdates()");
}
/* }}} */


/* {{{ MySQL_ResultSet::checkScrollable() -I- */
void
MySQL_ResultSet::checkScrollable() const
{
    CPP_ENTER("MySQL_ResultSet::checkScrollable");
    CPP_INFO_FMT("this=%p", this);
    if (resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY) {
        throw sql::NonScrollableException("Nonscrollable result set");
    }
    // reset last_queried_column
    last_queried_column = -1;
}
/* }}} */


/* {{{ MySQL_ResultSet::isScrollable() -I- */
bool
MySQL_ResultSet::isScrollable() const
{
//	CPP_ENTER("MySQL_ResultSet::isScrollable");
//	CPP_INFO_FMT("this=%p", this);
    return (resultset_type != sql::ResultSet::TYPE_FORWARD_ONLY);
}
/* }}} */


/* {{{ MySQL_ResultSet::checkValid() -I- */
void
MySQL_ResultSet::checkValid() const
{
    CPP_ENTER("MySQL_ResultSet::checkValid");
    CPP_INFO_FMT("this=%p", this);
    if (isClosed()) {
        throw sql::InvalidInstanceException("ResultSet has been closed");
    }
}
/* }}} */


/* {{{ MySQL_ResultSet::clearWarnings() -U- */
void
MySQL_ResultSet::clearWarnings()
{
    CPP_ENTER("MySQL_ResultSet::clearWarnings");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::clearWarnings()");
}
/* }}} */


/* {{{ MySQL_ResultSet::close() -I- */
void
MySQL_ResultSet::close()
{
    CPP_ENTER("MySQL_ResultSet::close");
    checkValid();
    result.reset();
//	result->dispose();
}
/* }}} */


/* {{{ MySQL_ResultSet::findColumn() -I- */
uint32_t
MySQL_ResultSet::findColumn(const sql::SQLString& columnLabel) const
{
    CPP_ENTER("MySQL_ResultSet::findColumn");
    checkValid();
#if A0
    std::cout << "Elements=" << field_name_to_index_map.size() << "\n";
    FieldNameIndexMap::const_iterator tmp_iter = field_name_to_index_map.begin();
    FieldNameIndexMap::const_iterator tmp_iter_end = field_name_to_index_map.end();
    for (;tmp_iter != tmp_iter_end; tmp_iter++) {
        std::cout << "[[" << tmp_iter->first << "]] second=[[" << tmp_iter->second << "]]\n";
    }
    sql::SQLString tmp(upstring.get());
    std::cout << "[" << tmp << "]\n";
#endif

    FieldNameIndexMap::const_iterator iter = field_name_to_index_map.find(columnLabel);
    if (iter == field_name_to_index_map.end()) {
        return 0;
    }
    /* findColumn returns 1-based indexes */
    return iter->second + 1;
}
/* }}} */


/* {{{ MySQL_ResultSet::first() -I- */
bool
MySQL_ResultSet::first()
{
    CPP_ENTER("MySQL_ResultSet::first");
    checkValid();
    checkScrollable();
    if (num_rows) {
        row_position = 1;
        seek();
    }
    return num_rows != 0;
}
/* }}} */


/* {{{ MySQL_ResultSet::getBlob() -I- */
std::istream *
MySQL_ResultSet::getBlob(const uint32_t columnIndex) const
{
    CPP_ENTER("MySQL_ResultSet::getBlob(int)");
    /* isBeforeFirst checks for validity */
    if (isBeforeFirstOrAfterLast()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getBoolean: can't fetch because not on result set");
    }
    return new std::istringstream(getString(columnIndex));
}
/* }}} */


/* {{{ MySQL_ResultSet::getBlob() -I- */
std::istream *
MySQL_ResultSet::getBlob(const sql::SQLString& columnLabel) const
{
    CPP_ENTER("MySQL_ResultSet::getBlob(string)");
    /* isBeforeFirst checks for validity */
    if (isBeforeFirstOrAfterLast()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getBoolean: can't fetch because not on result set");
    }
    return new std::istringstream(getString(columnLabel));
}
/* }}} */


/* {{{ MySQL_ResultSet::getBoolean() -I- */
bool
MySQL_ResultSet::getBoolean(const uint32_t columnIndex) const
{
    CPP_ENTER("MySQL_ResultSet::getBoolean(int)");
    /* isBeforeFirst checks for validity */
    if (isBeforeFirstOrAfterLast()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getBoolean: can't fetch because not on result set");
    }
    return getInt(columnIndex)? true:false;
}
/* }}} */


/* {{{ MySQL_ResultSet::getBoolean() -I- */
bool
MySQL_ResultSet::getBoolean(const sql::SQLString& columnLabel) const
{
    CPP_ENTER("MySQL_ResultSet::getBoolean(string)");
    /* isBeforeFirst checks for validity */
    if (isBeforeFirstOrAfterLast()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getBoolean: can't fetch because not on result set");
    }
    return getInt(columnLabel)? true:false;
}
/* }}} */


/* {{{ MySQL_ResultSet::getConcurrency() -U- */
int
MySQL_ResultSet::getConcurrency()
{
    CPP_ENTER("MySQL_ResultSet::getConcurrency");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::getConcurrency()");
    return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_ResultSet::getCursorName() -U- */
SQLString
MySQL_ResultSet::getCursorName()
{
    CPP_ENTER("MySQL_ResultSet::getCursorName");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::getCursorName()");
    return ""; // fool compilers
}
/* }}} */


/* {{{ MySQL_ResultSet::getDouble() -I- */
long double
MySQL_ResultSet::getDouble(const uint32_t columnIndex) const
{
    CPP_ENTER("MySQL_ResultSet::getDouble(int)");

    /* isBeforeFirst checks for validity */
    if (isBeforeFirstOrAfterLast()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getDouble: can't fetch because not on result set");
    }

    if (columnIndex == 0 || columnIndex > num_fields) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getDouble: invalid value of 'columnIndex'");
    }

    last_queried_column = columnIndex;

    if (row[columnIndex - 1] == NULL) {
        was_null = true;
        return 0.0;
    }
    was_null = false;
    if (getFieldMeta(columnIndex)->type == MYSQL_TYPE_BIT) {
        return static_cast<long double>(getInt64(columnIndex));
    }
    return sql::mysql::util::strtonum(row[columnIndex - 1]);
}
/* }}} */


/* {{{ MySQL_ResultSet::getDouble() -I- */
long double
MySQL_ResultSet::getDouble(const sql::SQLString& columnLabel) const
{
    CPP_ENTER("MySQL_ResultSet::getDouble(string)");
    return getDouble(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ResultSet::getFetchDirection() -U- */
int
MySQL_ResultSet::getFetchDirection()
{
    CPP_ENTER("MySQL_ResultSet::getFetchDirection");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::getFetchDirection()");
    return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_ResultSet::getFetchSize() -U- */
size_t
MySQL_ResultSet::getFetchSize()
{
    CPP_ENTER("MySQL_ResultSet::getFetchSize");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::getFetchSize()");
    return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_ResultSet::getFieldMeta() -U- */
MYSQL_FIELD * MySQL_ResultSet::getFieldMeta(unsigned int columnIndex) const
{
    return result->fetch_field_direct(columnIndex - 1);
}
/* }}} */


/* {{{ MySQL_ResultSet::getHoldability() -U- */
int
MySQL_ResultSet::getHoldability()
{
    CPP_ENTER("MySQL_ResultSet::getHoldability");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::getHoldability()");
    return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_ResultSet::getInt() -I- */
int32_t
MySQL_ResultSet::getInt(const uint32_t columnIndex) const
{
    CPP_ENTER("MySQL_ResultSet::getInt(int)");
    /* isBeforeFirst checks for validity */
    if (isBeforeFirstOrAfterLast()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getInt: can't fetch because not on result set");
    }
    if (columnIndex == 0 || columnIndex > num_fields) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getInt: invalid value of 'columnIndex'");
    }
    CPP_INFO_FMT("%ssigned", (getFieldMeta(columnIndex)->flags & UNSIGNED_FLAG)? "un":"");
    if (getFieldMeta(columnIndex)->flags & UNSIGNED_FLAG) {
        return static_cast<uint32_t>(getInt64(columnIndex));
    }
    return static_cast<int32_t>(getInt64(columnIndex));
}
/* }}} */


/* {{{ MySQL_ResultSet::getInt() -I- */
int32_t
MySQL_ResultSet::getInt(const sql::SQLString& columnLabel) const
{
    CPP_ENTER("MySQL_ResultSet::getInt(string)");
    return getInt(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ResultSet::getUInt() -I- */
uint32_t
MySQL_ResultSet::getUInt(const uint32_t columnIndex) const
{
    CPP_ENTER("MySQL_ResultSet::getUInt(int)");
    /* isBeforeFirst checks for validity */
    if (isBeforeFirstOrAfterLast()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getUInt: can't fetch because not on result set");
    }
    if (columnIndex == 0 || columnIndex > num_fields) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getUInt: invalid value of 'columnIndex'");
    }
    CPP_INFO_FMT("%ssigned", (getFieldMeta(columnIndex)->flags & UNSIGNED_FLAG)? "un":"");
    return static_cast<uint32_t>(getUInt64(columnIndex));// & 0xffffffff;
}
/* }}} */


/* {{{ MySQL_ResultSet::getUInt() -I- */
uint32_t
MySQL_ResultSet::getUInt(const sql::SQLString& columnLabel) const
{
    CPP_ENTER("MySQL_ResultSet::getUInt(string)");
    return getUInt(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ResultSet::getInt64() -I- */
int64_t
MySQL_ResultSet::getInt64(const uint32_t columnIndex) const
{
    CPP_ENTER("MySQL_ResultSet::getInt64(int)");

    /* isBeforeFirst checks for validity */
    if (isBeforeFirstOrAfterLast()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getInt64: can't fetch because not on result set");
    }

    if (columnIndex == 0 || columnIndex > num_fields) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getInt64: invalid value of 'columnIndex'");
    }

    last_queried_column = columnIndex;

    if (row[columnIndex - 1] == NULL) {
        was_null = true;
        return 0;
    }


    CPP_INFO_FMT("%ssigned", (getFieldMeta(columnIndex)->flags & UNSIGNED_FLAG)? "un":"");
    was_null = false;
    if (getFieldMeta(columnIndex)->type == MYSQL_TYPE_BIT &&
                getFieldMeta(columnIndex)->flags != (BINARY_FLAG|UNSIGNED_FLAG)) {
        uint64_t uval = 0;
        std::div_t length= std::div(getFieldMeta(columnIndex)->length, 8);
        if (length.rem) {
            ++length.quot;
        }

        switch (length.quot) {
            case 8:uval = (uint64_t) bit_uint8korr(row[columnIndex - 1]);break;
            case 7:uval = (uint64_t) bit_uint7korr(row[columnIndex - 1]);break;
            case 6:uval = (uint64_t) bit_uint6korr(row[columnIndex - 1]);break;
            case 5:uval = (uint64_t) bit_uint5korr(row[columnIndex - 1]);break;
            case 4:uval = (uint64_t) bit_uint4korr(row[columnIndex - 1]);break;
            case 3:uval = (uint64_t) bit_uint3korr(row[columnIndex - 1]);break;
            case 2:uval = (uint64_t) bit_uint2korr(row[columnIndex - 1]);break;
            case 1:uval = (uint64_t) bit_uint1korr(row[columnIndex - 1]);break;
        }
        return uval;
    }

    if (getFieldMeta(columnIndex)->flags & UNSIGNED_FLAG) {
        return strtoull(row[columnIndex - 1], NULL, 10);
    }
    return strtoll(row[columnIndex - 1], NULL, 10);
}
/* }}} */


/* {{{ MySQL_ResultSet::getInt64() -I- */
int64_t
MySQL_ResultSet::getInt64(const sql::SQLString& columnLabel) const
{
    CPP_ENTER("MySQL_ResultSet::getInt64(string)");
    return getInt64(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ResultSet::getUInt64() -I- */
uint64_t
MySQL_ResultSet::getUInt64(const uint32_t columnIndex) const
{
    CPP_ENTER("MySQL_ResultSet::getUInt64(int)");

    /* isBeforeFirst checks for validity */
    if (isBeforeFirstOrAfterLast()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getUInt64: can't fetch because not on result set");
    }

    if (columnIndex == 0 || columnIndex > num_fields) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getUInt64: invalid value of 'columnIndex'");
    }

    last_queried_column = columnIndex;

    if (row[columnIndex - 1] == NULL) {
        was_null = true;
        return 0;
    }


    CPP_INFO_FMT("%ssigned", (getFieldMeta(columnIndex)->flags & UNSIGNED_FLAG)? "un":"");
    was_null = false;
    if (getFieldMeta(columnIndex)->type == MYSQL_TYPE_BIT &&
                getFieldMeta(columnIndex)->flags != (BINARY_FLAG|UNSIGNED_FLAG)) {
        uint64_t uval = 0;
        std::div_t length= std::div(getFieldMeta(columnIndex)->length, 8);
        if (length.rem) {
            ++length.quot;
        }

        switch (length.quot) {
            case 8:uval = (uint64_t) bit_uint8korr(row[columnIndex - 1]);break;
            case 7:uval = (uint64_t) bit_uint7korr(row[columnIndex - 1]);break;
            case 6:uval = (uint64_t) bit_uint6korr(row[columnIndex - 1]);break;
            case 5:uval = (uint64_t) bit_uint5korr(row[columnIndex - 1]);break;
            case 4:uval = (uint64_t) bit_uint4korr(row[columnIndex - 1]);break;
            case 3:uval = (uint64_t) bit_uint3korr(row[columnIndex - 1]);break;
            case 2:uval = (uint64_t) bit_uint2korr(row[columnIndex - 1]);break;
            case 1:uval = (uint64_t) bit_uint1korr(row[columnIndex - 1]);break;
        }
        return uval;
    }

    if (getFieldMeta(columnIndex)->flags & UNSIGNED_FLAG) {
        return strtoull(row[columnIndex - 1], NULL, 10);
    }
    return strtoll(row[columnIndex - 1], NULL, 10);
}
/* }}} */


/* {{{ MySQL_ResultSet::getUInt64() -I- */
uint64_t
MySQL_ResultSet::getUInt64(const sql::SQLString& columnLabel) const
{
    CPP_ENTER("MySQL_ResultSet::getUInt64(string)");
    return getUInt64(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ResultSet::getMetaData() -I- */
sql::ResultSetMetaData *
MySQL_ResultSet::getMetaData() const
{
    CPP_ENTER("MySQL_ResultSet::getMetaData");
    checkValid();
    return rs_meta.get();
}
/* }}} */


/* {{{ MySQL_ResultSet::getRow() -I- */
size_t
MySQL_ResultSet::getRow() const
{
    CPP_ENTER("MySQL_ResultSet::getRow");
    checkValid();
    /* row_position is 0 based */
    return static_cast<size_t>(row_position);
}
/* }}} */


/* {{{ MySQL_ResultSet::getRowId() -U- */
sql::RowID *
MySQL_ResultSet::getRowId(uint32_t)
{
    CPP_ENTER("MySQL_ResultSet::getRowId");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::getRowId(uint32_t columnIndex)");
    return NULL; // fool compilers
}
/* }}} */


/* {{{ MySQL_ResultSet::getRowId() -U- */
sql::RowID *
MySQL_ResultSet::getRowId(const sql::SQLString &)
{
    CPP_ENTER("MySQL_ResultSet::getRowId");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::getRowId(const sql::SQLString & columnLabel)");
    return NULL; // fool compilers
}
/* }}} */


/* {{{ MySQL_ResultSet::getStatement() -I- */
const sql::Statement *
MySQL_ResultSet::getStatement() const
{
    CPP_ENTER("MySQL_ResultSet::getStatement");
    return parent;
}
/* }}} */


/* {{{ MySQL_ResultSet::getString() -I- */
SQLString
MySQL_ResultSet::getString(const uint32_t columnIndex) const
{
    CPP_ENTER("MySQL_ResultSet::getString(int)");
    CPP_INFO_FMT("this=%p column=%u", this, columnIndex);

    /* isBeforeFirst checks for validity */
    if (isBeforeFirstOrAfterLast()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getString: can't fetch because not on result set");
    }

    if (columnIndex == 0 || columnIndex > num_fields) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getString: invalid value of 'columnIndex'");
    }

    last_queried_column = columnIndex;

    if (row == NULL || row[columnIndex - 1] == NULL) {
        was_null = true;
        return "";
    }

    if (getFieldMeta(columnIndex)->type == MYSQL_TYPE_BIT) {
        char buf[30];
        snprintf(buf, sizeof(buf) - 1, "%llu", (unsigned long long) getUInt64(columnIndex));
        return sql::SQLString(buf);
    }

    size_t len = result->fetch_lengths()[columnIndex - 1];
    CPP_INFO_FMT("value=%*s",  len> 50? 50:len, row[columnIndex - 1]);
    was_null = false;
    return sql::SQLString(row[columnIndex - 1], len);
}
/* }}} */


/* {{{ MySQL_ResultSet::getString() -I- */
SQLString
MySQL_ResultSet::getString(const sql::SQLString& columnLabel) const
{
    CPP_ENTER("MySQL_ResultSet::getString(string)");
    return getString(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ResultSet::getType() -I- */
sql::ResultSet::enum_type
MySQL_ResultSet::getType() const
{
    CPP_ENTER("MySQL_ResultSet::getType");
    checkValid();
    return resultset_type;
}
/* }}} */


/* {{{ MySQL_ResultSet::getWarnings() -U- */
void
MySQL_ResultSet::getWarnings()
{
    CPP_ENTER("MySQL_ResultSet::getWarnings");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::getWarnings()");
}
/* }}} */


/* {{{ MySQL_ResultSet::insertRow() -U- */
void
MySQL_ResultSet::insertRow()
{
    CPP_ENTER("MySQL_ResultSet::insertRow");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::insertRow()");
}
/* }}} */


/* {{{ MySQL_ResultSet::isAfterLast() -I- */
bool
MySQL_ResultSet::isAfterLast() const
{
    CPP_ENTER("MySQL_ResultSet::isAfterLast");
    checkValid();
    checkScrollable();
    return (row_position == num_rows + 1);
}
/* }}} */


/* {{{ MySQL_ResultSet::isBeforeFirst() -I- */
bool
MySQL_ResultSet::isBeforeFirst() const
{
    CPP_ENTER("MySQL_ResultSet::isBeforeFirst");
    checkValid();
    return (row_position == 0);
}
/* }}} */


/* {{{ MySQL_ResultSet::isClosed() -I- */
bool
MySQL_ResultSet::isClosed() const
{
    CPP_ENTER("MySQL_ResultSet::isClosed");
    return !result;
//	return !result->isValid();
}
/* }}} */


/* {{{ MySQL_ResultSet::isFirst() -I- */
bool
MySQL_ResultSet::isFirst() const
{
    CPP_ENTER("MySQL_ResultSet::isFirst");
    checkValid();
    return (row_position == 1);
}
/* }}} */


/* {{{ MySQL_ResultSet::isLast() -I- */
bool
MySQL_ResultSet::isLast() const
{
    CPP_ENTER("MySQL_ResultSet::isLast");
    checkValid();
    checkScrollable();
    return (row_position == num_rows);
}
/* }}} */


/* {{{ MySQL_ResultSet::isNull() -I- */
bool
MySQL_ResultSet::isNull(const uint32_t columnIndex) const
{
    CPP_ENTER("MySQL_ResultSet::isNull(int)");
    checkValid();

    if (columnIndex == 0 || columnIndex > num_fields) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::isNull: invalid value of 'columnIndex'");
    }
    /* isBeforeFirst checks for validity */
    if (isBeforeFirstOrAfterLast()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::getDouble: can't fetch because not on result set");
    }
    return (row[columnIndex - 1] == NULL);
}
/* }}} */


/* {{{ MySQL_ResultSet::isNull() -I- */
bool
MySQL_ResultSet::isNull(const sql::SQLString& columnLabel) const
{
    CPP_ENTER("MySQL_ResultSet::isNull(string)");
    int32_t col_idx = findColumn(columnLabel);
    if (col_idx == 0) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::isNull: invalid value of 'columnLabel'");
    }
    return isNull(col_idx);
}
/* }}} */


/* {{{ MySQL_ResultSet::last() -I- */
bool
MySQL_ResultSet::last()
{
    CPP_ENTER("MySQL_ResultSet::last");
    checkValid();
    checkScrollable();
    if (num_rows) {
        row_position = num_rows;
        seek();
    }
    return num_rows != 0;
}
/* }}} */


/* {{{ MySQL_ResultSet::moveToCurrentRow() -U- */
void
MySQL_ResultSet::moveToCurrentRow()
{
    CPP_ENTER("MySQL_ResultSet::moveToCurrentRow");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::moveToCurrentRow()");
    checkScrollable();
}
/* }}} */


/* {{{ MySQL_ResultSet::moveToInsertRow() -U- */
void
MySQL_ResultSet::moveToInsertRow()
{
    CPP_ENTER("MySQL_ResultSet::moveToInsertRow");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::moveToInsertRow()");
    checkScrollable();
}
/* }}} */


/* {{{ MySQL_ResultSet::next() -I- */
bool
MySQL_ResultSet::next()
{
    CPP_ENTER("MySQL_ResultSet::next");
    checkValid();
    bool ret = false;
    if (isScrollable()) {
        if (isLast()) {
            afterLast();
        } else if (row_position < num_rows + 1) {
            row = result->fetch_row();


            if (row == NULL)
            {
              std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
              if (!proxy_p) {
                  throw sql::InvalidInstanceException("Connection has been closed");
              }
              if (proxy_p->errNo() == 2013 || proxy_p->errNo() == 2000)
              {
                CPP_ERR_FMT("Error fetching next row %d:(%s) %s",
                            proxy_p->errNo(), proxy_p->sqlstate().c_str(),
                            proxy_p->error().c_str());
                sql::SQLException e(proxy_p->error(), proxy_p->sqlstate(), proxy_p->errNo());
                throw e;
              }
            }
            ++row_position;
            ret = (row != NULL);
        }
    } else {
        // reset last_queried_column
        last_queried_column = -1;
        row = result->fetch_row();
        if (row == NULL)
        {
          std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
          if (!proxy_p) {
              throw sql::InvalidInstanceException("Connection has been closed");
          }
          if (proxy_p->errNo() == 2013 || proxy_p->errNo() == 2000)
          {
            CPP_ERR_FMT("Error fetching next row %d:(%s) %s",
                        proxy_p->errNo(), proxy_p->sqlstate().c_str(),
                        proxy_p->error().c_str());
            sql::SQLException e(proxy_p->error(), proxy_p->sqlstate(), proxy_p->errNo());
            throw e;
          }
        }
        if ((ret = (row != NULL))) {
            ++row_position;
        } else {
            row_position = 0;
        }
    }
    CPP_INFO_FMT("new_position=%llu num_rows=%llu", row_position, num_rows);
    return ret;
}
/* }}} */


/* {{{ MySQL_ResultSet::previous() -I- */
bool
MySQL_ResultSet::previous()
{
    CPP_ENTER("MySQL_ResultSet::previous");

    checkScrollable();
    /* isBeforeFirst checks for validity */
    if (isBeforeFirst()) {
        return false;
    } else if (isFirst()) {
        beforeFirst();
        return false;
    } else if (row_position > 1) {
        --row_position;
        seek();
        return true;
    }
    throw sql::SQLException("Impossible");
}
/* }}} */


/* {{{ MySQL_ResultSet::refreshRow() -U- */
void
MySQL_ResultSet::refreshRow()
{
    CPP_ENTER("MySQL_ResultSet::refreshRow");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::refreshRow()");
}
/* }}} */


/* {{{ MySQL_ResultSet::relative() -I- */
bool
MySQL_ResultSet::relative(const int rows)
{
    CPP_ENTER("MySQL_ResultSet::relative");
    checkValid();
    checkScrollable();
    if (rows != 0) {
        if ((row_position + rows) > num_rows || (row_position + rows) < 1) {
            row_position = rows > 0? num_rows + 1 : 0; /* after last or before first */
        } else {
            row_position += rows;
            seek();
        }
    }

    return (row_position > 0 && row_position <= num_rows);
}
/* }}} */


/* {{{ MySQL_ResultSet::rowDeleted() -U- */
bool
MySQL_ResultSet::rowDeleted()
{
    CPP_ENTER("MySQL_ResultSet::rowDeleted");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::rowDeleted()");
    return false; // fool compilers
}
/* }}} */


/* {{{ MySQL_ResultSet::rowInserted() -U- */
bool
MySQL_ResultSet::rowInserted()
{
    CPP_ENTER("MySQL_ResultSet::rowInserted");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::rowInserted()");
    return false; // fool compilers
}
/* }}} */


/* {{{ MySQL_ResultSet::rowUpdated() -U- */
bool
MySQL_ResultSet::rowUpdated()
{
    CPP_ENTER("MySQL_ResultSet::rowUpdated");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::rowUpdated()");
    return false; // fool compilers
}
/* }}} */


/* {{{ MySQL_ResultSet::rowsCount() -I- */
size_t
MySQL_ResultSet::rowsCount() const
{
    CPP_ENTER("MySQL_ResultSet::rowsCount");
    checkValid();
    checkScrollable();
    return static_cast<size_t>(result->num_rows());
}
/* }}} */


/* {{{ MySQL_ResultSet::setFetchSize() -U- */
void
MySQL_ResultSet::setFetchSize(size_t /* rows */)
{
    CPP_ENTER("MySQL_ResultSet::setFetchSize");
    checkValid();
    throw sql::MethodNotImplementedException("MySQL_ResultSet::setFetchSize()");
}
/* }}} */


/* {{{ MySQL_ResultSet::wasNull() -I- */
bool
MySQL_ResultSet::wasNull() const
{
    CPP_ENTER("MySQL_ResultSet::wasNull");
    checkValid();
    /* isBeforeFirst checks for validity */
    if (isBeforeFirstOrAfterLast()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::wasNull: can't fetch because not on result set");
    }
    if (last_queried_column == std::numeric_limits<uint32_t>::max()) {
        throw sql::InvalidArgumentException("MySQL_ResultSet::wasNull: should be called only after one of the getter methods");
    }
    return was_null;
}
/* }}} */


/* {{{ MySQL_ResultSet::isBeforeFirstOrAfterLast() -I- */
bool
MySQL_ResultSet::isBeforeFirstOrAfterLast() const
{
    CPP_ENTER("MySQL_ResultSet::isBeforeFirstOrAfterLast");
    checkValid();
    return (row_position == 0) || (isScrollable() && (row_position == num_rows + 1));
}
/* }}} */


/* {{{ MySQL_ResultSet::seek() -I- */
void
MySQL_ResultSet::seek()
{
    CPP_ENTER("MySQL_ResultSet::seek");
    checkScrollable();
    result->data_seek(row_position - 1);
    row = result->fetch_row();
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
