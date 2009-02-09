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

#ifndef _MYSQL_ART_RESULTSET_H_
#define _MYSQL_ART_RESULTSET_H_

#include <cppconn/resultset.h>
#include "mysql_res_wrapper.h"

#include "mysql_private_iface.h"

#include <list>
#include <vector>
#include <string.h>
#include <stdlib.h>


#if !defined(_WIN32) && !defined(_WIN64)
#include <stdint.h>
#else
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#endif	//	_WIN32


namespace sql
{
namespace mysql
{
namespace util {template<class T> class my_shared_ptr; }; // forward declaration.

class MySQL_DebugLogger;

class MyVal
{
	union {
		std::string * str; 
		double dval;
		int64_t lval;
		uint64_t ulval;
		bool bval;
		const void * pval;
	} val;
	enum
	{
		typeString,
		typeDouble,
		typeInt,
		typeUInt,
		typeBool,
		typePtr
	} val_type;

	void copy_obj(const MyVal & rhs)
	{
		val_type = rhs.val_type;
		if (val_type != typeString) {
			val = rhs.val;
		} else {
			val.str = new std::string(*rhs.val.str);
		}	
	}

public:
	MyVal(const std::string & s);

	MyVal(const char * const s);

	MyVal(double d) : val_type(typeDouble) { val.dval = d; } 

	MyVal(int64_t l) : val_type(typeInt) { val.lval = l; } 

	MyVal(uint64_t ul) : val_type(typeUInt) { val.ulval = ul; } 

	MyVal(bool b) : val_type(typeBool) { val.bval = b;} 

	MyVal(void * p) : val_type(typePtr) { val.pval = p; }

	MyVal(const MyVal & rhs) { copy_obj(rhs); }

	const MyVal & operator=(const MyVal & rhs) { copy_obj(rhs); return *this; }

	~MyVal()
	{
		if (val_type == typeString) {
			delete val.str;
		}
	}

	std::string getString();

	double getDouble();

	int64_t getInt64();

	uint64_t getUInt64();

	bool getBool();
};



class MySQL_ArtResultSet : public sql::ResultSet
{
public:
	typedef std::list<std::string> StringList;
	typedef std::vector< MyVal > row_t;
	typedef std::list< row_t > rset_t;

	MySQL_ArtResultSet(const StringList& fn, const rset_t & rset, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l);
	virtual ~MySQL_ArtResultSet();

	bool absolute(int row);

	void afterLast();

	void beforeFirst();

	void cancelRowUpdates();

	void clearWarnings();

	void close();

	unsigned int findColumn(const std::string& columnLabel) const;

	bool first();

	std::istream * getBlob(unsigned int columnIndex) const;

	std::istream * getBlob(const std::string& columnLabel) const;

	bool getBoolean(unsigned int columnIndex) const;

	bool getBoolean(const std::string& columnLabel) const;
	
	int getConcurrency();

	std::string getCursorName();

	// Get the given column as double
	double getDouble(unsigned int columnIndex) const;

	double getDouble(const std::string& columnLabel) const;

	int getFetchDirection();
	int getFetchSize();
	int getHoldability();

	// Get the given column as int
	int getInt(unsigned int columnIndex) const;

	int getInt(const std::string& columnLabel) const;

	// Get the given column as int
	long long getLong(unsigned int columnIndex) const;

	long long getLong(const std::string& columnLabel) const;

	sql::ResultSetMetaData * getMetaData() const;

	size_t getRow() const;

	sql::RowID * getRowId(unsigned int columnIndex);
	sql::RowID * getRowId(const std::string & columnLabel);

	const sql::Statement * getStatement() const;

	// Get the given column as string
	std::string getString(unsigned int columnIndex) const;

	std::string getString(const std::string& columnLabel) const;

	void getWarnings();

	void insertRow();

	bool isAfterLast() const;

	bool isBeforeFirst() const;

	bool isClosed() const;

	bool isFirst() const;

	// Retrieves whether the cursor is on the last row of this sql::ResultSet object.
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

protected:
	void checkValid() const;
	bool isBeforeFirstOrAfterLast() const;
	void seek();

public:

	unsigned int num_fields;
	rset_t rset;
	rset_t::iterator current_record;
	bool started;

	typedef std::map< std::string, int > FieldNameIndexMap;
	typedef std::pair< std::string, int > FieldNameIndexPair;

	FieldNameIndexMap field_name_to_index_map;
	std::string * field_index_to_name_map;

	my_ulonglong num_rows;
	my_ulonglong row_position; /* 0 = before first row, 1 - first row, 'num_rows + 1' - after last row */

	bool is_closed;

protected:
	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger;

private:
	/* Prevent use of these */
	MySQL_ArtResultSet(const MySQL_ArtResultSet &);
	void operator=(MySQL_ArtResultSet &);
};

}; /* namespace mysql */
}; /* namespace sql */
#endif // _MYSQL_ART_RESULTSET_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
