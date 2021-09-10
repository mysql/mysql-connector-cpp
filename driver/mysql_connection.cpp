/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates. All rights reserved.
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



#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <stdlib.h>
#include <memory>
#include <sstream>
#include <stdio.h>
#include <map>
#include <vector>
#include <algorithm>
#include <random>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif
#ifndef _WIN32
#ifdef __FreeBSD__
#include <netinet/in.h>
#endif
#include <resolv.h>
#else
#include <winsock2.h>
#include <windns.h>
#pragma comment(lib,"Dnsapi")
#endif
#include <mysqld_error.h>
#include <cppconn/exception.h>

#include "nativeapi/native_connection_wrapper.h"
#include "nativeapi/native_statement_wrapper.h"

#include "mysql_connection_options.h"
#include "mysql_util.h"
#include "mysql_uri.h"
#include "mysql_error.h"
#include "cppconn/version_info.h"

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

  using Host_data = MySQL_Uri::Host_data;

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
                                   :  driver (_driver),
                                      proxy  (&_proxy)
{
  sql::ConnectOptionsMap connection_properties;
  connection_properties["hostName"] = hostName;
  connection_properties["userName"] = userName;
  connection_properties["password"] = password;

  boost::shared_ptr< MySQL_DebugLogger > tmp_logger(new MySQL_DebugLogger());
  intern.reset(new MySQL_ConnectionData(tmp_logger));

  service.reset(createServiceStmt());
  init(connection_properties);
}
/* }}} */


/* {{{ MySQL_Connection::MySQL_Connection() -I- */
MySQL_Connection::MySQL_Connection(Driver * _driver,
                   ::sql::mysql::NativeAPI::NativeConnectionWrapper& _proxy,
                   sql::ConnectOptionsMap & properties)
  : driver(_driver), proxy(&_proxy)
{
  boost::shared_ptr<MySQL_DebugLogger> tmp_logger(new MySQL_DebugLogger());
  intern.reset(new MySQL_ConnectionData(tmp_logger));

  service.reset(createServiceStmt());
  init(properties);
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
}
/* }}} */

/* A struct to keep const reference data for mapping string value to int */
struct String2IntMap
{
  const char * key;
  int          value;
  bool         skip_list;
};

static const String2IntMap flagsOptions[]=
  {
    {OPT_CLIENT_COMPRESS,			CLIENT_COMPRESS, false},
    {OPT_CLIENT_FOUND_ROWS,		CLIENT_FOUND_ROWS, false},
    {OPT_CLIENT_IGNORE_SIGPIPE,	CLIENT_IGNORE_SIGPIPE, false},
    {OPT_CLIENT_IGNORE_SPACE,		CLIENT_IGNORE_SPACE, false},
    {OPT_CLIENT_INTERACTIVE,		CLIENT_INTERACTIVE, false},
    {OPT_CLIENT_LOCAL_FILES,		CLIENT_LOCAL_FILES, false},
    {OPT_CLIENT_MULTI_STATEMENTS,	CLIENT_MULTI_STATEMENTS, false},
    {OPT_CLIENT_NO_SCHEMA,		CLIENT_NO_SCHEMA, false}
  };

/* {{{ readFlag(::sql::SQLString, int= 0) -I- */
/** Check if connection option pointed by map iterator defines a connection
    flag */
static bool read_connection_flag(ConnectOptionsMap::const_iterator &cit, unsigned long &flags)
{
  const bool * value;

  for (size_t i = 0; i < sizeof(flagsOptions)/sizeof(String2IntMap); ++i) {

    if (!cit->first.compare(flagsOptions[i].key)) {

      try {
        value = (cit->second).get< bool >();
      } catch (sql::InvalidArgumentException&) {
        std::ostringstream msg;
        msg << "Wrong type passed for " << flagsOptions[i].key <<
            " expected bool";
        throw sql::InvalidArgumentException(msg.str());
      }
      if (!value) {
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
    {OPT_REPORT_DATA_TRUNCATION,  MYSQL_REPORT_DATA_TRUNCATION, false},
    {OPT_ENABLE_CLEARTEXT_PLUGIN, MYSQL_ENABLE_CLEARTEXT_PLUGIN, false},
    {OPT_CAN_HANDLE_EXPIRED_PASSWORDS, MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS, true},
    {OPT_CONNECT_ATTR_RESET,      MYSQL_OPT_CONNECT_ATTR_RESET, true},
    {OPT_RECONNECT,               MYSQL_OPT_RECONNECT, true},
#if MYCPPCONN_STATIC_MYSQL_VERSION_ID < 80000
    {"sslVerify",                   MYSQL_OPT_SSL_VERIFY_SERVER_CERT, false}, // Deprecated
    {"sslEnforce",                  MYSQL_OPT_SSL_ENFORCE, false} // Deprecated
#else
    {"sslVerify",                   MYSQL_OPT_SSL_MODE, true}, // Deprecated
    {"sslEnforce",                  MYSQL_OPT_SSL_MODE, true}, // Deprecated
    {OPT_GET_SERVER_PUBLIC_KEY,   MYSQL_OPT_GET_SERVER_PUBLIC_KEY, false},
    {OPT_OPTIONAL_RESULTSET_METADATA, MYSQL_OPT_OPTIONAL_RESULTSET_METADATA, false},
#endif

  };
/* Array for mapping of integer connection options to mysql_options call */
static const String2IntMap intOptions[]=
  {
    {OPT_CONNECT_TIMEOUT,     MYSQL_OPT_CONNECT_TIMEOUT, false},
    {OPT_READ_TIMEOUT,        MYSQL_OPT_READ_TIMEOUT, false},
    {OPT_WRITE_TIMEOUT,       MYSQL_OPT_WRITE_TIMEOUT, false},
    {OPT_LOCAL_INFILE,        MYSQL_OPT_LOCAL_INFILE, false},
#if MYCPPCONN_STATIC_MYSQL_VERSION_ID >= 50700
    {OPT_MAX_ALLOWED_PACKET,  MYSQL_OPT_MAX_ALLOWED_PACKET, false},
    {OPT_NET_BUFFER_LENGTH,   MYSQL_OPT_NET_BUFFER_LENGTH, false},
#endif
    {OPT_SSL_MODE,            MYSQL_OPT_SSL_MODE    , false},
#if MYCPPCONN_STATIC_MYSQL_VERSION_ID >= 80000
    {OPT_RETRY_COUNT,         MYSQL_OPT_RETRY_COUNT, false},
#endif
  };
/* Array for mapping of string connection options to mysql_options call */
static const String2IntMap stringOptions[]=
  {
    {OPT_INIT_COMMAND,         MYSQL_INIT_COMMAND, false},
    {OPT_SSL_KEY,              MYSQL_OPT_SSL_KEY, true},
    {OPT_SSL_CERT,             MYSQL_OPT_SSL_CERT, true},
    {OPT_SSL_CA,               MYSQL_OPT_SSL_CA, true},
    {OPT_SSL_CAPATH,           MYSQL_OPT_SSL_CAPATH, true},
    {OPT_SSL_CIPHER,           MYSQL_OPT_SSL_CIPHER, true},
    {OPT_SSL_CRL,              MYSQL_OPT_SSL_CRL, false},
    {OPT_SSL_CRLPATH,          MYSQL_OPT_SSL_CRLPATH, false},
    {OPT_SERVER_PUBLIC_KEY,    MYSQL_SERVER_PUBLIC_KEY, false},
    {OPT_SET_CHARSET_DIR,      MYSQL_SET_CHARSET_DIR, false},
    {OPT_PLUGIN_DIR,           MYSQL_PLUGIN_DIR, false},
    {OPT_DEFAULT_AUTH,         MYSQL_DEFAULT_AUTH, false},
    {OPT_CONNECT_ATTR_DELETE,  MYSQL_OPT_CONNECT_ATTR_DELETE, false},
    {OPT_READ_DEFAULT_GROUP,   MYSQL_READ_DEFAULT_GROUP, false},
    {OPT_READ_DEFAULT_FILE,    MYSQL_READ_DEFAULT_FILE, false},
    {OPT_CHARSET_NAME,         MYSQL_SET_CHARSET_NAME, true},
#if MYCPPCONN_STATIC_MYSQL_VERSION_ID >= 50700
    {OPT_TLS_VERSION,          MYSQL_OPT_TLS_VERSION, false},
#endif
    {OPT_LOAD_DATA_LOCAL_DIR, MYSQL_OPT_LOAD_DATA_LOCAL_DIR, false}
  };


//Option conversion for libmysqlclient < 80011

inline
::sql::mysql::MySQL_Connection_Options option_conversion(unsigned long client_version, int option)
{
  #if MYCPPCONN_STATIC_MYSQL_VERSION_ID >= 80000
  if(client_version <80011)
  {
    switch (option) {
    case MYSQL_OPT_GET_SERVER_PUBLIC_KEY:
      option = MYSQL_OPT_RETRY_COUNT;
      break;
    case MYSQL_OPT_RETRY_COUNT:
      option = MYSQL_OPT_GET_SERVER_PUBLIC_KEY;
      break;
    }
  }
  #endif

  return static_cast<::sql::mysql::MySQL_Connection_Options>(option);
}


template<class T>
bool process_connection_option(ConnectOptionsMap::const_iterator &option,
                const String2IntMap options_map[],
                size_t map_size,
                std::shared_ptr< NativeAPI::NativeConnectionWrapper > &proxy)
{
  const T * value;

  for (size_t i = 0; i < map_size; ++i) {

    if (!option->first.compare(options_map[i].key) && !options_map[i].skip_list) {
      try {
        value = (option->second).get<T>();
      } catch (sql::InvalidArgumentException&) {
        std::ostringstream msg;
        msg << "Wrong type passed for " << options_map[i].key <<
            " expected " << typeid(value).name();
        throw sql::InvalidArgumentException(msg.str());
      }

      if (!value) {
        sql::SQLString err("Option ");
        err.append(option->first).append(" is not of expected type");
        throw sql::InvalidArgumentException(err);
      }

      try {
        proxy->options(option_conversion(
                         proxy->get_client_version(),
                         options_map[i].value),
                       *value);
      } catch (sql::InvalidArgumentException& e) {
        std::string errorOption(options_map[i].key);
        throw ::sql::SQLUnsupportedOptionException(e.what(), errorOption);
      }
      return true;
    }
  }

  return false;
}


bool get_connection_option(const sql::SQLString optionName,
                void *optionValue,
                const String2IntMap options_map[],
                size_t map_size,
                std::shared_ptr< NativeAPI::NativeConnectionWrapper > &proxy)
{
  for (size_t i = 0; i < map_size; ++i) {
    if (!optionName.compare(options_map[i].key)) {
      try {
        proxy->get_option(option_conversion(
                            proxy->get_client_version(),
                            options_map[i].value),
                          optionValue);
      } catch (sql::InvalidArgumentException& e) {
        std::string errorOption(options_map[i].key);
        throw ::sql::SQLUnsupportedOptionException(e.what(), errorOption);
      }
      return true;
    }
  }
  return false;
}

struct Prio
{
  uint16_t prio;
  uint16_t weight;
  operator uint16_t() const
  {
    return prio;
  }

  bool operator < (const Prio &other) const
  {
    return prio < other.prio;
  }
};

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
  - sslEnforce (deprecated)
  - sslVerify (deprecated)
  - sslCRL
  - sslCRLPath
  - useLegacyAuth
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
  - OPT_DNS_SRV
  - OPT_CHARSET_NAME
  - OPT_REPORT_DATA_TRUNCATION
  - OPT_CAN_HANDLE_EXPIRED_PASSWORDS
  - OPT_ENABLE_CLEARTEXT_PLUGIN
  - OPT_LOCAL_INFILE
  - OPT_CONNECT_ATTR_ADD
  - OPT_CONNECT_ATTR_DELETE
  - OPT_CONNECT_ATTR_RESET
  - OPT_RETRY_COUNT,
  - OPT_GET_SERVER_PUBLIC_KEY,
  - OPT_OPTIONAL_RESULTSET_METADATA
  - OPT
  - preInit
  - postInit
  - rsaKey
  - charsetDir
  - pluginDir
  - defaultAuth
  - readDefaultGroup
  - readDefaultFile

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
  MySQL_Uri::Host_data host;


  sql::SQLString userName;
  sql::SQLString password;
  sql::SQLString defaultCharset("utf8mb4");
  sql::SQLString characterSetResults("utf8mb4");

  sql::SQLString sslKey, sslCert, sslCA, sslCAPath, sslCipher, postInit;
  bool ssl_used = false;
  unsigned long flags = CLIENT_MULTI_RESULTS;

  const int * p_i;
  const bool * p_b;
  const sql::SQLString * p_s;
  bool opt_reconnect = false;
  int  client_exp_pwd = false;
  bool opt_dns_srv = false;
  bool opt_multi_host = false;
#if MYCPPCONN_STATIC_MYSQL_VERSION_ID < 80000
  bool secure_auth= true;
#endif

  sql::ConnectOptionsMap::const_iterator it;

  /* Port from options must be set as default for all hosts where port
     is not specified */
  {
    it = properties.find("port");

    if (it != properties.end())	{
      try {
        p_i = (it->second).get< int >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for port expected int");
      }
      if (p_i) {
        uri.setDefaultPort(*p_i);
      } else {
        throw sql::InvalidArgumentException("No long long value passed for port");
      }
    }
  }

  /* Values set in properties individually should have priority over those
     we restore from Uri */
  {
    it = properties.find("hostName");

    if (it != properties.end())	{
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for userName expected sql::SQLString");
      }
      if (p_s) {
        /*
          Parsing uri prior to processing all parameters, so indivudually
          specified parameters precede over those in the uri
        */
        if(!parseUri(*p_s, uri))
          throw sql::InvalidArgumentException("Invalid hostname URI");

      } else {
        throw sql::InvalidArgumentException("No string value passed for hostName");
      }
    }
  }

  /*
    Note: We set pluginDir option early because other options that are plugin
    specific might require loading plugins before they can be set.
  */

  {
    it = properties.find(OPT_PLUGIN_DIR);

    if (it != properties.end()) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for pluginDir expected sql::SQLString");
      }
      if (p_s) {
        proxy->options(sql::mysql::MYSQL_PLUGIN_DIR, *p_s);
      }
      else {
        throw sql::InvalidArgumentException("No string value passed for pluginDir");
      }
    }
  }

#define PROCESS_CONN_OPTION(option_type, options_map) \
  process_connection_option< option_type >(it, options_map, sizeof(options_map)/sizeof(String2IntMap), proxy)

  for (it = properties.begin(); it != properties.end(); ++it) {
    if (!it->first.compare(OPT_USERNAME)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for userName expected sql::SQLString");
      }
      if (p_s) {
        userName = *p_s;
      } else {
        throw sql::InvalidArgumentException("No string value passed for userName");
      }
    } else if (!it->first.compare(OPT_PASSWORD)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for password expected sql::SQLString");
      }
      if (p_s) {
        password = *p_s;
      } else {
        throw sql::InvalidArgumentException("No string value passed for password");
      }
    } else if (!it->first.compare(OPT_PASSWORD1)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for password1 expected sql::SQLString");
      }
      if (p_s) {
        int num = 1;
        proxy->options(sql::mysql::MYSQL_OPT_USER_PASSWORD, num, *p_s);
      } else {
        throw sql::InvalidArgumentException("No string value passed for password1");
      }
    } else if (!it->first.compare(OPT_PASSWORD2)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for password2 expected sql::SQLString");
      }
      if (p_s) {
        int num = 2;
        proxy->options(sql::mysql::MYSQL_OPT_USER_PASSWORD, num, *p_s);
      } else {
        throw sql::InvalidArgumentException("No string value passed for password2");
      }
    } else if (!it->first.compare(OPT_PASSWORD3)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for password3 expected sql::SQLString");
      }
      if (p_s) {
        int num = 3;
        proxy->options(sql::mysql::MYSQL_OPT_USER_PASSWORD, num, *p_s);
      } else {
        throw sql::InvalidArgumentException("No string value passed for password3");
      }
    } else if (!it->first.compare(OPT_PORT)) {
      try {
        p_i = (it->second).get< int >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for port expected int");
      }
      if (p_i) {
        uri.setDefaultPort(*p_i);
      } else {
        throw sql::InvalidArgumentException("No long long value passed for port");
      }
    } else if (!it->first.compare(OPT_SOCKET)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for socket expected sql::SQLString");
      }
      if (p_s) {
        host.setSocket(*p_s);
        uri.setHost(host);
      } else {
        throw sql::InvalidArgumentException("No string value passed for socket");
      }
    } else if (!it->first.compare(OPT_PIPE)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for pipe expected sql::SQLString");
      }
      if (p_s) {
        host.setPipe(*p_s);
        uri.setHost(host);
      } else {
        throw sql::InvalidArgumentException("No string value passed for pipe");
      }
    } else if (!it->first.compare(OPT_SCHEMA)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for schema expected sql::SQLString");
      }
      if (p_s) {
        uri.setSchema(*p_s);
      } else {
        throw sql::InvalidArgumentException("No string value passed for schema");
      }
    } else if (!it->first.compare(OPT_CHARACTER_SET_RESULTS)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for characterSetResults expected sql::SQLString");
      }
      if (p_s) {
        characterSetResults = *p_s;
      } else {
        throw sql::InvalidArgumentException("No string value passed for characterSetResults");
      }
    } else if (!it->first.compare(OPT_SSL_KEY)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for sslKey expected sql::SQLString");
      }
      if (p_s) {
        sslKey = *p_s;
      } else {
        throw sql::InvalidArgumentException("No string value passed for sslKey");
      }
      ssl_used = true;
    } else if (!it->first.compare(OPT_SSL_CERT)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for sslCert expected sql::SQLString");
      }
      if (p_s) {
        sslCert = *p_s;
      } else {
        throw sql::InvalidArgumentException("No string value passed for sslCert");
      }
      ssl_used = true;
    } else if (!it->first.compare(OPT_SSL_CA)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for sslCA expected sql::SQLString");
      }
      if (p_s) {
        sslCA = *p_s;
      } else {
        throw sql::InvalidArgumentException("No string value passed for sslCA");
      }
      ssl_used = true;
    } else if (!it->first.compare(OPT_SSL_CAPATH)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for sslCAPath expected sql::SQLString");
      }
      if (p_s) {
        sslCAPath = *p_s;
      } else {
        throw sql::InvalidArgumentException("No string value passed for sslCAPath");
      }
      ssl_used = true;
    } else if (!it->first.compare(OPT_SSL_CIPHER)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for sslCipher expected sql::SQLString");
      }
      if (p_s) {
        sslCipher = *p_s;
      } else {
        throw sql::InvalidArgumentException("No string value passed for sslCipher");
      }
      ssl_used = true;
    } else if (!it->first.compare(OPT_DEFAULT_STMT_RESULT_TYPE)) {
      try {
        p_i = (it->second).get< int >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for defaultStatementResultType expected sql::SQLString");
      }
      if (!p_i) {
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
      try {
        p_i = (it->second).get< int >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for defaultPreparedStatementResultType expected sql::SQLString");
      }
      if (!(p_i)) {
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
    } else if (!it->first.compare(OPT_METADATA_INFO_SCHEMA)) {
      try {
        p_b = (it->second).get<bool>();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for metadataUseInfoSchema expected bool");
      }
      if (p_b) {
        intern->metadata_use_info_schema = *p_b;
      } else {
        throw sql::InvalidArgumentException("No bool value passed for metadataUseInfoSchema");
      }
    } else if (!it->first.compare(OPT_RECONNECT)) {
      try {
        p_b = (it->second).get<bool>();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for OPT_RECONNECT expected bool");
      }
      if (!(p_b)) {
        throw sql::InvalidArgumentException("No bool value passed for OPT_RECONNECT");
      }
      opt_reconnect = true;
      intern->reconnect= *p_b;
    } else if (!it->first.compare(OPT_DNS_SRV)) {
      try {
        p_b = (it->second).get<bool>();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for OPT_DNS_SRV, expected bool");
      }
      if (!(p_b)) {
        throw sql::InvalidArgumentException("No bool value passed for OPT_DNS_SRV");
      }
      opt_dns_srv = *p_b;
    } else if (!it->first.compare(OPT_MULTI_HOST)) {
      try {
        p_b = (it->second).get<bool>();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for OPT_MULTI_HOST, expected bool");
      }
      if (!(p_b)) {
        throw sql::InvalidArgumentException("No bool value passed for OPT_MULTI_HOST");
      }
      opt_multi_host = *p_b;
    } else if (!it->first.compare(OPT_CHARSET_NAME)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for OPT_CHARSET_NAME expected sql::SQLString");
      }
      if (!p_s) {
        throw sql::InvalidArgumentException("No SQLString value passed for OPT_CHARSET_NAME");
      }
      defaultCharset = *p_s;
    } else if (!it->first.compare(OPT_NAMED_PIPE)) {
      /* Not sure it is really needed */
      host.setProtocol(NativeAPI::PROTOCOL_PIPE);
      uri.setHost(host);
    } else if (!it->first.compare(OPT_CAN_HANDLE_EXPIRED_PASSWORDS)) {
      try {
        p_b = (it->second).get<bool>();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for OPT_CAN_HANDLE_EXPIRED_PASSWORDS expected bool");
      }
      if (!(p_b)) {
        throw sql::InvalidArgumentException("No bool value passed for "
                          "OPT_CAN_HANDLE_EXPIRED_PASSWORDS");
      }
      try {
        client_exp_pwd= proxy->options(MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS, (const char*)p_b);
      } catch (sql::InvalidArgumentException& e) {
        std::string errorOption("MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS");
        throw ::sql::SQLUnsupportedOptionException(e.what(), errorOption);
      }
    } else if (!it->first.compare(OPT_POST_INIT_COMMAND)) {
      try {
        p_s = (it->second).get< sql::SQLString >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for postInit expected sql::SQLString");
      }
      if (p_s) {
        postInit= *p_s;
      } else {
        throw sql::InvalidArgumentException("No string value passed for postInit");
      }
    } else if (!it->first.compare(OPT_LEGACY_AUTH)) {
      try {
        p_b = (it->second).get< bool >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for useLegacyAuth expected sql::SQLString");
      }
  #if MYCPPCONN_STATIC_MYSQL_VERSION_ID < 80000
      if (p_b) {
        secure_auth= !*p_b;
      } else {
        throw sql::InvalidArgumentException("No bool value passed for useLegacyAuth");
      }
  #endif
    } else if (!it->first.compare(OPT_CONNECT_ATTR_ADD)) {
      const std::map< sql::SQLString, sql::SQLString > *conVal;
      try {
        conVal= (it->second).get< std::map< sql::SQLString, sql::SQLString > >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for OPT_CONNECT_ATTR_ADD expected std::map< sql::SQLString, sql::SQLString >");
      }
      std::map< sql::SQLString, sql::SQLString >::const_iterator conn_attr_it;
      for (conn_attr_it = conVal->begin(); conn_attr_it != conVal->end(); conn_attr_it++) {
        try {
          proxy->options(sql::mysql::MYSQL_OPT_CONNECT_ATTR_ADD, conn_attr_it->first, conn_attr_it->second);
        } catch (sql::InvalidArgumentException& e) {
          std::string errorOption("MYSQL_OPT_CONNECT_ATTR_ADD");
          throw ::sql::SQLUnsupportedOptionException(e.what(), errorOption);
        }
      }
    } else if (!it->first.compare(OPT_CONNECT_ATTR_DELETE)) {
      const std::list< sql::SQLString > *conVal;
      try {
        conVal= (it->second).get< std::list< sql::SQLString > >();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for OPT_CONNECT_ATTR_DELETE expected std::list< sql::SQLString >");
      }
      std::list< sql::SQLString >::const_iterator conn_attr_it;
      for (conn_attr_it = conVal->begin(); conn_attr_it != conVal->end(); conn_attr_it++) {
        try {
          proxy->options(MYSQL_OPT_CONNECT_ATTR_DELETE, *conn_attr_it);
        } catch (sql::InvalidArgumentException& e) {
          std::string errorOption("MYSQL_OPT_CONNECT_ATTR_DELETE");
          throw ::sql::SQLUnsupportedOptionException(e.what(), errorOption);
        }
      }
    } else if (!it->first.compare(OPT_CONNECT_ATTR_RESET)) {
      proxy->options(MYSQL_OPT_CONNECT_ATTR_RESET, 0);

  #if MYCPPCONN_STATIC_MYSQL_VERSION_ID > 80000

    //Deprecated
    } else if (!it->first.compare("sslVerify")) {

      ssl_mode ssl_mode_val = (it->second).get< bool >() ? SSL_MODE_VERIFY_CA
                                            : SSL_MODE_PREFERRED;
      proxy->options(MYSQL_OPT_SSL_MODE, &ssl_mode_val);

    //Deprecated
    } else if (!it->first.compare("sslEnforce")) {
      ssl_mode ssl_mode_val = (it->second).get< bool >() ? SSL_MODE_REQUIRED
                                                          : SSL_MODE_PREFERRED;
      proxy->options(MYSQL_OPT_SSL_MODE, &ssl_mode_val);

  #endif
    } else if (!it->first.compare(OPT_OCI_CONFIG_FILE)) {
      try {
        p_s= (it->second).get<sql::SQLString>();
      } catch (sql::InvalidArgumentException&) {
        throw sql::InvalidArgumentException("Wrong type passed for OPT_OCI_CONFIG_FILE. Expected sql::SQLString.");
      }

      try {
        proxy->plugin_option(MYSQL_CLIENT_AUTHENTICATION_PLUGIN,
                                "authentication_oci_client",
                                "oci-config-file",
                                *p_s);
      }  catch (sql::InvalidArgumentException &e) {
        throw ::sql::SQLUnsupportedOptionException(
          "Failed to set config file for authentication_oci_client plugin",
          OPT_OCI_CONFIG_FILE
        );
      }

    }
    else if (!it->first.compare(OPT_PLUGIN_DIR)) {
      // Nothing to do here: this option was handeld before the loop

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

  for(auto h : uri)
  {

    // Throwing in case of wrong protocol
#ifdef _WIN32
    if (h.Protocol() == NativeAPI::PROTOCOL_SOCKET) {
      throw sql::InvalidArgumentException("Invalid for this platform protocol requested(MYSQL_PROTOCOL_SOCKET)");
    }
#else
    if (h.Protocol() == NativeAPI::PROTOCOL_PIPE) {
      throw sql::InvalidArgumentException("Invalid for this platform protocol requested(MYSQL_PROTOCOL_PIPE)");
    }
#endif

  }

#if MYCPPCONN_STATIC_MYSQL_VERSION_ID < 80000
  try {
    proxy->options(MYSQL_SECURE_AUTH, &secure_auth);
  } catch (sql::InvalidArgumentException& e) {
    std::string errorOption("MYSQL_SECURE_AUTH");
    throw ::sql::SQLUnsupportedOptionException(e.what(), errorOption);
  }
#endif

  try {
    proxy->options(MYSQL_SET_CHARSET_NAME, defaultCharset.c_str());
  } catch (sql::InvalidArgumentException& e) {
    std::string errorOption("MYSQL_SET_CHARSET_NAME");
    throw ::sql::SQLUnsupportedOptionException(e.what(), errorOption);
  }


  if (ssl_used) {
    /* According to the docs, always returns 0 */
    proxy->ssl_set(sslKey, sslCert, sslCA, sslCAPath, sslCipher.c_str());
  }

  /*
    Workaround for libmysqlclient... if OPT_TLS_VERSION or ssl_set is used,
    it overwrites OPT_SSL_MODE... setting it again.
  */

  it = properties.find(OPT_SSL_MODE);

  if (it != properties.end())
  {
     PROCESS_CONN_OPTION(int, intOptions);
  }

  if(!opt_multi_host && uri.size() > 1)
     throw sql::InvalidArgumentException("Missing option OPT_MULTI_HOST = true");

  if(opt_dns_srv && uri.size() > 1)
    throw sql::InvalidArgumentException("Specifying multiple hostnames with DNS SRV look up is not allowed.");

  CPP_INFO_FMT("OPT_DNS_SRV=%d", opt_dns_srv);

  auto connect = [this,flags,client_exp_pwd, opt_dns_srv](
                 const std::string &host,
                 const std::string &user,
                 const std::string &pwd,
                 const std::string &schema,
                 uint16_t port,
                 const std::string &socketOrPipe)
  {
    CPP_INFO_FMT("hostName=%s", host.c_str());
    CPP_INFO_FMT("user=%s", user.c_str());
    CPP_INFO_FMT("port=%d", port);
    CPP_INFO_FMT("schema=%s", schema.c_str());
    CPP_INFO_FMT("socket/pipe=%s", socketOrPipe.c_str());

    bool connect_result = !opt_dns_srv ?
                            proxy->connect(host, user, pwd, schema, port,
                                           socketOrPipe, flags)
                            :
                            proxy->connect_dns_srv(host, user, pwd,
                                                   schema, flags);

    if (!connect_result)
    {
      CPP_ERR_FMT("Couldn't connect : %d", proxy->errNo());
      CPP_ERR_FMT("Couldn't connect : (%s)", proxy->sqlstate().c_str());
      CPP_ERR_FMT("Couldn't connect : %s", proxy->error().c_str());
      CPP_ERR_FMT("Couldn't connect : %d:(%s) %s", proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());

      /* If error is "Password has expired" and application supports it while
         mysql client lib does not */
      std::string error_message;
      unsigned int native_error= proxy->errNo();

      if (native_error == ER_MUST_CHANGE_PASSWORD_LOGIN
          && client_exp_pwd) {

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
      throw e;
    }
  };

  uint16_t total_weight = 0;

  if(opt_dns_srv)
  {
    if(uri.size() > 1)
    {
      throw sql::InvalidArgumentException("Using more than one host with DNS SRV lookup is not allowed.");
    }

    if(uri.size() ==0)
    {
      throw sql::InvalidArgumentException("No hostname specified for DNS SRV lookup.");
    }

    host = *uri.begin();

    if(host.Protocol() == NativeAPI::PROTOCOL_SOCKET)
    {
      throw sql::InvalidArgumentException("Using Unix domain sockets with DNS SRV lookup is not allowed.");
    }

    if(host.Protocol() == NativeAPI::PROTOCOL_PIPE)
    {
      throw sql::InvalidArgumentException("Using pipe with DNS SRV lookup is not allowed.");
    }

    if(host.hasPort())
    {
      throw sql::InvalidArgumentException("Specifying a port number with DNS SRV lookup is not allowed.");
    }

  }

  //Connect loop
  {
    bool connected = false;
    std::random_device rd;
    std::mt19937 generator(rd());
    int error=0;
    std::string sql_state;

    while(uri.size() && !connected)
    {
      std::uniform_int_distribution<int> distribution(
            0, uri.size() - 1); // define the range of random numbers

      int pos = distribution(generator);
      auto el = uri.begin();

      std::advance(el, pos);
      proxy->use_protocol(el->Protocol());

      try {
        connect(el->Host(), userName,
                password,
                uri.Schema() /* schema */,
                el->hasPort() ?  el->Port() : uri.DefaultPort(),
                el->SocketOrPipe());
        connected = true;
        break;
      }
      catch (sql::SQLException& e)
      {
        error = e.getErrorCode();
        sql_state = e.getSQLState();
        switch (error)
        {
        case ER_CON_COUNT_ERROR:
        case CR_SOCKET_CREATE_ERROR:
        case CR_CONNECTION_ERROR:
        case CR_CONN_HOST_ERROR:
        case CR_IPSOCK_ERROR:
        case CR_UNKNOWN_HOST:
          //On Network errors, continue
          break;
        default:
          //If SQLSTATE not 08xxx, which is used for network errors
          if(e.getSQLState().compare(0,2, "08") != 0)
          {
            //Re-throw error and do not try another host
            throw;
          }
        }

      }

      uri.erase(el);

    };

    if(!connected)
    {
      std::stringstream err;
      if(opt_dns_srv)
        err << "Unable to connect to any of the hosts of " << host.Host() << " SRV";
      else if (uri.size() >1) {
        err << "Unable to connect to any of the hosts";
      }
      else {
        switch(host.Protocol())
        {
        case NativeAPI::PROTOCOL_SOCKET:
        case NativeAPI::PROTOCOL_PIPE:
          err << "Unable to connect to " << host.SocketOrPipe() ;
          break;
        default:
          err << "Unable to connect to " << host.Host() << ":" << host.Port();
          break;
        }
      }
      proxy.reset();
      throw sql::SQLException(err.str(), sql_state, error);
    }
  }



  if (opt_reconnect) {
    try {
      proxy->options(MYSQL_OPT_RECONNECT, (const char *) &intern->reconnect);
    } catch (sql::InvalidArgumentException& e) {
      std::string errorOption("MYSQL_OPT_RECONNECT");
      throw ::sql::SQLUnsupportedOptionException(e.what(), errorOption);
    }
  }

  setAutoCommit(true);
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
  if(proxy->commit())
    throw SQLException (proxy->error(), proxy->sqlstate(), proxy->errNo());

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

#define GET_CONN_OPTION(option_type, option_value, options_map) \
get_connection_option(option_type, option_value, options_map, sizeof(options_map)/sizeof(String2IntMap), proxy)

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
  } else if (!optionName.compare("multiByteMinLength")) {
    MY_CHARSET_INFO cs;
    proxy->get_character_set_info(&cs);
    *(static_cast<int *>(optionValue)) = cs.mbminlen;
  } else if (!optionName.compare("multiByteMaxLength")) {
    MY_CHARSET_INFO cs;
    proxy->get_character_set_info(&cs);
    *(static_cast<int *>(optionValue)) = cs.mbmaxlen;
  /* mysql_get_option() was added in mysql 5.7.3 version */
  } else if ( proxy->get_server_version() >= 50703 ) {
    try {
      if (GET_CONN_OPTION(optionName, optionValue, intOptions)) {
        return;
      } else if (GET_CONN_OPTION(optionName, optionValue, booleanOptions)) {
        return;
      } else if (GET_CONN_OPTION(optionName, optionValue, stringOptions)) {
        return;
      }
    } catch (sql::SQLUnsupportedOptionException& e) {
      CPP_ERR_FMT("Unsupported option : %d:(%s) %s", proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());
      throw e;
    }
  }
}
/* }}} */


/* {{{ MySQL_Connection::getClientOption() -I- */
sql::SQLString
MySQL_Connection::getClientOption(const sql::SQLString & optionName)
{
  CPP_ENTER_WL(intern->logger, "MySQL_Connection::getClientOption");

  if (!optionName.compare("characterSetResults")) {
    return sql::SQLString(getSessionVariable("character_set_results"));
  } else if (!optionName.compare("characterSetDirectory")) {
    MY_CHARSET_INFO cs;
    proxy->get_character_set_info(&cs);
    return cs.dir ? sql::SQLString(cs.dir) : "";
  } else if ( proxy->get_server_version() >= 50703 ) {
    const char* optionValue= NULL;
    if (GET_CONN_OPTION(optionName, &optionValue, stringOptions)) {
      return optionValue ? sql::SQLString(optionValue) : "";
    }
  }
  return "";
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
    return false;
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
  } else if (!optionName.compare(OPT_LOAD_DATA_LOCAL_DIR))
  {
    proxy->options(MYSQL_OPT_LOAD_DATA_LOCAL_DIR, optionValue);
  }
  return this;
}
/* }}} */


/* {{{ MySQL_Connection::setClientOption() -I- */
sql::Connection *
MySQL_Connection::setClientOption(const sql::SQLString & optionName, const sql::SQLString & optionValue)
{
  CPP_ENTER_WL(intern->logger, "MySQL_Connection::setClientOption");
  if (!optionName.compare("characterSetResults")) {
    setSessionVariable("character_set_results", optionValue);
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
  sql::SQLString q("SELECT @@");
  q.append(varname);

  boost::scoped_ptr< sql::ResultSet > rset(service->executeQuery(q));

  if (rset->next()) {
    if (intern->cache_sql_mode && intern->sql_mode_set == false && !varname.compare("sql_mode")) {
      intern->sql_mode = rset->getString(1);
      intern->sql_mode_set = true;
    }
    return rset->getString(1);
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

  sql::SQLString query("SET @@");
  query.append(varname).append("=");

  if (!value.compare("NULL")) {
    query.append("NULL");
  } else {
    query.append("'").append(value).append("'");
  }

  service->executeUpdate(query);
  if (intern->cache_sql_mode && !strncasecmp(varname.c_str(), "sql_mode", sizeof("sql_mode") - 1)) {
    intern->sql_mode= value;
  }
}
/* }}} */


/* {{{ MySQL_Connection::setSessionVariable() -I- */
void
MySQL_Connection::setSessionVariable(const sql::SQLString & varname, unsigned int value)
{
  CPP_ENTER_WL(intern->logger, "MySQL_Connection::setSessionVariable");
  checkClosed();

  sql::SQLString query("SET @@");
  query.append(varname).append("=");

  if (!value) {
    query.append("0");
  } else {
    std::ostringstream qstr;
    qstr << value;
    query.append(qstr.str());
  }

  service->executeUpdate(query);
}
/* }}} */


/* {{{ MySQL_Connection::isValid() -I- */
bool
MySQL_Connection::isValid()
{
   CPP_ENTER_WL(intern->logger, "MySQL_Connection::isValid");
   bool is_active= false;
   if (intern->is_valid) {
     if (intern->reconnect) {
       bool opt_reconnect_value= false;
       try {
           proxy->options(MYSQL_OPT_RECONNECT, (const char *) &opt_reconnect_value);
       } catch (sql::InvalidArgumentException& e) {
           std::string errorOption("MYSQL_OPT_RECONNECT");
           throw ::sql::SQLUnsupportedOptionException(e.what(), errorOption);
       }

       is_active= proxy->ping();

       opt_reconnect_value= true;
       try {
           proxy->options(MYSQL_OPT_RECONNECT, (const char *) &opt_reconnect_value);
       } catch (sql::InvalidArgumentException& e) {
           std::string errorOption("MYSQL_OPT_RECONNECT");
           throw ::sql::SQLUnsupportedOptionException(e.what(), errorOption);
       }

       if (is_active == 0) {
           return true;
       }
     } else {
       if (!proxy->ping()) {
           return true;
       }
     }
   }
   return false;
}
/* }}} */


/* {{{ MySQL_Connection::reconnect() -I- */
bool
MySQL_Connection::reconnect()
{
   CPP_ENTER_WL(intern->logger, "MySQL_Connection::reconnect");
   bool is_active= false;
   if (intern->is_valid) {
     if (intern->reconnect) {
       if (!proxy->ping()) {
           return true;
       }
     } else {
       bool opt_reconnect_value= true;
       try {
         proxy->options(MYSQL_OPT_RECONNECT, (const char *) &opt_reconnect_value);
       } catch (sql::InvalidArgumentException& e) {
         std::string errorOption("MYSQL_OPT_RECONNECT");
         throw ::sql::SQLUnsupportedOptionException(e.what(), errorOption);
       }

       is_active= proxy->ping();

       opt_reconnect_value= false;
       try {
         proxy->options(MYSQL_OPT_RECONNECT, (const char *) &opt_reconnect_value);
       } catch (sql::InvalidArgumentException& e) {
         std::string errorOption("MYSQL_OPT_RECONNECT");
         throw ::sql::SQLUnsupportedOptionException(e.what(), errorOption);
       }

       if (is_active == 0) {
           return true;
       }
     }
   }
   return false;
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

