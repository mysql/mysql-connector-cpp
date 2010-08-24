/*
  Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

  The MySQL Connector/C++ is licensed under the terms of the GPLv2
  <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
  MySQL Connectors. There are special exceptions to the terms and
  conditions of the GPLv2 as it is applied to this software, see the
  FLOSS License Exception
  <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include <memory>
#include <algorithm>

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
MySQL_Statement::MySQL_Statement(MySQL_Connection * conn, boost::shared_ptr< NativeAPI::NativeConnectionWrapper > & _proxy,
									sql::ResultSet::enum_type rset_type, boost::shared_ptr< MySQL_DebugLogger > & l)
	: warnings(NULL), connection(conn), proxy(_proxy), isClosed(false), warningsHasBeenLoaded(true),
		last_update_count(UL64(~0)), logger(l),	resultset_type(rset_type)
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
void
MySQL_Statement::do_query(const char *q, size_t length)
{
	CPP_ENTER("MySQL_Statement::do_query");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();

	if (proxy->query( ::sql::SQLString(q, length) ) && proxy->errNo()) {
		CPP_ERR_FMT("Error during proxy->query : %d:(%s) %s", proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());
		sql::mysql::util::throwSQLException(*proxy.get());
	}

	warningsHasBeenLoaded= false;
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
	//TODO: again - probably no need to catch-n-throw here. O maybe no need to throw further
	try {
		result= (resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY)? proxy->use_result(): proxy->store_result();
		if (!result) {
			sql::mysql::util::throwSQLException(*proxy.get());
		}
	} catch (::sql::SQLException & e) {
		CPP_ERR_FMT("Error during %s_result : %d:(%s) %s", resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY? "use":"store",
			proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());
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
	do_query(sql.c_str(), static_cast<int>(sql.length()));
	bool ret = proxy->field_count() > 0;
	last_update_count = ret? UL64(~0):proxy->affected_rows();
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
	do_query(sql.c_str(), static_cast<int>(sql.length()));
	sql::ResultSet *tmp =
				new MySQL_ResultSet(
						get_resultset(),
						resultset_type==sql::ResultSet::TYPE_FORWARD_ONLY? resultset_type:sql::ResultSet::TYPE_SCROLL_INSENSITIVE,
						this,
						logger
				);
	CPP_INFO_FMT("rset=%p", tmp);
	return tmp;
}
/* }}} */


/* {{{ MySQL_Statement::executeUpdate() -I- */
int
MySQL_Statement::executeUpdate(const sql::SQLString& sql)
{
	CPP_ENTER("MySQL_Statement::executeUpdate");
	CPP_INFO_FMT("this=%p", this);
	CPP_INFO_FMT("query=%s", sql.c_str());
	checkClosed();
	do_query(sql.c_str(), static_cast<int>(sql.length()));
	if (proxy->field_count()) {
		throw sql::InvalidArgumentException("Statement returning result set");
	}
	return static_cast<int>(last_update_count = proxy->affected_rows());
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

	sql::ResultSet::enum_type tmp_type;

	try {
		NativeAPI::NativeResultsetWrapper * tmp_ptr;
		switch (resultset_type) {
			case sql::ResultSet::TYPE_FORWARD_ONLY:
				if (!(tmp_ptr = proxy->use_result())) {
					sql::mysql::util::throwSQLException(*proxy.get());
				}
				result.reset(tmp_ptr);
				tmp_type = sql::ResultSet::TYPE_FORWARD_ONLY;
				break;
			default:
				if (!(tmp_ptr = proxy->store_result())) {
					sql::mysql::util::throwSQLException(*proxy.get());
				}
				result.reset(tmp_ptr);
				tmp_type = sql::ResultSet::TYPE_SCROLL_INSENSITIVE;
		}
	} catch (::sql::SQLException & e ) {
		if (proxy->errNo() != 0)
		{
			CPP_ERR_FMT("Error during %s_result : %d:(%s) %s", resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY? "use":"store",
				proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());
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

	sql::ResultSet * ret = new MySQL_ResultSet(result, tmp_type, this, logger);

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
MySQL_Statement::setQueryTimeout(unsigned int)
{
	CPP_ENTER("MySQL_Statement::setQueryTimeout");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Statement::setQueryTimeout");
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
	CPP_ENTER("MySQL_Statement::getMaxRows");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	last_update_count = UL64(~0);
	if (proxy->more_results()) {
		int next_result = proxy->next_result();
		if (next_result > 0) {
			CPP_ERR_FMT("Error during getMoreResults : %d:(%s) %s", proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());
			sql::mysql::util::throwSQLException(*proxy.get());
		} else if (next_result == 0) {
			return proxy->field_count() != 0;
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
	throw sql::MethodNotImplementedException("MySQL_Statement::getQueryTimeout");
	return 0; // fool compilers
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

	if (!warningsHasBeenLoaded)
	{
		warnings.reset(loadMysqlWarnings(connection));
		warningsHasBeenLoaded= true;
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
