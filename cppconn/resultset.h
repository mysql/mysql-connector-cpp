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

#ifndef _SQL_RESULTSET_H_
#define _SQL_RESULTSET_H_

#include "exception.h"
#include "resultset_metadata.h"
#include <list>
#include <string>
#include <map>
#include <iostream>

namespace sql
{

class Statement;

class RowID 
{
public:
	virtual ~RowID() {}
};

class ResultSet
{
public:
	enum
	{
		CLOSE_CURSORS_AT_COMMIT,
		HOLD_CURSORS_OVER_COMMIT
	};
	enum
	{
		CONCUR_READ_ONLY,
		CONCUR_UPDATABLE
	};
	enum
	{
		FETCH_FORWARD,
		FETCH_REVERSE,
		FETCH_UNKNOWN
	};
	enum
	{
		TYPE_FORWARD_ONLY,
		TYPE_SCROLL_INSENSITIVE,
		TYPE_SCROLL_SENSITIVE
	};

	virtual ~ResultSet() {}

	virtual bool absolute(int row) = 0;

	virtual void afterLast() = 0;

	virtual void beforeFirst() = 0;

	virtual void cancelRowUpdates() = 0;

	virtual void clearWarnings() = 0;

	virtual void close() = 0;

	virtual void deleteRow() { /* We don't support row deletion */ }

	virtual unsigned int findColumn(const std::string& columnLabel) const = 0;

	virtual bool first() = 0;

	virtual std::istream * getBlob(unsigned int columnIndex)  const = 0;
	virtual std::istream * getBlob(const std::string& columnLabel) const = 0;

	virtual bool getBoolean(unsigned int columnIndex) const = 0;
	virtual bool getBoolean(const std::string& columnLabel) const = 0;

	virtual int getConcurrency() = 0;
	virtual std::string getCursorName() = 0;

	virtual double getDouble(unsigned int columnIndex) const = 0;
	virtual double getDouble(const std::string& columnLabel) const = 0;

	virtual int getFetchDirection() = 0;
	virtual int getFetchSize() = 0;
	virtual int getHoldability() = 0;

	virtual int getInt(unsigned int columnIndex) const = 0;
	virtual int getInt(const std::string& columnLabel) const = 0;

	virtual long long getLong(unsigned int columnIndex) const = 0;
	virtual long long getLong(const std::string& columnLabel) const = 0;

	virtual ResultSetMetaData * getMetaData() const = 0;

	virtual size_t getRow() const = 0;

	virtual RowID * getRowId(unsigned int columnIndex) = 0;
	virtual RowID * getRowId(const std::string & columnLabel) = 0;

	virtual const Statement * getStatement() const = 0;

	virtual std::string getString(unsigned int columnIndex)  const = 0;
	virtual std::string getString(const std::string& columnLabel) const = 0;

	virtual void getWarnings() = 0;

	virtual void insertRow() = 0;

	virtual bool isAfterLast() const = 0;

	virtual bool isBeforeFirst() const = 0;

	virtual bool isClosed() const = 0;

	virtual bool isFirst() const = 0;

	virtual bool isLast() const = 0;

	virtual bool isNull(unsigned int columnIndex) const = 0;
	virtual bool isNull(const std::string& columnLabel) const = 0;

	virtual bool last() = 0;

	virtual bool next() = 0;

	virtual void moveToCurrentRow() = 0;

	virtual void moveToInsertRow() = 0;

	virtual bool previous() = 0;

	virtual void refreshRow() = 0;

	virtual bool relative(int rows) = 0;

	virtual bool rowDeleted() = 0;

	virtual bool rowInserted() = 0;

	virtual bool rowUpdated() = 0;

	virtual void setFetchSize(size_t rows) = 0;

	virtual size_t rowsCount() const = 0;

	virtual bool wasNull() const = 0;
};

}; /* namespace sql */

#endif /* _SQL_RESULTSET_H_ */
