/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
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

#ifndef MYSQLX_DOCUMENT_H
#define MYSQLX_DOCUMENT_H

/**
  @file
  Declaration of DbDoc and related classes.
*/

#include "common.h"
#include <memory>
#include <stdint.h>
#include <limits>
#include <vector>
#include <assert.h>

#undef min
#undef max

namespace mysqlx {

class Value;
class Field;
class DbDoc;
class DocResult;


// Field class
// ===========

/*
  %Field object/values represent fields in a document.

  TODO: _fld suffix

  @ingroup devapi_res
*/

class PUBLIC_API Field
{
  DLL_WARNINGS_PUSH
    string m_fld;
  DLL_WARNINGS_POP

public:

  Field(const string &s) : m_fld(s)
  {}

  // TODO: is it auto generated?
  Field(string &&s) : m_fld(std::move(s))
  {}

  Field(const char *s) : m_fld(s)
  {}

  operator const string&() const { return m_fld; }

  bool operator <(const Field &other) const
  {
    return m_fld < other.m_fld;
  }

  bool operator >(const Field &other) const
  {
    return m_fld > other.m_fld;
  }

  bool operator ==(const Field &other) const
  {
    return m_fld == other.m_fld;
  }

  bool operator !=(const Field &other) const
  {
    return m_fld != other.m_fld;
  }

};


// Document class
// ==============


/**
  Represents a collection of key-value pairs where value can be a scalar
  or another document.

  @note Internal document implementation is shared among DbDoc instances
  and thus using DbDoc objects should be cheap.

  @ingroup devapi_res
*/

class PUBLIC_API DbDoc
  : public internal::Printable
{
  class INTERNAL Impl;

DLL_WARNINGS_PUSH

  std::shared_ptr<Impl> m_impl;

DLL_WARNINGS_POP

  INTERNAL DbDoc(const std::shared_ptr<Impl>&);

public:

  /**
    Create null document instance.

    @note Null document is different from empty document that has
    no fields.
  */

  DbDoc() {}

  /**
    Creates DbDoc instance out of given JSON string description.
  */

  explicit DbDoc(const std::string&);


  /**
    Check if document is null
  */

  bool isNull() const { return NULL == m_impl.get(); }
  operator bool() const { return !isNull(); }


  /**
    Check if named field is a top-level field in the document.
  */

  virtual bool hasField(const Field&) const;


  /**
    Return Value::XXX constant that identifies type of value
    stored at given field.
  */

  virtual int  fieldType(const Field&) const;

  /**
    Return value of given field.
  */

  virtual const Value& operator[](const Field&) const;
  const Value& operator[](const char *name) const;
  const Value& operator[](const wchar_t *name) const;


  /**
    Print JSON description of the document.
  */

  virtual void print(std::ostream&) const;

  /**
    Iterator instance can iterate over (top-level) fields of a document.
    A new iterator is obtained from begin() method.

    @note Only one instance of an iterator can be used at a time (not
    thread safe!).
  */

  class Iterator;

  virtual Iterator begin();
  virtual Iterator end();

  friend Impl;
  friend DocResult;
  friend Value;
};


class PUBLIC_API DbDoc::Iterator
{
  DLL_WARNINGS_PUSH
  std::shared_ptr<DbDoc::Impl> m_impl;
  DLL_WARNINGS_POP
  bool         m_end;

public:

  Iterator& operator++();
  bool operator==(const Iterator&) const;
  bool operator!=(const Iterator &other) const { return !(*this == other); }
  const Field& operator*();

  friend DbDoc;
};


// Value class
// ===========

/**
  %Value object can store value of scalar type, string, array or document.

  Implicit conversions to and from corresponding C++ types are defined.
  If conversion to wrong type is attempted, an error is thrown. If Value
  object holds an array or document, then array elements or fields of
  the document can be accessed using operator[]. Array values can be used
  as STL containers.

  Only direct conversions of stored value to the corresponding C++ type
  are supported. There are no implicit number->string conversions etc.

  Values of type RAW can refer to a region of memory containing raw bytes.
  Such values are created from `bytes` and can by casted to `bytes` type.

  @note Value object copies the values it stores. Thus, after storing value
  in Value object, the original value can be destroyed without invalidating
  the copy. The only exception is RAW Value, which does not store the
  bytes it describes - it only stores pointers describing a region of memory.

  @ingroup devapi_res
*/

class PUBLIC_API Value : public internal::Printable
{
public:

  /**
    Possible types of values.

    @sa getType()
  */

  enum Type
  {
    VNULL,      ///< Null value
    UINT64,     ///< Unsigned integer
    INT64,      ///< Signed integer
    FLOAT,      ///< Float number
    DOUBLE,     ///< Double number
    BOOL,       ///< Boolean
    STRING,     ///< String
    DOCUMENT,   ///< Document
    RAW,        ///< Raw bytes
    ARRAY,      ///< Array of values
  };

  typedef std::vector<Value>::iterator iterator;
  typedef std::vector<Value>::const_iterator const_iterator;

  ///@name Value Constructors
  ///@{

  Value();  ///< Constructs Null value.
  Value(std::nullptr_t); ///< Constructs Null value.
  Value(const string&);
  Value(string&&);
  Value(const char *str) : Value(string(str)) {}
  Value(const wchar_t *str) : Value(string(str)) {}
  Value(const bytes&);
  Value(int64_t);
  Value(uint64_t);
  Value(float);
  Value(double);
  Value(bool);
  Value(const DbDoc& doc);

  Value(const std::initializer_list<Value> &list)
    : m_type(ARRAY)
  {
    m_arr = std::make_shared<Array>(list);
  }

  template <typename Iterator>
  Value(Iterator begin_, Iterator end_)
    : m_type(ARRAY)
  {
    m_arr = std::make_shared<Array>(begin_, end_);
  }

  ///@}

  /*
    Note: These templates are needed to disambiguate constructor resolution
    for integer types.
  */

  Value(const Value&) = default;

#if !defined(_MSC_VER) || _MSC_VER >= 1900

  Value(Value&&) = default;

#else

  Value(Value &&other)
  {
    *this = std::move(other);
  }

#endif

  template <
    typename T,
    typename std::enable_if<std::is_signed<T>::value>::type* = nullptr
  >
  Value(T x)
    : Value(static_cast<int64_t>(x))
  {}

  template <
    typename T,
    typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr
  >
  Value(T x)
    : Value(static_cast<uint64_t>(x))
  {}

  /*
    Assignment operator is implemented using constructors.
  */

  Value& operator=(const Value&) = default;

#if !defined(_MSC_VER) || _MSC_VER >= 1900

 Value& operator=(Value&&) = default;

#else

  Value& operator=(Value&&);

#endif

  template<typename T>
  Value& operator=(T x)
  {
    *this = Value(x);
    return *this;
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

  /**
    @name Conversion to C++ Types

    Attempt to convert value of non-compatible type will
    throw an error.
  */
  //@{

  operator int() const;
  operator unsigned() const;
  operator int64_t() const;
  operator uint64_t() const;
  operator float() const;
  operator double() const;
  operator bool() const;
  operator string() const;
  operator const bytes&() const;
  operator DbDoc() const;

  template<typename T>
  T get() { return static_cast<T>(*this); }

  //@}


  const bytes& getRawBytes() const
  {
    check_type(RAW);
    return m_raw;
  }


  /**
    Return type of the value stored in this instance (or VNULL if no
    value is stored).
  */

  Type  getType() const { return m_type; }

  /// Convenience method for checking if value is null.

  bool isNull() const { return VNULL == getType(); }

  /**
    Check if document value contains given (top-level) field.
    Throws error if this is not a document value.
  */

  bool  hasField(const Field&) const;

  /**
    If this value is not a document, throws error. Otherwise
    returns value of given field of the document.
  */

  const Value& operator[](const Field&) const;

  const Value& operator[](const char *name) const
  { return (*this)[Field(name)]; }

  const Value& operator[](const wchar_t *name) const
  { return (*this)[Field(name)]; }


//  typedef std::vector<Value>::iterator iterator;
//  typedef std::vector<Value>::const_iterator const_iterator;

  /**
    Access to elements of an array value.

    If non-array value is accessed like an array, an error is thrown.
  */
  //@{

  iterator begin();
  const_iterator begin() const;
  iterator end();
  const_iterator end() const;
  size_t   elementCount() const;

  const Value&  operator[](unsigned) const;
  const Value&  operator[](int pos) const
  {
    assert(pos >= 0);
    return operator[]((unsigned)pos);
  }

  //@}


  /// Print the value to a stream.

  void print(std::ostream&) const;

protected:

  Type m_type;

  void check_type(Type t) const
  {
    if (m_type != t)
      throw Error("Invalid value type");
  }

  union
  {
    uint64_t  _uint64_v;
    int64_t   _int64_v;
    float     _float_v;
    double    _double_v;
    bool      _bool_v;
  } m_val;

  /*
    TODO: Above union can not have members of type string or DbDoc.
    Find another way of sharing memory between representations of
    different values (try boost::variant?)
  */

  typedef std::vector<Value> Array;

  DbDoc  m_doc;

  DLL_WARNINGS_PUSH
  bytes  m_raw;
  string m_str;
  std::shared_ptr<Array>  m_arr;
  DLL_WARNINGS_POP

public:

  friend DbDoc;

  struct INTERNAL Access;
  friend Access;
};

static const Value nullvalue;


#if defined(_MSC_VER) && _MSC_VER < 1900

inline
Value& Value::operator=(Value &&other)
{
  m_type = other.m_type;
  m_val =  other.m_val;

  switch (m_type)
  {
  case STRING: m_str = std::move(other.m_str); break;
  case DOCUMENT: m_doc = std::move(other.m_doc); break;
  case RAW: m_raw = std::move(other.m_raw); break;
  case ARRAY: m_arr = std::move(other.m_arr); break;
  default: break;
  }

  return *this;
}

#endif


namespace internal {

/*
  Helper class to identify usage of expressions
*/

class ExprValue
: public Value
{
 bool m_is_expr = false;

public:

 ExprValue()
 {}

 template <typename V>
 ExprValue(V val)
 : Value(val)
 {}

 ExprValue(Value &&val)
 : Value(std::move(val))
 {}

 bool isExpression() const { return m_is_expr; }

 template <typename V>
 friend ExprValue expr(V s);
};


/**
  Function which indicates that a given string should be treated
  as expression.

  If `s` is a string value, then in contexts where values are
  expected, `expr(s)` will treat `s` as a DevAPI expression. For
  example statement

  table.select("foo > 1").execute();

  will return string `"foo > 1"` for each row in the table while

  table.select(expr("foo > 1")).execute();

  will return true/false, depending on the value of the expression.

  @ingroup devapi
*/

template <typename V>
inline
internal::ExprValue expr(V s)
{
  internal::ExprValue val(s);
  val.m_is_expr = true;
  return std::move(val);
}

}  // internal

using internal::expr;


inline
const Value& DbDoc::operator[](const char *name) const
{
  return (*this)[Field(name)];
}

inline
const Value& DbDoc::operator[](const wchar_t *name) const
{
  return (*this)[Field(name)];
}


/*
  Value type conversions
  ----------------------
  TODO: more informative errors
*/

inline Value::Value() : m_type(VNULL)
{}

inline Value::Value(std::nullptr_t) : m_type(VNULL)
{}

inline Value::Value(int64_t val) : m_type(INT64)
{
  m_val._int64_v = val;
}

inline Value::Value(uint64_t val) : m_type(UINT64)
{
  m_val._uint64_v = val;
}

inline Value::operator int() const
{
  int64_t val = (int64_t)*this;
  if (val > std::numeric_limits<int>::max())
    throw Error("Numeric conversion overflow");
  if (val < std::numeric_limits<int>::min())
    throw Error("Numeric conversion overflow");

  return (int) val;
}

inline Value::operator unsigned() const
{
  uint64_t val = static_cast<uint64_t>(*this);
  if (val > std::numeric_limits<unsigned>::max())
    throw Error("Numeric conversion overflow");

  return (unsigned) val;
}


inline Value::operator int64_t() const
{
  if (UINT64 != m_type && INT64 != m_type && BOOL != m_type)
    throw Error("Can not convert to integer value");

  if (BOOL == m_type)
    return m_val._bool_v ? 1 : 0;

  if (UINT64 == m_type
      && m_val._uint64_v > (uint64_t)std::numeric_limits<int64_t>::max())
    throw Error("Numeric conversion overflow");

  int64_t val = (INT64 == m_type ? m_val._int64_v : (int64_t)m_val._uint64_v);

  return val;
}

inline Value::operator uint64_t() const
{
  if (UINT64 != m_type && INT64 != m_type && BOOL != m_type)
    throw Error("Can not convert to integer value");

  if (BOOL == m_type)
    return m_val._bool_v ? 1 : 0;

  if (INT64 == m_type
    && 0 > m_val._int64_v)
    throw Error("Converting negative integer to unsigned value");

  uint64_t val = (UINT64 == m_type ? m_val._uint64_v : (uint64_t)m_val._int64_v);

  return val;
}

inline Value::Value(float val) : m_type(FLOAT)
{
  m_val._float_v = val;
}

inline
Value::operator float() const
{
  switch (m_type)
  {
  case INT64:  return 1.0F*m_val._int64_v;
  case UINT64: return 1.0F*m_val._uint64_v;
  case FLOAT:  return m_val._float_v;
  default:
    throw Error("Value can not be converted to float");
  }
}


inline Value::Value(double val) : m_type(DOUBLE)
{
  m_val._double_v = val;
}

inline
Value::operator double() const
{
  switch (m_type)
  {
  case INT64:  return 1.0*m_val._int64_v;
  case UINT64: return 1.0*m_val._uint64_v;
  case FLOAT:  return m_val._float_v;
  case DOUBLE: return m_val._double_v;
  default:
    throw Error("Value can not be converted to double");
  }
}


inline Value::Value(bool val) : m_type(BOOL)
{
  m_val._bool_v = val;
}

inline Value::Value(const DbDoc &doc)
  : m_type(DOCUMENT)
  , m_doc(doc)
{
}


inline
Value::operator bool() const
{
  switch (m_type)
  {
  case INT64:  return m_val._int64_v != 0;
  case UINT64: return m_val._uint64_v != 0;
  case BOOL: return m_val._bool_v;
  default:
    throw Error("Value can not be converted to double");
  }
}


inline Value::Value(const string &val) : m_type(STRING)
{
  m_str = val;
}

inline Value::Value(string &&val) : m_type(STRING)
{
  m_str = std::move(val);
}

inline
Value::operator string() const
{
  check_type(STRING);
  return m_str;
}


inline Value::Value(const bytes &data) : m_type(RAW)
{
  m_raw = data;
}

inline
Value::operator const bytes&() const
{
  return getRawBytes();
}


inline
Value::operator DbDoc() const
{
  check_type(DOCUMENT);
  return m_doc;
}


inline
bool Value::hasField(const Field &fld) const
{
  try {
    check_type(DOCUMENT);
    return m_doc.hasField(fld);
  }
  CATCH_AND_WRAP
}

inline
const Value& Value::operator[](const Field &fld) const
{
  try {
    check_type(DOCUMENT);
    return m_doc[fld];
  }
  CATCH_AND_WRAP
}

inline
int DbDoc::fieldType(const Field &fld) const
{
  try {
    return (*this)[fld].getType();
  }
  CATCH_AND_WRAP
}

// Array access


inline
Value::iterator Value::begin()
{
  if (ARRAY != m_type)
    throw Error("Attempt to iterate over non-array value");
  return m_arr->begin();
}

inline
Value::const_iterator Value::begin() const
{
  if (ARRAY != m_type)
    throw Error("Attempt to iterate over non-array value");
  return m_arr->begin();
}

inline
Value::iterator Value::end()
{
  if (ARRAY != m_type)
    throw Error("Attempt to iterate over non-array value");
  return m_arr->end();
}

inline
Value::const_iterator Value::end() const
{
  if (ARRAY != m_type)
    throw Error("Attempt to iterate over non-array value");
  return m_arr->end();
}

inline
const Value& Value::operator[](unsigned pos) const
{
  try {
    check_type(ARRAY);
    return m_arr->at(pos);
  }
  CATCH_AND_WRAP
}

inline
size_t Value::elementCount() const
{
  check_type(ARRAY);
  return m_arr->size();
}


}  // mysqlx


#endif
