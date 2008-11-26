/* Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   There are special exceptions to the terms and conditions of the GPL
   as it is applied to this software. View the full text of the
   exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
   software distribution.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include <memory>
#include <stdio.h>
#include "mysql_private_iface.h"

#ifndef _WIN32
#include <string.h>
#endif

#include "mysql_connection.h"
#include "mysql_connection_data.h"
#include "mysql_prepared_statement.h"
#include "mysql_statement.h"
#include "mysql_metadata.h"
#include "mysql_resultset.h"
#include "mysql_warning.h"

#include "common/ccppTypes.h"

#include "mysql_debug.h"
#include "mysql_util.h"

namespace sql
{
namespace mysql
{

/* {{{ MySQL_Savepoint::MySQL_Savepoint() -I- */
MySQL_Savepoint::MySQL_Savepoint(const std::string &savepoint):
  name(savepoint)
{
}
/* }}} */


/* {{{ MySQL_Savepoint::getSavepointId() -I- */
int
MySQL_Savepoint::getSavepointId()
{
	throw sql::InvalidArgumentException("Only named savepoints are supported.");
}
/* }}} */


/* {{{ MySQL_Savepoint::getSavepointName() -I- */
std::string &
MySQL_Savepoint::getSavepointName()
{
	return name;
}
/* }}} */


/* {{{ MySQL_Connection::MySQL_Connection() -I- */
MySQL_Connection::MySQL_Connection(const std::string& hostName,
								   const std::string& userName,
								   const std::string& password)
{
	intern = new MySQL_ConnectionData();
	intern->is_valid = true;
	bool protocol_tcp = true;
	std::string host;
	std::string socket;
	unsigned int port = 3306;

	intern->logger = new sql::mysql::util::my_shared_ptr< MySQL_DebugLogger >(new MySQL_DebugLogger()); /* should be before CPP_ENTER */
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::MySQL_Connection");

	try {
		if (!(intern->mysql = mysql_init(NULL))) {
			throw sql::SQLException("Insufficient memory: cannot create MySQL handle using mysql_init()", "HY000", 1000);
		}
#ifndef CPPDBC_WIN32
		if (!hostName.compare(0, sizeof("unix://") - 1, "unix://")) {
			protocol_tcp = false;
			host = "localhost";
			socket = hostName.substr(sizeof("unix://") - 1, std::string::npos);
		} else
#endif
		if (!hostName.compare(0, sizeof("tcp://") - 1, "tcp://") ) {
			size_t port_pos;
			host = hostName.substr(sizeof("tcp://") - 1, std::string::npos);
			if (std::string::npos != (port_pos = host.find_last_of(':', std::string::npos))) {
				port = atoi(host.substr(port_pos + 1, std::string::npos).c_str());
				host = host.substr(0, port_pos);
			}
		} else {
			host = hostName.c_str();
		}
		/* libmysql shouldn't think it is too smart */
		if (protocol_tcp && !host.compare(0, sizeof("localhost") - 1, "localhost")) {
			host = "127.0.0.1";
		}
		if (!mysql_real_connect(intern->mysql,
							host.c_str(),
							userName.c_str(),
							password.c_str(),
							NULL /* schema */,
							port,
							protocol_tcp == false? socket.c_str():NULL /*socket*/,
							0)) {
			sql::SQLException e(mysql_error(intern->mysql), mysql_sqlstate(intern->mysql), mysql_errno(intern->mysql));
			mysql_close(intern->mysql);
			intern->mysql = NULL;
			throw e;
		}
		mysql_set_server_option(intern->mysql, MYSQL_OPTION_MULTI_STATEMENTS_OFF);
		setAutoCommit(true);
		setTransactionIsolation(sql::TRANSACTION_REPEATABLE_READ);

		intern->sql_mode = getSessionVariable("SQL_MODE");
	} catch (sql::SQLException &e) {
		intern->logger->freeReference();		
		throw e;
	} catch (std::runtime_error &e) {
		intern->logger->freeReference();		
		throw e;
	} catch (std::bad_alloc &e) {
		intern->logger->freeReference();		
		throw e;
	}
}
/* }}} */


/* {{{ MySQL_Connection::~MySQL_Connection() -I- */
MySQL_Connection::~MySQL_Connection()
{
	/*
	  We need this outter block, because the on-stack object
	  created by CPP_ENTER references `intern->logger`. And if there is no block
	  the on-stack object will be destructed after `delete intern->logger` leading
	  to a faulty memory access.
	*/
	{
		CPP_ENTER_WL(intern->logger, "MySQL_Connection::~MySQL_Connection");
		if (!isClosed()) {
			mysql_close(intern->mysql);
		}
	}
	intern->logger->freeReference();
	delete intern;
}
/* }}} */


/* {{{ MySQL_Connection::clearWarnings() -I- */
void
MySQL_Connection::clearWarnings()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::clearWarnings");
	intern->warnings.reset();
}
/* }}} */


/* {{{ MySQL_Connection::checkClosed() -I- */
void
MySQL_Connection::checkClosed()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::checkClosed");
	if (!intern->is_valid) {
		throw sql::SQLException("Connection has been closed", "HY000", 1000);
	}
}
/* }}} */


/* {{{ MySQL_Connection::close() -I- */
void
MySQL_Connection::close()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::close");
	checkClosed();
	mysql_close(intern->mysql);
	intern->mysql = NULL;
	intern->is_valid = false;
}
/* }}} */


/* {{{ MySQL_Connection::commit() -I- */
void
MySQL_Connection::commit()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::commit");
	checkClosed();
	mysql_commit(intern->mysql);
}
/* }}} */


/* {{{ MySQL_Connection::createStatement() -I- */
sql::Statement * MySQL_Connection::createStatement()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::createStatement");
	checkClosed();
	return new MySQL_Statement(this, intern->logger);
}
/* }}} */


/* {{{ MySQL_Connection::getAutoCommit() -I- */
bool
MySQL_Connection::getAutoCommit()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getAutoCommit");
	return intern->autocommit;
}
/* }}} */


/* {{{ MySQL_Connection::getCatalog() -I- */
std::string
MySQL_Connection::getCatalog()
{
  CPP_ENTER_WL(intern->logger, "MySQL_Connection::getCatalog");
  return std::string("");
}
/* }}} */


/**
  Added for consistency. Not present in jdbc interface. Is still subject for discussion.
*/
/* {{{ MySQL_Connection::getSchema() -I- */
std::string
MySQL_Connection::getSchema()
{
  CPP_ENTER_WL(intern->logger, "MySQL_Connection::getSchema");
  checkClosed();
  std::auto_ptr<sql::Statement> stmt(createStatement());
  std::auto_ptr<ResultSet> rset(stmt->executeQuery("SELECT DATABASE()")); //SELECT SCHEMA()
  rset->next();
  return std::string(rset->getString(1));
}
/* }}} */


/* {{{ MySQL_Connection::getClientInfo() -I- */
const std::string&
MySQL_Connection::getClientInfo(const std::string&)
{
	static const std::string clientInfo("cppconn");
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getClientInfo");
	return clientInfo;
}
/* }}} */


/* {{{ MySQL_Connection::getClientInfo() -U- */
void
MySQL_Connection::getClientOption(const std::string & /* optionName */, void * /* optionValue */)
{
	throw sql::MethodNotImplementedException("MySQL_Connection::getClientOption");
	return;
}
/* }}} */


/* {{{ MySQL_Connection::getMetaData() -I- */
DatabaseMetaData *
MySQL_Connection::getMetaData()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getMetaData");
	checkClosed();
	return new MySQL_ConnectionMetaData(this, this->intern->logger);
}
/* }}} */


/* {{{ MySQL_Connection::getMySQLHandle() -I- */
MYSQL *
MySQL_Connection::getMySQLHandle()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getMySQLHandle");
	checkClosed();
	return intern->mysql;
}
/* }}} */


/* {{{ MySQL_Connection::getTransactionIsolation() -I- */
enum_transaction_isolation
MySQL_Connection::getTransactionIsolation()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getTransactionIsolation");
	return intern->txIsolationLevel;
}
/* }}} */


/* {{{ MySQL_Connection::getWarnings() -I- */
const SQLWarning *
MySQL_Connection::getWarnings()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getWarnings");
	checkClosed();

	intern->warnings.reset(loadMysqlWarnings(this));

	return intern->warnings.get();
}
/* }}} */


/* {{{ MySQL_Connection::isClosed() -I- */
bool
MySQL_Connection::isClosed()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::isClosed");
	return !intern->is_valid;
}
/* }}} */


/* {{{ MySQL_Connection::isReadOnly() -U- */
bool
MySQL_Connection::isReadOnly()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::isReadOnly");
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Connection::isReadOnly");
	return false;
}
/* }}} */


/* {{{ MySQL_Connection::nativeSQL() -I- */
std::string *
MySQL_Connection::nativeSQL(const std::string& sql)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::nativeSQL");
	checkClosed();
	return new std::string(sql.c_str());
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -I- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const std::string& sql)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	CPP_INFO_FMT("query=%s", sql.c_str());
	checkClosed();
	MYSQL_STMT *stmt = mysql_stmt_init(intern->mysql);

	if (!stmt) {
		CPP_ERR("Exception, no statement");
		throw sql::SQLException(mysql_error(intern->mysql), mysql_sqlstate(intern->mysql), mysql_errno(intern->mysql));
	}

	if (mysql_stmt_prepare(stmt, sql.c_str(), sql.length())) {
		CPP_ERR_FMT("Cannot prepare [%d:%s:%s]", mysql_stmt_errno(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_error(stmt));
		sql::SQLException e(mysql_stmt_error(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_errno(stmt));
		mysql_stmt_close(stmt);
		throw e;
	}

	return new MySQL_Prepared_Statement(stmt, this, this->intern->logger);
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -U- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const std::string& /* sql */, int /* autoGeneratedKeys */)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Connection::prepareStatement(const std::string& sql, int autoGeneratedKeys)");
	return NULL;
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -U- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const std::string& /* sql */, int /* columnIndexes */ [])
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Connection::prepareStatement(const std::string& sql, int* columnIndexes)");
	return NULL;
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -U- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const std::string& /* sql */, int /* resultSetType */, int /* resultSetConcurrency */)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Connection::prepareStatement(const std::string& sql, int resultSetType, int resultSetConcurrency)");
	return NULL;
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -U- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const std::string& /* sql */, int /* resultSetType */, int /* resultSetConcurrency */, int /* resultSetHoldability */)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Connection::prepareStatement(const std::string& sql, int resultSetType, int resultSetConcurrency, int resultSetHoldability)");
	return NULL;
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -U- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const std::string& /* sql */, std::string /* columnNames*/ [])
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Connection::prepareStatement(const std::string& sql, std::string columnNames[])");
	return NULL;
}
/* }}} */


/* {{{ MySQL_Connection::releaseSavepoint() -I- */
void
MySQL_Connection::releaseSavepoint(Savepoint * savepoint)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::releaseSavepoint");
	checkClosed();
	if (mysql_get_server_version(intern->mysql) < 50001) {
		throw sql::MethodNotImplementedException("releaseSavepoint not available in this server version");
	}
	if (getAutoCommit()) {
		throw sql::InvalidArgumentException("The connection is in autoCommit mode");
	}
	std::string sql("RELEASE SAVEPOINT ");
	sql.append(savepoint->getSavepointName());

	std::auto_ptr<sql::Statement> stmt(createStatement());
	stmt->execute(sql);
}
/* }}} */


/* {{{ MySQL_Connection::rollback() -I- */
void
MySQL_Connection::rollback()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::rollback");
	checkClosed();
	mysql_rollback(intern->mysql);
}
/* }}} */


/* {{{ MySQL_Connection::rollback() -I- */
void
MySQL_Connection::rollback(Savepoint * savepoint)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::rollback");
	checkClosed();
	if (getAutoCommit()) {
		throw sql::InvalidArgumentException("The connection is in autoCommit mode");
	}
	std::string sql("ROLLBACK TO SAVEPOINT ");
	sql.append(savepoint->getSavepointName());

	std::auto_ptr<sql::Statement> stmt(createStatement());
	stmt->execute(sql);
}
/* }}} */


/* {{{ MySQL_Connection::setCatalog() -I- */
void
MySQL_Connection::setCatalog(const std::string&)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setCatalog");
	checkClosed();
}
/* }}} */


/* {{{ MySQL_Connection::setSchema() -I- (not part of JDBC) */
void
MySQL_Connection::setSchema(const std::string& catalog)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setCatalog");
	checkClosed();
	std::string sql("USE ");
	sql.append(catalog);

	std::auto_ptr<sql::Statement> stmt(createStatement());
	stmt->execute(sql);
}
/* }}} */


/* {{{ MySQL_Connection::setClientOption() -I- */
void
MySQL_Connection::setClientOption(const std::string & optionName, const void * optionValue)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setClientOption");
	if (!optionName.compare("libmysql_debug")) {
		mysql_debug(static_cast<const char *>(optionValue));
	} else if (!optionName.compare("client_trace")) {
		if (*(static_cast<const bool *>(optionValue))) {
			intern->logger->get()->enableTracing();
			CPP_INFO("Tracing enabled");
		} else {
			intern->logger->get()->disableTracing();
			CPP_INFO("Tracing disabled");
		}
	}
}
/* }}} */


/* {{{ MySQL_Connection::setHoldability() -U- */
void
MySQL_Connection::setHoldability(int /* holdability */)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setHoldability");
	throw sql::MethodNotImplementedException("MySQL_Connection::setHoldability()");
}
/* }}} */


/* {{{ MySQL_Connection::setReadOnly() -U- */
void
MySQL_Connection::setReadOnly(bool /* readOnly */)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setReadOnly");
	throw sql::MethodNotImplementedException("MySQL_Connection::setReadOnly()");
}
/* }}} */


/* {{{ MySQL_Connection::setSavepoint() -U- */
Savepoint *
MySQL_Connection::setSavepoint()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setSavepoint");
	checkClosed();
	throw sql::MethodNotImplementedException("Please use MySQL_Connection::setSavepoint(const std::string& name)");
	return NULL;
}
/* }}} */


/* {{{ MySQL_Connection::setSavepoint() -I- */
Savepoint *
MySQL_Connection::setSavepoint(const std::string& name)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setSavepoint");
	checkClosed();
	if (getAutoCommit()) {
		throw sql::InvalidArgumentException("The connection is in autoCommit mode");
	}
	if (!name.length()) {
		throw sql::InvalidArgumentException("Savepoint name cannot be empty string");
	}
	std::string sql("SAVEPOINT ");
	sql.append(name);

	std::auto_ptr<sql::Statement> stmt(createStatement());
	stmt->execute(sql);

	return new MySQL_Savepoint(name);
}
/* }}} */


/* {{{ MySQL_Connection::setAutoCommit() -I- */
void
MySQL_Connection::setAutoCommit(bool autoCommit)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setAutoCommit");
	checkClosed();
	mysql_autocommit(intern->mysql, autoCommit);
	intern->autocommit = autoCommit;
}
/* }}} */


/* {{{ MySQL_Connection::setTransactionIsolation() -I- */
void
MySQL_Connection::setTransactionIsolation(enum_transaction_isolation level)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setTransactionIsolation");
	checkClosed();
	const char * q;
	switch (level) {
		case TRANSACTION_SERIALIZABLE:
			q = "SET SESSION TRANSACTION ISOLATION LEVEL SERIALIZABLE";
			break;
		case TRANSACTION_REPEATABLE_READ:
			q =  "SET SESSION TRANSACTION ISOLATION LEVEL REPEATABLE READ";
			break;
		case TRANSACTION_READ_COMMITTED:
			q = "SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED";
			break;
		case TRANSACTION_READ_UNCOMMITTED:
			q = "SET SESSION TRANSACTION ISOLATION LEVEL READ UNCOMMITTED";
			break;
		default:
			throw sql::InvalidArgumentException("MySQL_Connection::setTransactionIsolation()");
	}
	intern->txIsolationLevel = level;
	mysql_query(intern->mysql, q);
}
/* }}} */


/* {{{ MySQL_Connection::getSessionVariable() -I- */
std::string
MySQL_Connection::getSessionVariable(const std::string & varname)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getSessionVariable");
	checkClosed();

	if (!strncasecmp(varname.c_str(), "sql_mode", sizeof("sql_mode") - 1)) {
		return intern->sql_mode;
	}
	std::auto_ptr<sql::Statement> stmt(createStatement());
	std::string q = std::string("SHOW SESSION VARIABLES LIKE '").append(varname).append("'");

	std::auto_ptr<ResultSet> rset(stmt->executeQuery(q));

	if (rset->next()) {
		return rset->getString(2);
	}
	return NULL;
}
/* }}} */

}; /* namespace intern->mysql */
}; /* namespace sql */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

