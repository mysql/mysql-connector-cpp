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

#include "mysql_art_resultset.h"
#include "mysql_art_rset_metadata.h"
#include <sstream>

#if !defined(_WIN32) && !defined(_WIN64)
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


/* {{{ MyVal::MyVal() -I- */
MyVal::MyVal(const std::string & s)
  : val_type(typeString)
{
	// Init it clearly 
	val.str = NULL;
	val_type = typeString;
	std::auto_ptr< std::vector< char > > local_val(new std::vector< char >());
	std::vector< char > * tmp = local_val.get();
	tmp->reserve(s.size() + 1);
	for (size_t i = 0; i < s.size(); i++) {
		(*tmp)[i] = s[i];
	}
	(*tmp)[tmp->size()] = '\0';
	// Finished, release it, thus not destructing it
	local_val.release();
	val.str = tmp;
}
/* }}} */


/* {{{ MyVal::getString() -I- */
std::string
MyVal::getString()
{
	switch (val_type) {
		case typeString:
			return std::string(val.str->begin(), val.str->end());
		case typeDouble:
		{
			char buf[31];
			snprintf(buf, sizeof(buf) - 1, "%14.14f", val.dval);
			return buf;	
		}
		case typeInt:
		{
			char buf[20];
			snprintf(buf, sizeof(buf) - 1, "%lld", val.lval);
			return buf;	
		}
		case typeUInt:
		{
			char buf[20];
			snprintf(buf, sizeof(buf) - 1, "%llu", val.ulval);
			return buf;	
		}
		case typeBool:
		{
			char buf[3];
			snprintf(buf, sizeof(buf) - 1, "%d", val.bval);
			return buf;	
		}
		case typePtr:
			return "";
	}
	throw std::runtime_error("impossible");
}
/* }}} */


/* {{{ MyVal::getDouble() -I- */
double
MyVal::getDouble()
{
	switch (val_type) {
		case typeString:
			return atof(std::string(val.str->begin(), val.str->end()).c_str());
		case typeDouble:
			return val.dval;
		case typeInt:
			return val.lval;
		case typeUInt:
			return val.ulval;
		case typeBool:
			return val.bval;
		case typePtr:
			return .0;
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
			return atoll(std::string(val.str->begin(), val.str->end()).c_str());
		case typeDouble:
			return val.dval;
		case typeInt:
			return val.lval;
		case typeUInt:
			return val.ulval;
		case typeBool:
			return val.bval;
		case typePtr:
			return 0;
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
			return atoll(std::string(val.str->begin(), val.str->end()).c_str());
		case typeDouble:
			return val.dval;
		case typeInt:
			return val.lval;
		case typeUInt:
			return val.ulval;
		case typeBool:
			return val.bval;
		case typePtr:
			return 0;
	}
	throw std::runtime_error("impossible");
}
/* }}} */


/* {{{ MyVal::getBool() -I- */
bool
MyVal::getBool()
{
	switch (val_type) {
		case typeString:
			return static_cast<bool>(atoi(std::string(val.str->begin(), val.str->end()).c_str()));
		case typeDouble:
			return static_cast<bool>(val.dval);
		case typeInt:
			return static_cast<bool>(val.lval);
		case typeUInt:
			return static_cast<bool>(val.ulval);
		case typeBool:
			return static_cast<bool>(val.bval);
		case typePtr:
			return val.pval != NULL;
	}
	throw std::runtime_error("impossible");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::MySQL_ArtResultSet() -I- */
MySQL_ArtResultSet::MySQL_ArtResultSet(const StringList& fn, const rset_t & rs, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l)
  : rset(rs), current_record(rset.begin()), started(false), row_position(0), is_closed(false), logger(l? l->getReference():NULL)
{
	CPP_ENTER("MySQL_ArtResultSet::MySQL_ArtResultSet");
	CPP_INFO_FMT("metadata.size=%d resultset.size=%d", fn.size(), rset.size());
	num_fields = static_cast<int>(fn.size());

	num_rows = rset.size();

	field_index_to_name_map = new std::string[num_fields];

	unsigned int idx = 0;
	for (StringList::const_iterator it = fn.begin(), e = fn.end(); it != e; it++, idx++) {
		char *tmp = cppmysql_utf8_strup(it->c_str(), 0);
		field_name_to_index_map[std::string(tmp)] = idx;
		field_index_to_name_map[idx] = std::string(tmp);
		free(tmp);
	}
}
/* }}} */


/* {{{ MySQL_ArtResultSet::~MySQL_ArtResultSet() -I- */
MySQL_ArtResultSet::~MySQL_ArtResultSet()
{
	/* Don't remove the block or we can get into problems with logger */
	{
		CPP_ENTER("MySQL_ArtResultSet::~MySQL_ArtResultSet");
		if (!isClosed()) {
			close();
		}
	}
	logger->freeReference();
}
/* }}} */


/* {{{ MySQL_ArtResultSet::seek() -I- */
void MySQL_ArtResultSet::seek()
{
	CPP_ENTER("MySQL_ArtResultSet::seek");
	current_record = rset.begin();
	/* i should be signed, or when row_position is 0 `i` will overflow */
	for (long long i = row_position - 1; i > 0; --i) {
		current_record++;
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
	delete [] field_index_to_name_map;
	is_closed = true;
}
/* }}} */


/* {{{ MySQL_ArtResultSet::findColumn() -I- */
unsigned int
MySQL_ArtResultSet::findColumn(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ArtResultSet::columnLabel");
	checkValid();
	char * tmp = cppmysql_utf8_strup(columnLabel.c_str(), 0);
	FieldNameIndexMap::const_iterator iter = field_name_to_index_map.find(tmp);
	free(tmp);

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
MySQL_ArtResultSet::getBlob(const unsigned int columnIndex) const
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
MySQL_ArtResultSet::getBlob(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ArtResultSet::getBlob(string)");
	return new std::istringstream(getString(columnLabel));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getBoolean() -I- */
bool
MySQL_ArtResultSet::getBoolean(const unsigned int columnIndex) const
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
MySQL_ArtResultSet::getBoolean(const std::string& columnLabel) const
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
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getCursorName() -U- */
std::string
MySQL_ArtResultSet::getCursorName()
{
	CPP_ENTER("MySQL_ArtResultSet::getCursorName");
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getCursorName()");
}
/* }}} */


// Get the given column as double
/* {{{ MySQL_ArtResultSet::getDouble() -I- */
double
MySQL_ArtResultSet::getDouble(unsigned int columnIndex) const
{
	CPP_ENTER("MySQL_ArtResultSet::getDouble(int)");

	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_ArtResultSet::getDouble: can't fetch because not on result set");
	}

	if (columnIndex > num_fields || columnIndex == 0) {
		throw sql::InvalidArgumentException("MySQL_ArtResultSet::getDouble: invalid value of 'columnIndex'");
	}

	MySQL_ArtResultSet::row_t & tmp_row = *current_record;
	return atof(tmp_row[columnIndex - 1].c_str());
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getDouble() -I- */
double
MySQL_ArtResultSet::getDouble(const std::string& columnLabel) const
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
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getFetchSize() -U- */
int
MySQL_ArtResultSet::getFetchSize()
{
	CPP_ENTER("MySQL_ArtResultSet::getFetchSize");
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getFetchSize()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getHoldability() -U- */
int
MySQL_ArtResultSet::getHoldability()
{
	CPP_ENTER("MySQL_ArtResultSet::getHoldability");
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getHoldability()");
}
/* }}} */


// Get the given column as int
/* {{{ MySQL_ArtResultSet::getInt() -I- */
int
MySQL_ArtResultSet::getInt(unsigned int columnIndex) const
{
	CPP_ENTER("MySQL_ArtResultSet::getInt(int)");

	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_ArtResultSet::getInt: can't fetch because not on result set");
	}

	if (columnIndex > num_fields || columnIndex == 0) {
		throw sql::InvalidArgumentException("MySQL_ArtResultSet::getInt: invalid value of 'columnIndex'");
	}

	MySQL_ArtResultSet::row_t & tmp_row = *current_record;
	return atoi(tmp_row[columnIndex - 1].c_str());
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getInt() -I- */
int
MySQL_ArtResultSet::getInt(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ArtResultSet::getInt(string)");
	return getInt(findColumn(columnLabel));
}
/* }}} */


// Get the given column as int
/* {{{ MySQL_ArtResultSet::getLong() -I- */
long long
MySQL_ArtResultSet::getLong(unsigned int columnIndex) const
{
	CPP_ENTER("MySQL_ArtResultSet::getLong(int)");

	/* isBeforeFirst checks for validity */
	if (isBeforeFirstOrAfterLast()) {
		throw sql::InvalidArgumentException("MySQL_ArtResultSet::getLong: can't fetch because not on result set");
	}

	if (columnIndex > num_fields || columnIndex == 0) {
		throw sql::InvalidArgumentException("MySQL_ArtResultSet::getLong: invalid value of 'columnIndex'");
	}

	MySQL_ArtResultSet::row_t & tmp_row = *current_record;
	return atoll(tmp_row[columnIndex - 1].c_str());
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getLong() -I- */
long long
MySQL_ArtResultSet::getLong(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ArtResultSet::getLong(string)");
	return getLong(findColumn(columnLabel));
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getMetaData() -I- */
sql::ResultSetMetaData *
MySQL_ArtResultSet::getMetaData() const
{
	CPP_ENTER("MySQL_ArtResultSet::getMetaData");
	checkValid();
	return new MySQL_ArtResultSetMetaData(this, logger);
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
MySQL_ArtResultSet::getRowId(unsigned int)
{
	CPP_ENTER("MySQL_ArtResultSet::getRowId");
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getRowId()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getRowId() -U- */
sql::RowID *
MySQL_ArtResultSet::getRowId(const std::string &)
{
	CPP_ENTER("MySQL_ArtResultSet::getRowId");
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ArtResultSet::getRowId()");
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
std::string
MySQL_ArtResultSet::getString(unsigned int columnIndex) const
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

	MySQL_ArtResultSet::row_t & tmp_row = *current_record;
	return (tmp_row[columnIndex - 1]);
}
/* }}} */


/* {{{ MySQL_ArtResultSet::getString() -I- */
std::string
MySQL_ArtResultSet::getString(const std::string& columnLabel) const
{
	CPP_ENTER("MySQL_ArtResultSet::getString(string)");
	return getString(findColumn(columnLabel));
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


/* {{{ MySQL_ArtResultSet::isLast() -I- */
bool
MySQL_ArtResultSet::isNull(const unsigned int columnIndex) const
{
	CPP_ENTER("MySQL_ArtResultSet::isNull(int)");
	checkValid();
	if (columnIndex > num_fields || columnIndex == 0) {
		throw sql::InvalidArgumentException("MySQL_ArtResultSet::isNull: invalid value of 'columnIndex'");
	}
	return false;
}
/* }}} */


/* {{{ MySQL_ArtResultSet::isNull() -I- */
bool
MySQL_ArtResultSet::isNull(const std::string& columnLabel) const
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
	return num_rows? true:false;
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
		current_record++;
		row_position++;
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
		row_position--;
		current_record--;
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

	return (row_position < (num_rows + 1) || row_position > 0);
}
/* }}} */


/* {{{ MySQL_ArtResultSet::rowDeleted() -U- */
bool
MySQL_ArtResultSet::rowDeleted()
{
	CPP_ENTER("MySQL_ArtResultSet::rowDeleted");
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ArtResultSet::rowDeleted()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::rowInserted() -U- */
bool
MySQL_ArtResultSet::rowInserted()
{
	CPP_ENTER("MySQL_ArtResultSet::rowInserted");
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ArtResultSet::rowInserted()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::rowUpdated() -U- */
bool
MySQL_ArtResultSet::rowUpdated()
{
	CPP_ENTER("MySQL_ArtResultSet::rowUpdated");
	checkValid();
	throw sql::MethodNotImplementedException("MySQL_ArtResultSet::rowUpdated()");
}
/* }}} */


/* {{{ MySQL_ArtResultSet::rowsCount() -I- */
size_t
MySQL_ArtResultSet::rowsCount() const
{
	CPP_ENTER("MySQL_ArtResultSet::rowsCount");
	checkValid();
	return num_rows;
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
	return false;
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
