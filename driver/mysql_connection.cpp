/*
Copyright (c) 2008, 2013, Oracle and/or its affiliates. All rights reserved.

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
#include <mysqld_error.h>
#include <cppconn/exception.h>

#include "nativeapi/native_connection_wrapper.h"
#include "nativeapi/native_statement_wrapper.h"

#include "mysql_connection_options.h"
#include "mysql_util.h"
#include "mysql_uri.h"
#include "mysql_error.h"

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

#ifndef ER_MUST_CHANGE_PASSWORD_LOGIN
# define ER_MUST_CHANGE_PASSWORD_LOGIN 1820
#endif

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


/* {{{ MySQL_Connection::createServiceStmt() */
MySQL_Statement *
MySQL_Connection::createServiceStmt() {

	/* We need to have it storing results, not using */
	return new MySQL_Statement(this, proxy,
							   sql::ResultSet::TYPE_SCROLL_INSENSITIVE,
							   intern->logger);
}

/* {{{ MySQL_Connection::MySQL_Connection() -I- */
MySQL_Connection::MySQL_Connection(Driver * _driver,
								   ::sql::mysql::NativeAPI::NativeConnectionWrapper& _proxy,
								   const sql::SQLString& hostName,
								   const sql::SQLString& userName,
								   const sql::SQLString& password)
	:	driver	(_driver),
		proxy	(&_proxy),
		intern	(NULL)
{
	sql::ConnectOptionsMap connection_properties;
	connection_properties["hostName"] = hostName;
	connection_properties["userName"] = userName;
	connection_properties["password"] = password;

	boost::shared_ptr< MySQL_DebugLogger > tmp_logger(new MySQL_DebugLogger());
	std::auto_ptr< MySQL_ConnectionData > tmp_intern(new MySQL_ConnectionData(tmp_logger));
	intern = tmp_intern.get();

	service.reset(createServiceStmt());

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

	service.reset(createServiceStmt());

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

/* A struct to keep const reference data for mapping string value to int */
struct String2IntMap
{
	const char * key;
	int		     value;
};

static const String2IntMap flagsOptions[]=
	{
		{"CLIENT_COMPRESS",			CLIENT_COMPRESS},
		{"CLIENT_FOUND_ROWS",		CLIENT_FOUND_ROWS},
		{"CLIENT_IGNORE_SIGPIPE",	CLIENT_IGNORE_SIGPIPE},
		{"CLIENT_IGNORE_SPACE",		CLIENT_IGNORE_SPACE},
		{"CLIENT_INTERACTIVE",		CLIENT_INTERACTIVE},
		{"CLIENT_LOCAL_FILES",		CLIENT_LOCAL_FILES},
		{"CLIENT_MULTI_STATEMENTS",	CLIENT_MULTI_STATEMENTS},
		{"CLIENT_NO_SCHEMA",		CLIENT_NO_SCHEMA}
	};

/* {{{ readFlag(::sql::SQLString, int= 0) -I- */
/** Check if connection option pointed by map iterator defines a connection
    flag */
static bool read_connection_flag(ConnectOptionsMap::const_iterator &cit, int &flags)
{
	const bool * value;

	for (int i = 0; i < sizeof(flagsOptions)/sizeof(String2IntMap); ++i) {

		if (!cit->first.compare(flagsOptions[i].key)) {

			if (!(value = boost::get<bool>(&cit->second))) {
				sql::SQLString err("No bool value passed for ");
				err.append(flagsOptions[i].key);
				throw sql::InvalidArgumentException(err);
			}
			if (*value) {
				flags |= flagsOptions[i].value;
			}
			return true;
		}
	}
	return false;
}
/* }}} */

/* Array for mapping of boolean connection options to mysql_options call */
static const String2IntMap booleanOptions[]= 
	{
		{"OPT_REPORT_DATA_TRUNCATION",	MYSQL_REPORT_DATA_TRUNCATION},
		{"OPT_ENABLE_CLEARTEXT_PLUGIN",	MYSQL_ENABLE_CLEARTEXT_PLUGIN}
	};
/* Array for mapping of integer connection options to mysql_options call */
static const String2IntMap intOptions[]= 
	{
		{"OPT_CONNECT_TIMEOUT",	MYSQL_OPT_CONNECT_TIMEOUT},
		{"OPT_READ_TIMEOUT",	MYSQL_OPT_READ_TIMEOUT},
		{"OPT_WRITE_TIMEOUT",	MYSQL_OPT_WRITE_TIMEOUT}
	};
/* Array for mapping of string connection options to mysql_options call */
static const String2IntMap stringOptions[]= 
	{
		{"preInit",	MYSQL_INIT_COMMAND}
	};

template<class T>
bool process_connection_option(ConnectOptionsMap::const_iterator &option,
								const String2IntMap options_map[],
								size_t map_size,
								boost::shared_ptr< NativeAPI::NativeConnectionWrapper > &proxy)
{
	const T * value;

	for (unsigned int i = 0; i < map_size; ++i) {

		if (!option->first.compare(options_map[i].key)) {

			if (!(value = boost::get<T>(&option->second))) {
				sql::SQLString err("Option ");
				err.append(option->first).append(" is not of expected type"); 
				throw sql::InvalidArgumentException(err); 
			}
			proxy->options(static_cast<sql::mysql::MySQL_Connection_Options>(options_map[i].value),
							*value);

			return true;
		}
	}

	return false;
}


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
  - OPT_CAN_HANDLE_EXPIRED_PASSWORDS
  - OPT_ENABLE_CLEARTEXT_PLUGIN
  - preInit
  - postInit

  To add new connection option that maps to a myql_options call, only add its
  mapping to sql::mysql::MySQL_Connection_Options value to one of arrays above
  - booleanOptions, intOptions, stringOptions. You might need to add new member
  to the sql::mysql::MySQL_Connection_Options enum
*/

/* {{{ MySQL_Connection::init() -I- */
void MySQL_Connection::init(ConnectOptionsMap & properties)
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::init");

	intern->is_valid = true;

	MySQL_Uri uri;

	sql::SQLString userName;
	sql::SQLString password;
	sql::SQLString defaultCharset("utf8");
	sql::SQLString characterSetResults("utf8");

	sql::SQLString sslKey, sslCert, sslCA, sslCAPath, sslCipher, postInit;
	bool ssl_used = false;
	int flags = CLIENT_MULTI_RESULTS;

	const int * p_i;
	const bool * p_b;
	const sql::SQLString * p_s;
	bool opt_reconnect = false;
	bool opt_reconnect_value = false;
	bool client_doesnt_support_exp_pwd = false;


	/* Values set in properties individually should have priority over those
	   we restore from Uri */
	sql::ConnectOptionsMap::const_iterator it = properties.find("hostName");

	if (it != properties.end())	{
		if ((p_s = boost::get< sql::SQLString >(&it->second))) {
            /* Parsing uri prior to processing all parameters, so indivudually
               specified parameters precede over those in the uri */
			parseUri(*p_s, uri);
		} else {
			throw sql::InvalidArgumentException("No string value passed for hostName");
		}
	}

#define PROCESS_CONN_OPTION(option_type, options_map) process_connection_option< option_type >(it, options_map, sizeof(options_map)/sizeof(String2IntMap), proxy)

	for (it = properties.begin(); it != properties.end(); ++it) {
		if (!it->first.compare("userName")) {
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
				uri.setPort(static_cast<unsigned int>(*p_i));
			} else {
				throw sql::InvalidArgumentException("No long long value passed for port");
			}
		} else if (!it->first.compare("socket")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				uri.setSocket(*p_s);
			} else {
				throw sql::InvalidArgumentException("No string value passed for socket");
			}
		} else if (!it->first.compare("pipe")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				uri.setPipe(*p_s);
			} else {
				throw sql::InvalidArgumentException("No string value passed for pipe");
			}
		} else if (!it->first.compare("schema")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				uri.setSchema(*p_s);
			} else {
				throw sql::InvalidArgumentException("No string value passed for schema");
			}
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
		} else if (!it->first.compare("OPT_NAMED_PIPE")) {
			/* Not sure it is really needed */
			uri.setProtocol(NativeAPI::PROTOCOL_PIPE);
		} else if (!it->first.compare("OPT_CAN_HANDLE_EXPIRED_PASSWORDS")) {
			/* We need to know client version at runtime */
			long client_ver= proxy->get_client_version();
			if (proxy->get_client_version() < 50610) {
				// TODO: I think we should throw a warning here
				/* We only need this flag set if application has said it supports expired
				   password mode */
				client_doesnt_support_exp_pwd= true;
			} else {
				if (!(p_b = boost::get< bool >(&it->second))) {
					throw sql::InvalidArgumentException("No bool value passed for "
														"OPT_CAN_HANDLE_EXPIRED_PASSWORDS");
				}
				/* We do not care here about server version */
				proxy->options(MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS, (const char*)p_b);
			}
		} else if (!it->first.compare("postInit")) {
			if ((p_s = boost::get< sql::SQLString >(&it->second))) {
				postInit= *p_s;
			} else {
				throw sql::InvalidArgumentException("No string value passed for postInit");
			}

		/* If you need to add new integer connection option that should result in
		   calling mysql_optiong - add its mapping to the intOptions array
		 */
		} else if (PROCESS_CONN_OPTION(int, intOptions)) {
			// Nothing to do here

		/* For boolean coonection option - add mapping to booleanOptions array */
		} else if (PROCESS_CONN_OPTION(bool, booleanOptions)) {
			// Nothing to do here

		/* For string coonection option - add mapping to stringOptions array */
		} else if (PROCESS_CONN_OPTION(sql::SQLString, stringOptions)) {
			// Nothing to do here
		} else if (read_connection_flag(it, flags)) {
			// Nothing to do here
		} else {
			// TODO: Shouldn't we really create a warning here? as soon as we are able to
			//       create a warning
		}
        
	} /* End of cycle on connection options map */

#undef PROCESS_CONNSTR_OPTION

	/* libmysql shouldn't think it is too smart */
	if (tcpProtocol(uri) && !uri.Host().compare(util::LOCALHOST)) {
		uri.setHost("127.0.0.1");
	}

// Throwing in case of wrong protocol
#ifdef _WIN32
	if (uri.Protocol() == NativeAPI::PROTOCOL_SOCKET) {
		throw sql::InvalidArgumentException("Invalid for this platform protocol requested(MYSQL_PROTOCOL_SOCKET)");
	}
#else
	if (uri.Protocol() == NativeAPI::PROTOCOL_PIPE) {
		throw sql::InvalidArgumentException("Invalid for this platform protocol requested(MYSQL_PROTOCOL_PIPE)");
	}
#endif

	proxy->use_protocol(uri.Protocol());

	{
		const char tmp_bool = 1;
		proxy->options(MYSQL_SECURE_AUTH, &tmp_bool);
	}

	proxy->options(MYSQL_SET_CHARSET_NAME, defaultCharset.c_str());

	if (ssl_used) {
		/* According to the docs, always returns 0 */
		proxy->ssl_set(sslKey.c_str(), sslCert.c_str(), sslCA.c_str(), sslCAPath.c_str(), sslCipher.c_str());
	}
	CPP_INFO_FMT("hostName=%s", uri.Host().c_str());
	CPP_INFO_FMT("user=%s", userName.c_str());
	CPP_INFO_FMT("port=%d", uri.Port());
	CPP_INFO_FMT("schema=%s", uri.Schema().c_str());
	CPP_INFO_FMT("socket/pipe=%s", uri.SocketOrPipe().c_str());
	if (!proxy->connect(uri.Host(),
						userName,
						password,
						uri.Schema() /* schema */,
						uri.Port(),
						uri.SocketOrPipe() /*socket or named pipe */,
						flags))
	{
		CPP_ERR_FMT("Couldn't connect : %d", proxy->errNo());
		CPP_ERR_FMT("Couldn't connect : (%s)", proxy->sqlstate().c_str());
		CPP_ERR_FMT("Couldn't connect : %s", proxy->error().c_str());
		CPP_ERR_FMT("Couldn't connect : %d:(%s) %s", proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());

		/* If error is "Password has expired" and application supports it while 
		   mysql client lib does not */
		std::string error_message;
		int native_error= proxy->errNo();

		if (native_error == ER_MUST_CHANGE_PASSWORD_LOGIN
			&& client_doesnt_support_exp_pwd) {

			native_error= deCL_CANT_HANDLE_EXP_PWD;
			error_message= "Your password has expired, but your instance of"
				" Connector/C++ is not linked against mysql client library that"
				" allows to reset it. To resolve this you either need to change"
				" the password with mysql client that is capable to do that,"
				" or rebuild your instance of Connector/C++ against mysql client"
				" library that supports resetting of an expired password.";
		} else {
			error_message= proxy->error();
		}

		sql::SQLException e(error_message, proxy->sqlstate(), native_error);
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
	intern->meta.reset(new MySQL_ConnectionMetaData(service.get(), proxy, intern->logger));

	if (postInit.length() > 0) {
		service->executeUpdate(postInit);
	}
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

	service->executeUpdate(q);
}
/* }}} */


/* {{{ MySQL_Connection::getLastStatementInfo() -I- */
sql::SQLString
MySQL_Connection::getLastStatementInfo()
{
	CPP_ENTER_WL(intern->logger, "MySQL_Connection::getLastStatementInfo");
	checkClosed();

	return proxy->info();
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
	sql::SQLString q("SHOW SESSION VARIABLES LIKE '");
	q.append(varname).append("'");

	boost::scoped_ptr< sql::ResultSet > rset(service->executeQuery(q));

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

	sql::SQLString q("SET SESSION ");
	q.append(varname).append("=");

	if (!value.compare("NULL")) {
		q.append("NULL");
	} else {
		q.append("'").append(value).append("'");
	}

	service->executeUpdate(q);
	if (intern->cache_sql_mode && !strncasecmp(varname.c_str(), "sql_mode", sizeof("sql_mode") - 1)) {
		intern->sql_mode= value;
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

