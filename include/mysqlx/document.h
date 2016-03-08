/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
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
#include <mysql/cdk.h>
#include <memory>
#include <stdint.h>
#include <limits>
#include <vector>
#include <assert.h>

namespace mysqlx {

class Value;
class Field;
class DbDoc;
class DocResult;


// Field class
// ===========

/**
  Field object/values represent fields in a document.

  TODO: _fld suffix
*/

class Field
    : public cdk::string
    , public cdk::Doc_path
{
public:

  Field(const string &s) : string(s)
  {}

  // TODO: is it auto generated?
  Field(string &&s) : string(s)
  {}

  Field(const char *s) : string(s)
  {}

  virtual unsigned length() const
  {
    return empty() ? 0 : 1;
  }

  virtual Type get_type(unsigned pos) const
  {
    return Type::MEMBER;
  }

  virtual const cdk::string* get_name(unsigned pos) const
  {
    if (pos != 0)
      return NULL;
    return this;
  }

  virtual const uint32_t* get_index(unsigned pos) const
  {
    return NULL;
  }
};


// Document class
// ==============


/**
  Represents a collection of key-value pairs where value can be a scalar
  or another document.

  @note Internal document implementation is shared among DbDoc instances
  and thus using DbDoc objects should be cheap.
*/

class DbDoc
  : public Printable
{
  class Impl;

  std::shared_ptr<Impl> m_impl;

  DbDoc(const std::shared_ptr<Impl>&);

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

  DbDoc(const std::string&);

  /// Check if document is null

  bool isNull() const { return NULL == m_impl.get(); }
  operator bool() const { return !isNull(); }

  /// Check if named field is a top-level field in the document.

  virtual bool hasField(const Field&);

  /// Return Value::XXX constant that identifies type of value
  /// stored at given field.

  virtual int  fieldType(const Field&);

  /// Return value of given field.

  virtual Value operator[](const Field&);
  Value operator[](const char *name);
  Value operator[](const wchar_t *name);

  /// Print JSON description of the document.

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

  friend class Impl;
  friend class DocResult;
  friend class Value;
};


class DbDoc::Iterator
{
  std::shared_ptr<DbDoc::Impl> m_impl;
  bool         m_end;

public:

  Iterator& operator++();
  bool operator==(const Iterator&) const;
  bool operator!=(const Iterator &other) const { return !(*this == other); }
  const Field& operator*();

  friend class DbDoc;
};


// Value class
// ===========

/**
  Value object can store value of scalar type, string, array or document.

  Implicit conversions to and from corresponding C++ types are defined.
  If conversion to wrong type is attempted, an error is thrown. If Value
  object holds an array or document, then array elements or fields of
  the document can be accessed using operator[]. Array values can be used
  as STL containers.

  Only direct conversions of stored value to the corresponding C++ type
  are supported. There are no implicit number->string conversions etc.

  @note Value object copies the values it stores. Thus, after storing value
  in Value object, the original value can be destroyed without invalidating
  the copy.
*/

class Value : public Printable
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
  Value(const string&);
  Value(const char *str) : Value(string(str)) {}
  Value(const wchar_t *str) : Value(string(str)) {}
  Value(int64_t);
  Value(uint64_t);
  Value(float);
  Value(double);
  Value(bool);
  Value(int x) : Value((int64_t)x) {}
  Value(unsigned x) : Value((uint64_t)x) {}
  Value(const DbDoc& doc);
  template <typename Iterator>
  Value(Iterator begin, Iterator end)
    : m_type(ARRAY)
  {
    m_arr = std::make_shared<Array>(begin,end);
  }


  ///@}

  /**
    @name Conversion to C++ Types

    Attempt to convert value of non-compatible type will
    throw an error.
  */
  //@{

  operator int() const;
  operator unsigned() const;
  explicit operator int64_t() const;
  explicit operator uint64_t() const;
  operator float() const;
  operator double() const;
  operator bool() const;
  operator string() const;
  operator DbDoc() const;

  //@}

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

  bool  hasField(const Field&);

  /**
    If this value is not a document, throws error. Otherwise
    returns value of given field of the document.
  */

  Value operator[](const Field&);
  Value operator[](const char *name) { return (*this)[Field(name)]; }
  Value operator[](const wchar_t *name) { return (*this)[Field(name)]; }


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
  Value    operator[](unsigned);
  Value    operator[](int pos)
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
      THROW("Invalid value type");
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

  string m_str;
  DbDoc  m_doc;
  std::shared_ptr<Array>  m_arr;

public:
  struct Access;
  friend Access;

  friend class DbDoc;
};


inline
Value DbDoc::operator[](const char *name)
{ return (*this)[Field(name)]; }

inline
Value DbDoc::operator[](const wchar_t *name)
{
  return (*this)[Field(name)];
}


// Value type conversions
// ----------------------

inline Value::Value() : m_type(VNULL)
{}

inline Value::Value(int64_t val) : m_type(INT64)
{
  m_val._int64_v = val;
}

inline Value::Value(uint64_t val) : m_type(UINT64)
{
  m_val._uint64_v = val;
}

// TODO: Other integer conversions

inline Value::operator int() const
{
  int64_t val = (int64_t)*this;
  if (val > std::numeric_limits<int>::max())
    THROW("Overflow");

  return (int) val;
}

inline Value::operator unsigned() const
{
  uint64_t val = static_cast<uint64_t>(*this);
  if (val > std::numeric_limits<unsigned>::max())
    THROW("Overflow");

  return (unsigned) val;
}


inline Value::operator int64_t() const
{
  if (UINT64 != m_type && INT64 != m_type)
    THROW("Not an integer value");

  if (UINT64 == m_type
      && m_val._uint64_v > (uint64_t)std::numeric_limits<int64_t>::max())
    THROW("Overflow");

  int64_t val = (INT64 == m_type ? m_val._int64_v : (int64_t)m_val._uint64_v);

  return val;
}

inline Value::operator uint64_t() const
{
  if (UINT64 != m_type && INT64 != m_type)
    THROW("Not an integer value");

  if (INT64 == m_type
    && 0 > m_val._int64_v)
    THROW("Converting negative integer to unsigned value");

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
  check_type(FLOAT);
  return m_val._float_v;
}


inline Value::Value(double val) : m_type(DOUBLE)
{
  m_val._double_v = val;
}

inline
Value::operator double() const
{
  check_type(DOUBLE);
  return m_val._double_v;
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
  check_type(BOOL);
  return m_val._bool_v;
}


inline Value::Value(const string &val) : m_type(STRING)
{
  m_str = val;
}

inline
Value::operator string() const
{
  check_type(STRING);
  return m_str;
}

inline
Value::operator DbDoc() const
{
  check_type(DOCUMENT);
  return m_doc;
}


inline
bool Value::hasField(const Field &fld)
{
  check_type(DOCUMENT);
  return m_doc.hasField(fld);
}

inline
Value Value::operator[](const Field &fld)
{
  check_type(DOCUMENT);
  return m_doc[fld];
}


inline
int DbDoc::fieldType(const Field &fld)
{
  return (*this)[fld].getType();
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
size_t Value::elementCount() const
{
  check_type(ARRAY);
  return m_arr->size();
}

inline
Value Value::operator[](unsigned pos)
{
  check_type(ARRAY);
  return m_arr->at(pos);
}

}  // mysqlx


#endif
