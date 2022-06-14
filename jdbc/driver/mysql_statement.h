/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates.
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



#ifndef _MYSQL_STATEMENT_H_
#define _MYSQL_STATEMENT_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include "mysql_resultbind.h"

#include "mysql_warning.h"

#include "mysql_util.h"

namespace sql
{
namespace mysql
{
class MySQL_Connection;
class MySQL_DebugLogger;


namespace NativeAPI
{
class NativeResultsetWrapper;
class NativeConnectionWrapper;
}

class MySQL_Statement : public sql::Statement
{
protected:
  boost::scoped_ptr<MySQL_Warning> warnings;
  MySQL_Connection * connection;
  std::weak_ptr< NativeAPI::NativeConnectionWrapper > proxy;

  MySQL_AttributesBind attrbind;

  void do_query(const ::sql::SQLString &q);
  bool isClosed;
  bool warningsHaveBeenLoaded;

  uint64_t last_update_count;

  boost::shared_ptr< MySQL_DebugLogger > logger;

  sql::ResultSet::enum_type resultset_type;

  unsigned int warningsCount;

  virtual boost::shared_ptr< NativeAPI::NativeResultsetWrapper > get_resultset();
  virtual void checkClosed();

public:
  MySQL_Statement(MySQL_Connection * conn, std::shared_ptr< NativeAPI::NativeConnectionWrapper > & _proxy,
          sql::ResultSet::enum_type rset_type, boost::shared_ptr< MySQL_DebugLogger > & l);

  ~MySQL_Statement();

  sql::Connection * getConnection() override;

  void cancel() override;

  void clearWarnings() override;

  void close() override;

  bool execute(const sql::SQLString& sql) override;

  sql::ResultSet * executeQuery(const sql::SQLString& sql) override;

  int executeUpdate(const sql::SQLString& sql) override;

  size_t getFetchSize() override;

  unsigned int getMaxFieldSize() override;

  uint64_t getMaxRows() override;

  bool getMoreResults() override;

  unsigned int getQueryTimeout() override;

  sql::ResultSet * getResultSet() override;

  sql::ResultSet::enum_type getResultSetType() override;

  uint64_t getUpdateCount() override;

  const SQLWarning * getWarnings() override;/* should return differen type */

  Statement * setBuffered();

  void setCursorName(const sql::SQLString & name) override;

  void setEscapeProcessing(bool enable) override;

  void setFetchSize(size_t rows) override;

  void setMaxFieldSize(unsigned int max) override;

  void setMaxRows(unsigned int max) override;

  void setQueryTimeout(unsigned int seconds) override;

  sql::Statement * setResultSetType(sql::ResultSet::enum_type type) override;

  int setQueryAttrBigInt(const sql::SQLString &name, const sql::SQLString& value) override;
  int setQueryAttrBoolean(const sql::SQLString &name, bool value) override;
  int setQueryAttrDateTime(const sql::SQLString &name, const sql::SQLString& value) override;
  int setQueryAttrDouble(const sql::SQLString &name, double value) override;
  int setQueryAttrInt(const sql::SQLString &name, int32_t value) override;
  int setQueryAttrUInt(const sql::SQLString &name, uint32_t value) override;
  int setQueryAttrInt64(const sql::SQLString &name, int64_t value) override;
  int setQueryAttrUInt64(const sql::SQLString &name, uint64_t value) override;
  int setQueryAttrNull(const sql::SQLString &name) override;
  int setQueryAttrString(const sql::SQLString &name, const sql::SQLString& value) override;

  void clearAttributes() override;
private:
  /* Prevent use of these */
  MySQL_Statement(const MySQL_Statement &);
  void operator=(MySQL_Statement &);
};

} /* namespace mysql */
} /* namespace sql */
#endif // _MYSQL_STATEMENT_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
