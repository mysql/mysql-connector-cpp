/*
   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/
#ifndef _WIN32
#include <string.h>
#endif

#include <sstream>

#include <cppconn/exception.h>
#include "mysql_resultset.h"
#include "mysql_resultset_metadata.h"
#include "mysql_statement.h"
#include "mysql_res_wrapper.h"

#include "mysql_debug.h"
#include "mysql_util.h"

namespace sql
{
namespace mysql
{



/* {{{ MySQL_ResultSet::MySQL_ResultSet() -I- */
MySQL_ResultSet::MySQL_ResultSet(MYSQL_RES_Wrapper * res, MySQL_Statement * par, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l)
	: row(NULL), result(res), row_position(0), was_null(false), parent(par), logger(l? l->getReference():NULL)
{
	CPP_ENTER("MySQL_ResultSet::MySQL_ResultSet");
	num_rows = mysql_num_rows(result->get());

	num_fields = mysql_num_fields(result->get());
	for (unsigned int i = 0; i < num_fields; ++i) {
		char *tmp = sql::mysql::util::utf8_strup(mysql_fetch_field_direct(result->get(), i)->name, 0);
		field_name_to_index_map[std::string(tmp)] = i;
		delete [] tmp;
	}
}
/* }}} */


/* {{{ MySQL_ResultSet::~MySQL_ResultSet() -I- */
MySQL_ResultSet::~MySQL_ResultSet()
{
	/* Don't remove the block or we can get into problems with logger */
	{
		CPP_ENTER("MySQL_ResultSet::~MySQL_ResultSet");
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
MySQL_ResultSet::absolute(const int new_pos)
{
	CPP_ENTER("MySQL_ResultSet::absolute");
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
	mysql_data_seek(result->get(), 0);
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
	result->dispose();
}
/* }}} */


/* {{{ MySQL_ResultSet::findColumn() -I- */
uint32_t
MySQL_ResultSet::findColumn(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ResultSet::findColumn");
	checkValid();
	char *tmp = sql::mysql::util::utf8_strup(columnLabel.c_str(), 0);
	FieldNameIndexMap::const_iterator iter = field_name_to_index_map.find(tmp);
	delete [] tmp;

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
MySQL_ResultSet::getBlob(const std::string& columnLabel) const
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
MySQL_ResultSet::getBoolean(const std::string& columnLabel) const
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
std::string
MySQL_ResultSet::getCursorName()
{
	CPP_ENTER("MySQL_ResultSet::getCursorName");
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::getCursorName()");
	return ""; // fool compilers
}
/* }}} */


/* {{{ MySQL_ResultSet::getDouble() -I- */
double
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
	if (row[columnIndex - 1] == NULL) {
		was_null = true;
		return 0.0;
	}
	was_null = false;
	return atof(row[columnIndex - 1]);
}
/* }}} */


/* {{{ MySQL_ResultSet::getDouble() -I- */
double
MySQL_ResultSet::getDouble(const std::string& columnLabel) const
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
int
MySQL_ResultSet::getFetchSize()
{
	CPP_ENTER("MySQL_ResultSet::getFetchSize");
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::getFetchSize()");
	return 0; // fool compilers
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
	return static_cast<int>(getInt64(columnIndex) );// & 0xffffffff;
}
/* }}} */


/* {{{ MySQL_ResultSet::getInt() -I- */
int32_t
MySQL_ResultSet::getInt(const std::string& columnLabel) const
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
	return static_cast<unsigned int>(getUInt64(columnIndex));// & 0xffffffff;
}
/* }}} */


/* {{{ MySQL_ResultSet::getUInt() -I- */
uint32_t
MySQL_ResultSet::getUInt(const std::string& columnLabel) const
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

	if (row[columnIndex - 1] == NULL) {
		was_null = true;
		return 0;
	}
	was_null = false;
	return atoll(row[columnIndex - 1]);
}
/* }}} */


/* {{{ MySQL_ResultSet::getInt64() -I- */
int64_t
MySQL_ResultSet::getInt64(const std::string& columnLabel) const
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

	if (row[columnIndex - 1] == NULL) {
		was_null = true;
		return 0;
	}
	was_null = false;
	return strtoull(row[columnIndex - 1], NULL, 10);
}
/* }}} */


/* {{{ MySQL_ResultSet::getUInt64() -I- */
uint64_t
MySQL_ResultSet::getUInt64(const std::string& columnLabel) const
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
	return new MySQL_ResultSetMetaData(result->getReference(), logger);
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
MySQL_ResultSet::getRowId(const std::string &)
{
	CPP_ENTER("MySQL_ResultSet::getRowId");
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::getRowId(const std::string & columnLabel)");
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
std::string
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

	if (row[columnIndex - 1] == NULL) {
		was_null= true;
		return "";
	}
	was_null= false;
	return std::string(row[columnIndex - 1], mysql_fetch_lengths(result->get())[columnIndex - 1]);
}
/* }}} */


/* {{{ MySQL_ResultSet::getString() -I- */
std::string
MySQL_ResultSet::getString(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ResultSet::getString(string)");
	return getString(findColumn(columnLabel));
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
	return !result->isValid();
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
	return (row[columnIndex - 1] == NULL);
}
/* }}} */


/* {{{ MySQL_ResultSet::isNull() -I- */
bool
MySQL_ResultSet::isNull(const std::string& columnLabel) const
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
}
/* }}} */


/* {{{ MySQL_ResultSet::moveToInsertRow() -U- */
void
MySQL_ResultSet::moveToInsertRow()
{
	CPP_ENTER("MySQL_ResultSet::moveToInsertRow");
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ResultSet::moveToInsertRow()");
}
/* }}} */


/* {{{ MySQL_ResultSet::next() -I- */
bool
MySQL_ResultSet::next()
{
	CPP_ENTER("MySQL_ResultSet::next");
	checkValid();
	bool ret = false;
	if (isLast()) {
		afterLast();
	} else if (row_position < num_rows + 1) {
		row = mysql_fetch_row(result->get());
		++row_position;
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
	return (size_t) mysql_num_rows(result->get());
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
	return was_null;
}
/* }}} */


/* {{{ MySQL_ResultSet::isBeforeFirstOrAfterLast() -I- */
bool
MySQL_ResultSet::isBeforeFirstOrAfterLast() const
{
	CPP_ENTER("MySQL_ResultSet::isBeforeFirstOrAfterLast");
	checkValid();
	return (row_position == 0) || (row_position == num_rows + 1);
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
