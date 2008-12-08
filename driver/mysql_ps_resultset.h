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

#ifndef _MYSQL_PS_RESULTSET_H_
#define _MYSQL_PS_RESULTSET_H_

#include <cppconn/resultset.h>

#include "mysql_private_iface.h"

namespace sql
{
namespace mysql
{
namespace util {template<class T> class my_shared_ptr; }; // forward declaration.
class MySQL_Prepared_Statement;
class MySQL_DebugLogger;

class MySQL_Prepared_ResultSet : public sql::ResultSet
{
private:
	MYSQL_ROW row;
	MYSQL_STMT *stmt;

	mutable int last_queried_column;  // this is updated by calls to getInt(int), getString(int), etc...

	unsigned int num_fields;
	my_ulonglong num_rows;
	my_ulonglong row_position;

	typedef std::map< std::string, unsigned int > FieldNameIndexMap;

	FieldNameIndexMap field_name_to_index_map;
	bool was_null;

	const MySQL_Prepared_Statement * parent;

	bool is_valid;

	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger;

	std::auto_ptr<sql::ResultSetMetaData> rs_meta;
protected:
	void checkValid() const;
	void closeIntern();
	bool isBeforeFirstOrAfterLast() const;
	void seek();

public:
	MySQL_Prepared_ResultSet(MYSQL_STMT *s, MySQL_Prepared_Statement * par, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l);

	virtual ~MySQL_Prepared_ResultSet();

	bool absolute(int row);

	void afterLast();

	void beforeFirst();

	void cancelRowUpdates();

	void clearWarnings();

	void close();

	unsigned int findColumn(const std::string& columnLabel) const;

	bool first();

	bool getBoolean(unsigned int columnIndex) const;
	bool getBoolean(const std::string& columnLabel) const;

	int getConcurrency();

	std::string getCursorName();

	double getDouble(unsigned int columnIndex) const;
	double getDouble(const std::string& columnLabel) const;

	int getFetchDirection();
	int getFetchSize();
	int getHoldability();

	int getInt(unsigned int columnIndex) const;
	int getInt(const std::string& columnLabel) const;
	
	long long getLong(unsigned int columnIndex) const;
	long long getLong(const std::string& columnLabel) const;

	sql::ResultSetMetaData * getMetaData() const;

	size_t getRow() const;

	sql::RowID * getRowId(unsigned int columnIndex);
	sql::RowID * getRowId(const std::string & columnLabel);

	const sql::Statement * getStatement() const;

	std::string getString(unsigned int columnIndex) const;	
	std::string getString(const std::string& columnLabel) const;
	
	void getWarnings();

	bool isAfterLast() const;

	bool isBeforeFirst() const;

	bool isClosed() const;

	void insertRow();

	bool isFirst() const;

	bool isLast() const;

	bool isNull(unsigned int columnIndex) const;

	bool isNull(const std::string& columnLabel) const;

	bool last();

	void moveToCurrentRow();

	void moveToInsertRow();

	bool next();

	bool previous();

	void refreshRow();

	bool relative(int rows);

	bool rowDeleted();

	bool rowInserted();

	bool rowUpdated();

	size_t rowsCount() const;

	void setFetchSize(size_t rows);

	bool wasNull() const;
};

}; /* namespace mysql*/
}; /* namespace sql*/

#endif // _MYSQL_PS_RESULTSET_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
