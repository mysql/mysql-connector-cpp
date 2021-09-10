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



#ifndef _MYSQL_LIBMYSQL_LOADER_H_
#define _MYSQL_LIBMYSQL_LOADER_H_

#include <cppconn/config.h>
#include <cppconn/sqlstring.h>

#include "mysql_client_api.h"
#include "library_loader.h"

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

class LibmysqlDynamicProxy : public sql::mysql::util::LibraryLoader, public IMySQLCAPI
{
private:
  void init_loader();

public:

  LibmysqlDynamicProxy();

  LibmysqlDynamicProxy(const LibmysqlDynamicProxy &);

  LibmysqlDynamicProxy(const SQLString & path2libFile);
  LibmysqlDynamicProxy(const SQLString & dir2look, const SQLString & libFileName);

  virtual ~LibmysqlDynamicProxy();


  // MySQL C-API calls wrappers

  my_ulonglong affected_rows(MYSQL *) override;

  my_bool autocommit(MYSQL * , my_bool) override;

  void close (MYSQL *mysql) override;

  my_bool commit(MYSQL *mysql) override;

  void data_seek(MYSQL_RES *, my_ulonglong) override;

  void debug(const char *) override;

  unsigned int mysql_errno(MYSQL *) override;

  const char *  error(MYSQL *) override;

  MYSQL_FIELD * fetch_field(MYSQL_RES *) override;

  MYSQL_FIELD * fetch_field_direct(MYSQL_RES *, unsigned int) override;

  unsigned long * fetch_lengths(MYSQL_RES *) override;

  MYSQL_ROW fetch_row(MYSQL_RES *) override;

  unsigned int field_count(MYSQL *) override;

  void free_result(MYSQL_RES *) override;

  unsigned long get_client_version() override;

  const char *  get_server_info(MYSQL *) override;

  unsigned long get_server_version(MYSQL *) override;

  void get_character_set_info(MYSQL *, void *) override;

  const char * info(MYSQL *) override;

  MYSQL * init(MYSQL *mysql) override;

  int library_init(int, char **, char **) override;

  void library_end() override;

  my_bool more_results(MYSQL *) override;

  int next_result(MYSQL *) override;

  unsigned int num_fields(MYSQL_RES *) override;

  my_ulonglong num_rows(MYSQL_RES *) override;

  int options (MYSQL *, enum mysql_option, const void *) override;

  int options (MYSQL *, enum mysql_option, const void *, const void *) override;

  int get_option (MYSQL *, enum mysql_option, const void *) override;

  st_mysql_client_plugin* client_find_plugin(MYSQL *, const char *plugin_name, int plugin_type) override;

  int plugin_options(st_mysql_client_plugin*, const char *option, const void *value) override;

  int plugin_get_option(st_mysql_client_plugin*, const char *option, void *value) override;

  int ping(MYSQL *) override;

  int query(MYSQL *, const char *) override;

  MYSQL * real_connect(MYSQL * mysql, const char *  host,
              const char *  user,
              const char *  passwd,
              const char *  db,
              unsigned int  port,
              const char *  unix_socket,
              unsigned long client_flag) override;

  MYSQL * real_connect_dns_srv(MYSQL * mysql,
              const char *  host,
              const char *  user,
              const char *  passwd,
              const char *  db,
              unsigned long client_flag) override;

  bool bind_param(MYSQL *mysql, unsigned n_params,
                  MYSQL_BIND *binds, const char **names) override;


  unsigned long real_escape_string(MYSQL * mysql, char * to, const char * from, unsigned long length) override;

  int real_query(MYSQL *, const char *, unsigned long) override;

  my_bool rollback(MYSQL *) override;

  const char * sqlstate(MYSQL *) override;

  my_bool ssl_set(MYSQL * mysql,
          const char * key,
          const char * cert,
          const char * ca,
          const char * capath,
          const char * cipher) override;

  MYSQL_RES * store_result(MYSQL *) override;

  MYSQL_RES * use_result(MYSQL *) override;

  unsigned int warning_count(MYSQL *) override;

  /* Prepared Statement stmt_* functions */
  my_ulonglong  stmt_affected_rows  (MYSQL_STMT *) override;

  my_bool stmt_attr_set(MYSQL_STMT *, enum enum_stmt_attr_type, const void *) override;

  my_bool stmt_bind_param(MYSQL_STMT *, MYSQL_BIND *) override;

  my_bool stmt_bind_result(MYSQL_STMT *, MYSQL_BIND *) override;

  my_bool stmt_close(MYSQL_STMT *) override;

  void stmt_data_seek(MYSQL_STMT *, my_ulonglong) override;

  unsigned int stmt_errno(MYSQL_STMT *) override;

  const char * stmt_error(MYSQL_STMT *) override;

  int stmt_execute(MYSQL_STMT *) override;

  int stmt_fetch(MYSQL_STMT *) override;

  unsigned int stmt_field_count(MYSQL_STMT *) override;

  MYSQL_STMT * stmt_init(MYSQL *) override;

  my_ulonglong stmt_num_rows(MYSQL_STMT *) override;

  unsigned long stmt_param_count(MYSQL_STMT *) override;

  int stmt_prepare(MYSQL_STMT *, const char *, unsigned long) override;

  MYSQL_RES * stmt_result_metadata(MYSQL_STMT *) override;

  my_bool stmt_send_long_data (MYSQL_STMT * , unsigned int, const char *, unsigned long) override;

  const char * stmt_sqlstate(MYSQL_STMT *) override;

  int stmt_store_result(MYSQL_STMT *) override;

  int stmt_next_result(MYSQL_STMT *) override;

  bool stmt_free_result(MYSQL_STMT *) override;

  void thread_init() override;

  void thread_end() override;
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
