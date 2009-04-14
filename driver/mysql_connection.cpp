/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include <stdlib.h>
#include <memory>
#include <sstream>
#include <stdio.h>
#include <cppconn/exception.h>

#include "mysql_util.h"

#if defined(_WIN32) || defined(_WIN64)
/* MySQL 5.1 might have defined it before in include/config-win.h */
#ifdef strncasecmp
#undef strncasecmp
#endif

#define strncasecmp(s1,s2,n) _strnicmp(s1,s2,n)

#else
#include <string.h>
#endif

#include "mysql_connection.h"
#include "mysql_connection_data.h"
#include "mysql_prepared_statement.h"
#include "mysql_statement.h"
#include "mysql_metadata.h"
#include "mysql_resultset.h"
#include "mysql_warning.h"
#include "mysql_debug.h"


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
	return 0; // fool compilers
}
/* }}} */


/* {{{ MySQL_Savepoint::getSavepointName() -I- */
std::string
MySQL_Savepoint::getSavepointName()
{
	return name;
}
/* }}} */


/* {{{ MySQL_Connection::MySQL_Connection() -I- */
MySQL_Connection::MySQL_Connection(const std::string& hostName, const std::string& userName, const std::string& password)
	: intern(NULL)
{
	std::map<std::string, sql::ConnectPropertyVal> connection_properties;
	{
		sql::ConnectPropertyVal tmp;
		tmp.str.val = hostName.c_str();
		tmp.str.len = hostName.length();
		connection_properties[std::string("hostName")] = tmp;
	}
	{
		sql::ConnectPropertyVal tmp;
		tmp.str.val = userName.c_str();
		tmp.str.len = userName.length();
		connection_properties[std::string("userName")] = tmp;
	}
	{
		sql::ConnectPropertyVal tmp;
		tmp.str.val = password.c_str();
		tmp.str.len = password.length();
		connection_properties[std::string("password")] = tmp;
	}

	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * tmp_logger = new sql::mysql::util::my_shared_ptr< MySQL_DebugLogger >(new MySQL_DebugLogger());

	std::auto_ptr< MySQL_ConnectionData > tmp_intern(new MySQL_ConnectionData(tmp_logger));
	intern = tmp_intern.get();
	
	init(connection_properties);
	// No exception so far, thus intern can still point to the MySQL_ConnectionData object
	// and in the dtor we will clean it up
	tmp_intern.release();
}
/* }}} */


/* {{{ MySQL_Connection::MySQL_Connection() -I- */
MySQL_Connection::MySQL_Connection(std::map< std::string, sql::ConnectPropertyVal > & properties)
	:intern(NULL)
{
	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * tmp_logger = new sql::mysql::util::my_shared_ptr< MySQL_DebugLogger >(new MySQL_DebugLogger());

	std::auto_ptr< MySQL_ConnectionData > tmp_intern(new MySQL_ConnectionData(tmp_logger));
	intern = tmp_intern.get();

	init(properties);
	// No exception so far, thus intern can still point to the MySQL_ConnectionData object
	// and in the dtor we will clean it up
	tmp_intern.release();
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
	delete intern;
}
/* }}} */

/*
  We support :
  - hostName
  - userName
  - password
  - port
  - socket
  - pipe
  - characterSetResults
  - schema
  - sslKey
  - sslCert
  - sslCA
  - sslCAPath
  - sslCipher
  - defaultStatementResultType
  - defaultPreparedStatementResultType
  - CLIENT_COMPRESS
  - CLIENT_FOUND_ROWS
  - CLIENT_IGNORE_SIGPIPE
  - CLIENT_IGNORE_SPACE
  - CLIENT_INTERACTIVE
  - CLIENT_LOCAL_FILES
  - CLIENT_MULTI_RESULTS
  - CLIENT_MULTI_STATEMENTS
  - CLIENT_NO_SCHEMA
  - CLIENT_COMPRESS
  - OPT_CONNECT_TIMEOUT
  - OPT_NAMED_PIPE
  - OPT_READ_TIMEOUT
  - OPT_WRITE_TIMEOUT
  - OPT_RECONNECT
  - OPT_CHARSET_NAME
  - OPT_REPORT_DATA_TRUNCATION
*/


/* {{{ MySQL_Connection::init() -I- */
void MySQL_Connection::init(std::map<std::string, sql::ConnectPropertyVal> & properties)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::init");

	intern->is_valid = true;
	bool protocol_tcp = true;
	std::string host;
	std::string socket_or_pipe;
	unsigned int port = 3306;

	std::string hostName;
	std::string userName;
	std::string password;
	std::string schema;
	std::string defaultCharset("utf8");
	std::string characterSetResults("utf8");

	const char *sslKey = NULL, *sslCert = NULL, *sslCA = NULL, *sslCAPath = NULL, *sslCipher = NULL;
	bool ssl_used = false, schema_used = false;
	int flags = CLIENT_MULTI_RESULTS;

	std::map<std::string, sql::ConnectPropertyVal>::const_iterator it = properties.begin();
	for (; it != properties.end(); ++it) {
		if (!it->first.compare("hostName")) {
			hostName = it->second.str.val;
		} else if (!it->first.compare("userName")) {
			userName = it->second.str.val;
		} else if (!it->first.compare("password")) {
			password = it->second.str.val;
		} else if (!it->first.compare("port")) {
			port = static_cast<unsigned int>(it->second.lval);
		} else if (!it->first.compare("socket")) {
			socket_or_pipe = it->second.str.val;
			protocol_tcp = false;
		} else if (!it->first.compare("pipe")) {
			socket_or_pipe = it->second.str.val;
			protocol_tcp = false;
		} else if (!it->first.compare("schema")) {
			schema = std::string(it->second.str.val);
			schema_used = true;
		} else if (!it->first.compare("characterSetResults")) {
			characterSetResults = std::string(it->second.str.val);
		} else if (!it->first.compare("sslKey")) {
			sslKey = it->second.str.val;
			ssl_used = true;
		} else if (!it->first.compare("sslCert")) {
			sslCert = it->second.str.val;
			ssl_used = true;
		} else if (!it->first.compare("sslCA")) {
			sslCA = it->second.str.val;
			ssl_used = true;
		} else if (!it->first.compare("sslCAPath")) {
			sslCAPath = it->second.str.val;
			ssl_used = true;
		} else if (!it->first.compare("sslCipher")) {
			sslCipher = it->second.str.val;
			ssl_used = true;
		} else if (!it->first.compare("defaultStatementResultType")) {
			do {
				if (static_cast< int >(sql::ResultSet::TYPE_FORWARD_ONLY) == it->second.lval) break;
				if (static_cast< int >(sql::ResultSet::TYPE_SCROLL_INSENSITIVE) == it->second.lval) break;
				if (static_cast< int >(sql::ResultSet::TYPE_SCROLL_SENSITIVE) == it->second.lval) {
					std::ostringstream msg;
					msg << "Invalid value " << it->second.lval <<
						" for option defaultStatementResultType. TYPE_SCROLL_SENSITIVE is not supported";
					throw sql::InvalidArgumentException(msg.str());			
				}
				std::ostringstream msg;
				msg << "Invalid value (" << it->second.lval << " for option defaultStatementResultType";
				throw sql::InvalidArgumentException(msg.str());
			} while (0);
			intern->defaultStatementResultType = static_cast< sql::ResultSet::enum_type >(it->second.lval);
		/* The connector is not ready for unbuffered as we need to refetch */
		} else if (!it->first.compare("defaultPreparedStatementResultType")) {
#if WE_SUPPORT_USE_RESULT_WITH_PS
			do {
				if (static_cast< int >(sql::ResultSet::TYPE_FORWARD_ONLY) == it->second.lval) break;
				if (static_cast< int >(sql::ResultSet::TYPE_SCROLL_INSENSITIVE) == it->second.lval) break;
				if (static_cast< int >(sql::ResultSet::TYPE_SCROLL_SENSITIVE) == it->second.lval) {
					std::ostringstream msg;
					msg << "Invalid value " << it->second.lval <<
						" for option defaultPreparedStatementResultType. TYPE_SCROLL_SENSITIVE is not supported";
					throw sql::InvalidArgumentException(msg.str());			
				}
				std::ostringstream msg;
				msg << "Invalid value (" << it->second.lval << " for option defaultPreparedStatementResultType";
				throw sql::InvalidArgumentException(msg.str());
			} while (0);
			intern->defaultPreparedStatementResultType = static_cast< sql::ResultSet::enum_type >(it->second.lval);
#else
			throw SQLException("defaultPreparedStatementResultType parameter still not implemented");	

#endif
		} else if (!it->first.compare("metadataUseInfoSchema")) {
			intern->metadata_use_info_schema = it->second.bval;
		} else if (!it->first.compare("CLIENT_COMPRESS")) {
			if (it->second.bval && (flags & CLIENT_COMPRESS)) {
				flags |= CLIENT_COMPRESS;
			}
		} else if (!it->first.compare("CLIENT_FOUND_ROWS")) {
			if (it->second.bval && (flags & CLIENT_FOUND_ROWS)) {
				flags |= CLIENT_FOUND_ROWS;
			}
		} else if (!it->first.compare("CLIENT_IGNORE_SIGPIPE")) {
			if (it->second.bval && (flags & CLIENT_IGNORE_SIGPIPE)) {
				flags |= CLIENT_IGNORE_SIGPIPE;
			}
		} else if (!it->first.compare("CLIENT_IGNORE_SPACE")) {
			if (it->second.bval && (flags & CLIENT_IGNORE_SPACE)) {
				flags |= CLIENT_IGNORE_SPACE;
			}
		} else if (!it->first.compare("CLIENT_INTERACTIVE")) {
			if (it->second.bval && (flags & CLIENT_INTERACTIVE)) {
				flags |= CLIENT_INTERACTIVE;
			}
		} else if (!it->first.compare("CLIENT_LOCAL_FILES")) {
			if (it->second.bval && (flags & CLIENT_LOCAL_FILES)) {
				flags |= CLIENT_LOCAL_FILES;
			}
		} else if (!it->first.compare("CLIENT_MULTI_STATEMENTS")) {
			if (it->second.bval && (flags & CLIENT_MULTI_STATEMENTS)) {
				flags |= CLIENT_MULTI_STATEMENTS;
			}
		} else if (!it->first.compare("CLIENT_NO_SCHEMA")) {
			if (it->second.bval && (flags & CLIENT_NO_SCHEMA)) {
				flags |= CLIENT_NO_SCHEMA;
			}
		}
	}
	if (!(intern->mysql = mysql_init(NULL))) {
		throw sql::SQLException("Insufficient memory: cannot create MySQL handle using mysql_init()");
	}
#if !defined(_WIN32) && !defined(_WIN64)
	if (!hostName.compare(0, sizeof("unix://") - 1, "unix://")) {
		protocol_tcp = false;
		socket_or_pipe = hostName.substr(sizeof("unix://") - 1, std::string::npos);
		host = "localhost";
		int tmp_protocol = MYSQL_PROTOCOL_SOCKET;
		mysql_options(intern->mysql, MYSQL_OPT_PROTOCOL, (const char *) &tmp_protocol);
	} else
#else
	if (!hostName.compare(0, sizeof("pipe://") - 1, "pipe://")) {
		protocol_tcp = false;
		socket_or_pipe = hostName.substr(sizeof("pipe://") - 1, std::string::npos);
		host = ".";
		int tmp_protocol = MYSQL_PROTOCOL_PIPE;
		mysql_options(intern->mysql, MYSQL_OPT_PROTOCOL, (const char *) &tmp_protocol);
	} else
#endif
	if (!hostName.compare(0, sizeof("tcp://") - 1, "tcp://") ) {
		size_t port_pos, schema_pos;
		host = hostName.substr(sizeof("tcp://") - 1, std::string::npos);
		schema_pos = host.find('/');
		if (schema_pos != std::string::npos) {
			++schema_pos; // skip the slash
							/* TODO: tcp://127.0.0.1/
							-> host set, schema empty, schema property ignored */
			schema = host.substr(schema_pos, host.size() - schema_pos);
			schema_used = true;
			host = host.substr(0, schema_pos - 1);
		}
		port_pos = host.find_last_of(':', std::string::npos);
		if (port_pos != std::string::npos) {
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
	std::map<std::string, sql::ConnectPropertyVal>::const_iterator it_tmp = properties.begin();
	for (; it_tmp != properties.end(); ++it_tmp) {
		if (!it_tmp->first.compare("OPT_CONNECT_TIMEOUT")) {
			mysql_options(intern->mysql, MYSQL_OPT_CONNECT_TIMEOUT, (const char *) &it_tmp->second.lval);
		} else if (!it_tmp->first.compare("OPT_READ_TIMEOUT")) {
			mysql_options(intern->mysql, MYSQL_OPT_READ_TIMEOUT, (const char *) &it_tmp->second.lval);
		} else if (!it_tmp->first.compare("OPT_WRITE_TIMEOUT")) {
			mysql_options(intern->mysql, MYSQL_OPT_WRITE_TIMEOUT, (const char *) &it_tmp->second.lval);
		} else if (!it_tmp->first.compare("OPT_RECONNECT")) {
			mysql_options(intern->mysql, MYSQL_OPT_RECONNECT, (const char *) &it_tmp->second.bval);
		} else if (!it_tmp->first.compare("OPT_CHARSET_NAME")) {
			defaultCharset = it_tmp->second.str.val;
		} else if (!it_tmp->first.compare("OPT_REPORT_DATA_TRUNCATION")) {
			mysql_options(intern->mysql, MYSQL_REPORT_DATA_TRUNCATION, (const char *) &it_tmp->second.bval);
#if defined(_WIN32) || defined(_WIN64)
		} else if (!it_tmp->first.compare("OPT_NAMED_PIPE")) {
			mysql_options(intern->mysql, MYSQL_OPT_NAMED_PIPE, NULL);
#endif
		}
	}
	{
		my_bool tmp_bool = 1;
		mysql_options(intern->mysql, MYSQL_SECURE_AUTH, (const char *) &tmp_bool);
	}
	{
		mysql_options(intern->mysql, MYSQL_SET_CHARSET_NAME, defaultCharset.c_str());
	}
	if (ssl_used) {
		/* According to the docs, always returns 0 */
		mysql_ssl_set(intern->mysql, sslKey, sslCert, sslCA, sslCAPath, sslCipher);
	}
	CPP_INFO_FMT("hostName=%s", hostName.c_str());
	CPP_INFO_FMT("user=%s", userName.c_str());
	CPP_INFO_FMT("port=%d", port);
	CPP_INFO_FMT("schema=%s", schema.c_str());
	CPP_INFO_FMT("socket/pipe=%s", socket_or_pipe.c_str());
	if (!mysql_real_connect(intern->mysql,
						host.c_str(),
						userName.c_str(),
						password.c_str(),
						schema_used && schema.size()? schema.c_str():NULL /* schema */,
						port,
						protocol_tcp == false? socket_or_pipe.c_str():NULL /*socket or named pipe */,
						flags)) {
		CPP_ERR_FMT("Couldn't connect : %d:(%s) %s", mysql_errno(intern->mysql), mysql_sqlstate(intern->mysql), mysql_error(intern->mysql));
		sql::SQLException e(mysql_error(intern->mysql), mysql_sqlstate(intern->mysql), mysql_errno(intern->mysql));
		mysql_close(intern->mysql);
		intern->mysql = NULL;
		throw e;
	}
	mysql_set_server_option(intern->mysql, MYSQL_OPTION_MULTI_STATEMENTS_ON);
	setAutoCommit(true);
	setTransactionIsolation(sql::TRANSACTION_REPEATABLE_READ);
	// Different Values means we have to set different result set encoding
	if (characterSetResults.compare(defaultCharset)) {
		setSessionVariable("character_set_results", characterSetResults.length() ? characterSetResults:std::string("NULL"));
	}
	intern->meta.reset(new MySQL_ConnectionMetaData(this, intern->logger));
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
		throw sql::SQLException("Connection has been closed");
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
	return new MySQL_Statement(this, intern->defaultStatementResultType, intern->logger);
}
/* }}} */


/* {{{ MySQL_Connection::getAutoCommit() -I- */
bool
MySQL_Connection::getAutoCommit()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getAutoCommit");
	checkClosed();
	return intern->autocommit;
}
/* }}} */


/* {{{ MySQL_Connection::getCatalog() -I- */
std::string
MySQL_Connection::getCatalog()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getCatalog");
	checkClosed();
	return mysql_get_server_version(intern->mysql) > 60006 ? std::string("def") : std::string("");
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
	std::auto_ptr< sql::Statement > stmt(createStatement());
	std::auto_ptr< sql::ResultSet > rset(stmt->executeQuery("SELECT DATABASE()")); //SELECT SCHEMA()
	rset->next();
	return rset->getString(1);
}
/* }}} */


/* {{{ MySQL_Connection::getClientInfo() -I- */
std::string
MySQL_Connection::getClientInfo()
{
	static const std::string clientInfo("cppconn");
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getClientInfo");
	return clientInfo;
}
/* }}} */


/* {{{ MySQL_Connection::getClientOption() -I- */
void
MySQL_Connection::getClientOption(const std::string & optionName, void * optionValue)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getClientOption");
	if (!optionName.compare("metadataUseInfoSchema")) {
		*(static_cast<bool *>(optionValue)) = intern->metadata_use_info_schema;
	} else if (!optionName.compare("defaultStatementResultType")) {
		*(static_cast<int *>(optionValue)) = intern->defaultStatementResultType;	
	} else if (!optionName.compare("defaultPreparedStatementResultType")) {
		*(static_cast<int *>(optionValue)) = intern->defaultPreparedStatementResultType;	
	} else if (!optionName.compare("characterSetResults")) {
		*(static_cast<std::string **>(optionValue)) = new std::string(getSessionVariable("characterSetResults"));
	}
}
/* }}} */


/* {{{ MySQL_Connection::getMetaData() -I- */
DatabaseMetaData *
MySQL_Connection::getMetaData()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getMetaData");
	checkClosed();
	return intern->meta.get();
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
	checkClosed();
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


/* {{{ MySQL_Connection::nativeSQL() -I- */
std::string
MySQL_Connection::nativeSQL(const std::string& sql)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::nativeSQL");
	checkClosed();
	return std::string(sql.c_str());
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -I- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const std::string& sql)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	CPP_INFO_FMT("query=%s", sql.c_str());
	checkClosed();
	MYSQL_STMT * stmt = mysql_stmt_init(intern->mysql);

	if (!stmt) {
		CPP_ERR_FMT("No statement : %d:(%s) %s", mysql_errno(intern->mysql), mysql_sqlstate(intern->mysql), mysql_error(intern->mysql));
		sql::mysql::util::throwSQLException(intern->mysql);
	}

	if (mysql_stmt_prepare(stmt, sql.c_str(), static_cast<unsigned long>(sql.length()))) {
		CPP_ERR_FMT("Cannot prepare %d:(%s) %s", mysql_stmt_errno(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_error(stmt));
		sql::SQLException e(mysql_stmt_error(stmt), mysql_stmt_sqlstate(stmt), mysql_stmt_errno(stmt));
		mysql_stmt_close(stmt);
		throw e;
	}

	return new MySQL_Prepared_Statement(stmt, this, intern->defaultPreparedStatementResultType, intern->logger);
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

	std::auto_ptr< sql::Statement > stmt(createStatement());
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

	std::auto_ptr< sql::Statement > stmt(createStatement());
	stmt->execute(sql);
}
/* }}} */


/* {{{ MySQL_Connection::setClientOption() -I- */
sql::Connection *
MySQL_Connection::setClientOption(const std::string & optionName, const void * optionValue)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setClientOption");
	if (!optionName.compare("libmysql_debug")) {
		mysql_debug(static_cast<const char *>(optionValue));
	} else if (!optionName.compare("clientTrace")) {
		if (*(static_cast<const bool *>(optionValue))) {
			intern->logger->get()->enableTracing();
			CPP_INFO("Tracing enabled");
		} else {
			intern->logger->get()->disableTracing();
			CPP_INFO("Tracing disabled");
		}
	} else if (!optionName.compare("characterSetResults")) {
		setSessionVariable("character_set_results", optionValue? static_cast<const char *>(optionValue) : std::string("NULL"));
	} else if (!optionName.compare("metadataUseInfoSchema")) {
		intern->metadata_use_info_schema = *(static_cast<const bool *>(optionValue));
	} else if (!optionName.compare("defaultStatementResultType")) {
		int int_value =  *static_cast<const int *>(optionValue);
		do {
			if (static_cast< int >(sql::ResultSet::TYPE_FORWARD_ONLY) == int_value) break;
			if (static_cast< int >(sql::ResultSet::TYPE_SCROLL_INSENSITIVE) == int_value) break;
			if (static_cast< int >(sql::ResultSet::TYPE_SCROLL_SENSITIVE) == int_value) {
				std::ostringstream msg;
				msg << "Invalid value " << int_value <<
					" for option defaultStatementResultType. TYPE_SCROLL_SENSITIVE is not supported";
				throw sql::InvalidArgumentException(msg.str());			
			}
			std::ostringstream msg;
			msg << "Invalid value (" << int_value << " for option defaultStatementResultType";
			throw sql::InvalidArgumentException(msg.str());
		} while (0);
		intern->defaultStatementResultType = static_cast< sql::ResultSet::enum_type >(int_value);
	} else if (!optionName.compare("defaultPreparedStatementResultType")) {
#if WE_SUPPORT_USE_RESULT_WITH_PS
		/* The connector is not ready for unbuffered as we need to refetch */
		intern->defaultPreparedStatementResultType = *(static_cast<const bool *>(optionValue));
#else
		throw MethodNotImplementedException("MySQL_Prepared_Statement::setResultSetType");	
#endif
	}
	return this;
}
/* }}} */


/* {{{ MySQL_Connection::setSavepoint() -I- */
sql::Savepoint *
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

	std::auto_ptr< sql::Statement > stmt(createStatement());
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

	if (intern->cache_sql_mode && intern->sql_mode_set == true &&
		!strncasecmp(varname.c_str(), "sql_mode", sizeof("sql_mode") - 1))
	{
		CPP_INFO_FMT("sql_mode=%s", intern->sql_mode.c_str());
		return intern->sql_mode;
	}
	std::auto_ptr< sql::Statement > stmt(createStatement());
	std::string q = std::string("SHOW SESSION VARIABLES LIKE '").append(varname).append("'");

	std::auto_ptr< sql::ResultSet > rset(stmt->executeQuery(q));

	if (rset->next()) {
		if (intern->cache_sql_mode && intern->sql_mode_set == false &&
			!strncasecmp(varname.c_str(), "sql_mode", sizeof("sql_mode") - 1))
		{
			intern->sql_mode = rset->getString(2);
			intern->sql_mode_set = true;
		}
		return rset->getString(2);
	}
	return "";
}
/* }}} */


/* {{{ MySQL_Connection::setSessionVariable() -I- */
void
MySQL_Connection::setSessionVariable(const std::string & varname, const std::string & value)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setSessionVariable");
	checkClosed();

	std::auto_ptr< sql::Statement > stmt(createStatement());
	std::string q(std::string("SET SESSION ").append(varname).append("="));
	if (!value.compare("NULL")) {
		q.append("NULL");
	} else {
		q.append("'").append(value).append("'");
	}

	stmt->executeUpdate(q);
	if (intern->cache_sql_mode && !strncasecmp(varname.c_str(), "sql_mode", sizeof("sql_mode") - 1)) {
		intern->sql_mode = value;
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

