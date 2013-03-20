/*
Copyright (c) 2009, 2013, Oracle and/or its affiliates. All rights reserved.

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



#include <sqlstring.h>
#include <exception.h>

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
	return api->options(mysql, static_cast< mysql_option >(option), value);
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::options(SQLString &) */
int
MySQL_NativeConnectionWrapper::options(::sql::mysql::MySQL_Connection_Options option,
									   const ::sql::SQLString &str)
{
	return api->options(mysql, static_cast< mysql_option >(option), str.c_str());
}
/* }}} */


/* {{{ MySQL_NativeConnectionWrapper::options(bool &) */
int
MySQL_NativeConnectionWrapper::options(::sql::mysql::MySQL_Connection_Options option,
									   const bool &option_val)
{
	my_bool dummy= option_val ? '\1' : '\0';
	return api->options(mysql, static_cast< mysql_option >(option), &dummy);
}


/* {{{ MySQL_NativeConnectionWrapper::options(int &) */
int
MySQL_NativeConnectionWrapper::options(::sql::mysql::MySQL_Connection_Options option,
									   const int &option_val)
{
	return api->options(mysql, static_cast< mysql_option >(option), &option_val);
}


/* {{{ MySQL_NativeConnectionWrapper::query() */
int
MySQL_NativeConnectionWrapper::query(const SQLString & stmt_str)
{
	return api->real_query(mysql, stmt_str.c_str(), stmt_str.length());
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
	::st_mysql_res * raw= api->store_result(mysql);

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
	::st_mysql_res * raw= api->use_result(mysql);

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
	::st_mysql_stmt * raw= api->stmt_init(mysql);

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
