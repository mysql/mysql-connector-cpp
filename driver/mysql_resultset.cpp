/* Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.

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

#include "mysql_resultset.h"
#include "mysql_resultset_metadata.h"
#include "mysql_statement.h"
#include "mysql_res_wrapper.h"

#ifndef _WIN32
#include <string.h>
#endif

#ifndef _WIN32
#include <stdlib.h>
#else
#define atoll(x) _atoi64((x))
#endif	//	_WIN32

#include "mysql_debug.h"
#include "mysql_util.h"

namespace sql
{
namespace mysql
{

extern char * cppmysql_utf8_strup(const char *src, size_t srclen);


/* {{{ MySQL_ResultSet::MySQL_ResultSet() -I- */
MySQL_ResultSet::MySQL_ResultSet(MYSQL_RES_Wrapper * res, MySQL_Statement * par, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l)
	: row(NULL), result(res), row_position(0), was_null(false), parent(par), logger(l? l->getReference():NULL)
{
	CPP_ENTER("MySQL_ResultSet::MySQL_ResultSet");
	num_rows = mysql_num_rows(result->get());

	num_fields = mysql_num_fields(result->get());
	for (unsigned int i = 0; i < num_fields; i++) {
		char *tmp = cppmysql_utf8_strup(mysql_fetch_field_direct(result->get(), i)->name, 0);
		field_name_to_index_map[std::string(tmp)] = i;
		free(tmp);
	}
	metadata = new MySQL_ResultSetMetaData(result->getReference(), logger);
}
/* }}} */


/* {{{ MySQL_ResultSet::~MySQL_ResultSet() -I- */
MySQL_ResultSet::~MySQL_ResultSet()
{
	/* Don't remove the block or we can get into problems with logger */
	{
		CPP_ENTER("MySQL_ResultSet::~MySQL_ResultSet");
		CPP_INFO_FMT("this=%p", this);
		delete metadata;
		if (!isClosed()) {
			result->dispose();
		}
		result->deleteReference();
	}
	logger->freeReference();
}
/* }}} */


/* {{{ MySQL_ResultSet::absolute() -I- */
bool
MySQL_ResultSet::absolute(int new_pos)
{
	CPP_ENTER("MySQL_ResultSet::absolute");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	if (new_pos > 0) {
		if (new_pos > (int) num_rows) {
			row_position = num_rows + 1; /* after last row */
		} else {
			row_position = (my_ulonglong) new_pos; /* the cast is inspected and is valid */
			seek();
			return true;
		}
	} else if (new_pos < 0) {
		if ((-new_pos) > (int) num_rows) {
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
		mysql_data_seek(result->get(), 0);
	}
	return (row_position > 0 && row_position < (num_rows + 1));
}
/* }}} */


/* {{{ MySQL_ResultSet::afterLast() -I- */
void
MySQL_ResultSet::afterLast()
{
	CPP_ENTER("MySQL_ResultSet::afterLast");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	row_position = num_rows + 1;
}
/* }}} */


/* {{{ MySQL_ResultSet::beforeFirst() -I- */
void
MySQL_ResultSet::beforeFirst()
{
	CPP_ENTER("MySQL_ResultSet::beforeFirst");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	mysql_data_seek(result->get(), 0);
	row_position = 0;
}
/* }}} */


/* {{{ MySQL_ResultSet::cancelRowUpdates() -U- */
void
MySQL_ResultSet::cancelRowUpdates()
{
	CPP_ENTER("MySQL_ResultSet::cancelRowUpdates");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::cancelRowUpdates()");
}
/* }}} */


/* {{{ MySQL_ResultSet::checkValid() -I- */
void
MySQL_ResultSet::checkValid() const
{
	CPP_ENTER("MySQL_ResultSet::checkValid");
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
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::clearWarnings()");
}
/* }}} */


/* {{{ MySQL_ResultSet::close() -I- */
void
MySQL_ResultSet::close()
{
	CPP_ENTER("MySQL_ResultSet::close");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	result->dispose();
}
/* }}} */


/* {{{ MySQL_ResultSet::findColumn() -I- */
unsigned int
MySQL_ResultSet::findColumn(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ResultSet::findColumn");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	char *tmp = cppmysql_utf8_strup(columnLabel.c_str(), 0);
	FieldNameIndexMap::const_iterator iter = field_name_to_index_map.find(tmp);
	free(tmp);

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
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	if (num_rows) {
		row_position = 1;
		seek();
	}
	return num_rows != 0;
}
/* }}} */


/* {{{ MySQL_ResultSet::getBoolean() -I- */
bool
MySQL_ResultSet::getBoolean(unsigned int columnIndex) const
{
	CPP_ENTER("MySQL_ResultSet::getBoolean(int)");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	if (isBeforeFirst() || isAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_ResultSet::getBoolean: can't fetch because not on result set");
	}
	return getInt(columnIndex)? true:false;
}
/* }}} */


/* {{{ MySQL_ResultSet::getBoolean() -I- */
bool
MySQL_ResultSet::getBoolean(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ResultSet::getBoolean(string)");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	if (isBeforeFirst() || isAfterLast()) {
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
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::getConcurrency()");
}
/* }}} */


/* {{{ MySQL_ResultSet::getCursorName() -U- */
std::string
MySQL_ResultSet::getCursorName()
{
	CPP_ENTER("MySQL_ResultSet::getCursorName");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::getCursorName()");
}
/* }}} */


/* {{{ MySQL_ResultSet::getDouble() -I- */
double
MySQL_ResultSet::getDouble(unsigned int columnIndex) const
{
	CPP_ENTER("MySQL_ResultSet::getDouble(int)");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	/* internally zero based */
	columnIndex--;
	if (columnIndex >= num_fields) {
		throw sql::InvalidArgumentException("MySQL_ResultSet::getDouble: invalid value of 'columnIndex'");
	}
	if (isBeforeFirst() || isAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_ResultSet::getDouble: can't fetch because not on result set");
	}

	if (row[columnIndex] == NULL) {
		was_null = true;
		return 0.0;
	}
	was_null = false;
	return atof(row[columnIndex]);
}
/* }}} */


/* {{{ MySQL_ResultSet::getDouble() -I- */
double
MySQL_ResultSet::getDouble(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ResultSet::getDouble(string)");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	return getDouble(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ResultSet::getFetchDirection() -U- */
int
MySQL_ResultSet::getFetchDirection()
{
	CPP_ENTER("MySQL_ResultSet::getFetchDirection");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::getFetchDirection()");
}
/* }}} */


/* {{{ MySQL_ResultSet::getFetchSize() -U- */
int
MySQL_ResultSet::getFetchSize()
{
	CPP_ENTER("MySQL_ResultSet::getFetchSize");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::getFetchSize()");
}
/* }}} */


/* {{{ MySQL_ResultSet::getHoldability() -U- */
int
MySQL_ResultSet::getHoldability()
{
	CPP_ENTER("MySQL_ResultSet::getHoldability");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::getHoldability()");
}
/* }}} */


/* {{{ MySQL_ResultSet::getInt() -I- */
int
MySQL_ResultSet::getInt(unsigned int columnIndex) const
{
	CPP_ENTER("MySQL_ResultSet::getInt(int)");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	/* internally zero based */
	columnIndex--;
	if (columnIndex >= num_fields) {
		throw sql::InvalidArgumentException("MySQL_ResultSet::getInt: invalid value of 'columnIndex'");
	}
	if (isBeforeFirst() || isAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_ResultSet::getInt: can't fetch because not on result set");
	}

	if (row[columnIndex] == NULL) {
		was_null = true;
		return 0;
	}
	was_null = false;
	return atoi(row[columnIndex]);
}
/* }}} */


/* {{{ MySQL_ResultSet::getInt() -I- */
int
MySQL_ResultSet::getInt(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ResultSet::getInt(string)");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	return getInt(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ResultSet::getLong() -I- */
long long
MySQL_ResultSet::getLong(unsigned int columnIndex) const
{
	CPP_ENTER("MySQL_ResultSet::getLong(int)");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	/* internally zero based */
	columnIndex--;
	if (columnIndex >= num_fields) {
		throw sql::InvalidArgumentException("MySQL_ResultSet::getLong: invalid value of 'columnIndex'");
	}
	if (isBeforeFirst() || isAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_ResultSet::getInt: can't fetch because not on result set");
	}

	if (row[columnIndex] == NULL) {
		was_null = true;
		return 0;
	}
	was_null = false;
	return atoll(row[columnIndex]);
}
/* }}} */


/* {{{ MySQL_ResultSet::getLong() -I- */
long long
MySQL_ResultSet::getLong(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ResultSet::getLong(string)");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	return getLong(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ResultSet::getMetaData() -I- */
sql::ResultSetMetaData *
MySQL_ResultSet::getMetaData() const
{
	CPP_ENTER("MySQL_ResultSet::getMetaData");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	return metadata;
}
/* }}} */


/* {{{ MySQL_ResultSet::getRow() -I- */
size_t
MySQL_ResultSet::getRow() const
{
	CPP_ENTER("MySQL_ResultSet::getRow");
	/* row_position is 0 based */
	return static_cast<size_t>(row_position);
}
/* }}} */


/* {{{ MySQL_ResultSet::getRowId() -U- */
sql::RowID *
MySQL_ResultSet::getRowId(unsigned int)
{
	CPP_ENTER("MySQL_ResultSet::getRowId");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::getRowId(unsigned int columnIndex)");
}
/* }}} */


/* {{{ MySQL_ResultSet::getRowId() -U- */
sql::RowID *
MySQL_ResultSet::getRowId(const std::string &)
{
	CPP_ENTER("MySQL_ResultSet::getRowId");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::getRowId(const std::string & columnLabel)");
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
std::string
MySQL_ResultSet::getString(unsigned int columnIndex) const
{
	CPP_ENTER("MySQL_ResultSet::getString(int)");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	/* internally zero based */
	columnIndex--;
	if (columnIndex >= num_fields) {
		throw sql::InvalidArgumentException("MySQL_ResultSet::getString: invalid value of 'columnIndex'");
	}
	if (isBeforeFirst() || isAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_ResultSet::getString: can't fetch because not on result set");
	}

	if(row[columnIndex] == NULL) {
		was_null= true;
		return "";
	}
	was_null= false;
	CPP_INFO_FMT("value=%*s", strlen(row[columnIndex]), row[columnIndex]);
	return row[columnIndex];
}
/* }}} */


/* {{{ MySQL_ResultSet::getString() -I- */
std::string
MySQL_ResultSet::getString(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ResultSet::getString(string)");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	return getString(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ResultSet::getWarnings() -U- */
void
MySQL_ResultSet::getWarnings()
{
	CPP_ENTER("MySQL_ResultSet::getWarnings");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::getWarnings()");
}
/* }}} */


/* {{{ MySQL_ResultSet::insertRow() -U- */
void
MySQL_ResultSet::insertRow()
{
	CPP_ENTER("MySQL_ResultSet::insertRow");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::insertRow()");
}
/* }}} */


/* {{{ MySQL_ResultSet::isAfterLast() -I- */
bool
MySQL_ResultSet::isAfterLast() const
{
	CPP_ENTER("MySQL_ResultSet::isAfterLast");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	return (row_position == num_rows + 1);
}
/* }}} */


/* {{{ MySQL_ResultSet::isBeforeFirst() -I- */
bool
MySQL_ResultSet::isBeforeFirst() const
{
	CPP_ENTER("MySQL_ResultSet::isBeforeFirst");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	return (row_position == 0);
}
/* }}} */


/* {{{ MySQL_ResultSet::isClosed() -I- */
bool
MySQL_ResultSet::isClosed() const
{
	CPP_ENTER("MySQL_ResultSet::isClosed");
	return !result->isValid();
}
/* }}} */


/* {{{ MySQL_ResultSet::isFirst() -I- */
bool
MySQL_ResultSet::isFirst() const
{
	CPP_ENTER("MySQL_ResultSet::isFirst");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	return (row_position == 1);
}
/* }}} */


/* {{{ MySQL_ResultSet::isLast() -I- */
bool
MySQL_ResultSet::isLast() const
{
	CPP_ENTER("MySQL_ResultSet::isLast");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	return (row_position == num_rows);
}
/* }}} */


/* {{{ MySQL_ResultSet::isNull() -I- */
bool
MySQL_ResultSet::isNull(unsigned int columnIndex) const
{
	CPP_ENTER("MySQL_ResultSet::isNull(int)");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	/* internally zero based */
	columnIndex--;
	if(columnIndex >= num_fields) {
		throw sql::InvalidArgumentException("MySQL_ResultSet::isNull: invalid value of 'columnIndex'");
	}
	return (row[columnIndex] == NULL);
}
/* }}} */


/* {{{ MySQL_ResultSet::isNull() -I- */
bool
MySQL_ResultSet::isNull(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ResultSet::isNull(string)");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	int col_idx = findColumn(columnLabel);
	if (col_idx == -1) {
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
	CPP_INFO_FMT("this=%p", this);
	checkValid();
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
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::moveToCurrentRow()");
}
/* }}} */


/* {{{ MySQL_ResultSet::moveToInsertRow() -U- */
void
MySQL_ResultSet::moveToInsertRow()
{
	CPP_ENTER("MySQL_ResultSet::moveToInsertRow");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::moveToInsertRow()");
}
/* }}} */


/* {{{ MySQL_ResultSet::next() -I- */
bool
MySQL_ResultSet::next()
{
	CPP_ENTER("MySQL_ResultSet::next");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	bool ret = false;
	if (row_position == num_rows) {
		row_position++;
	} else if (row_position < num_rows + 1) {
		row = mysql_fetch_row(result->get());
		row_position++;
		ret = (row != 0);
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
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	if (isBeforeFirst()) {
		return false;
	} else if (isFirst()) {
		beforeFirst();
		return false;
	} else if (row_position > 1) {
		row_position--;
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
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::refreshRow()");
}
/* }}} */


/* {{{ MySQL_ResultSet::relative() -I- */
bool
MySQL_ResultSet::relative(int rows)
{
	CPP_ENTER("MySQL_ResultSet::relative");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	if (rows != 0) {
		if ((row_position + rows) > num_rows || (row_position + rows) < 1) {
			row_position = rows > 0? num_rows + 1 : 0; /* after last or before first */
		} else {
			row_position += rows;
			seek();
		}
	}

	return (row_position < (num_rows + 1) || row_position > 0);
}
/* }}} */


/* {{{ MySQL_ResultSet::rowDeleted() -U- */
bool
MySQL_ResultSet::rowDeleted()
{
	CPP_ENTER("MySQL_ResultSet::rowDeleted");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::rowDeleted()");
}
/* }}} */


/* {{{ MySQL_ResultSet::rowInserted() -U- */
bool
MySQL_ResultSet::rowInserted()
{
	CPP_ENTER("MySQL_ResultSet::rowInserted");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::rowInserted()");
}
/* }}} */


/* {{{ MySQL_ResultSet::rowUpdated() -U- */
bool
MySQL_ResultSet::rowUpdated()
{
	CPP_ENTER("MySQL_ResultSet::rowUpdated");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::rowUpdated()");
}
/* }}} */


/* {{{ MySQL_ResultSet::rowsCount() -I- */
size_t
MySQL_ResultSet::rowsCount() const
{
	CPP_ENTER("MySQL_ResultSet::rowsCount");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	return (size_t) mysql_num_rows(result->get());
}
/* }}} */


/* {{{ MySQL_ResultSet::setFetchSize() -U- */
void
MySQL_ResultSet::setFetchSize(size_t /* rows */)
{
	CPP_ENTER("MySQL_ResultSet::setFetchSize");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::setFetchSize()");
}
/* }}} */


/* {{{ MySQL_ResultSet::wasNull() -I- */
bool
MySQL_ResultSet::wasNull() const
{
	CPP_ENTER("MySQL_ResultSet::wasNull");
	CPP_INFO_FMT("this=%p", this);
	checkValid();
	return was_null;
}
/* }}} */


/* {{{ MySQL_ResultSet::seek() -I- */
void
MySQL_ResultSet::seek()
{
	CPP_ENTER("MySQL_ResultSet::seek");
	mysql_data_seek(result->get(), row_position - 1);
	row = mysql_fetch_row(result->get());
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
