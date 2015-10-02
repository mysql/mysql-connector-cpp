#ifndef MYSQLX_DOCUMENT_H
#define MYSQLX_DOCUMENT_H

/**
  @file Declaration of DbDoc and related classes.
*/

#include "common.h"
#include <memory>
#include <stdint.h>
#include <limits>

namespace mysqlx {

class Value;
class Field;
class DbDoc;
class DocResult;

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
  DbDoc() {}

public:

  /**
    Creates DbDoc instance out of given JSON string description.
  */

  DbDoc(const std::string&);

  /// Check if named field is a top-level filed in the document.

  virtual bool hasField(const Field&);
  
  /// Return Value::XXX constant that identifies type of value
  /// stored at given field.

  virtual int  fieldType(const Field&);
  
  /// Return value of given field.

  virtual Value operator[](const Field&);
  
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
  Value object can store value of scalar type, string or document.

  Implicit conversions to and from corresponding C++ types are defined.
  If conversion to wrong type is attempted, an error is thrown. If Value
  object holds a document, then fields of this document can be accessed
  using operator[].

  Only direct conversions of stored value to the corresponding C++ type
  are supported. There are no implicit number->string conversions etc.

  @note Value object copies the values it stores. Thus, after storing value
  in Value object, the original value can be destroyed without invalidating
  the copy.
*/

class Value : public Printable
{
public:

  enum Type
  {
    VNULL,
    UINT64,
    INT64,
    FLOAT,
    DOUBLE,
    BOOL,
    STRING,
    DOCUMENT,
    RAW,
    // TODO: ARRAY
  };

  Value();
  Value(const string&);
  Value(int64_t);
  Value(uint64_t);
  Value(float);
  Value(double);
  Value(bool);

  operator int();
  operator float();
  operator double();
  operator bool();
  operator string();
  operator DbDoc();

  /// Get type of the value stored in this instance (or VNULL if no
  /// value is stored).

  Type  getType() const { return m_type; }

  /// Check if document value contains given (top-level) field.
  /// Throws error if this is not a document value.

  bool  hasField(const Field&);

  /*
    Note: thre is built-in operator <intege>[<pointer>]. Since Value
    can be converted to integer, expression val["name"] is ambiguous:
    it could be a call of Value::operator[] but, since "name" literal
    is trated as const char pointer, it can also be treated
    as a call to the builtin operator after converting val to integer
    value. To disambiguate one has to type val[Field("name")].

    TODO: Can this ambiguity be removed somehow?
  */

  /// If this value is not a document, throws error. Otherwise
  /// returns value of given field of the document.

  Value operator[](const Field&);

  /// Prtint stored value to a stream.

  void print(std::ostream&) const;

protected:

  Type m_type;

  void check_type(Type t)
  {
    if (m_type != t)
      throw "Invalid value type";
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
    different values.
  */

  string m_str;
  DbDoc  m_doc;

public:
  struct Access;
  friend Access;

  friend class DbDoc;
};


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

inline Value::operator int()
{
  if (UINT64 != m_type && INT64 != m_type)
    throw "Not an integer value";

  if (UINT64 == m_type 
      && m_val._uint64_v > (uint64_t)std::numeric_limits<int64_t>::max())
    throw "Overflow";

  int64_t val = (INT64 == m_type ? m_val._int64_v : (int64_t)m_val._uint64_v);
  if (val > std::numeric_limits<int>::max())
    throw "Overflow";

  return (int)val;
}


inline Value::Value(float val) : m_type(FLOAT)
{
  m_val._float_v = val;
}

inline
Value::operator float()
{
  check_type(FLOAT);
  return m_val._float_v;
}


inline Value::Value(double val) : m_type(DOUBLE)
{
  m_val._double_v = val;
}

inline
Value::operator double()
{
  check_type(DOUBLE);
  return m_val._double_v;
}


inline Value::Value(bool val) : m_type(BOOL)
{
  m_val._bool_v = val;
}

inline
Value::operator bool()
{
  check_type(BOOL);
  return m_val._bool_v;
}


inline Value::Value(const string &val) : m_type(STRING)
{
  m_str = val;
}

inline
Value::operator string()
{
  check_type(STRING);
  return m_str;
}

inline
Value::operator DbDoc()
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


// Field class
// ===========

/**
  Field object/values represent fields in a document.

  TODO: _fld suffix
*/

class Field : public string
{
public:

  Field(const string &s) : string(s)
  {}

  // TODO: is it auto generated?
  Field(string &&s) : string(s)
  {}

  Field(const char *s) : string(s)
  {}
};


}  // mysqlx


#endif
