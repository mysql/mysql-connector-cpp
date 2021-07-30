/*
 * Copyright (c) 2009, 2020, Oracle and/or its affiliates.
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



#include <cppconn/sqlstring.h>
#include <cppconn/exception.h>

#include <boost/scoped_array.hpp>

#include "../mysql_util.h"
#include "../mysql_connection_options.h"

#include "mysql_client_api.h"
#include "mysql_native_resultset_wrapper.h"
#include "mysql_native_statement_wrapper.h"

#include "mysql_native_connection_wrapper.h"


namespace sql
{
namespace mysql
{
namespace NativeAPI
{

/*
  Function to convert sql::mysql::MySQL_Connection_Options to
  libmysqlclient mysql_option
*/
mysql_option
get_mysql_option(sql::mysql::MySQL_Connection_Options opt)
{
  switch(opt)
  {
  case sql::mysql::MYSQL_OPT_CONNECT_TIMEOUT: return ::MYSQL_OPT_CONNECT_TIMEOUT;
  case sql::mysql::MYSQL_OPT_COMPRESS: return ::MYSQL_OPT_COMPRESS;
  case sql::mysql::MYSQL_OPT_NAMED_PIPE: return ::MYSQL_OPT_NAMED_PIPE;
  case sql::mysql::MYSQL_INIT_COMMAND: return ::MYSQL_INIT_COMMAND;
  case sql::mysql::MYSQL_READ_DEFAULT_FILE: return ::MYSQL_READ_DEFAULT_FILE;
  case sql::mysql::MYSQL_READ_DEFAULT_GROUP: return ::MYSQL_READ_DEFAULT_GROUP;
  case sql::mysql::MYSQL_SET_CHARSET_DIR: return ::MYSQL_SET_CHARSET_DIR;
  case sql::mysql::MYSQL_SET_CHARSET_NAME: return ::MYSQL_SET_CHARSET_NAME;
  case sql::mysql::MYSQL_OPT_LOCAL_INFILE: return ::MYSQL_OPT_LOCAL_INFILE;
  case sql::mysql::MYSQL_OPT_PROTOCOL: return ::MYSQL_OPT_PROTOCOL;
  case sql::mysql::MYSQL_SHARED_MEMORY_BASE_NAME: return ::MYSQL_SHARED_MEMORY_BASE_NAME;
  case sql::mysql::MYSQL_OPT_READ_TIMEOUT: return ::MYSQL_OPT_READ_TIMEOUT;
  case sql::mysql::MYSQL_OPT_WRITE_TIMEOUT: return ::MYSQL_OPT_WRITE_TIMEOUT;
  case sql::mysql::MYSQL_OPT_USE_RESULT: return ::MYSQL_OPT_USE_RESULT;
  case sql::mysql::MYSQL_REPORT_DATA_TRUNCATION: return ::MYSQL_REPORT_DATA_TRUNCATION;
  case sql::mysql::MYSQL_OPT_RECONNECT: return ::MYSQL_OPT_RECONNECT;
  case sql::mysql::MYSQL_PLUGIN_DIR: return ::MYSQL_PLUGIN_DIR;
  case sql::mysql::MYSQL_DEFAULT_AUTH: return ::MYSQL_DEFAULT_AUTH;
  case sql::mysql::MYSQL_OPT_BIND: return ::MYSQL_OPT_BIND;
  case sql::mysql::MYSQL_OPT_SSL_KEY: return ::MYSQL_OPT_SSL_KEY;
  case sql::mysql::MYSQL_OPT_SSL_CERT: return ::MYSQL_OPT_SSL_CERT;
  case sql::mysql::MYSQL_OPT_SSL_CA: return ::MYSQL_OPT_SSL_CA;
  case sql::mysql::MYSQL_OPT_SSL_CAPATH: return ::MYSQL_OPT_SSL_CAPATH;
  case sql::mysql::MYSQL_OPT_SSL_CIPHER: return ::MYSQL_OPT_SSL_CIPHER;
  case sql::mysql::MYSQL_OPT_SSL_CRL: return ::MYSQL_OPT_SSL_CRL;
  case sql::mysql::MYSQL_OPT_SSL_CRLPATH: return ::MYSQL_OPT_SSL_CRLPATH;
  case sql::mysql::MYSQL_OPT_CONNECT_ATTR_RESET: return ::MYSQL_OPT_CONNECT_ATTR_RESET;
  case sql::mysql::MYSQL_OPT_CONNECT_ATTR_ADD: return ::MYSQL_OPT_CONNECT_ATTR_ADD;
  case sql::mysql::MYSQL_OPT_CONNECT_ATTR_DELETE: return ::MYSQL_OPT_CONNECT_ATTR_DELETE;
  case sql::mysql::MYSQL_SERVER_PUBLIC_KEY: return ::MYSQL_SERVER_PUBLIC_KEY;
  case sql::mysql::MYSQL_ENABLE_CLEARTEXT_PLUGIN: return ::MYSQL_ENABLE_CLEARTEXT_PLUGIN;
  case sql::mysql::MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS: return ::MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS;
#if MYCPPCONN_STATIC_MYSQL_VERSION_ID >= 50700
  case sql::mysql::MYSQL_OPT_MAX_ALLOWED_PACKET: return ::MYSQL_OPT_MAX_ALLOWED_PACKET;
  case sql::mysql::MYSQL_OPT_NET_BUFFER_LENGTH: return ::MYSQL_OPT_NET_BUFFER_LENGTH;
  case sql::mysql::MYSQL_OPT_TLS_VERSION: return ::MYSQL_OPT_TLS_VERSION;
  case sql::mysql::MYSQL_OPT_SSL_MODE: return ::MYSQL_OPT_SSL_MODE;
#endif


#if MYCPPCONN_STATIC_MYSQL_VERSION_ID >= 80000
  case sql::mysql::MYSQL_OPT_RETRY_COUNT: return ::MYSQL_OPT_RETRY_COUNT;
  case sql::mysql::MYSQL_OPT_GET_SERVER_PUBLIC_KEY: return ::MYSQL_OPT_GET_SERVER_PUBLIC_KEY;
  case sql::mysql::MYSQL_OPT_OPTIONAL_RESULTSET_METADATA: return ::MYSQL_OPT_OPTIONAL_RESULTSET_METADATA;
  case sql::mysql::MYSQL_OPT_SSL_FIPS_MODE: return ::MYSQL_OPT_SSL_FIPS_MODE;
  case sql::mysql::MYSQL_OPT_TLS_CIPHERSUITES: return ::MYSQL_OPT_TLS_CIPHERSUITES;
  case sql::mysql::MYSQL_OPT_COMPRESSION_ALGORITHMS: return ::MYSQL_OPT_COMPRESSION_ALGORITHMS;
  case sql::mysql::MYSQL_OPT_ZSTD_COMPRESSION_LEVEL: return ::MYSQL_OPT_ZSTD_COMPRESSION_LEVEL;
  case sql::mysql::MYSQL_OPT_LOAD_DATA_LOCAL_DIR: return ::MYSQL_OPT_LOAD_DATA_LOCAL_DIR;
  case sql::mysql::MYSQL_OPT_USER_PASSWORD: return ::MYSQL_OPT_USER_PASSWORD;
#else
  case sql::mysql::MYSQL_OPT_SSL_VERIFY_SERVER_CERT: return ::MYSQL_OPT_SSL_VERIFY_SERVER_CERT;
  case sql::mysql::MYSQL_OPT_USE_REMOTE_CONNECTION: return ::MYSQL_OPT_USE_REMOTE_CONNECTION;
  case sql::mysql::MYSQL_OPT_USE_EMBEDDED_CONNECTION: return ::MYSQL_OPT_USE_EMBEDDED_CONNECTION;
  case sql::mysql::MYSQL_OPT_GUESS_CONNECTION: return ::MYSQL_OPT_GUESS_CONNECTION;
  case sql::mysql::MYSQL_SET_CLIENT_IP: return ::MYSQL_SET_CLIENT_IP;
  case sql::mysql::MYSQL_SECURE_AUTH: return ::MYSQL_SECURE_AUTH;
# if MYCPPCONN_STATIC_MYSQL_VERSION_ID >= 50700
  case sql::mysql::MYSQL_OPT_SSL_ENFORCE: return ::MYSQL_OPT_SSL_ENFORCE;
# endif
#endif

  }

  //return something, just for remove compiler warning
  //will never get here!
  return ::MYSQL_OPT_CONNECT_TIMEOUT;
}

/* {{{ MySQL_NativeConnectionWrapper::MySQL_NativeConnectionWrapper() */
MySQL_NativeConnectionWrapper::MySQL_NativeConnectionWrapper(boost::shared_ptr<IMySQLCAPI> _api)
  : api(_api), mysql(api->init(NULL))
{
  if (mysql == NULL) {
    throw sql::SQLException("Insufficient memory: cannot create MySQL handle using mysql_init()");
  }
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::~MySQL_NativeConnectionWrapper() */
MySQL_NativeConnectionWrapper::~MySQL_NativeConnectionWrapper()
{
  api->close(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::affected_rows() */
uint64_t
MySQL_NativeConnectionWrapper::affected_rows()
{
  return api->affected_rows(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::autocommit() */
bool
MySQL_NativeConnectionWrapper::autocommit(bool mode)
{
  return (api->autocommit(mysql, mode) != '\0');
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::connect() */
bool
MySQL_NativeConnectionWrapper::connect(const ::sql::SQLString & host,
                  const ::sql::SQLString & user,
                  const ::sql::SQLString & passwd,
                  const ::sql::SQLString & db,
                  unsigned int			 port,
                  const ::sql::SQLString & socket_or_pipe,
                  unsigned long			client_flag)
{
  return (NULL != api->real_connect(mysql, nullIfEmpty(host), user.c_str(),
                  nullIfEmpty(passwd),
                  nullIfEmpty(db), port,
                  nullIfEmpty(socket_or_pipe), client_flag));
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::connect_dns_srv() */
bool
MySQL_NativeConnectionWrapper::connect_dns_srv(const ::sql::SQLString & host,
                  const ::sql::SQLString & user,
                  const ::sql::SQLString & passwd,
                  const ::sql::SQLString & db,
                  unsigned long			client_flag)
{
  return (NULL != api->real_connect_dns_srv(mysql, nullIfEmpty(host), user.c_str(),
                  nullIfEmpty(passwd),
                  nullIfEmpty(db), client_flag));
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::commit() */
bool
MySQL_NativeConnectionWrapper::commit()
{
  return (api->commit(mysql) != '\0');
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::debug() */
void
MySQL_NativeConnectionWrapper::debug(const SQLString & debug)
{
  api->debug(debug.c_str());
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::errNo() */
unsigned int
MySQL_NativeConnectionWrapper::errNo()
{
  return api->mysql_errno(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::escapeString() */
SQLString
MySQL_NativeConnectionWrapper::escapeString(const SQLString & s)
{
  boost::scoped_array< char > buffer(new char[s.length() * 2 + 1]);
  if (!buffer.get()) {
    return "";
  }
  unsigned long return_len = api->real_escape_string(mysql, buffer.get(), s.c_str(), (unsigned long) s.length());
  return sql::SQLString(buffer.get(), return_len);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::error() */
SQLString
MySQL_NativeConnectionWrapper::error()
{
  return api->error(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::field_count() */
unsigned int
MySQL_NativeConnectionWrapper::field_count()
{
  return api->field_count(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::get_client_version() */
unsigned long
MySQL_NativeConnectionWrapper::get_client_version()
{
  return api->get_client_version();
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::get_server_info() */
const SQLString &
MySQL_NativeConnectionWrapper::get_server_info()
{
  serverInfo = api->get_server_info(mysql);
  return serverInfo;
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::get_server_version() */
unsigned long
MySQL_NativeConnectionWrapper::get_server_version()
{
  return api->get_server_version(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::get_character_set_info() */
void
MySQL_NativeConnectionWrapper::get_character_set_info(void *cs)
{
  return api->get_character_set_info(mysql, cs);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::more_results() */
bool
MySQL_NativeConnectionWrapper::more_results()
{
  return (api->more_results(mysql) != '\0');
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::next_result() */
int
MySQL_NativeConnectionWrapper::next_result()
{
  return api->next_result(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::options() */
int
MySQL_NativeConnectionWrapper::options(::sql::mysql::MySQL_Connection_Options option, const void * value)
{
  return api->options(mysql, get_mysql_option(option), value);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::options(SQLString &) */
int
MySQL_NativeConnectionWrapper::options(::sql::mysql::MySQL_Connection_Options option,
                     const ::sql::SQLString &str)
{
  return api->options(mysql,get_mysql_option(option), str.c_str());
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::options(bool &) */
int
MySQL_NativeConnectionWrapper::options(::sql::mysql::MySQL_Connection_Options option,
                     const bool &option_val)
{
  my_bool dummy= option_val ? '\1' : '\0';
  return api->options(mysql, get_mysql_option(option), &dummy);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::options(int &) */
int
MySQL_NativeConnectionWrapper::options(::sql::mysql::MySQL_Connection_Options option,
                     const int &option_val)
{
  return api->options(mysql, get_mysql_option(option), &option_val);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::options(SQLString &, SQLString &) */
int
MySQL_NativeConnectionWrapper::options(::sql::mysql::MySQL_Connection_Options option,
               const ::sql::SQLString &key, const ::sql::SQLString &value)
{
  return api->options(mysql, get_mysql_option(option), key.c_str(), value.c_str());
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::options(int &, SQLString &) */
int
MySQL_NativeConnectionWrapper::options(::sql::mysql::MySQL_Connection_Options option,
               const int &factor, const ::sql::SQLString &value)
{
  return api->options(mysql, get_mysql_option(option), &factor, value.c_str());
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::get_option() */
int
MySQL_NativeConnectionWrapper::get_option(::sql::mysql::MySQL_Connection_Options option, const void * value)
{
  return api->get_option(mysql, get_mysql_option(option), value);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::get_option(SQLString &) */
int
MySQL_NativeConnectionWrapper::get_option(::sql::mysql::MySQL_Connection_Options option,
                     const ::sql::SQLString &str)
{
  return api->get_option(mysql, get_mysql_option(option), str.c_str());
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::get_option(bool &) */
int
MySQL_NativeConnectionWrapper::get_option(::sql::mysql::MySQL_Connection_Options option,
                     const bool &option_val)
{
  my_bool dummy= option_val ? '\1' : '\0';
  return api->get_option(mysql, get_mysql_option(option), &dummy);
}


/* {{{ MySQL_NativeConnectionWrapper::get_option(int &) */
int
MySQL_NativeConnectionWrapper::get_option(::sql::mysql::MySQL_Connection_Options option,
                     const int &option_val)
{
  return api->get_option(mysql, get_mysql_option(option), &option_val);
}
/* }}} */

int
MySQL_NativeConnectionWrapper::plugin_option(
    int plugin_type,
    const ::sql::SQLString & plugin_name,
    const ::sql::SQLString & option,
    const ::sql::SQLString & value)
try{

  /* load client authentication plugin if required */
  struct st_mysql_client_plugin *plugin =
      api->client_find_plugin(mysql, plugin_name.c_str(), plugin_type);

  /* set option value in plugin */
  return api->plugin_options(plugin, option.c_str(), value.c_str());

}
catch(sql::InvalidArgumentException &e)
{
  std::string err(e.what());
  err+= " for plugin " + plugin_name;
  throw sql::InvalidArgumentException(err);
}

int MySQL_NativeConnectionWrapper::get_plugin_option(
    int plugin_type,
    const ::sql::SQLString & plugin_name,
    const ::sql::SQLString & option,
    const ::sql::SQLString & value)
{


  /* load client authentication plugin if required */
  struct st_mysql_client_plugin *plugin =
      api->client_find_plugin(mysql, plugin_name.c_str(),
                              plugin_type);

  /* get option value from plugin */
  return api->plugin_get_option(plugin, option.c_str(), (void*)value.c_str());
}


/* {{{ MySQL_NativeConnectionWrapper::has_query_attributes() */
bool MySQL_NativeConnectionWrapper::has_query_attributes()
{
  return mysql->server_capabilities & CLIENT_QUERY_ATTRIBUTES;
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::query_attr(unsigned ,const char **, MYSQL_BIND *) */
bool
MySQL_NativeConnectionWrapper::query_attr(unsigned nr_attr,const char **names, MYSQL_BIND *bind)
{
  return api->bind_param(mysql, nr_attr, bind, names);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::query() */
int
MySQL_NativeConnectionWrapper::query(const SQLString & stmt_str)
{
  return api->real_query(mysql, stmt_str.c_str(), static_cast<unsigned long>(stmt_str.length()));
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::ping() */
int
MySQL_NativeConnectionWrapper::ping()
{
  return api->ping(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::rollback() */
bool
MySQL_NativeConnectionWrapper::rollback()
{
  return (api->rollback(mysql) != '\0');
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::sqlstate() */
SQLString
MySQL_NativeConnectionWrapper::sqlstate()
{
  return api->sqlstate(mysql);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::sqlstate() */
SQLString
MySQL_NativeConnectionWrapper::info()
{
  const char * result= api->info(mysql);
  return (result ? result : "");
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::ssl_set() */
bool
MySQL_NativeConnectionWrapper::ssl_set(const SQLString & key,
                const SQLString & cert,
                const SQLString & ca,
                const SQLString & capath,
                const SQLString & cipher)
{
  return ('\0' != api->ssl_set(mysql, nullIfEmpty(key), nullIfEmpty(cert),
              nullIfEmpty(ca), nullIfEmpty(capath), nullIfEmpty(cipher)));
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::store_result() */
NativeResultsetWrapper *
MySQL_NativeConnectionWrapper::store_result()
{
  ::MYSQL_RES * raw= api->store_result(mysql);

  if (raw == NULL) {
    /*CPP_ERR_FMT("Error during %s_result : %d:(%s) %s", resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY? "use":"store",
      this->errNo(), this->sqlstate(), this->error());*/
    return NULL;
  }

  return new MySQL_NativeResultsetWrapper(raw, api);
}
/* }}} */


static const int protocolType2mysql[PROTOCOL_COUNT][2]= {
  {PROTOCOL_TCP,		MYSQL_PROTOCOL_TCP},
  {PROTOCOL_SOCKET,	MYSQL_PROTOCOL_SOCKET},
  {PROTOCOL_PIPE,		MYSQL_PROTOCOL_PIPE}
};
/* {{{ MySQL_NativeConnectionWrapper::use_protocol() */
int MySQL_NativeConnectionWrapper::use_protocol(Protocol_Type protocol)
{
  for (int i= 0; i< PROTOCOL_COUNT; ++i)
  {
    if (protocolType2mysql[i][0] == protocol)
      return options(MYSQL_OPT_PROTOCOL, (const char *)&protocolType2mysql[i][1]);
  }

  return -1;
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::use_result() */
NativeResultsetWrapper *
MySQL_NativeConnectionWrapper::use_result()
{
  ::MYSQL_RES * raw= api->use_result(mysql);

  if (raw == NULL) {
    /*CPP_ERR_FMT("Error during %s_result : %d:(%s) %s", resultset_type == sql::ResultSet::TYPE_FORWARD_ONLY? "use":"store",
              this->errNo(), this->sqlstate(), this->error());*/
    return NULL;
  }

  return new MySQL_NativeResultsetWrapper(raw, api);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::stmt_init() */
NativeStatementWrapper &
MySQL_NativeConnectionWrapper::stmt_init()
{
  ::MYSQL_STMT * raw= api->stmt_init(mysql);

  if (raw == NULL) {
    /*CPP_ERR_FMT("No statement : %d:(%s) %s", e->errNo(), proxy->sqlstate(), proxy->error());*/
    ::sql::mysql::util::throwSQLException(*this);
  }

  return *(new MySQL_NativeStatementWrapper(raw, api, this));
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::warning_count() */
unsigned int
MySQL_NativeConnectionWrapper::warning_count()
{
  return api->warning_count(mysql);
}
/* }}} */

} /* namespace NativeAPI */
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
