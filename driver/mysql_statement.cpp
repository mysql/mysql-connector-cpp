/*
   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include <memory>
#include <algorithm>

/*
 * mysql_util.h includes private_iface, ie libmysql headers. and they must go
 * prior to any header including config.h to avoid annoying redefenition warnings
 */
#include "mysql_util.h"

#include <cppconn/exception.h>
#include "mysql_connection.h"
#include "mysql_statement.h"
#include "mysql_resultset.h"
#include "mysql_warning.h"

#include "mysql_debug.h"

namespace sql
{
namespace mysql
{

/* {{{ MySQL_Statement::MySQL_Statement() -I- */
MySQL_Statement::MySQL_Statement(MySQL_Connection * conn, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l)
	: warnings(NULL), connection(conn), isClosed(false), last_update_count(UL64(~0)), logger(l? l->getReference():NULL)
{
	CPP_ENTER("MySQL_Statement::MySQL_Statement");
	CPP_INFO_FMT("this=%p", this);
}
/* }}} */


/* {{{ MySQL_Statement::~MySQL_Statement() -I- */
MySQL_Statement::~MySQL_Statement()
{
	/* Don't remove the block or we can get into problems with logger */
	{
		CPP_ENTER("MySQL_Statement::~MySQL_Statement");
		CPP_INFO_FMT("this=%p", this);
	}
	for (sql::SQLWarning * tmp = warnings, * next_tmp = warnings; tmp; tmp = next_tmp) {
		next_tmp = const_cast<sql::SQLWarning *>(tmp->getNextWarning());
		delete tmp;
	}	
	logger->freeReference();
}
/* }}} */


/* {{{ MySQL_Statement::do_query() -I- */
void
MySQL_Statement::do_query(const char *q, size_t length)
{
	CPP_ENTER("MySQL_Statement::do_query");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	MYSQL * mysql = connection->getMySQLHandle();
	if (mysql_real_query(mysql, q, static_cast<unsigned long>(length)) && mysql_errno(mysql)) {
		CPP_ERR_FMT("Error during mysql_real_query [%d:%s:%s]", mysql_errno(mysql), mysql_sqlstate(mysql), mysql_error(mysql));
		throw sql::SQLException(mysql_error(mysql), mysql_sqlstate(mysql), mysql_errno(mysql));
	}
}
/* }}} */


/* {{{ MySQL_Statement::get_resultset() -I- */
MYSQL_RES_Wrapper *
MySQL_Statement::get_resultset()
{
	CPP_ENTER("MySQL_Statement::get_resultset");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();

	MYSQL * mysql = connection->getMySQLHandle();

	MYSQL_RES * result = mysql_store_result(mysql);
	if (result == NULL) {
		CPP_ERR("Error during store result");
		throw sql::SQLException(mysql_error(mysql), mysql_sqlstate(mysql), mysql_errno(mysql));
	}

	return new MYSQL_RES_Wrapper(result);
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
MySQL_Statement::execute(const std::string& sql)
{
	CPP_ENTER("MySQL_Statement::execute");
	CPP_INFO_FMT("this=%p", this);
	CPP_INFO_FMT("query=%s", sql.c_str());
	checkClosed();
	do_query(sql.c_str(), static_cast<int>(sql.length()));
	bool ret = mysql_field_count(connection->getMySQLHandle()) > 0;
	last_update_count = ret? UL64(~0):mysql_affected_rows(connection->getMySQLHandle());
	return ret;
}
/* }}} */


/* {{{ MySQL_Statement::executeQuery() -I- */
sql::ResultSet *
MySQL_Statement::executeQuery(const std::string& sql)
{
	CPP_ENTER("MySQL_Statement::executeQuery");
	CPP_INFO_FMT("this=%p", this);
	CPP_INFO_FMT("query=%s", sql.c_str());
	checkClosed();
	last_update_count = UL64(~0);
	do_query(sql.c_str(), static_cast<int>(sql.length()));
	sql::ResultSet *tmp = new MySQL_ResultSet(get_resultset(), this, logger);
	CPP_INFO_FMT("rset=%p", tmp);
	return tmp;
}
/* }}} */


/* {{{ MySQL_Statement::executeUpdate() -I- */
int
MySQL_Statement::executeUpdate(const std::string& sql)
{
	CPP_ENTER("MySQL_Statement::executeUpdate");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	do_query(sql.c_str(), static_cast<int>(sql.length()));
	if (mysql_field_count(connection->getMySQLHandle())) {
		throw sql::InvalidArgumentException("Statement returning result set");
	}
	return static_cast<int>(last_update_count = mysql_affected_rows(connection->getMySQLHandle()));
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
unsigned int
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

	MYSQL * mysql = connection->getMySQLHandle();

	MYSQL_RES * result = mysql_store_result(mysql);
	if (!result) {
		return NULL;
	}

	sql::ResultSet * ret = new MySQL_ResultSet(new MYSQL_RES_Wrapper(result), this, logger);
	CPP_INFO_FMT("res=%p", ret);
	return ret;
}
/* }}} */


/* {{{ MySQL_Statement::setFetchSize() -U- */
void
MySQL_Statement::setFetchSize(unsigned int)
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
	for (sql::SQLWarning * tmp = warnings, * next_tmp = warnings; tmp; tmp = next_tmp) {
		next_tmp = const_cast<sql::SQLWarning *>(tmp->getNextWarning());
		delete tmp;
	}
	warnings = NULL;
}
/* }}} */


/* {{{ MySQL_Statement::close() -i- */
void
MySQL_Statement::close()
{
	CPP_ENTER("MySQL_Statement::close");
	CPP_INFO_FMT("this=%p", this);
	checkClosed();
	for (sql::SQLWarning * tmp = warnings, * next_tmp = warnings; tmp; tmp = next_tmp) {
		next_tmp = const_cast<sql::SQLWarning *>(tmp->getNextWarning());
		delete tmp;
	}
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
	MYSQL * conn = connection->getMySQLHandle();
	if (mysql_more_results(conn)) {
		int next_result = mysql_next_result(conn);
		if (next_result > 0) {
			CPP_ERR_FMT("Error during getMoreResults [%d:%s:%s]", mysql_errno(conn), mysql_sqlstate(conn), mysql_error(conn));
			throw sql::SQLException(mysql_error(conn), mysql_sqlstate(conn), mysql_errno(conn));
		} else if (next_result == 0) {
			return mysql_field_count(conn) != 0;
		} else if (next_result == -1) {
			throw sql::SQLException("Impossible! more_results() said true, next_result says no more results", "HY000", 1000);
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

	return warnings;
}
/* }}} */


/* {{{ MySQL_Statement::setCursorName() -U- */
void
MySQL_Statement::setCursorName(const std::string &)
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

}; /* namespace mysql */
}; /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
