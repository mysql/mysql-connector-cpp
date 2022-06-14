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



#include <memory>
#include <algorithm>
#include <sstream>
#include <cstring>


/*
 * mysql_util.h includes private_iface, ie libmysql headers. and they must go
 * prior to any header including config.h to avoid annoying redefenition warnings
 */
#include "mysql_util.h"

#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/warning.h>
#include "mysql_connection.h"
#include "mysql_statement.h"
#include "mysql_resultset.h"
#include "mysql_warning.h"
#include "nativeapi/native_connection_wrapper.h"
#include "nativeapi/native_resultset_wrapper.h"

#include "mysql_debug.h"

namespace sql
{
namespace mysql
{

/* {{{ MySQL_Statement::MySQL_Statement() -I- */
MySQL_Statement::MySQL_Statement(MySQL_Connection * conn, std::shared_ptr< NativeAPI::NativeConnectionWrapper > & _proxy,
                  sql::ResultSet::enum_type rset_type, boost::shared_ptr< MySQL_DebugLogger > & l)
  : warnings(NULL), connection(conn), proxy(_proxy), isClosed(false), warningsHaveBeenLoaded(true),
  last_update_count(UL64(~0)), logger(l), resultset_type(rset_type), warningsCount(0)
{
  CPP_ENTER("MySQL_Statement::MySQL_Statement");
  CPP_INFO_FMT("this=%p", this);
}
/* }}} */


/* {{{ MySQL_Statement::~MySQL_Statement() -I- */
MySQL_Statement::~MySQL_Statement()
{
  CPP_ENTER("MySQL_Statement::~MySQL_Statement");
  CPP_INFO_FMT("this=%p", this);

  warnings.reset();
}
/* }}} */


/* {{{ MySQL_Statement::do_query() -I- */
/* All callers passed c_str from string, and we here created new string to pass to
   proxy->query. It didn't make much sense so changed interface here */
void
MySQL_Statement::do_query(const ::sql::SQLString &q)
{
  CPP_ENTER("MySQL_Statement::do_query");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  if(attrbind.nrAttr() != 0)
  {
    proxy_p->query_attr(attrbind.nrAttr(), attrbind.getNames(), attrbind.getBinds());
  }

  if (proxy_p->query(q) && proxy_p->errNo()) {
    CPP_ERR_FMT("Error during proxy->query : %d:(%s) %s", proxy_p->errNo(), proxy_p->sqlstate().c_str(), proxy_p->error().c_str());
    sql::mysql::util::throwSQLException(*proxy_p.get());
  }

  warningsCount= proxy_p->warning_count();

  warningsHaveBeenLoaded= false;
}
/* }}} */


/* {{{ MySQL_Statement::get_resultset() -I- */
boost::shared_ptr< NativeAPI::NativeResultsetWrapper >
MySQL_Statement::get_resultset()
{
  CPP_ENTER("MySQL_Statement::get_resultset");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();

  NativeAPI::NativeResultsetWrapper * result;

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  //TODO: again - probably no need to catch-n-throw here. Or maybe no need to throw further
  try {
    result= (resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY)
        ? proxy_p->use_result()
        : proxy_p->store_result();
    if (!result) {
      sql::mysql::util::throwSQLException(*proxy_p.get());
    }
  } catch (::sql::SQLException & e) {
    CPP_ERR_FMT("Error during %s_result : %d:(%s) %s", resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY? "use":"store",
      proxy_p->errNo(), proxy_p->sqlstate().c_str(), proxy_p->error().c_str());
    if(e.getErrorCode() == 0 && strlen(e.what()) == 0)
      throw ::sql::SQLException("No result available");
    throw e;
  }

  return boost::shared_ptr< NativeAPI::NativeResultsetWrapper >(result);
}
/* }}} */


/* {{{ MySQL_Statement::cancel() -U- */
void
MySQL_Statement::cancel()
{
  CPP_ENTER("MySQL_Statement::cancel");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  throw sql::MethodNotImplementedException("MySQL_Statement::cancel");
}
/* }}} */


/* {{{ MySQL_Statement::execute() -I- */
bool
MySQL_Statement::execute(const sql::SQLString& sql)
{
  CPP_ENTER("MySQL_Statement::execute");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("query=%s", sql.c_str());
  checkClosed();
  do_query(sql);
  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }
  bool ret = proxy_p->field_count() > 0;
  last_update_count = ret? UL64(~0):proxy_p->affected_rows();
  return ret;
}
/* }}} */


/* {{{ MySQL_Statement::executeQuery() -I- */
sql::ResultSet *
MySQL_Statement::executeQuery(const sql::SQLString& sql)
{
  CPP_ENTER("MySQL_Statement::executeQuery");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("query=%s", sql.c_str());
  checkClosed();
  last_update_count = UL64(~0);
  do_query(sql);
  sql::ResultSet *tmp =
        new MySQL_ResultSet(
            get_resultset(),
            proxy,
            resultset_type==sql::ResultSet::TYPE_FORWARD_ONLY ? resultset_type : sql::ResultSet::TYPE_SCROLL_INSENSITIVE,
            this,
            logger
        );
  CPP_INFO_FMT("rset=%p", tmp);
  return tmp;
}
/* }}} */


/*{{{ sql::mysql::dirty_drop_resultset -I- */
void
dirty_drop_rs(std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy)
{
  boost::scoped_ptr<NativeAPI::NativeResultsetWrapper> result(proxy->store_result());
  // Destructor will do the job on result freeing
}

/* {{{ MySQL_Statement::executeUpdate() -I- */
int
MySQL_Statement::executeUpdate(const sql::SQLString& sql)
{
  CPP_ENTER("MySQL_Statement::executeUpdate");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("query=%s", sql.c_str());
  checkClosed();

  do_query(sql);

  bool got_rs= false;

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  do {
    if (proxy_p->field_count()) {
      /* We can't just throw - we need to walk through rest of results */
      got_rs= true;
      dirty_drop_rs(proxy_p);
    } else {
      /* We return update count for last query */
      last_update_count= proxy_p->affected_rows();
    }

    if (!proxy_p->more_results()) {
      if (got_rs){
        throw sql::InvalidArgumentException("Statement returning result set");
      } else {
        return static_cast<int>(last_update_count);
      }
    }

    switch (proxy_p->next_result()) {
    case 0:
      // There is next result and we go on next cycle iteration to process it
      break;
    case -1:
      throw sql::SQLException("Impossible! more_results() said true, next_result says no more results");
    default/* > 0 */:
      CPP_ERR_FMT("Error during executeUpdate : %d:(%s) %s", proxy_p->errNo(), proxy_p->sqlstate().c_str(), proxy_p->error().c_str());
      sql::mysql::util::throwSQLException(*proxy_p.get());
    }
  } while (1);

  /* Should not actually get here*/
  return 0;
}
/* }}} */


/* {{{ MySQL_Statement::getConnection() -I- */
sql::Connection *
MySQL_Statement::getConnection()
{
  CPP_ENTER("MySQL_Statement::getConnection");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  return connection;
}
/* }}} */


/* {{{ MySQL_Statement::getFetchSize() -U- */
size_t
MySQL_Statement::getFetchSize()
{
  CPP_ENTER("MySQL_Statement::getFetchSize");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  throw sql::MethodNotImplementedException("MySQL_Statement::getFetchSize");
  return 0;
}
/* }}} */


/* {{{ MySQL_Statement::getResultSet() -I- */
sql::ResultSet *
MySQL_Statement::getResultSet()
{
  CPP_ENTER("MySQL_Statement::getResultSet");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();

  last_update_count = UL64(~0);

  boost::shared_ptr< NativeAPI::NativeResultsetWrapper > result;

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();

  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  sql::ResultSet::enum_type tmp_type;

  try {
    NativeAPI::NativeResultsetWrapper * tmp_ptr;
    switch (resultset_type) {
      case sql::ResultSet::TYPE_FORWARD_ONLY:
        if (!(tmp_ptr = proxy_p->use_result())) {
          sql::mysql::util::throwSQLException(*proxy_p.get());
        }
        result.reset(tmp_ptr);
        tmp_type = sql::ResultSet::TYPE_FORWARD_ONLY;
        break;
      default:
        if (!(tmp_ptr = proxy_p->store_result())) {
          sql::mysql::util::throwSQLException(*proxy_p.get());
        }
        result.reset(tmp_ptr);
        tmp_type = sql::ResultSet::TYPE_SCROLL_INSENSITIVE;
    }
  } catch (::sql::SQLException & e ) {
    if (proxy_p->errNo() != 0)
    {
      CPP_ERR_FMT("Error during %s_result : %d:(%s) %s", resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY? "use":"store",
        proxy_p->errNo(), proxy_p->sqlstate().c_str(), proxy_p->error().c_str());
      throw e;
    }
    else
    {
      return NULL;
    }
  }

  if (!result) {
    /* if there was an update then this method should return NULL and not throw */
    return NULL;
  }

  sql::ResultSet * ret = new MySQL_ResultSet(result, proxy, tmp_type, this, logger);

  CPP_INFO_FMT("res=%p", ret);
  return ret;
}
/* }}} */


/* {{{ MySQL_Statement::setFetchSize() -U- */
void
MySQL_Statement::setFetchSize(size_t /* fetch */)
{
  CPP_ENTER("MySQL_Statement::setFetchSize");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  throw sql::MethodNotImplementedException("MySQL_Statement::setFetchSize");
}
/* }}} */


/* {{{ MySQL_Statement::setQueryTimeout() -U- */
void
MySQL_Statement::setQueryTimeout(unsigned int timeout)
{
  CPP_ENTER("MySQL_Statement::setQueryTimeout");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  connection->setSessionVariable("max_execution_time", timeout*1000);
}
/* }}} */


/* {{{ MySQL_Statement::clearWarnings() -I- */
void
MySQL_Statement::clearWarnings()
{
  CPP_ENTER("MySQL_Statement::clearWarnings");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  warnings.reset();
}
/* }}} */


/* {{{ MySQL_Statement::close() -i- */
void
MySQL_Statement::close()
{
  CPP_ENTER("MySQL_Statement::close");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  clearWarnings();
  isClosed = true;
}
/* }}} */


/* {{{ MySQL_Statement::getMaxFieldSize() -U- */
unsigned int
MySQL_Statement::getMaxFieldSize()
{
  CPP_ENTER("MySQL_Statement::getMaxFieldSize");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  throw sql::MethodNotImplementedException("MySQL_Statement::getMaxFieldSize");
  return 0;
}
/* }}} */


/* {{{ MySQL_Statement::getMaxRows() -U- */
uint64_t
MySQL_Statement::getMaxRows()
{
  CPP_ENTER("MySQL_Statement::getMaxRows");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  throw sql::MethodNotImplementedException("MySQL_Statement::getMaxRows");
  return 0;
}
/* }}} */


/* {{{ MySQL_Statement::getMoreResults() -I- */
bool
MySQL_Statement::getMoreResults()
{
  CPP_ENTER("MySQL_Statement::getMoreResults");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();
  last_update_count = UL64(~0);
  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();

  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  if (proxy_p->more_results()) {

    int next_result = proxy_p->next_result();

    if (next_result > 0) {
      CPP_ERR_FMT("Error during getMoreResults : %d:(%s) %s", proxy_p->errNo(), proxy_p->sqlstate().c_str(), proxy_p->error().c_str());
      sql::mysql::util::throwSQLException(*proxy_p.get());
    } else if (next_result == 0) {
      bool ret = proxy_p->field_count() > 0;
      last_update_count = ret ? UL64(~0) : proxy_p->affected_rows();
      return  ret;
    } else if (next_result == -1) {
      throw sql::SQLException("Impossible! more_results() said true, next_result says no more results");
    }
  }
  return false;
}
/* }}} */


/* {{{ MySQL_Statement::getQueryTimeout() -U- */
unsigned int
MySQL_Statement::getQueryTimeout()
{
  checkClosed();
  sql::SQLString value= connection->getSessionVariable("max_execution_time");
  if (value.length() > 0) {
    unsigned int timeout;
    std::istringstream buffer(value);
    buffer >> timeout;
    timeout/=1000;
    if (buffer.rdstate() & std::istringstream::failbit) {
      return 0;
    } else {
      return timeout;
    }
  } else {
    return 0;
  }
}
/* }}} */


/* {{{ MySQL_Statement::getResultSetType() -I- */
sql::ResultSet::enum_type
MySQL_Statement::getResultSetType()
{
  CPP_ENTER("MySQL_Statement::getResultSetType");
  checkClosed();
  return resultset_type;
}
/* }}} */


/* {{{ MySQL_Statement::getUpdateCount() -I- */
uint64_t
MySQL_Statement::getUpdateCount()
{
  CPP_ENTER("MySQL_Statement::getUpdateCount");
  checkClosed();
  if (last_update_count == UL64(~0)) {
    return UL64(~0);
  }
  uint64_t ret = last_update_count;
  last_update_count = UL64(~0); /* the value will be returned once per result set */
  return ret;
}
/* }}} */


/* {{{ MySQL_Statement::getWarnings() -I- */
const SQLWarning *
MySQL_Statement::getWarnings()
{
  CPP_ENTER("MySQL_Statement::getWarnings");
  CPP_INFO_FMT("this=%p", this);
  checkClosed();

  if (!warningsHaveBeenLoaded)
  {
    warnings.reset(loadMysqlWarnings(connection, warningsCount));
    warningsHaveBeenLoaded= true;
  }

  return warnings.get();
}
/* }}} */


/* {{{ MySQL_Statement::setCursorName() -U- */
void
MySQL_Statement::setCursorName(const sql::SQLString &)
{
  checkClosed();
  throw sql::MethodNotImplementedException("MySQL_Statement::setCursorName");
}
/* }}} */


/* {{{ MySQL_Statement::setEscapeProcessing() -U- */
void
MySQL_Statement::setEscapeProcessing(bool)
{
  checkClosed();
  throw sql::MethodNotImplementedException("MySQL_Statement::setEscapeProcessing");
}
/* }}} */


/* {{{ MySQL_Statement::setMaxFieldSize() -U- */
void
MySQL_Statement::setMaxFieldSize(unsigned int)
{
  checkClosed();
  throw sql::MethodNotImplementedException("MySQL_Statement::setMaxFieldSize");
}
/* }}} */


/* {{{ MySQL_Statement::setMaxRows() -U- */
void
MySQL_Statement::setMaxRows(unsigned int)
{
  checkClosed();
  throw sql::MethodNotImplementedException("MySQL_Statement::setMaxRows");
}
/* }}} */


/* {{{ MySQL_Statement::setResultSetType() -I- */
sql::Statement *
MySQL_Statement::setResultSetType(sql::ResultSet::enum_type type)
{
  checkClosed();
  resultset_type = type;
  return this;
}
/* }}} */


/* {{{ MySQL_Statement::setQueryAttrBigInt() -U- */
int
MySQL_Statement::setQueryAttrBigInt(const sql::SQLString &name, const sql::SQLString& value)
{
  CPP_ENTER("MySQL_Statement::setQueryAttrBigInt");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%s", name.c_str(), value.c_str());

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  if(!proxy_p->has_query_attributes())
    return 0;

  return attrbind.setQueryAttrBigInt(name, value);
}
/* }}} */


/* {{{ MySQL_Statement::setQueryAttrBoolean() -U- */
int
MySQL_Statement::setQueryAttrBoolean(const sql::SQLString &name, bool value)
{
  CPP_ENTER("MySQL_Statement::setQueryAttrBoolean");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%s", name.c_str(), value ? "true" : "false");

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  if(!proxy_p->has_query_attributes())
    return 0;

  return attrbind.setQueryAttrBoolean(name, value);
}
/* }}} */


/* {{{ MySQL_Statement::setQueryAttrDateTime() -U- */
int
MySQL_Statement::setQueryAttrDateTime(const sql::SQLString &name, const sql::SQLString& value)
{
  CPP_ENTER("MySQL_Statement::setQueryAttrDateTime");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%s", name.c_str(), value.c_str());

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  if(!proxy_p->has_query_attributes())
    return 0;

  return attrbind.setQueryAttrDateTime(name, value);
}
/* }}} */


/* {{{ MySQL_Statement::setQueryAttrDouble() -U- */
int
MySQL_Statement::setQueryAttrDouble(const sql::SQLString &name, double value)
{
  CPP_ENTER("MySQL_Statement::setQueryAttrDouble");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%f", name.c_str(), value);

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  if(!proxy_p->has_query_attributes())
    return 0;

  return attrbind.setQueryAttrDouble(name, value);
}
/* }}} */


/* {{{ MySQL_Statement::setQueryAttrInt() -U- */
int
MySQL_Statement::setQueryAttrInt(const sql::SQLString &name, int32_t value)
{
  CPP_ENTER("MySQL_Statement::setQueryAttrInt");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%d", name.c_str(), value);

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  if(!proxy_p->has_query_attributes())
    return 0;

  return attrbind.setQueryAttrInt(name, value);
}
/* }}} */


/* {{{ MySQL_Statement::setQueryAttrUInt() -U- */
int
MySQL_Statement::setQueryAttrUInt(const sql::SQLString &name, uint32_t value)
{
  CPP_ENTER("MySQL_Statement::setQueryAttrUInt");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%u", name.c_str(), value);

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  if(!proxy_p->has_query_attributes())
    return 0;

  return attrbind.setQueryAttrUInt(name, value);
}
/* }}} */


/* {{{ MySQL_Statement::setQueryAttrInt64() -U- */
int
MySQL_Statement::setQueryAttrInt64(const sql::SQLString &name, int64_t value)
{
  CPP_ENTER("MySQL_Statement::setQueryAttrInt64");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%ld", name.c_str(), value);

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  if(!proxy_p->has_query_attributes())
    return 0;

  return attrbind.setQueryAttrInt64(name, value);
}
/* }}} */


/* {{{ MySQL_Statement::setQueryAttrUInt64() -U- */
int
MySQL_Statement::setQueryAttrUInt64(const sql::SQLString &name, uint64_t value)
{
  CPP_ENTER("MySQL_Statement::setQueryAttrInt");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%ul", name.c_str(), value);

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  if(!proxy_p->has_query_attributes())
    return 0;

  return attrbind.setQueryAttrUInt64(name, value);
}
/* }}} */


/* {{{ MySQL_Statement::setQueryAttrNull() -U- */
int
MySQL_Statement::setQueryAttrNull(const sql::SQLString &name)
{
  CPP_ENTER("MySQL_Statement::setQueryAttrNull");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s", name.c_str());

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  if(!proxy_p->has_query_attributes())
    return 0;

  return attrbind.setQueryAttrNull(name);
}
/* }}} */


/* {{{ MySQL_Statement::setQueryAttrString() -U- */
int
MySQL_Statement::setQueryAttrString(const sql::SQLString &name, const sql::SQLString& value)
{
  CPP_ENTER("MySQL_Statement::setQueryAttrString");
  CPP_INFO_FMT("this=%p", this);
  CPP_INFO_FMT("name=%s value=%s", name.c_str(), value.c_str());

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy_p = proxy.lock();
  if (!proxy_p) {
    throw sql::InvalidInstanceException("Connection has been closed");
  }

  if(!proxy_p->has_query_attributes())
    return 0;

  return attrbind.setQueryAttrString(name, value);
}
/* }}} */


/* {{{ MySQL_Statement::clearAttributes() -U- */
void
MySQL_Statement::clearAttributes()
{
  attrbind.clearAttributes();
}
/* }}} */


/* {{{ MySQL_Statement::checkClosed() -I- */
void
MySQL_Statement::checkClosed()
{
  CPP_ENTER("MySQL_Statement::checkClosed");
  if (isClosed) {
    CPP_ERR("Statement has been closed");
    throw sql::InvalidInstanceException("Statement has been closed");
  }
}
/* }}} */

} /* namespace mysql */
} /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
