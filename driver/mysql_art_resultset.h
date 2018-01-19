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



#ifndef _MYSQL_ART_RESULTSET_H_
#define _MYSQL_ART_RESULTSET_H_

#include <list>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include <memory>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>

#include <cppconn/resultset.h>


namespace sql
{
namespace mysql
{

class MySQL_DebugLogger;

class CPPCONN_PUBLIC_FUNC MyVal
{
  union {
    sql::SQLString * str;
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
      val.str = new sql::SQLString(*rhs.val.str);
    }
  }

public:
  MyVal(const sql::SQLString & s);

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

  sql::SQLString getString();

  long double getDouble();

  int64_t getInt64();

  uint64_t getUInt64();

  bool getBool();
};

class MySQL_ArtResultSetMetaData;

class MySQL_ArtResultSet : public sql::ResultSet
{
public:
  typedef std::list<sql::SQLString> StringList;
  typedef std::vector< MyVal > row_t;
  typedef std::list< row_t > rset_t;

  MySQL_ArtResultSet(const StringList& fn, boost::shared_ptr< rset_t > &rset, boost::shared_ptr< MySQL_DebugLogger > & l);
  virtual ~MySQL_ArtResultSet();

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

  // Get the given column as double
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

  // Get the given column as string
  SQLString getString(uint32_t columnIndex) const;

  SQLString getString(const sql::SQLString& columnLabel) const;

  sql::ResultSet::enum_type getType() const;

  void getWarnings();

  void insertRow();

  bool isAfterLast() const;

  bool isBeforeFirst() const;

  bool isClosed() const;

  bool isFirst() const;

  // Retrieves whether the cursor is on the last row of this sql::ResultSet object.
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

protected:
  void checkValid() const;
  bool isBeforeFirstOrAfterLast() const;
  void seek();

public:

  unsigned int num_fields;
  boost::shared_ptr< MySQL_ArtResultSet::rset_t > rset;
  rset_t::iterator current_record;
  bool started;

  typedef std::map< sql::SQLString, int > FieldNameIndexMap;

  FieldNameIndexMap field_name_to_index_map;
  boost::scoped_array< sql::SQLString > field_index_to_name_map;

  uint64_t num_rows;
  uint64_t row_position; /* 0 = before first row, 1 - first row, 'num_rows + 1' - after last row */

  bool is_closed;

  boost::scoped_ptr< MySQL_ArtResultSetMetaData > meta;

protected:
  boost::shared_ptr< MySQL_DebugLogger > logger;

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
