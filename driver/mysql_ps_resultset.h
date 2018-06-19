/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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



#ifndef _MYSQL_PS_RESULTSET_H_
#define _MYSQL_PS_RESULTSET_H_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <cppconn/resultset.h>
#include <config.h>


#include "nativeapi/mysql_private_iface.h"
namespace sql
{
namespace mysql
{
class MySQL_Prepared_Statement;
class MySQL_DebugLogger;
class MySQL_PreparedResultSetMetaData;
class MySQL_ResultBind;

namespace NativeAPI
{
class NativeStatementWrapper;
}

class MySQL_Prepared_ResultSet : public sql::ResultSet
{
private:
  boost::shared_ptr< NativeAPI::NativeStatementWrapper > proxy;

  mutable uint32_t last_queried_column;  // this is updated by calls to getInt(int), getString(int), etc...

  unsigned int num_fields;
  uint64_t num_rows;
  uint64_t row_position;

  typedef std::map< sql::SQLString, unsigned int > FieldNameIndexMap;

  FieldNameIndexMap field_name_to_index_map;

  const MySQL_Prepared_Statement * parent;

  bool is_valid;

  boost::shared_ptr< MySQL_DebugLogger > logger;

  boost::scoped_ptr< MySQL_PreparedResultSetMetaData > rs_meta;

  boost::shared_ptr< MySQL_ResultBind > result_bind;

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
  MySQL_Prepared_ResultSet(boost::shared_ptr< NativeAPI::NativeStatementWrapper > & s,
    boost::shared_ptr< MySQL_ResultBind > & r_bind, sql::ResultSet::enum_type rset_type,
    MySQL_Prepared_Statement * par, boost::shared_ptr< MySQL_DebugLogger > &l);

  virtual ~MySQL_Prepared_ResultSet();

  bool absolute(int row);

  void afterLast();

  void beforeFirst();

  void cancelRowUpdates();

  void clearWarnings();

  void close();

  uint32_t findColumn(const sql::SQLString& columnLabel) const;

  bool first();

  std::istream * getBlob(uint32_t columnIndex) const;
  std::istream * getBlob(const sql::SQLString& columnLabel) const;

  bool getBoolean(uint32_t columnIndex) const;
  bool getBoolean(const sql::SQLString& columnLabel) const;

  int getConcurrency();

  SQLString getCursorName();

  long double getDouble(uint32_t columnIndex) const;
  long double getDouble(const sql::SQLString& columnLabel) const;

  int getFetchDirection();
  size_t getFetchSize();
  int getHoldability();

  int32_t getInt(uint32_t columnIndex) const;
  int32_t getInt(const sql::SQLString& columnLabel) const;

  uint32_t getUInt(uint32_t columnIndex) const;
  uint32_t getUInt(const sql::SQLString& columnLabel) const;

  int64_t getInt64(uint32_t columnIndex) const;
  int64_t getInt64(const sql::SQLString& columnLabel) const;

  uint64_t getUInt64(uint32_t columnIndex) const;
  uint64_t getUInt64(const sql::SQLString& columnLabel) const;

  sql::ResultSetMetaData * getMetaData() const;

  size_t getRow() const;

  sql::RowID * getRowId(uint32_t columnIndex);
  sql::RowID * getRowId(const sql::SQLString & columnLabel);

  const sql::Statement * getStatement() const;

  SQLString getString(uint32_t columnIndex) const;
  SQLString getString(const sql::SQLString& columnLabel) const;

  sql::ResultSet::enum_type getType() const;

  void getWarnings();

  bool isAfterLast() const;

  bool isBeforeFirst() const;

  bool isClosed() const;

  void insertRow();

  bool isFirst() const;

  bool isLast() const;

  bool isNull(uint32_t columnIndex) const;

  bool isNull(const sql::SQLString& columnLabel) const;

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
