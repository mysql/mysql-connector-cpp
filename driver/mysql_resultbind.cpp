/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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




#include <cppconn/exception.h>
#include "mysql_util.h"
#include "mysql_debug.h"
#include "mysql_resultbind.h"

#include "nativeapi/native_statement_wrapper.h"
#include "nativeapi/native_resultset_wrapper.h"

#include <string.h>

#include <boost/scoped_ptr.hpp>

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
  switch (field->type)
  {
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
#if LIBMYSQL_VERSION_ID > 50700
    case MYSQL_TYPE_JSON:
#endif //LIBMYSQL_VERSION_ID > 50700
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


/* {{{ MySQL_Bind::MySQL_Bind -I- */
MySQL_Bind::MySQL_Bind()
{
  // Since this structure will be used as an array of MYSQL_BIND, its size
  // should be the same. This only encapsulates to enable safe alloc/free
  assert(sizeof(MySQL_Bind) == sizeof(MYSQL_BIND));
  memset(this, 0, sizeof (MySQL_Bind));
}
/* }}} */


/* {{{ MySQL_Bind::MySQL_Bind() -I- */
MySQL_Bind::MySQL_Bind(MySQL_Bind && other)
{
  memcpy(this, &other, sizeof(MySQL_Bind));
  memset(&other, 0, sizeof (MySQL_Bind));
}
/* }}} */


/* {{{ MySQL_Bind::~MySQL_Bind() -I- */
MySQL_Bind::~MySQL_Bind()
{
  clear();
}
/* }}} */


/* {{{ void MySQL_Bind::setBigInt() -I- */
void MySQL_Bind::setBigInt(const sql::SQLString& value)
{
  setString(value);
}
/* }}} */


/* {{{ MySQL_Bind::setBoolean() -I- */
void
MySQL_Bind::setBoolean(bool value)
{
  setInt(value);
}
/* }}} */


/* {{{ MySQL_Bind::setDateTime() -I- */
void
MySQL_Bind::setDateTime(const sql::SQLString& value)
{
  setString(value);
}
/* }}} */


template <typename T>
inline
void valdup(MYSQL_BIND* bind, T& value)
{
  bind->buffer_length = sizeof (T);
  bind->buffer = memcpy(new char[sizeof(T)], &value, bind->buffer_length);
  bind->is_unsigned = std::is_unsigned<T>();
}

template <>
inline
void
valdup(MYSQL_BIND* bind, const sql::SQLString& value)
{
  bind->buffer = memcpy(new char[value.length() + 1], value.c_str(), value.length() + 1);
  bind->buffer_length = static_cast<unsigned long>(value.length()) + 1;
}


/* {{{ MySQL_Bind::setDouble() -I- */
void
MySQL_Bind::setDouble(double value)
{
  clear();
  buffer_type	= MYSQL_TYPE_DOUBLE;
  valdup(this, value);
}
/* }}} */


/* {{{ MySQL_Bind::setInt() -I- */
void
MySQL_Bind::setInt(int32_t value)
{
  clear();
  buffer_type = MYSQL_TYPE_LONG;
  valdup(this, value);
}
/* }}} */


/* {{{ MySQL_Bind::setUInt() -I- */
void
MySQL_Bind::setUInt(uint32_t value)
{
  clear();
  buffer_type = MYSQL_TYPE_LONG;
  valdup(this, value);
}
/* }}} */


/* {{{ MySQL_Bind::setInt64() -I- */
void
MySQL_Bind::setInt64(int64_t value)
{
  clear();
  buffer_type = MYSQL_TYPE_LONGLONG;
  valdup(this, value);
}
/* }}} */


/* {{{ MySQL_Bind::setUInt64() -I- */
void
MySQL_Bind::setUInt64(uint64_t value)
{
  clear();
  buffer_type = MYSQL_TYPE_LONGLONG;
  valdup(this, value);
}
/* }}} */


/* {{{ MySQL_Bind::setNull() -I- */
void
MySQL_Bind::setNull()
{
  clear();
  buffer_type	= MYSQL_TYPE_NULL;
}
/* }}} */


/* {{{ MySQL_Bind::setString() -I- */
void
MySQL_Bind::setString(const sql::SQLString& value)
{
  clear();
  buffer_type	= MYSQL_TYPE_STRING;
  valdup(this, value);
}
/* }}} */


/* {{{ MySQL_Bind::clear() -I- */
void
MySQL_Bind::clear()
{
  delete [] static_cast<char *>(buffer);
  buffer = nullptr;
  buffer_length = 0;
  is_unsigned = false;
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

  boost::scoped_ptr< NativeAPI::NativeResultsetWrapper > resultMeta(proxy->result_metadata());

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


/* {{{ MySQL_ResultBind::MySQL_ResultBind() */
MySQL_AttributesBind::MySQL_AttributesBind()
{
}
/* }}} */

/* {{{ MySQL_AttributesBind::~MySQL_AttributesBind() */
MySQL_AttributesBind::~MySQL_AttributesBind()
{
  clearAttributes();
}
/* }}} */


/* {{{ MySQL_AttributesBind::getBindPos(const sql::SQLString &) */
int
MySQL_AttributesBind::getBindPos(const sql::SQLString &name)
{
  size_t pos;
  for(pos = 0; pos < names.size(); ++pos) {
    if(name == names[pos])
    {
      break;
    }
  }
  if(pos == names.size())
  {
    size_t length = name.length()+1;
    names.push_back( static_cast<const char*>(memcpy(new char[length] , name.c_str(), length)));
    bind.resize(names.size());
  }
  return pos;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrBigInt(const SQLString &, const sql::SQLString& ) */
int
MySQL_AttributesBind::setQueryAttrBigInt(const SQLString &name, const sql::SQLString& value)
{
  int pos =getBindPos(name);
  bind[pos].setBigInt(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrBoolean(const sql::SQLString &, bool) */
int
MySQL_AttributesBind::setQueryAttrBoolean(const sql::SQLString &name, bool value)
{
  int pos =getBindPos(name);
  bind[pos].setBoolean(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrDateTime(const sql::SQLString &, const sql::SQLString& ) */
int
MySQL_AttributesBind::setQueryAttrDateTime(const sql::SQLString &name, const sql::SQLString& value)
{
  int pos =getBindPos(name);
  bind[pos].setDateTime(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrDouble(const sql::SQLString &, double) */
int
MySQL_AttributesBind::setQueryAttrDouble(const sql::SQLString &name, double value)
{
  int pos =getBindPos(name);
  bind[pos].setDouble(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrInt(const sql::SQLString &, int32_t) */
int
MySQL_AttributesBind::setQueryAttrInt(const sql::SQLString &name, int32_t value)
{
  int pos =getBindPos(name);
  bind[pos].setInt(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrUInt(const SQLString &, uint32_t) */
int
MySQL_AttributesBind::setQueryAttrUInt(const SQLString &name, uint32_t value)
{
  int pos =getBindPos(name);
  bind[pos].setUInt(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrInt64(const SQLString &, int64_t) */
int
MySQL_AttributesBind::setQueryAttrInt64(const SQLString &name, int64_t value)
{
  int pos =getBindPos(name);
  bind[pos].setInt64(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrUInt64(const sql::SQLString &, uint64_t) */
int
MySQL_AttributesBind::setQueryAttrUInt64(const sql::SQLString &name, uint64_t value)
{
  int pos =getBindPos(name);
  bind[pos].setUInt64(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrNull(const sql::SQLString &) */
int
MySQL_AttributesBind::setQueryAttrNull(const sql::SQLString &name)
{
  int pos =getBindPos(name);
  bind[pos].setNull();
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrString(const sql::SQLString &, const sql::SQLString&) */
int
MySQL_AttributesBind::setQueryAttrString(const sql::SQLString &name, const sql::SQLString& value)
{
  int pos =getBindPos(name);
  bind[pos].setString(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::clearAttributes() */
void
MySQL_AttributesBind::clearAttributes()
{
  bind.clear();

  for(auto el : names)
    delete [] el;

  names.clear();
}
/* }}} */


/* {{{ MySQL_AttributesBind::nrAttr() */
int
MySQL_AttributesBind::nrAttr()
{
  return names.size();
}
/* }}} */


/* {{{ MySQL_AttributesBind::getBinds() */
MYSQL_BIND*
MySQL_AttributesBind::getBinds()
{
  return bind.data();
}
/* }}} */


/* {{{ MySQL_AttributesBind::getNames() */
const char**
MySQL_AttributesBind::getNames()
{
  return names.data();
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

