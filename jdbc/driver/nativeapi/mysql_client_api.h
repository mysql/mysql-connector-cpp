/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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



#ifndef _MYSQL_LIBMYSQL_API_H_
#define _MYSQL_LIBMYSQL_API_H_

#include "mysql_private_iface.h"
#include <boost/shared_ptr.hpp>

namespace sql
{
class SQLString;

namespace mysql
{
namespace NativeAPI
{

typedef my_ulonglong (STDCALL *ptr2mysql_affected_rows)(MYSQL *);

typedef my_bool (STDCALL *ptr2mysql_autocommit)(MYSQL *, my_bool);

typedef void (STDCALL *ptr2mysql_close)(MYSQL *mysql);

typedef my_bool (STDCALL *ptr2mysql_commit)(MYSQL *mysql);

typedef void (STDCALL *ptr2mysql_data_seek)(MYSQL_RES *, my_ulonglong);

typedef void (STDCALL *ptr2mysql_debug)(const char *debug);

typedef unsigned int (STDCALL *ptr2mysql_errno)(MYSQL *mysql);

typedef const char * (STDCALL *ptr2mysql_error)(MYSQL *mysql);

typedef MYSQL_FIELD * (STDCALL *ptr2mysql_fetch_field)(MYSQL_RES *);

typedef MYSQL_FIELD * (STDCALL *ptr2mysql_fetch_field_direct)(MYSQL_RES *, unsigned int);

typedef unsigned long * (STDCALL *ptr2mysql_fetch_lengths)(MYSQL_RES *);

typedef MYSQL_ROW (STDCALL *ptr2mysql_fetch_row)(MYSQL_RES *);

typedef unsigned int (STDCALL *ptr2mysql_field_count)(MYSQL *);

typedef void (STDCALL *ptr2mysql_free_result)(MYSQL_RES *);

typedef unsigned long (STDCALL *ptr2mysql_get_client_version)();

typedef const char * (STDCALL *ptr2mysql_get_server_info)(MYSQL *);

typedef unsigned long (STDCALL *ptr2mysql_get_server_version)(MYSQL *);

typedef void (STDCALL *ptr2mysql_get_character_set_info)(MYSQL *, void *);

typedef char * (STDCALL *ptr2mysql_info)(MYSQL *mysql);

typedef MYSQL * (STDCALL *ptr2mysql_init)(MYSQL *mysql);

typedef int (STDCALL *ptr2mysql_library_init)(int, char **, char **);

typedef void (STDCALL *ptr2mysql_library_end)(void);

typedef my_bool (STDCALL *ptr2mysql_more_results)(MYSQL *);

typedef int (STDCALL *ptr2mysql_next_result)(MYSQL *);

typedef unsigned int (STDCALL *ptr2mysql_num_fields)(MYSQL_RES *);

typedef my_ulonglong (STDCALL *ptr2mysql_num_rows)(MYSQL_RES *);

typedef int (STDCALL *ptr2mysql_options)(MYSQL *, enum mysql_option, const void *);

typedef int (STDCALL *ptr2mysql_options4)(MYSQL *, enum mysql_option, const void *, const void *);

typedef int (STDCALL *ptr2mysql_get_option)(MYSQL *, enum mysql_option, const void *);

typedef st_mysql_client_plugin* (*ptr2mysql_client_find_plugin)(MYSQL *, const char *, int);

typedef int (*ptr2mysql_plugin_options)(st_mysql_client_plugin*, const char *, const void *);

typedef int (*ptr2mysql_plugin_get_option)(st_mysql_client_plugin*, const char *, void *);

typedef int (STDCALL *ptr2mysql_query)(MYSQL *, const char *);

typedef int (STDCALL *ptr2mysql_ping)(MYSQL *);

typedef MYSQL * (STDCALL *ptr2mysql_real_connect)(MYSQL *, const char *, const char *, const char * , const char *, unsigned int, const char *, unsigned long);

typedef MYSQL * (STDCALL *ptr2mysql_real_connect_dns_srv)(MYSQL *, const char *, const char *, const char * , const char *, unsigned long);

typedef bool (STDCALL *ptr2mysql_bind_param)(MYSQL *mysql, unsigned n_params, MYSQL_BIND *binds, const char **names);

typedef unsigned long (STDCALL *ptr2mysql_real_escape_string)(MYSQL * mysql, char *, const char *, unsigned long);

typedef int (STDCALL *ptr2mysql_real_query)(MYSQL *, const char *, unsigned long);

typedef my_bool (STDCALL *ptr2mysql_rollback)(MYSQL *mysql);

typedef const char * (STDCALL *ptr2mysql_sqlstate)(MYSQL *mysql);

typedef my_bool (STDCALL *ptr2mysql_ssl_set)(MYSQL *, const char *, const char *, const char *, const char *, const char *);

typedef MYSQL_RES * (STDCALL *ptr2mysql_store_result)(MYSQL *);

typedef MYSQL_RES * (STDCALL *ptr2mysql_use_result)(MYSQL *);

typedef unsigned int (STDCALL *ptr2mysql_warning_count)(MYSQL *);

/* Prepared Statement stmt_* functions */
typedef my_ulonglong (STDCALL *ptr2mysql_stmt_affected_rows)(MYSQL_STMT *);

typedef my_bool (STDCALL *ptr2mysql_stmt_attr_set)(MYSQL_STMT *, enum enum_stmt_attr_type, const void *);

typedef my_bool (STDCALL *ptr2mysql_stmt_bind_param)(MYSQL_STMT *, MYSQL_BIND *);

typedef my_bool (STDCALL *ptr2mysql_stmt_bind_result)(MYSQL_STMT *, MYSQL_BIND *);

typedef my_bool (STDCALL *ptr2mysql_stmt_close)(MYSQL_STMT *);

typedef void (STDCALL *ptr2mysql_stmt_data_seek)(MYSQL_STMT *, my_ulonglong);

typedef unsigned int (STDCALL *ptr2mysql_stmt_errno)(MYSQL_STMT *);

typedef const char * (STDCALL *ptr2mysql_stmt_error)(MYSQL_STMT *);

typedef int (STDCALL *ptr2mysql_stmt_execute)(MYSQL_STMT *);

typedef int (STDCALL *ptr2mysql_stmt_fetch)(MYSQL_STMT *);

typedef unsigned int (STDCALL *ptr2mysql_stmt_field_count)(MYSQL_STMT *);

typedef MYSQL_STMT * (STDCALL *ptr2mysql_stmt_init)(MYSQL *);

typedef my_ulonglong (STDCALL *ptr2mysql_stmt_num_rows)(MYSQL_STMT *);

typedef unsigned long (STDCALL *ptr2mysql_stmt_param_count)(MYSQL_STMT *);

typedef int (STDCALL *ptr2mysql_stmt_prepare)(MYSQL_STMT *, const char *, unsigned long);

typedef MYSQL_RES * (STDCALL *ptr2mysql_stmt_result_metadata)(MYSQL_STMT *);

typedef my_bool (STDCALL *ptr2mysql_stmt_send_long_data)(MYSQL_STMT *, unsigned int, const char *, unsigned long);

typedef const char * (STDCALL *ptr2mysql_stmt_sqlstate)(MYSQL_STMT *);

typedef int (STDCALL *ptr2mysql_stmt_store_result)(MYSQL_STMT *);

typedef int (STDCALL *ptr2mysql_stmt_next_result)(MYSQL_STMT *);

typedef bool (STDCALL *ptr2mysql_stmt_free_result)(MYSQL_STMT *);

typedef void (STDCALL *ptr2mysql_thread_init)();

typedef void (STDCALL *ptr2mysql_thread_end)();

/*
 * Interface MySQL C-API wrapper class should implement.
 * At the moment we must have at least 2 implementation - for static and dynamic
 * mysql client library linking
 */
class IMySQLCAPI
{
public:
  virtual my_ulonglong affected_rows(MYSQL *) = 0;

  virtual my_bool autocommit(MYSQL *, my_bool) = 0;

  virtual void close(MYSQL *mysql) = 0;

  virtual my_bool commit(MYSQL *mysql) = 0;

  virtual void data_seek(MYSQL_RES *, my_ulonglong) = 0;

  virtual void debug(const char *) = 0;

  virtual unsigned int mysql_errno(MYSQL *mysql) = 0;

  virtual const char * error(MYSQL *mysql) = 0;

  virtual MYSQL_FIELD * fetch_field(MYSQL_RES *) = 0;

  virtual MYSQL_FIELD * fetch_field_direct (MYSQL_RES *, unsigned int) = 0;

  virtual unsigned long * fetch_lengths(MYSQL_RES * ) = 0;

  virtual MYSQL_ROW fetch_row(MYSQL_RES * ) = 0;

  virtual unsigned int field_count(MYSQL *) = 0;

  virtual void free_result(MYSQL_RES * ) = 0;

  virtual unsigned long get_client_version() = 0;

  virtual const char * get_server_info(MYSQL *) = 0;

  virtual unsigned long get_server_version(MYSQL *) = 0;

  virtual void get_character_set_info(MYSQL *, void *) = 0;

  virtual const char * info(MYSQL *mysql) = 0;

  virtual MYSQL * init(MYSQL *mysql) = 0;

  virtual int library_init(int argc, char **argv, char **groups) = 0;

  virtual void library_end() = 0;

  virtual my_bool more_results(MYSQL *) = 0;

  virtual int next_result(MYSQL *) = 0;

  virtual unsigned int num_fields(MYSQL_RES * ) = 0;

  virtual my_ulonglong num_rows(MYSQL_RES * ) = 0;

  virtual int options(MYSQL *, enum mysql_option option , const void *arg) = 0;

  virtual int options(MYSQL *, enum mysql_option option , const void *arg1, const void *arg2) = 0;

  virtual int get_option(MYSQL *, enum mysql_option option , const void *arg) = 0;

  virtual st_mysql_client_plugin* client_find_plugin(MYSQL *, const char *plugin_name, int plugin_type) = 0;

  virtual int plugin_options(st_mysql_client_plugin*, const char *option, const void *value) = 0;

  virtual int plugin_get_option(st_mysql_client_plugin*, const char *option, void *value) =0;

  virtual int ping(MYSQL *) = 0;

  virtual int query(MYSQL *, const char *) = 0;

  virtual MYSQL * real_connect(MYSQL * mysql,
                const char *  host,
                const char *  user,
                const char *  passwd,
                const char *  db,
                unsigned int  port,
                const char *  unix_socket,
                unsigned long client_flag) = 0;

  virtual MYSQL * real_connect_dns_srv(MYSQL * mysql,
                const char *  host,
                const char *  user,
                const char *  passwd,
                const char *  db,
                unsigned long client_flag) = 0;

  virtual bool bind_param(MYSQL *mysql, unsigned n_params,
                          MYSQL_BIND *binds, const char **names) = 0;

  virtual unsigned long real_escape_string(MYSQL * mysql, char * to, const char * from, unsigned long length) = 0;

  virtual int real_query(MYSQL *, const char *, unsigned long) = 0;

  virtual my_bool rollback(MYSQL *) = 0;

  virtual const char * sqlstate(MYSQL *) = 0;

  virtual my_bool ssl_set(MYSQL * mysql,
              const char * key,
              const char * cert,
              const char * ca,
              const char * capath,
              const char * cipher) = 0;

  virtual MYSQL_RES * store_result(MYSQL *) = 0;

  virtual MYSQL_RES * use_result(MYSQL *) = 0;

  virtual unsigned int warning_count(MYSQL *) = 0;

  /* Methods - wrappers of prepared statement stmt_* functions */
  virtual my_ulonglong  stmt_affected_rows (MYSQL_STMT *) = 0;

  virtual my_bool stmt_attr_set(MYSQL_STMT *stmt, enum enum_stmt_attr_type option , const void *arg) = 0;

  virtual my_bool stmt_bind_param(MYSQL_STMT *, MYSQL_BIND *) = 0;

  virtual my_bool stmt_bind_result(MYSQL_STMT *, MYSQL_BIND *) = 0;

  virtual my_bool stmt_close(MYSQL_STMT *) = 0;

  virtual void stmt_data_seek(MYSQL_STMT *, my_ulonglong) = 0;

  virtual unsigned int stmt_errno(MYSQL_STMT *) = 0;

  virtual const char * stmt_error(MYSQL_STMT *) = 0;

  virtual int stmt_execute(MYSQL_STMT *) = 0;

  virtual int stmt_fetch(MYSQL_STMT *) = 0;

  virtual unsigned int stmt_field_count(MYSQL_STMT *) = 0;

  virtual MYSQL_STMT * stmt_init(MYSQL *) = 0;

  virtual my_ulonglong stmt_num_rows(MYSQL_STMT *) = 0;

  virtual unsigned long stmt_param_count(MYSQL_STMT *) = 0;

  virtual int stmt_prepare(MYSQL_STMT *, const char *, unsigned long) = 0;

  virtual MYSQL_RES * stmt_result_metadata(MYSQL_STMT *) = 0;

  virtual my_bool stmt_send_long_data(MYSQL_STMT * stmt , unsigned int par_number, const char * data, unsigned long len) = 0;

  virtual const char *  stmt_sqlstate(MYSQL_STMT *) = 0;

  virtual int stmt_store_result(MYSQL_STMT *) = 0;

  virtual int stmt_next_result(MYSQL_STMT *) = 0;

  virtual bool stmt_free_result(MYSQL_STMT *) = 0;

  virtual void thread_end() = 0;

  virtual void thread_init() = 0;
};

boost::shared_ptr<IMySQLCAPI> getCApiHandle(const sql::SQLString & name);

} /* namespace NativeAPI */
} /* namespace mysql */
} /* namespace sql*/

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
