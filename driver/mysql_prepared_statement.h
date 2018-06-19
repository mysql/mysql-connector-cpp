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



#ifndef _MYSQL_PREPARED_STATEMENT_H_
#define _MYSQL_PREPARED_STATEMENT_H_

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include <cppconn/prepared_statement.h>
#include <cppconn/parameter_metadata.h>


namespace sql
{
namespace mysql
{
class MySQL_DebugLogger;
class MySQL_ParamBind;
class MySQL_ParameterMetaData;
class MySQL_PreparedResultSetMetaData;
class MySQL_ResultBind;
class MySQL_Warning;

namespace NativeAPI
{
class NativeStatementWrapper;
}


class MySQL_Prepared_Statement : public sql::PreparedStatement
{
protected:
  sql::Connection * connection;
  boost::shared_ptr< NativeAPI::NativeStatementWrapper > proxy;
  boost::scoped_ptr< MySQL_ParamBind > param_bind;
  unsigned int param_count;

  int resultSetConcurrency;
  int resultSetType;

  boost::scoped_ptr< MySQL_Warning > warnings;

  bool isClosed;
  bool warningsHaveBeenLoaded;

  boost::shared_ptr< MySQL_DebugLogger > logger;

  boost::scoped_ptr< MySQL_PreparedResultSetMetaData > res_meta;
  boost::scoped_ptr< MySQL_ParameterMetaData > param_meta;

  sql::ResultSet::enum_type resultset_type;

  boost::shared_ptr< MySQL_ResultBind > result_bind;

  unsigned int warningsCount;

  virtual void do_query();
  virtual void checkClosed();
  virtual void closeIntern();

  bool sendLongDataBeforeParamBind();

public:

  MySQL_Prepared_Statement(boost::shared_ptr<NativeAPI::NativeStatementWrapper> & s,
              sql::Connection * conn, sql::ResultSet::enum_type rset_type,
              boost::shared_ptr< MySQL_DebugLogger > & log);
  virtual ~MySQL_Prepared_Statement();

  sql::Connection *getConnection();

  void cancel();

  void clearParameters();

  void clearWarnings();

  void close();

  bool execute();
  bool execute(const sql::SQLString& sql);

  sql::ResultSet *executeQuery();
  sql::ResultSet *executeQuery(const sql::SQLString& sql);

  int executeUpdate();
  int executeUpdate(const sql::SQLString& sql);

  size_t getFetchSize();

  unsigned int getMaxFieldSize();

  sql::ResultSetMetaData * getMetaData();

  uint64_t getMaxRows();

  bool getMoreResults();

  sql::ParameterMetaData * getParameterMetaData();

  unsigned int getQueryTimeout();

  sql::ResultSet * getResultSet();

  sql::ResultSet::enum_type getResultSetType();

  uint64_t getUpdateCount();

  const SQLWarning * getWarnings();/* should return different type */

  Statement * setBuffered();

  void setBlob(unsigned int parameterIndex, std::istream * blob);

  void setBoolean(unsigned int parameterIndex, bool value);

  void setBigInt(unsigned int parameterIndex, const sql::SQLString& value);

  void setCursorName(const sql::SQLString &name);

  void setDateTime(unsigned int parameterIndex, const sql::SQLString& value);

  void setDouble(unsigned int parameterIndex, double value);

  void setEscapeProcessing(bool enable);

  void setFetchSize(size_t rows);

  void setInt(unsigned int parameterIndex, int32_t value);

  void setUInt(unsigned int parameterIndex, uint32_t value);

  void setInt64(unsigned int parameterIndex, int64_t value);

  void setUInt64(unsigned int parameterIndex, uint64_t value);

  void setMaxFieldSize(unsigned int max);

  void setMaxRows(unsigned int max);

  void setNull(unsigned int parameterIndex, int sqlType);

  void setResultSetConcurrency(int concurrencyFlag);

  void setString(unsigned int parameterIndex, const sql::SQLString& value);

  void setQueryTimeout(unsigned int seconds);

  sql::PreparedStatement * setResultSetType(sql::ResultSet::enum_type type);

private:
  /* Prevent use of these */
  MySQL_Prepared_Statement(const MySQL_Prepared_Statement &);
  void operator=(MySQL_Prepared_Statement &);
};

} /* namespace mysql */
} /* namespace sql */
#endif // _MYSQL_PREPARED_STATEMENT_H_


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
