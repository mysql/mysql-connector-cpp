/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
class MySQL_Prepared_ResultSetMetaData;
class MySQL_ResultBind;

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

	std::auto_ptr< MySQL_Prepared_ResultSetMetaData > rs_meta;
	std::auto_ptr< MySQL_ResultBind > result_bind;

	sql::ResultSet::enum_type resultset_type;

protected:
	void checkValid() const;
	void checkScrollable() const;
	bool isScrollable() const;
	void closeIntern();
	bool isBeforeFirstOrAfterLast() const;
	void seek();

	int64_t getInt64_intern(const uint32_t columnIndex, bool cutTooBig) const;
	uint64_t getUInt64_intern(const uint32_t columnIndex, bool cutTooBig) const;

public:
	MySQL_Prepared_ResultSet(MYSQL_STMT *s, MySQL_ResultBind * r_bind, sql::ResultSet::enum_type rset_type,
							MySQL_Prepared_Statement * par, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l);

	virtual ~MySQL_Prepared_ResultSet();

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

	std::string getString(uint32_t columnIndex) const;
	std::string getString(const std::string& columnLabel) const;

	sql::ResultSet::enum_type getType() const;

	bool isAfterLast() const;

	bool isBeforeFirst() const;

	bool isClosed() const;

	bool isFirst() const;

	bool isLast() const;

	bool isNull(uint32_t columnIndex) const;

	bool isNull(const std::string& columnLabel) const;

	bool last();

	bool next();

	bool previous();

	bool relative(int rows);

	size_t rowsCount() const;

	bool wasNull() const;
};

} /* namespace mysql*/
} /* namespace sql*/

#endif // _MYSQL_PS_RESULTSET_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
