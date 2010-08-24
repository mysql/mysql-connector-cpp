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

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <stdlib.h>
#include <memory>
#include <sstream>
#include <stdio.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#include <cppconn/exception.h>

#include "nativeapi/native_connection_wrapper.h"
#include "nativeapi/native_statement_wrapper.h"

#include "mysql_connection_options.h"
#include "mysql_util.h"

/*
 * _WIN32 is defined by 64bit compiler too
 * (see http://msdn.microsoft.com/en-us/library/aa489554.aspx)
 * So no need to check for _WIN64 too
 */
#ifdef _WIN32
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
MySQL_Savepoint::MySQL_Savepoint(const sql::SQLString &savepoint):
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
sql::SQLString
MySQL_Savepoint::getSavepointName()
{
	return name;
}
/* }}} */


/* {{{ MySQL_Connection::MySQL_Connection() -I- */
MySQL_Connection::MySQL_Connection(Driver * _driver,
								   ::sql::mysql::NativeAPI::NativeConnectionWrapper& _proxy,
								   const sql::SQLString& hostName,
								   const sql::SQLString& userName,
								   const sql::SQLString& password)
	: driver(_driver), proxy(&_proxy), intern(NULL)
{
	sql::ConnectOptionsMap connection_properties;
	connection_properties["hostName"] = hostName;
	connection_properties["userName"] = userName;
	connection_properties["password"] = password;

	boost::shared_ptr< MySQL_DebugLogger > tmp_logger(new MySQL_DebugLogger());
	std::auto_ptr< MySQL_ConnectionData > tmp_intern(new MySQL_ConnectionData(tmp_logger));
	intern = tmp_intern.get();

	init(connection_properties);
	// No exception so far, thus intern can still point to the MySQL_ConnectionData object
	// and in the dtor we will clean it up
	tmp_intern.release();
}
/* }}} */


/* {{{ MySQL_Connection::MySQL_Connection() -I- */
MySQL_Connection::MySQL_Connection(Driver * _driver,
								   ::sql::mysql::NativeAPI::NativeConnectionWrapper& _proxy,
								   sql::ConnectOptionsMap & properties)
	: driver(_driver), proxy(&_proxy), intern(NULL)
{
	boost::shared_ptr<MySQL_DebugLogger> tmp_logger(new MySQL_DebugLogger());
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
void MySQL_Connection::init(ConnectOptionsMap & properties)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::init");

	intern->is_valid = true;
	bool protocol_tcp = true;
	sql::SQLString host;
	sql::SQLString socket_or_pipe;
	unsigned int port = 3306;

	sql::SQLString hostName;
	sql::SQLString userName;
	sql::SQLString password;
	sql::SQLString schema;
	sql::SQLString defaultCharset("utf8");
	sql::SQLString characterSetResults("utf8");

	sql::SQLString sslKey, sslCert, sslCA, sslCAPath, sslCipher;
	bool ssl_used = false, schema_used = false;
	int flags = CLIENT_MULTI_RESULTS;

	const int * p_i;
	const bool * p_b;
	const sql::SQLString * p_s;
	bool opt_reconnect = false;
	bool opt_reconnect_value = false;

	sql::ConnectOptionsMap::const_iterator it = properties.begin();
	for (; it != properties.end(); ++it) {
		if (!it->first.compare("hostName")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				hostName = *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for hostName");
			}
		} else if (!it->first.compare("userName")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				userName = *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for userName");
			}
		} else if (!it->first.compare("password")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				password = *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for password");
			}
		} else if (!it->first.compare("port")) {
			if ((p_i = boost::get< int >(&it->second))) {
				port = static_cast<unsigned int>(*p_i);
			} else {
				throw sql::InvalidArgumentException("No long long value passed for port");
			}
		} else if (!it->first.compare("socket")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				socket_or_pipe = *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for socket");
			}
			protocol_tcp = false;
		} else if (!it->first.compare("pipe")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				socket_or_pipe = *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for pipe");
			}
			protocol_tcp = false;
		} else if (!it->first.compare("schema")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				schema = *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for schema");
			}
			schema_used = true;
		} else if (!it->first.compare("characterSetResults")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				characterSetResults = *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for characterSetResults");
			}
		} else if (!it->first.compare("sslKey")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				sslKey = *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for sslKey");
			}
			ssl_used = true;
		} else if (!it->first.compare("sslCert")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				sslCert = *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for sslCert");
			}
			ssl_used = true;
		} else if (!it->first.compare("sslCA")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				sslCA = *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for sslCA");
			}
			ssl_used = true;
		} else if (!it->first.compare("sslCAPath")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				sslCAPath = *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for sslCAPath");
			}
			ssl_used = true;
		} else if (!it->first.compare("sslCipher")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				sslCipher = *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for sslCipher");
			}
			ssl_used = true;
		} else if (!it->first.compare("defaultStatementResultType")) {
			if (!(p_i = boost::get< int >(&it->second))) {
				throw sql::InvalidArgumentException("No long long value passed for defaultStatementResultType");
			}
			do {
				if (static_cast< int >(sql::ResultSet::TYPE_FORWARD_ONLY) == *p_i) break;
				if (static_cast< int >(sql::ResultSet::TYPE_SCROLL_INSENSITIVE) == *p_i) break;
				if (static_cast< int >(sql::ResultSet::TYPE_SCROLL_SENSITIVE) == *p_i) {
					std::ostringstream msg;
					msg << "Invalid value " << *p_i <<
						" for option defaultStatementResultType. TYPE_SCROLL_SENSITIVE is not supported";
					throw sql::InvalidArgumentException(msg.str());
				}
				std::ostringstream msg;
				msg << "Invalid value (" << *p_i << " for option defaultStatementResultType";
				throw sql::InvalidArgumentException(msg.str());
			} while (0);
			intern->defaultStatementResultType = static_cast< sql::ResultSet::enum_type >(*p_i);
		/* The connector is not ready for unbuffered as we need to refetch */
		} else if (!it->first.compare("defaultPreparedStatementResultType")) {
#if WE_SUPPORT_USE_RESULT_WITH_PS
			if (!(p_i = boost::get< int >(&it->second))) {
				throw sql::InvalidArgumentException("No long long value passed for defaultPreparedStatementResultType");
			}
			do {
				if (static_cast< int >(sql::ResultSet::TYPE_FORWARD_ONLY) == *p_i) break;
				if (static_cast< int >(sql::ResultSet::TYPE_SCROLL_INSENSITIVE) == *p_i) break;
				if (static_cast< int >(sql::ResultSet::TYPE_SCROLL_SENSITIVE) == *p_i) {
					std::ostringstream msg;
					msg << "Invalid value " << *p_i <<
						" for option defaultPreparedStatementResultType. TYPE_SCROLL_SENSITIVE is not supported";
					throw sql::InvalidArgumentException(msg.str());
				}
				std::ostringstream msg;
				msg << "Invalid value (" << *p_i << " for option defaultPreparedStatementResultType";
				throw sql::InvalidArgumentException(msg.str());
			} while (0);
			intern->defaultPreparedStatementResultType = static_cast< sql::ResultSet::enum_type >(*p_i);
#else
			throw SQLException("defaultPreparedStatementResultType parameter still not implemented");

#endif
		} else if (!it->first.compare("metadataUseInfoSchema")) {
			if ((p_b = boost::get<bool>(&it->second))) {
				intern->metadata_use_info_schema = *p_b;
			} else {
				throw sql::InvalidArgumentException("No bool value passed for metadataUseInfoSchema");
			}
		} else if (!it->first.compare("CLIENT_COMPRESS")) {
			if (!(p_b = boost::get<bool>(&it->second))) {
				throw sql::InvalidArgumentException("No bool value passed for CLIENT_COMPRESS");
			}
			if (*p_b) {
				flags |= CLIENT_COMPRESS;
			}
		} else if (!it->first.compare("CLIENT_FOUND_ROWS")) {
			if (!(p_b = boost::get<bool>(&it->second))) {
				throw sql::InvalidArgumentException("No bool value passed for CLIENT_FOUND_ROWS");
			}
			if (*p_b) {
				flags |= CLIENT_FOUND_ROWS;
			}
		} else if (!it->first.compare("CLIENT_IGNORE_SIGPIPE")) {
			if (!(p_b = boost::get<bool>(&it->second))) {
				throw sql::InvalidArgumentException("No bool value passed for CLIENT_IGNORE_SIGPIPE");
			}
			if (*p_b) {
				flags |= CLIENT_IGNORE_SIGPIPE;
			}
		} else if (!it->first.compare("CLIENT_IGNORE_SPACE")) {
			if (!(p_b = boost::get<bool>(&it->second))) {
				throw sql::InvalidArgumentException("No bool value passed for CLIENT_IGNORE_SPACE");
			}
			if (*p_b) {
				flags |= CLIENT_IGNORE_SPACE;
			}
		} else if (!it->first.compare("CLIENT_INTERACTIVE")) {
			if (!(p_b = boost::get<bool>(&it->second))) {
				throw sql::InvalidArgumentException("No bool value passed for CLIENT_INTERACTIVE");
			}
			if (*p_b) {
				flags |= CLIENT_INTERACTIVE;
			}
		} else if (!it->first.compare("CLIENT_LOCAL_FILES")) {
			if (!(p_b = boost::get<bool>(&it->second))) {
				throw sql::InvalidArgumentException("No bool value passed for CLIENT_LOCAL_FILES");
			}
			if (*p_b) {
				flags |= CLIENT_LOCAL_FILES;
			}
		} else if (!it->first.compare("CLIENT_MULTI_STATEMENTS")) {
			if (!(p_b = boost::get<bool>(&it->second))) {
				throw sql::InvalidArgumentException("No bool value passed for CLIENT_MULTI_STATEMENTS");
			}
			if (*p_b) {
				flags |= CLIENT_MULTI_STATEMENTS;
			}
		} else if (!it->first.compare("CLIENT_NO_SCHEMA")) {
			if (!(p_b = boost::get<bool>(&it->second))) {
				throw sql::InvalidArgumentException("No bool value passed for CLIENT_NO_SCHEMA");
			}
			if (*p_b) {
				flags |= CLIENT_NO_SCHEMA;
			}
		}
	}

#ifndef _WIN32
	if (!hostName.compare(0, sizeof("unix://") - 1, "unix://")) {
		protocol_tcp = false;
		socket_or_pipe = hostName.substr(sizeof("unix://") - 1, sql::SQLString::npos);
		host = "localhost";
		int tmp_protocol = MYSQL_PROTOCOL_SOCKET;
		proxy->options(MYSQL_OPT_PROTOCOL, (const char *) &tmp_protocol);
	} else
#else
	if (!hostName.compare(0, sizeof("pipe://") - 1, "pipe://")) {
		protocol_tcp = false;
		socket_or_pipe = hostName.substr(sizeof("pipe://") - 1, sql::SQLString::npos);
		host = ".";
		int tmp_protocol = MYSQL_PROTOCOL_PIPE;
		proxy->options(MYSQL_OPT_PROTOCOL, (const char *) &tmp_protocol);
	} else
#endif
	if (!hostName.compare(0, sizeof("tcp://") - 1, "tcp://") ) {
		size_t port_pos, schema_pos;
		host = hostName.substr(sizeof("tcp://") - 1, sql::SQLString::npos);
		schema_pos = host.find('/');
		if (schema_pos != sql::SQLString::npos) {
			++schema_pos; // skip the slash
							/* TODO: tcp://127.0.0.1/
							-> host set, schema empty, schema property ignored */
			schema = host.substr(schema_pos, host.length() - schema_pos);
			schema_used = true;
			host = host.substr(0, schema_pos - 1);
		}
		port_pos = host.find_last_of(':', sql::SQLString::npos);
		if (port_pos != sql::SQLString::npos) {
		port = atoi(host.substr(port_pos + 1, sql::SQLString::npos).c_str());
			host = host.substr(0, port_pos);
		}
	} else {
		host = hostName.c_str();
	}
	/* libmysql shouldn't think it is too smart */
	if (protocol_tcp && !host.compare(0, sizeof("localhost") - 1, "localhost")) {
		host = "127.0.0.1";
	}

	it = properties.begin();
	for (; it != properties.end(); ++it) {
		if (!it->first.compare("OPT_CONNECT_TIMEOUT")) {
			if (!(p_i = boost::get< int >(&it->second))) {
				throw sql::InvalidArgumentException("No long long value passed for OPT_CONNECT_TIMEOUT");
			}
			long l_tmp = static_cast<long>(*p_i);
			proxy->options(MYSQL_OPT_CONNECT_TIMEOUT, (const char *) &l_tmp);
		} else if (!it->first.compare("OPT_READ_TIMEOUT")) {
			if (!(p_i = boost::get< int >(&it->second))) {
				throw sql::InvalidArgumentException("No long long value passed for OPT_READ_TIMEOUT");
			}
			long l_tmp = static_cast<long>(*p_i);
			proxy->options(MYSQL_OPT_READ_TIMEOUT, (const char *) &l_tmp);
		} else if (!it->first.compare("OPT_WRITE_TIMEOUT")) {
			if (!(p_i = boost::get< int >(&it->second))) {
				throw sql::InvalidArgumentException("No long long value passed for OPT_WRITE_TIMEOUT");
			}
			long l_tmp = static_cast<long>(*p_i);
			proxy->options(MYSQL_OPT_WRITE_TIMEOUT, (const char *) &l_tmp);
		} else if (!it->first.compare("OPT_RECONNECT")) {
			if (!(p_b = boost::get<bool>(&it->second))) {
				throw sql::InvalidArgumentException("No bool value passed for OPT_RECONNECT");
			}
			opt_reconnect = true;
			opt_reconnect_value = *p_b;
		} else if (!it->first.compare("OPT_CHARSET_NAME")) {
			if (!(p_s = boost::get< sql::SQLString >(&it->second))) {
				throw sql::InvalidArgumentException("No SQLString value passed for OPT_CHARSET_NAME");
			}
			defaultCharset = *p_s;
		} else if (!it->first.compare("OPT_REPORT_DATA_TRUNCATION")) {
			if (!(p_b = boost::get< bool >(&it->second))) {
				throw sql::InvalidArgumentException("No bool value passed for OPT_REPORT_DATA_TRUNCATION");
			}
			proxy->options(MYSQL_REPORT_DATA_TRUNCATION, (const char *) p_b);
#ifdef _WIN32
		} else if (!it->first.compare("OPT_NAMED_PIPE")) {
			proxy->options(MYSQL_OPT_NAMED_PIPE, NULL);
#endif
		}
	}

	{
		my_bool tmp_bool = 1;
		proxy->options(MYSQL_SECURE_AUTH, (const char *) &tmp_bool);
	}

	proxy->options(MYSQL_SET_CHARSET_NAME, defaultCharset.c_str());

	if (ssl_used) {
		/* According to the docs, always returns 0 */
		proxy->ssl_set(sslKey.c_str(), sslCert.c_str(), sslCA.c_str(), sslCAPath.c_str(), sslCipher.c_str());
	}
	CPP_INFO_FMT("hostName=%s", hostName.c_str());
	CPP_INFO_FMT("user=%s", userName.c_str());
	CPP_INFO_FMT("port=%d", port);
	CPP_INFO_FMT("schema=%s", schema.c_str());
	CPP_INFO_FMT("socket/pipe=%s", socket_or_pipe.c_str());
	if (!proxy->connect(host,
						userName,
						password,
						schema_used && schema.length()? schema: (schema= "") /* schema */,
						port,
						protocol_tcp == false? socket_or_pipe : "" /*socket or named pipe */,
						flags))
	{
		CPP_ERR_FMT("Couldn't connect : %d", proxy->errNo());
		CPP_ERR_FMT("Couldn't connect : (%s)", proxy->sqlstate().c_str());
		CPP_ERR_FMT("Couldn't connect : %s", proxy->error().c_str());
		CPP_ERR_FMT("Couldn't connect : %d:(%s) %s", proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());
		sql::SQLException e(proxy->error(), proxy->sqlstate(), proxy->errNo());
		proxy.reset();
		throw e;
	}
	if (opt_reconnect) {
		proxy->options(MYSQL_OPT_RECONNECT, (const char *) &opt_reconnect_value);
	}

	setAutoCommit(true);
	setTransactionIsolation(sql::TRANSACTION_REPEATABLE_READ);
	// Different Values means we have to set different result set encoding
	if (characterSetResults.compare(defaultCharset)) {
		setSessionVariable("character_set_results", characterSetResults.length() ? characterSetResults:"NULL");
	}
	intern->meta.reset(new MySQL_ConnectionMetaData(this, proxy, intern->logger));
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
	proxy.reset();
	clearWarnings();
	intern->is_valid = false;
}
/* }}} */


/* {{{ MySQL_Connection::commit() -I- */
void
MySQL_Connection::commit()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::commit");
	checkClosed();
	proxy->commit();
}
/* }}} */


/* {{{ MySQL_Connection::createStatement() -I- */
sql::Statement * MySQL_Connection::createStatement()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::createStatement");
	checkClosed();
	return new MySQL_Statement(this, proxy, intern->defaultStatementResultType, intern->logger);
}
/* }}} */


/* {{{ MySQL_Connection::escapeString() -I- */
sql::SQLString MySQL_Connection::escapeString(const sql::SQLString & s)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::escapeString");
	checkClosed();
	return proxy->escapeString(s);
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
sql::SQLString
MySQL_Connection::getCatalog()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getCatalog");
	checkClosed();
	return proxy->get_server_version() > 60006 ? "def" : "";
}
/* }}} */


/* {{{ MySQL_Connection::getDriver() -I- */
Driver * MySQL_Connection::getDriver()
{
	return driver;
}
/* }}} */


/**
  Added for consistency. Not present in jdbc interface. Is still subject for discussion.
*/
/* {{{ MySQL_Connection::getSchema() -I- */
sql::SQLString
MySQL_Connection::getSchema()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getSchema");
	checkClosed();
	boost::scoped_ptr< sql::Statement > stmt(createStatement());
	boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery("SELECT DATABASE()")); //SELECT SCHEMA()
	rset->next();
	return rset->getString(1);
}
/* }}} */


/* {{{ MySQL_Connection::getClientInfo() -I- */
sql::SQLString
MySQL_Connection::getClientInfo()
{
	const sql::SQLString clientInfo("cppconn");
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getClientInfo");
	return clientInfo;
}
/* }}} */


/* {{{ MySQL_Connection::getClientOption() -I- */
void
MySQL_Connection::getClientOption(const sql::SQLString & optionName, void * optionValue)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getClientOption");
	if (!optionName.compare("metadataUseInfoSchema")) {
		*(static_cast<bool *>(optionValue)) = intern->metadata_use_info_schema;
	} else if (!optionName.compare("defaultStatementResultType")) {
		*(static_cast<int *>(optionValue)) = intern->defaultStatementResultType;
	} else if (!optionName.compare("defaultPreparedStatementResultType")) {
		*(static_cast<int *>(optionValue)) = intern->defaultPreparedStatementResultType;
	} else if (!optionName.compare("characterSetResults")) {
		*(static_cast<sql::SQLString **>(optionValue)) = new sql::SQLString(getSessionVariable("characterSetResults"));
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

	clearWarnings();

	intern->warnings.reset(loadMysqlWarnings(this));

	return intern->warnings.get();
}
/* }}} */


/* {{{ MySQL_Connection::isClosed() -I- */
bool
MySQL_Connection::isClosed()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::isClosed");
	if (intern->is_valid) {
		if (!proxy->ping()) {
			return false;
		}
		close();
	}
	return true;
}
/* }}} */


/* {{{ MySQL_Connection::isReadOnly() -U- */
bool
MySQL_Connection::isReadOnly()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::isReadOnly");
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Connection::isReadOnly");
	return false; // fool compiler
}
/* }}} */


/* {{{ MySQL_Connection::nativeSQL() -I- */
sql::SQLString
MySQL_Connection::nativeSQL(const sql::SQLString& sql)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::nativeSQL");
	checkClosed();
	return sql::SQLString(sql.c_str());
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -I- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const sql::SQLString& sql)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	CPP_INFO_FMT("query=%s", sql.c_str());
	checkClosed();
	boost::shared_ptr< NativeAPI::NativeStatementWrapper > stmt;

	//TODO change - probably no need to catch and throw here. Logging can be done inside proxy
	try {
		 stmt.reset(&proxy->stmt_init());
	} catch (sql::SQLException& e) {
		CPP_ERR_FMT("No statement : %d:(%s) %s", proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());
		throw e;
	}

	if (stmt->prepare(sql)) {
		CPP_ERR_FMT("Cannot prepare %d:(%s) %s", stmt->errNo(), stmt->sqlstate().c_str(), stmt->error().c_str());
		sql::SQLException e(stmt->error(), stmt->sqlstate(), stmt->errNo());
		stmt.reset();
		throw e;
	}

	return new MySQL_Prepared_Statement(stmt, this, intern->defaultPreparedStatementResultType, intern->logger);
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -U- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const sql::SQLString& /* sql */, int /* autoGeneratedKeys */)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Connection::prepareStatement(const sql::SQLString& sql, int autoGeneratedKeys)");
	return NULL; // fool compiler
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -U- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const sql::SQLString& /* sql */, int /* columnIndexes */ [])
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Connection::prepareStatement(const sql::SQLString& sql, int* columnIndexes)");
	return NULL; // fool compiler
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -U- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const sql::SQLString& /* sql */, int /* resultSetType */, int /* resultSetConcurrency */)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Connection::prepareStatement(const sql::SQLString& sql, int resultSetType, int resultSetConcurrency)");
	return NULL; // fool compiler
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -U- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const sql::SQLString& /* sql */, int /* resultSetType */, int /* resultSetConcurrency */, int /* resultSetHoldability */)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Connection::prepareStatement(const sql::SQLString& sql, int resultSetType, int resultSetConcurrency, int resultSetHoldability)");
	return NULL; // fool compiler
}
/* }}} */


/* {{{ MySQL_Connection::prepareStatement() -U- */
sql::PreparedStatement *
MySQL_Connection::prepareStatement(const sql::SQLString& /* sql */, sql::SQLString /* columnNames*/ [])
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::prepareStatement");
	checkClosed();
	throw sql::MethodNotImplementedException("MySQL_Connection::prepareStatement(const sql::SQLString& sql, sql::SQLString columnNames[])");
	return NULL; // fool compiler
}
/* }}} */


/* {{{ MySQL_Connection::releaseSavepoint() -I- */
void
MySQL_Connection::releaseSavepoint(Savepoint * savepoint)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::releaseSavepoint");
	checkClosed();
	if (proxy->get_server_version() < 50001) {
		throw sql::MethodNotImplementedException("releaseSavepoint not available in this server version");
	}
	if (getAutoCommit()) {
		throw sql::InvalidArgumentException("The connection is in autoCommit mode");
	}
	sql::SQLString sql("RELEASE SAVEPOINT ");
	sql.append(savepoint->getSavepointName());

	boost::scoped_ptr<sql::Statement> stmt(createStatement());
	stmt->execute(sql);
}
/* }}} */


/* {{{ MySQL_Connection::rollback() -I- */
void
MySQL_Connection::rollback()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::rollback");
	checkClosed();
	proxy->rollback();
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
	sql::SQLString sql("ROLLBACK TO SAVEPOINT ");
	sql.append(savepoint->getSavepointName());

	boost::scoped_ptr< sql::Statement > stmt(createStatement());
	stmt->execute(sql);
}
/* }}} */


/* {{{ MySQL_Connection::setCatalog() -I- */
void
MySQL_Connection::setCatalog(const sql::SQLString&)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setCatalog");
	checkClosed();
}
/* }}} */


/* {{{ MySQL_Connection::setSchema() -I- (not part of JDBC) */
void
MySQL_Connection::setSchema(const sql::SQLString& catalog)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setCatalog");
	checkClosed();
	sql::SQLString sql("USE `");
	sql.append(catalog).append("`");

	boost::scoped_ptr< sql::Statement > stmt(createStatement());
	stmt->execute(sql);
}
/* }}} */


/* {{{ MySQL_Connection::setClientOption() -I- */
sql::Connection *
MySQL_Connection::setClientOption(const sql::SQLString & optionName, const void * optionValue)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setClientOption");
	if (!optionName.compare("libmysql_debug")) {
		proxy->debug(static_cast<const char *>(optionValue));
	} else if (!optionName.compare("clientTrace")) {
		if (*(static_cast<const bool *>(optionValue))) {
			intern->logger->enableTracing();
			CPP_INFO("Tracing enabled");
		} else {
			intern->logger->disableTracing();
			CPP_INFO("Tracing disabled");
		}
	} else if (!optionName.compare("characterSetResults")) {
		setSessionVariable("character_set_results", optionValue? static_cast<const char *>(optionValue) : sql::SQLString("NULL"));
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
	throw sql::MethodNotImplementedException("Please use MySQL_Connection::setSavepoint(const sql::SQLString& name)");
	return NULL;
}
/* }}} */


/* {{{ MySQL_Connection::setSavepoint() -I- */
sql::Savepoint *
MySQL_Connection::setSavepoint(const sql::SQLString& name)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setSavepoint");
	checkClosed();
	if (getAutoCommit()) {
		throw sql::InvalidArgumentException("The connection is in autoCommit mode");
	}
	if (!name.length()) {
		throw sql::InvalidArgumentException("Savepoint name cannot be empty string");
	}
	sql::SQLString sql("SAVEPOINT ");
	sql.append(name);

	boost::scoped_ptr< sql::Statement > stmt(createStatement());
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
	proxy->autocommit(autoCommit);
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
	proxy->query(q);
}
/* }}} */


/* {{{ MySQL_Connection::getSessionVariable() -I- */
sql::SQLString
MySQL_Connection::getSessionVariable(const sql::SQLString & varname)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getSessionVariable");
	checkClosed();

	if (intern->cache_sql_mode && intern->sql_mode_set == true && !varname.compare("sql_mode")) {
		CPP_INFO_FMT("sql_mode=%s", intern->sql_mode.c_str());
		return intern->sql_mode;
	}
	boost::scoped_ptr< sql::Statement > stmt(createStatement());
	sql::SQLString q("SHOW SESSION VARIABLES LIKE '");
	q.append(varname).append("'");

	boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery(q));

	if (rset->next()) {
		if (intern->cache_sql_mode && intern->sql_mode_set == false && !varname.compare("sql_mode")) {
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
MySQL_Connection::setSessionVariable(const sql::SQLString & varname, const sql::SQLString & value)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::setSessionVariable");
	checkClosed();

	boost::scoped_ptr< sql::Statement > stmt(createStatement());
	sql::SQLString q("SET SESSION ");
	q.append(varname).append("=");

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

