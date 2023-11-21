/*
 * Copyright (c) 2008, 2023, Oracle and/or its affiliates. All rights reserved.
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
#include <assert.h>

#include <memory>

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
      return st_buffer_size_type(new char[field->max_length], field->max_length, MYSQL_TYPE_BIT);
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
  memset(static_cast<MYSQL_BIND *>(this), 0, sizeof(MYSQL_BIND));
}
/* }}} */


/* {{{ MySQL_Bind::MySQL_Bind() -I- */
MySQL_Bind::MySQL_Bind(MySQL_Bind && other)
{
  memcpy(static_cast<MYSQL_BIND *>(this), static_cast<MYSQL_BIND *>(&other), sizeof(MYSQL_BIND));
  memset(static_cast<MYSQL_BIND *>(&other), 0, sizeof(MYSQL_BIND));
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

  // Free the length allocation in case it was used.
  if (length)
    delete length;
    
  length = nullptr;
  buffer = nullptr;
  buffer_length = 0;
  is_unsigned = false;
}
/* }}} */


/* {{{ MySQL_ResultBind::MySQL_ResultBind -I- */
MySQL_ResultBind::MySQL_ResultBind(
    std::shared_ptr<NativeAPI::NativeStatementWrapper> &stmt,
    std::shared_ptr<MySQL_DebugLogger> &log)
    : num_fields(0),
      is_null(NULL),
      err(NULL),
      len(NULL),
      proxy(stmt),
      logger(log),
      rbind(NULL) {}
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

  std::unique_ptr<NativeAPI::NativeResultsetWrapper> resultMeta(
      proxy->result_metadata());

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
unsigned
MySQL_AttributesBind::get_bind_pos(
  const sql::SQLString &name, bool &is_external
)
{
  unsigned pos = getNamePos(name, is_external);
  if(MySQL_Names::size() > bind.size())
    bind.resize(MySQL_Names::size());
  return pos;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrBigInt(const SQLString &, const sql::SQLString&, bool ) */
int
MySQL_AttributesBind::setQueryAttrBigInt(const SQLString &name, const sql::SQLString& value,
  bool is_external)
{
  bool pos_external = is_external;
  unsigned int pos = get_bind_pos(name, pos_external);

  // See ::setQueryAttrString() for explanation.
  
  if (pos_external != is_external)
    return pos+1;

  bind[pos].setBigInt(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrBoolean(const sql::SQLString &, bool, bool) */
int
MySQL_AttributesBind::setQueryAttrBoolean(const sql::SQLString &name, bool value,
  bool is_external)
{
  bool pos_external = is_external;
  unsigned int pos = get_bind_pos(name, pos_external);

  // See ::setQueryAttrString() for explanation.
  
  if (pos_external != is_external)
    return pos+1;

  bind[pos].setBoolean(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrDateTime(const sql::SQLString &, const sql::SQLString&, bool ) */
int
MySQL_AttributesBind::setQueryAttrDateTime(const sql::SQLString &name, const sql::SQLString& value,
  bool is_external)
{
  bool pos_external = is_external;
  unsigned int pos = get_bind_pos(name, pos_external);

  // See ::setQueryAttrString() for explanation.
  
  if (pos_external != is_external)
    return pos+1;

  bind[pos].setDateTime(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrDouble(const sql::SQLString &, double, bool) */
int
MySQL_AttributesBind::setQueryAttrDouble(const sql::SQLString &name, double value,
  bool is_external)
{
  bool pos_external = is_external;
  unsigned int pos = get_bind_pos(name, pos_external);

  // See ::setQueryAttrString() for explanation.
  
  if (pos_external != is_external)
    return pos+1;

  bind[pos].setDouble(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrInt(const sql::SQLString &, int32_t, bool) */
int
MySQL_AttributesBind::setQueryAttrInt(const sql::SQLString &name, int32_t value,
  bool is_external)
{
  bool pos_external = is_external;
  unsigned int pos = get_bind_pos(name, pos_external);

  // See ::setQueryAttrString() for explanation.
  
  if (pos_external != is_external)
    return pos+1;

  bind[pos].setInt(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrUInt(const SQLString &, uint32_t, bool) */
int
MySQL_AttributesBind::setQueryAttrUInt(const SQLString &name, uint32_t value,
  bool is_external)
{
  bool pos_external = is_external;
  unsigned int pos = get_bind_pos(name, pos_external);

  // See ::setQueryAttrString() for explanation.
  
  if (pos_external != is_external)
    return pos+1;

  bind[pos].setUInt(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrInt64(const SQLString &, int64_t, bool) */
int
MySQL_AttributesBind::setQueryAttrInt64(const SQLString &name, int64_t value,
  bool is_external)
{
  bool pos_external = is_external;
  unsigned int pos = get_bind_pos(name, pos_external);

  // See ::setQueryAttrString() for explanation.
  
  if (pos_external != is_external)
    return pos+1;

  bind[pos].setInt64(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrUInt64(const sql::SQLString &, uint64_t, bool) */
int
MySQL_AttributesBind::setQueryAttrUInt64(const sql::SQLString &name, uint64_t value,
  bool is_external)
{
  bool pos_external = is_external;
  unsigned int pos = get_bind_pos(name, pos_external);

  // See ::setQueryAttrString() for explanation.
  
  if (pos_external != is_external)
    return pos+1;

  bind[pos].setUInt64(value);
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrNull(const sql::SQLString &, bool) */
int
MySQL_AttributesBind::setQueryAttrNull(const sql::SQLString &name,
  bool is_external)
{
  bool pos_external = is_external;
  unsigned int pos = get_bind_pos(name, pos_external);

  // See ::setQueryAttrString() for explanation.

  if (pos_external != is_external)
    return pos+1;

  bind[pos].setNull();
  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::setQueryAttrString(const sql::SQLString &, const sql::SQLString&, bool) */
int
MySQL_AttributesBind::setQueryAttrString(const sql::SQLString &name, const sql::SQLString& value,
  bool is_external)
{
  bool pos_external = is_external;
  unsigned int pos = get_bind_pos(name, pos_external);

  /*
    Note: In case we are setting internal value (`is_external` is false) and
    the attribute already has external value `pos_external` will be true and
    different from `is_external`. In all other cases the internal/external
    status of the attribute that was found or added should be as requested
    by `is_external` so that `pos_external` and `is_external` are equal.
    In this case it is OK to overwrite old value with the new one.
  */

  if (pos_external != is_external)
    return pos+1;

  auto &mysql_bind = bind[pos];

  mysql_bind.setString(value);

  /*
    Set correct length for the attribute value.

    Note: setString() stores value string with the terminating null
    character without setting its length explicitly. This results in
    the terminating null char being treated as part of the value. This
    is not correct -- attribute value should be the given string without
    null terminator.
  */

  if (!mysql_bind.length)
    mysql_bind.length = new unsigned long;

  *mysql_bind.length = value.length();

  return pos+1;
}
/* }}} */


/* {{{ MySQL_AttributesBind::clearAttributes() */
void
MySQL_AttributesBind::clearAttributes()
{
  bind.clear();
  clearNames();
}
/* }}} */


/* {{{ MySQL_AttributesBind::nrAttr() */
size_t
MySQL_AttributesBind::size()
{
  return MySQL_Names::size();
}
/* }}} */


/* {{{ MySQL_AttributesBind::getBinds() */
MYSQL_BIND*
MySQL_AttributesBind::getBinds()
{
  return bind.data();
}
/* }}} */


/* {{{ MySQL_Names::getNames() */
const char**
MySQL_Names::getNames()
{
  for (size_t i = 0; i < s_names.size(); ++i)
  {
    // Need to re-adjust pointers to their corresponding string storage
    auto &s = s_names[i];
    names[i] = s.length() ? s.c_str() : nullptr;
  }
  return names.data();
}
/* }}} */

/* {{{ MySQL_Names::countTotal() - the total count of parameters and attributes together */
size_t
MySQL_Names::size()
{
  return names.size();
}
/* }}} */


/* {{{ MySQL_Names::clearNames() */
void
MySQL_Names::clearNames()
{
  names.clear();
  s_names.clear();
  name_set_type.clear();
}
/* }}} */


/* {{{ MySQL_Names::getNamePos() */
unsigned
MySQL_Names::get_name_pos(const sql::SQLString &name, set_type &type)
{
  size_t names_count = s_names.size();
  size_t pos;
  ssize_t free_pos = -1;

  // Try to find the position with the name specified
  // in the parameter.

  for (pos = 0; pos < names_count; ++pos)
  {
    if (-1 == free_pos && !isSet(pos))
    {
      free_pos = pos;
      continue;
    }
    if (name.caseCompare(s_names[pos]) == 0)
      break;
  }

  if (pos < names_count)
  {
    // The name was found at 'pos'
  }
  else if (UNSET == type)
  {
    // report that name was not found without trying to add it
    return 0;
  }
  else if (-1 != free_pos)
  {
    // use free slot
    pos = free_pos;
  }
  else
  {
    // append new name
    s_names.emplace_back(name);
    pos = s_names.size()-1;
    names.emplace_back(s_names[pos].c_str());
    name_set_type.emplace_back();
  }

  // Upgrade current type to "higher" one if requested.

  assert(UNSET < INTERNAL);
  assert(INTERNAL < EXTERNAL);

  if (type > name_set_type[pos])
    name_set_type[pos] = type;

  type = name_set_type[pos];

  // Note: 1-based to distinguish from 0 which means "not found"
  return pos + 1;
}
/* }}} */


/* {{{ MySQL_Names::unset() */
void
MySQL_Names::unset(unsigned int position)
{
  name_set_type[position] = set_type::UNSET;
  s_names[position] = "";
  names[position] = nullptr;
}
/* }}} */

/* {{{ MySQL_Names::isSet() */
bool
MySQL_Names::isSet(unsigned int position)
{
  return name_set_type[position] != set_type::UNSET;
}
/* }}} */

/* {{{ MySQL_Names::isInternal() */
bool
MySQL_Names::isInternal(unsigned int position)
{
  return name_set_type[position] == set_type::INTERNAL;
}
/* }}} */

/* {{{ MySQL_Names::isExternal() */
bool
MySQL_Names::isExternal(unsigned int position)
{
  return name_set_type[position] == set_type::EXTERNAL;
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
