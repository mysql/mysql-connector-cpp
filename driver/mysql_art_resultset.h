/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_ART_RESULTSET_H_
#define _MYSQL_ART_RESULTSET_H_

#include <list>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <memory>

#include <cppconn/resultset.h>
#include "mysql_res_wrapper.h"

#include "mysql_private_iface.h"


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
		long double dval;
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

	MyVal(long double d) : val_type(typeDouble) { val.dval = d; }

	MyVal(double d) : val_type(typeDouble) { val.dval = d; }

	MyVal(int64_t l) : val_type(typeInt) { val.lval = l; }

	MyVal(uint64_t ul) : val_type(typeUInt) { val.ulval = ul; }

	MyVal(bool b) : val_type(typeBool) { val.bval = b; } 

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

	long double getDouble();

	int64_t getInt64();

	uint64_t getUInt64();

	bool getBool();
};

class MySQL_ArtResultSetMetaData;

class MySQL_ArtResultSet : public sql::ResultSet
{
public:
	typedef std::list<std::string> StringList;
	typedef std::vector< MyVal > row_t;
	typedef std::list< row_t > rset_t;

	MySQL_ArtResultSet(const StringList& fn, rset_t * const rset, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l);
	virtual ~MySQL_ArtResultSet();

	bool absolute(int row);

	void afterLast();

	void beforeFirst();

	void close();

	uint32_t findColumn(const std::string& columnLabel) const;

	bool first();

	std::istream * getBlob(uint32_t columnIndex) const;

	std::istream * getBlob(const std::string& columnLabel) const;

	bool getBoolean(uint32_t columnIndex) const;

	bool getBoolean(const std::string& columnLabel) const;

	// Get the given column as double
	long double getDouble(uint32_t columnIndex) const;

	long double getDouble(const std::string& columnLabel) const;

	int32_t getInt(uint32_t columnIndex) const;

	int32_t getInt(const std::string& columnLabel) const;

	uint32_t getUInt(uint32_t columnIndex) const;

	uint32_t getUInt(const std::string& columnLabel) const;

	int64_t getInt64(uint32_t columnIndex) const;

	int64_t getInt64(const std::string& columnLabel) const;

	uint64_t getUInt64(uint32_t columnIndex) const;

	uint64_t getUInt64(const std::string& columnLabel) const;

	sql::ResultSetMetaData * getMetaData() const;

	size_t getRow() const;

	const sql::Statement * getStatement() const;

	// Get the given column as string
	std::string getString(uint32_t columnIndex) const;

	std::string getString(const std::string& columnLabel) const;

	sql::ResultSet::enum_type getType() const;

	bool isAfterLast() const;

	bool isBeforeFirst() const;

	bool isClosed() const;

	bool isFirst() const;

	// Retrieves whether the cursor is on the last row of this sql::ResultSet object.
	bool isLast() const;

	bool isNull(uint32_t columnIndex) const;

	bool isNull(const std::string& columnLabel) const;

	bool last();

	bool next();

	bool previous();

	bool relative(int rows);

	size_t rowsCount() const;

	bool wasNull() const;

protected:
	void checkValid() const;
	bool isBeforeFirstOrAfterLast() const;
	void seek();

public:

	unsigned int num_fields;
	std::auto_ptr< MySQL_ArtResultSet::rset_t > rset;
	rset_t::iterator current_record;
	bool started;

	typedef std::map< std::string, int > FieldNameIndexMap;
	typedef std::pair< std::string, int > FieldNameIndexPair;

	FieldNameIndexMap field_name_to_index_map;
	std::string * field_index_to_name_map;

	my_ulonglong num_rows;
	my_ulonglong row_position; /* 0 = before first row, 1 - first row, 'num_rows + 1' - after last row */

	bool is_closed;

	std::auto_ptr< MySQL_ArtResultSetMetaData > meta;

protected:
	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger;

private:
	/* Prevent use of these */
	MySQL_ArtResultSet(const MySQL_ArtResultSet &);
	void operator=(MySQL_ArtResultSet &);
};

} /* namespace mysql */
} /* namespace sql */
#endif // _MYSQL_ART_RESULTSET_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
