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



#ifndef _NATIVE_STATEMENT_WRAPPER_H_
#define _NATIVE_STATEMENT_WRAPPER_H_

#include <cppconn/config.h>
#include <boost/noncopyable.hpp>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include "../mysql_statement_options.h"
#include <cppconn/version_info.h>

#if (MYCPPCONN_STATIC_MYSQL_VERSION_ID > 80004)
struct MYSQL_BIND;
struct MYSQL_RES;
#else
struct st_mysql_bind;
struct st_mysql_res;
#define MYSQL_BIND st_mysql_bind
#define MYSQL_RES st_mysql_res
#endif

namespace sql
{
class SQLString;

namespace mysql
{
namespace NativeAPI
{
class NativeResultsetWrapper;
/*
 *
 *
 *
 */
class NativeStatementWrapper : public boost::noncopyable
{
public:

  virtual ~NativeStatementWrapper(){}

  virtual uint64_t affected_rows() = 0;

  virtual bool attr_set(MySQL_Statement_Options attr, const void *arg) = 0;

  virtual bool bind_param(::MYSQL_BIND *) = 0;

  virtual bool bind_result(::MYSQL_BIND *) = 0;

  virtual void data_seek(uint64_t) = 0;

  virtual unsigned int errNo() = 0;

  virtual ::sql::SQLString error() = 0;

  virtual int execute() = 0;

  virtual int fetch() = 0;

  virtual unsigned int field_count() = 0;

  virtual bool more_results() = 0;

  virtual int next_result() = 0;

  virtual uint64_t num_rows() = 0;

  virtual unsigned long param_count() = 0;

  virtual int prepare(const ::sql::SQLString &) = 0;

  virtual NativeResultsetWrapper * result_metadata () = 0;

  virtual bool send_long_data(unsigned int par_number, const char * data, unsigned long len) = 0;

  virtual ::sql::SQLString sqlstate() = 0;

  virtual int store_result() = 0;

  virtual int stmt_next_result() = 0;

  virtual bool stmt_free_result() = 0;

  /* some enhancements comparing to mysql api */
  virtual unsigned int warning_count() = 0;
};


} /* namespace NativeAPI */
} /* namespace mysql */
} /* namespace sql */

#endif /* _NATIVE_STATEMENT_WRAPPER_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
