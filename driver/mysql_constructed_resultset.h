/*
   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_CONSTRUCTED_RESULTSET_H_
#define _MYSQL_CONSTRUCTED_RESULTSET_H_

#include <cppconn/resultset.h>
#include "mysql_res_wrapper.h"

#include "mysql_private_iface.h"

namespace sql
{
namespace mysql
{
namespace util {template<class T> class my_shared_ptr; }; // forward declaration.

class MySQL_DebugLogger;

class MySQL_ConstructedResultSet : public sql::ResultSet
{
public:
	typedef std::list<std::string> StringList;

	MySQL_ConstructedResultSet(const StringList& fn, const StringList& rset, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l);
	virtual ~MySQL_ConstructedResultSet();

	bool absolute(int row);

	void afterLast();

	void beforeFirst();

	void cancelRowUpdates();

	void clearWarnings();

	void close();

	uint32_t findColumn(const std::string& columnLabel) const;

	bool first();

	std::istream * getBlob(uint32_t columnIndex) const;

	std::istream * getBlob(const std::string& columnLabel) const;

	bool getBoolean(uint32_t columnIndex) const;

	bool getBoolean(const std::string& columnLabel) const;

	int getConcurrency();

	std::string getCursorName();

	// Get the given column as double
	double getDouble(uint32_t columnIndex) const;

	double getDouble(const std::string& columnLabel) const;

	int getFetchDirection();
	int getFetchSize();
	int getHoldability();

	// Get the given column as int
	int getInt(uint32_t columnIndex) const;

	int getInt(const std::string& columnLabel) const;

	unsigned int getUInt(uint32_t columnIndex) const;

	unsigned int getUInt(const std::string& columnLabel) const;

	int64_t getInt64(uint32_t columnIndex) const;

	int64_t getInt64(const std::string& columnLabel) const;

	uint64_t getUInt64(uint32_t columnIndex) const;

	uint64_t getUInt64(const std::string& columnLabel) const;

	sql::ResultSetMetaData * getMetaData() const;

	size_t getRow() const;

	sql::RowID * getRowId(uint32_t columnIndex);
	sql::RowID * getRowId(const std::string & columnLabel);

	const sql::Statement * getStatement() const;

	// Get the given column as string
	std::string getString(uint32_t columnIndex) const;

	std::string getString(const std::string& columnLabel) const;

	void getWarnings();

	void insertRow();

	bool isAfterLast() const;

	bool isBeforeFirst() const;

	bool isClosed() const;

	bool isFirst() const;

	// Retrieves whether the cursor is on the last row of this sql::ResultSet object.
	bool isLast() const;

	bool isNull(uint32_t columnIndex) const;

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
	StringList rs;
	bool started;

	typedef std::map< std::string, int > FieldNameIndexMap;
	typedef std::pair< std::string, int > FieldNameIndexPair;

	FieldNameIndexMap field_name_to_index_map;
	std::string * field_index_to_name_map;
	StringList::iterator current_record;

	my_ulonglong num_rows;
	my_ulonglong row_position; /* 0 = before first row, 1 - first row, 'num_rows + 1' - after last row */

	bool is_closed;

protected:
	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger;

private:
	/* Prevent use of these */
	MySQL_ConstructedResultSet(const MySQL_ConstructedResultSet &);
	void operator=(MySQL_ConstructedResultSet &);
};

}; /* namespace mysql */
}; /* namespace sql */
#endif // _MYSQL_CONSTRUCTED_RESULTSET_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
