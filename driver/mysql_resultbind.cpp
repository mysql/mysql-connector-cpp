/*
  Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

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


#include <cppconn/exception.h>
#include "mysql_util.h"
#include "mysql_debug.h"
#include "mysql_resultbind.h"

#include "nativeapi/native_statement_wrapper.h"
#include "nativeapi/native_resultset_wrapper.h"

#include <string.h>

namespace sql
{
namespace mysql
{

struct st_buffer_size_type
{
	char * buffer;
	size_t size;
	enum_field_types type;
	st_buffer_size_type(char * b, size_t s, enum_field_types t) : buffer(b), size(s), type(t) {}
};


/* {{{ allocate_buffer_for_field() -I- */
typedef std::pair<char *, size_t> BufferSizePair;
static struct st_buffer_size_type
allocate_buffer_for_field(const MYSQL_FIELD * const field)
{
	switch (field->type) {
		case MYSQL_TYPE_NULL:
			return st_buffer_size_type(NULL, 0, field->type);
		case MYSQL_TYPE_TINY:
			return st_buffer_size_type(new char[1], 1, field->type);
		case MYSQL_TYPE_SHORT:
			return st_buffer_size_type(new char[2], 2, field->type);
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_FLOAT:
			return st_buffer_size_type(new char[4], 4, field->type);
		case MYSQL_TYPE_DOUBLE:
		case MYSQL_TYPE_LONGLONG:
			return st_buffer_size_type(new char[8], 8, field->type);
		case MYSQL_TYPE_YEAR:
			return st_buffer_size_type(new char[2], 2, MYSQL_TYPE_SHORT);
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATETIME:
			return st_buffer_size_type(new char[sizeof(MYSQL_TIME)], sizeof(MYSQL_TIME), field->type);


		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
			return st_buffer_size_type(new char[field->max_length + 1], field->max_length + 1, field->type);

		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
			return st_buffer_size_type(new char[64], 64, field->type);
#if A1
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_YEAR:
			return st_buffer_size_type(new char[10], 10, field->type);
#endif
#if A0
		// There two are not sent over the wire
		case MYSQL_TYPE_ENUM:
		case MYSQL_TYPE_SET:
#endif
		case MYSQL_TYPE_BIT:
			return st_buffer_size_type(new char[8], 8, MYSQL_TYPE_BIT);
		case MYSQL_TYPE_GEOMETRY:
		default:
			// TODO: Andrey, there can be crashes when we go through this. Please fix.
			throw sql::InvalidArgumentException("allocate_buffer_for_field: invalid rbind data type");
	}
}
/* }}} */


/* {{{ MySQL_ResultBind::MySQL_ResultBind -I- */
MySQL_ResultBind::MySQL_ResultBind(boost::shared_ptr< NativeAPI::NativeStatementWrapper > & stmt,
									boost::shared_ptr< MySQL_DebugLogger > & log)
	: num_fields(0), is_null(NULL), err(NULL), len(NULL), proxy(stmt), logger(log), rbind(NULL)
{
}
/* }}} */


/* {{{ MySQL_ResultBind::~MySQL_ResultBind() -I- */
MySQL_ResultBind::~MySQL_ResultBind()
{
	if (rbind.get()) {
		for (unsigned int i = 0; i < num_fields; ++i) {
			delete[] (char *) rbind[i].buffer;
		}
	}
}
/* }}} */


/* {{{ MySQL_ResultBind::bindResult() -I- */
void MySQL_ResultBind::bindResult()
{
	CPP_ENTER("MySQL_Prepared_Statement::bindResult");
	for (unsigned int i = 0; i < num_fields; ++i) {
		delete[] (char *) rbind[i].buffer;
	}
	rbind.reset(NULL);
	is_null.reset(NULL);
	err.reset(NULL);
	len.reset(NULL);

	num_fields = proxy->field_count();
	if (!num_fields) {
		return;
	}

	rbind.reset(new MYSQL_BIND[num_fields]);
	memset(rbind.get(), 0, sizeof(MYSQL_BIND) * num_fields);

	is_null.reset(new my_bool[num_fields]);
	memset(is_null.get(), 0, sizeof(my_bool) * num_fields);

	err.reset(new my_bool[num_fields]);
	memset(err.get(), 0, sizeof(my_bool) * num_fields);

	len.reset(new unsigned long[num_fields]);
	memset(len.get(), 0, sizeof(unsigned long) * num_fields);

	std::auto_ptr< NativeAPI::NativeResultsetWrapper > resultMeta(proxy->result_metadata());

	for (unsigned int i = 0; i < num_fields; ++i) {
		MYSQL_FIELD * field = resultMeta->fetch_field();

		struct st_buffer_size_type p = allocate_buffer_for_field(field);
		rbind[i].buffer_type= p.type;
		rbind[i].buffer		= p.buffer;
		rbind[i].buffer_length= static_cast<unsigned long>(p.size);
		rbind[i].length		= &len[i];
		rbind[i].is_null	= &is_null[i];
		rbind[i].error		= &err[i];
		rbind[i].is_unsigned= field->flags & UNSIGNED_FLAG;
	}
	if (proxy->bind_result(rbind.get())) {
		CPP_ERR_FMT("Couldn't bind : %d:(%s) %s", proxy->errNo(), proxy->sqlstate().c_str(), proxy->error().c_str());
		sql::mysql::util::throwSQLException(*proxy.get());
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

