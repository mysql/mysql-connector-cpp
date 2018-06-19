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



#ifndef _MYSQL_STATEMENT_H_
#define _MYSQL_STATEMENT_H_

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

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
  boost::weak_ptr< NativeAPI::NativeConnectionWrapper > proxy;

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
  MySQL_Statement(MySQL_Connection * conn, boost::shared_ptr< NativeAPI::NativeConnectionWrapper > & _proxy,
          sql::ResultSet::enum_type rset_type, boost::shared_ptr< MySQL_DebugLogger > & l);

  ~MySQL_Statement();

  sql::Connection * getConnection();

  void cancel();

  void clearWarnings();

  void close();

  bool execute(const sql::SQLString& sql);

  sql::ResultSet * executeQuery(const sql::SQLString& sql);

  int executeUpdate(const sql::SQLString& sql);

  size_t getFetchSize();

  unsigned int getMaxFieldSize();

  uint64_t getMaxRows();

  bool getMoreResults();

  unsigned int getQueryTimeout();

  sql::ResultSet * getResultSet();

  sql::ResultSet::enum_type getResultSetType();

  uint64_t getUpdateCount();

  const SQLWarning * getWarnings();/* should return differen type */

  Statement * setBuffered();

  void setCursorName(const sql::SQLString & name);

  void setEscapeProcessing(bool enable);

  void setFetchSize(size_t rows);

  void setMaxFieldSize(unsigned int max);

  void setMaxRows(unsigned int max);

  void setQueryTimeout(unsigned int seconds);

  sql::Statement * setResultSetType(sql::ResultSet::enum_type type);
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
