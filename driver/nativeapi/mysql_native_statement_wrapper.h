/*
  Copyright (c) 2009, 2010, Oracle and/or its affiliates. All rights reserved.

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

#ifndef _MYSQL_NativeStatementWrapper_H_
#define _MYSQL_NativeStatementWrapper_H_


#include <boost/shared_ptr.hpp>

#include "native_statement_wrapper.h"

struct st_mysql_stmt;

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

	boost::shared_ptr<IMySQLCAPI>	api;
	::st_mysql_stmt *				stmt;
	NativeConnectionWrapper *		conn;

	MySQL_NativeStatementWrapper(){}

public:
	MySQL_NativeStatementWrapper(::st_mysql_stmt *, boost::shared_ptr<IMySQLCAPI>, NativeConnectionWrapper * connProxy);
	~MySQL_NativeStatementWrapper();

	uint64_t affected_rows();

	bool attr_set(MySQL_Statement_Options option, const void *arg);

	bool bind_param(::st_mysql_bind *);

	bool bind_result(::st_mysql_bind *);

	void data_seek(uint64_t );

	unsigned int errNo();

	sql::SQLString error();

	int execute ();

	int fetch();

	unsigned int field_count();

	bool more_results();

	int next_result();

	uint64_t num_rows();

	unsigned long param_count();

	int prepare (const ::sql::SQLString &);

	NativeResultsetWrapper * result_metadata();

	bool send_long_data(unsigned int par_number, const char * data, unsigned long len);

	::sql::SQLString sqlstate();

	int store_result();
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
