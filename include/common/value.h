/*
 * Copyright (c) 2015, 2017, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C++ is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef MYSQLX_COMMON_VALUE_H
#define MYSQLX_COMMON_VALUE_H

#include <string>

#include "error.h"
#include "util.h"


namespace mysqlx {
namespace common {

class Value_conv;

/*
  Class representing a polymorphic value of one of the supported types.

  TODO: Extend it with array and document types (currently these are implemented
  in derived mysqlx::Value class of DevAPI).

  TODO: When storing raw bytes, currently they are copied inside the Value
  object. Consider if this can be avoided.
*/

class PUBLIC_API Value
  : public virtual Printable
{
public:

  enum Type
  {
    VNULL,      ///< Null value
    UINT64,     ///< Unsigned integer
    INT64,      ///< Signed integer
    FLOAT,      ///< Float number
    DOUBLE,     ///< Double number
    BOOL,       ///< Boolean
    STRING,     ///< String
    WSTRING,    ///< Wide string
    RAW,        ///< Raw bytes
    EXPR,       ///< String to be interpreted as an expression
    JSON        ///< JSON string
  };

protected:

  Type m_type;

  // TODO: Use std::variant to save space

  DLL_WARNINGS_PUSH

  std::string   m_str;
  std::wstring  m_wstr;

  DLL_WARNINGS_POP

  union {
    double   v_double;
    float    v_float;
    int64_t  v_sint;
    uint64_t v_uint;
    bool     v_bool;
  } m_val;

  void print(std::ostream&) const override;

  template <typename T>
  Value(Type type, T &&init)
    : Value(std::forward<T>(init))
  {
    m_type = type;
  }

public:

  // Construct a NULL item
  Value() : m_type(VNULL)
  {}

  // Construct an item from a string
  Value(const std::string& str) : m_type(STRING), m_str(str)
  {}

  // Construct an item from a string
  Value(const std::wstring& str) : m_type(WSTRING), m_wstr(str)
  {}

  // Construct an item from a signed 64-bit integer
  Value(int64_t v) : m_type(INT64)
  { m_val.v_sint = v; }

  // Construct an item from an unsigned 64-bit integer
  Value(uint64_t v) : m_type(UINT64)
  { m_val.v_uint = v; }

  // Construct an item from a float
  Value(float v) : m_type(FLOAT)
  { m_val.v_float = v; }

  // Construct an item from a double
  Value(double v) : m_type(DOUBLE)
  { m_val.v_double = v; }

  // Construct an item from a bool
  Value(bool v) : m_type(BOOL)
  { m_val.v_bool = v; }

  // Construct an item from bytes
  Value(const byte *ptr, size_t len) : m_type(RAW)
  {
    // Note: bytes are copied to m_str member.
    m_str.assign((const char*)ptr, len);
  }

  // Other numeric conversions

  template <
    typename T,
    enable_if_t<std::is_unsigned<T>::value>* = nullptr
  >
  Value(T val)
    : Value(uint64_t(val))
  {}

  template <
    typename T,
    enable_if_t<!std::is_unsigned<T>::value>* = nullptr,
    enable_if_t<std::is_integral<T>::value>* = nullptr
  >
  Value(T val)
    : Value(int64_t(val))
  {}


  bool is_null() const
  {
    return VNULL == m_type;
  }

  bool get_bool() const
  {
    switch (m_type)
    {
    case BOOL:   return m_val.v_bool;
    case UINT64: return 0 != m_val.v_uint;
    case INT64:  return 0 != m_val.v_sint;
    default:
      throw Error("Can not convert to Boolean value");
    }
  }

  uint64_t get_uint() const
  {
    if (UINT64 != m_type && INT64 != m_type && BOOL != m_type)
      throw Error("Can not convert to integer value");

    if (BOOL == m_type)
      return m_val.v_bool ? 1 : 0;

    if (INT64 == m_type && 0 > m_val.v_sint)
      throw Error("Converting negative integer to unsigned value");

    uint64_t val = (UINT64 == m_type ? m_val.v_uint: (uint64_t)m_val.v_sint);

    return val;
  }

  int64_t get_sint() const
  {
    if (INT64 == m_type)
      return m_val.v_sint;

    uint64_t val = get_uint();

    if (!check_num_limits<int64_t>(val))
      throw Error("Value cannot be converted to signed integer number");

    return val;
  }

  float get_float() const
  {
    switch (m_type)
    {
    case INT64:  return 1.0F*m_val.v_sint;
    case UINT64: return 1.0F*m_val.v_uint;
    case FLOAT:  return m_val.v_float;
    default:
        throw Error("Value cannot be converted to float number");
    }
  }

  double get_double() const
  {
    switch (m_type)
    {
    case INT64:  return 1.0*m_val.v_sint;
    case UINT64: return 1.0*m_val.v_uint;
    case FLOAT:  return m_val.v_float;
    case DOUBLE: return m_val.v_double;
    default:
      throw Error("Value can not be converted to double number");
    }
  }

  const byte* get_bytes(size_t *size) const
  {
    switch (m_type)
    {
    case WSTRING: // TODO
    default:
      if (m_str.empty())
        throw Error("Value cannot be converted to raw bytes");
    case RAW:
    case STRING:
      if (size)
        *size = m_str.length();
      return (const byte*)m_str.data();

    }
  }

  // Note: these methods perform utf8 conversions as necessary.

  const std::string& get_string() const
  {
    switch (m_type)
    {
    case RAW:
    case STRING:
      return m_str;
    case WSTRING: // TODO
    case EXPR:
    case JSON:
    default:
      throw Error("Value cannot be converted to string");
    }
  }

  const std::wstring& get_wstring() const
  {
    switch (m_type)
    {
    case WSTRING:
    case EXPR:
    case JSON:
      return m_wstr;

    case RAW:  // TODO
    case STRING: // TODO
    default:
      throw Error("Value cannot be converted to string");
    }
  }

  Type get_type() const
  {
    return m_type;
  }

private:

  /*
    Note: Avoid implicit conversion from pointer types to bool.
    Without this declaration, Value(bool) constructor is invoked
    for pointer types. Here we declare and hide an explicit constructor
    for pointer types which prevents compiler to pick Value(bool).
  */

  template <typename T>
  Value(const T*);

public:

  friend Value_conv;

  struct Access;
  friend Access;
};

}}  // mysqlx::internal namespace

#endif
