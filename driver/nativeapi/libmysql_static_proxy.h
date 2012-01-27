/*
Copyright (c) 2009, 2011, Oracle and/or its affiliates. All rights reserved.

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



#ifndef _MYSQL_LIBMYSQL_STATIC_H_
#define _MYSQL_LIBMYSQL_STATIC_H_

#include "mysql_client_api.h"
#include <cppconn/config.h>
#include <boost/noncopyable.hpp>

namespace sql
{
namespace mysql
{
namespace util
{
template <class T>
class Singleton: public boost::noncopyable
{
protected:
	Singleton(){}

public:

	static boost::shared_ptr<T> & theInstance()
	{
		//shared::ptr is used only to be able to use in getCApiHandle
		static boost::shared_ptr<T> instance(new T());

		return instance;
	}
};


// macros to use in private/protected part of singleton-ed class
#define CCPP_SINGLETON(classname) classname();\
	friend class sql::mysql::util::Singleton<classname>
}

namespace NativeAPI
{


class LibmysqlStaticProxy : public IMySQLCAPI, public ::sql::mysql::util::Singleton< LibmysqlStaticProxy >
{
private:

	CCPP_SINGLETON(LibmysqlStaticProxy);

public:

	virtual ~LibmysqlStaticProxy();

	// MySQL C-API calls wrappers

	my_ulonglong affected_rows(MYSQL * mysql);

	my_bool autocommit(MYSQL *, my_bool);

	void close(MYSQL * mysql);

	my_bool commit(MYSQL * mysql);

	void data_seek(MYSQL_RES *, my_ulonglong);

	void debug(const char *);

	unsigned int mysql_errno(MYSQL * mysql);

	const char * error(MYSQL * mysql);

	MYSQL_FIELD *fetch_field(MYSQL_RES *);

	MYSQL_FIELD *fetch_field_direct(MYSQL_RES *, unsigned int);

	unsigned long *fetch_lengths(MYSQL_RES *);

	MYSQL_ROW fetch_row(MYSQL_RES *);

	unsigned int field_count(MYSQL * mysql);

	void free_result(MYSQL_RES *);

	unsigned long get_client_version();

	const char * get_server_info(MYSQL * mysql);

	unsigned long get_server_version(MYSQL * mysql);

	const char * info(MYSQL * mysql);

	MYSQL * init(MYSQL * mysql);

	int library_init(int, char **, char **);

	void library_end();

	my_bool more_results(MYSQL * mysql);

	int next_result(MYSQL * mysql);

	unsigned int num_fields(MYSQL_RES *);

	my_ulonglong num_rows(MYSQL_RES *);

	int options(MYSQL *, enum mysql_option, const void *);

	int ping(MYSQL *);

	int query(MYSQL *, const char *);

	MYSQL * real_connect(MYSQL * mysql,
						const char *  host,
						const char *  user,
						const char *  passwd,
						const char *  db,
						unsigned int  port,
						const char *  unix_socket,
						unsigned long client_flag);

	unsigned long real_escape_string(MYSQL * mysql, char * to, const char * from, unsigned long length);

	int real_query(MYSQL *, const char *, unsigned long);

	my_bool rollback(MYSQL * mysql);

	const char * sqlstate(MYSQL * mysql);

	my_bool ssl_set(MYSQL * mysql,
					const char * key,
					const char * cert,
					const char * ca,
					const char * capath,
					const char * cipher);

	MYSQL_RES *  store_result(MYSQL * mysql);

	MYSQL_RES *  use_result(MYSQL * mysql);

  unsigned int warning_count(MYSQL * mysql);

	/* Prepared Statementstmt_* functions */
	my_ulonglong stmt_affected_rows(MYSQL_STMT *);

	my_bool stmt_attr_set(MYSQL_STMT *, enum enum_stmt_attr_type, const void *);

	my_bool stmt_bind_param(MYSQL_STMT *, MYSQL_BIND *);

	my_bool stmt_bind_result(MYSQL_STMT *, MYSQL_BIND *);

	my_bool stmt_close(MYSQL_STMT *);

	void stmt_data_seek(MYSQL_STMT *, my_ulonglong);

	unsigned int stmt_errno(MYSQL_STMT *);

	const char * stmt_error(MYSQL_STMT *);

	int stmt_execute(MYSQL_STMT *);

	int stmt_fetch(MYSQL_STMT *);

	unsigned int stmt_field_count(MYSQL_STMT *);

	MYSQL_STMT * stmt_init(MYSQL * mysql);

	my_ulonglong stmt_num_rows(MYSQL_STMT *);

	unsigned long stmt_param_count(MYSQL_STMT *);

	int stmt_prepare(MYSQL_STMT *, const char *, unsigned long);

	MYSQL_RES * stmt_result_metadata(MYSQL_STMT *);

	my_bool stmt_send_long_data(MYSQL_STMT *, unsigned int, const char *, unsigned long);

	const char * stmt_sqlstate(MYSQL_STMT *);

	int stmt_store_result(MYSQL_STMT *);

	void thread_init();

	void thread_end();
};

} /* namespace NativeAPI */
} /* namespace mysql */
} /* namespace sql */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
