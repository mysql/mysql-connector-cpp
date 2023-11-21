/*
 * Copyright (c) 2009, 2022, Oracle and/or its affiliates. All rights reserved.
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



#ifndef _MYSQL_NativeStatementWrapper_H_
#define _MYSQL_NativeStatementWrapper_H_

#include <memory>

#include "native_statement_wrapper.h"

#if (MYCPPCONN_STATIC_MYSQL_VERSION_ID > 80004)
struct MYSQL_STMT;
#else
struct st_mysql_stmt;
#define MYSQL_STMT st_mysql_stmt
#endif

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

class IMySQLCAPI;
class NativeConnectionWrapper;
/*
 *
 *
 *
 */
class MySQL_NativeStatementWrapper : public NativeStatementWrapper
{
  std::shared_ptr<IMySQLCAPI> api;
  ::MYSQL_STMT *				stmt;
  NativeConnectionWrapper *		conn;

  MySQL_NativeStatementWrapper(){}

public:
 MySQL_NativeStatementWrapper(::MYSQL_STMT *, std::shared_ptr<IMySQLCAPI>,
                              NativeConnectionWrapper *connProxy);
 ~MySQL_NativeStatementWrapper();

 unsigned long server_version() override;

 uint64_t affected_rows() override;

 bool attr_set(MySQL_Statement_Options option, const void *arg) override;

 bool bind_named_param(::MYSQL_BIND *, unsigned, const char **) override;

 bool bind_param(::MYSQL_BIND *) override;

 bool bind_result(::MYSQL_BIND *) override;

 void data_seek(uint64_t) override;

 unsigned int errNo() override;

 sql::SQLString error() override;

 int execute() override;

 int fetch() override;

 unsigned int field_count() override;

 bool more_results() override;

 int next_result() override;

 uint64_t num_rows() override;

 unsigned long param_count() override;

 int prepare(const ::sql::SQLString &) override;

 NativeResultsetWrapper *result_metadata() override;

 bool send_long_data(unsigned int par_number, const char *data,
                     unsigned long len) override;

 ::sql::SQLString sqlstate() override;

 int store_result() override;

 int stmt_next_result() override;

 bool stmt_free_result() override;

 unsigned int warning_count() override;
};


}
}
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
