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

#ifndef MYSQLX_RESULT_H
#define MYSQLX_RESULT_H

/**
  @file
  Classes used to access query and command execution results.
*/


#include "common.h"
#include "document.h"
#include "collations.h"

#include <memory>


namespace cdk {

  class Reply;

}  // cdk


namespace mysqlx {

using std::ostream;

class NodeSession;
class Schema;
class Collection;
class Result;
class Row;
class RowResult;
class SqlResult;
class DbDoc;
class DocResult;

class Task;
class Executable;


/*
  @todo Add diagnostics information (warnings)
*/

namespace internal {

  class BaseResult : nocopy
  {
    class Impl;
    Impl  *m_impl = NULL;
    bool m_owns_impl = false;
    row_count_t  m_pos = 0;

    BaseResult(cdk::Reply*);
    BaseResult(cdk::Reply*, const std::vector<GUID>&);

  protected:

    BaseResult()
    {}

    BaseResult& operator=(BaseResult &&other)
    {
      init(std::move(other));
      return *this;
    }

    void init(BaseResult&&);

    const Impl& get_impl() const;
    Impl& get_impl()
    {
      return const_cast<Impl&>(
        const_cast<const BaseResult*>(this)->get_impl()
      );
    }

  public:

    BaseResult(BaseResult &&other) { init(std::move(other)); }
    virtual ~BaseResult();

    friend class mysqlx::NodeSession;
    friend class mysqlx::Result;
    friend class mysqlx::RowResult;
    friend class mysqlx::SqlResult;
    friend class mysqlx::DocResult;
    friend class mysqlx::Task;

    struct Access;
    friend struct Access;
  };

  inline
  void BaseResult::init(BaseResult &&init_)
  {
    m_pos = 0;
    m_impl = init_.m_impl;
    if (!init_.m_owns_impl)
      m_owns_impl = false;
    else
    {
      m_owns_impl = true;
      init_.m_owns_impl = false;
    }
  }

}

/**
  Represents result of an operation that does not return data.

  Generic result which can be returned by operations which only
  modify data.

  `Result` instance can store result of executing an operation:

  ~~~~~~
  Result res = operation.execute();
  ~~~~~~

  Storing another result in `Result` instance will overwrite
  previous result.

  @todo Implement other methods for getting information about
  the result specified by DevAPI.
*/

class Result : public internal::BaseResult
{
public:

  Result& operator=(BaseResult &&other)
  {
    init(std::move(other));
    return *this;
  }

  /**
    Return id of the document added to a collection.
  */

  const GUID& getDocumentId() const;

  /**
    Return list of ids of documents added to a collection on a chain add() call.
  */

  internal::List_init<GUID> getDocumentIds() const;

};



// Row based results
// -----------------

class RowResult;

/**
  Represents a single row from a result that contains rows.

  Such a row consists of a number of fields, each storing single
  value. The number of fields and types of values stored in each
  field are described by `RowResult` instance that produced this
  row.

  Values of fields can be accessed with `get()` method or using
  `row[pos]` expression. Fields are identified by 0-based position.
  It is also possible to get raw bytes representing value of a
  given field with `getBytes()` method.

  @sa `Value` class.
  @todo Support for iterating over row fields with range-for loop.
*/

class Row
{
  class Impl;
  std::shared_ptr<Impl>  m_impl;

  Impl& get_impl()
  { return const_cast<Impl&>(const_cast<const Row*>(this)->get_impl()); }
  const Impl& get_impl() const;

  Row(std::shared_ptr<Impl> &&impl) : m_impl(std::move(impl))
  {}

  void set_values(col_count_t pos, const Value &val)
  {
    set(pos, val);
  }

  template<typename... Types>
  void set_values(col_count_t pos, const Value &val, Types... rest)
  {
    set(pos, val);
    set_values(pos + 1, rest...);
  }

public:

  Row() {}

  template<typename... Types>
  Row(const Value &val, Types... vals)
  {
    set_values(0, val, vals...);
  }

  virtual ~Row() {}

  col_count_t colCount() const;


  /**
    Get raw bytes representing value of row field at position `pos`.

    @returns null bytes range if given field is NULL.
    @throws out_of_range if given row was not fetched from server.
  */

  bytes getBytes(col_count_t pos) const;


  /**
    Get reference to row field at position `pos`.

    @throws out_of_range if given field does not exist in the row.
  */

  Value& get(col_count_t pos);


  /**
    Set value of row field at position `pos`.

    Creates new field if it does not exist.

    @returns Reference to the field that was set.
  */

  Value& set(col_count_t pos, const Value&);

  /**
    Get const reference to row field at position `pos`.

    This is const version of method `get()`.

    @throws out_of_range if given field does not exist in the row.
  */

  const Value& operator[](col_count_t pos) const
  {
    return const_cast<Row*>(this)->get(pos);
  }


  /**
    Get modifiable reference to row field at position `pos`.

    The field is created if it does not exist. In this case
    the initial value of the field is NULL.
  */

  Value& operator[](col_count_t pos)
  {
    try {
      return get(pos);
    }
    catch (const out_of_range&)
    {
      return set(pos, Value());
    }
  }

  /// Check if this row contains fields or is null.
  bool isNull() const { return NULL == m_impl; }
  operator bool() const { return !isNull(); }

  void clear();

  friend class RowResult;
};


class Column;

namespace internal {

  /*
    Helper class which is used to initialize arbitrary STL container
    that can store Column instances with all columns meta-data returned
    by RowResult::getColumns(). An instance of ColumnListInitializer
    is returned by RowResult::getColumns() and stores reference to the
    RowResult object. It can be then used to initialize an STL container
    via the type cast operator. ColumnListInitializer instances are not
    to be used on its own.
  */

  class ColumnListInitializer
  {
    const RowResult &m_impl;

    ColumnListInitializer(const RowResult &impl)
      : m_impl(impl)
    {}

#if defined _MSC_VER && _MSC_VER < 1900

    /*
      There is bug in earlier version of MSVC:
      https://connectbeta.microsoft.com/VisualStudio/feedback/details/1255564/unable-to-implicit-specify-defaulted-move-constructor-and-move-assignment-operator
    */

    ColumnListInitializer(ColumnListInitializer &&other)
      : m_impl(other.m_impl)
    {}

#else

    ColumnListInitializer(ColumnListInitializer&&) = default;

#endif

    ColumnListInitializer(const ColumnListInitializer&) = delete;

  public:

    const Column& operator[](col_count_t) const;

    template <class C> operator C() const;

    friend RowResult;
  };

} // internal


/**
  %Result of an operation that returns rows.
*/

class RowResult : public internal::BaseResult
{
  std::forward_list<Row> m_row_cache;
  uint64_t m_row_cache_size = 0;
  bool m_cache = false;

public:

  RowResult()
  {}

  /*
    Note: Even though we have RowResult(BaseResult&&) constructor below,
    we still need move-ctor for such copy-initialization to work:

      RowResult res= coll...execute();

    This copy-initialization works as follows
    (see http://en.cppreference.com/w/cpp/language/copy_initialization):

    1. A temporary prvalue of type RowResult is created by type-conversion
       of the Result prvalue coll...execute(). Constructor RowResult(Result&&)
       is calld to do the conversion.

    2. Now res is direct-initialized
       (http://en.cppreference.com/w/cpp/language/direct_initialization)
       from the prvalue produced in step 1.

    Since RowResult has disabled copy constructor, a move constructor is
    required for direct-initialization in step 2. Even though move-constructor
    is actually not called (because of copy-elision), it must be declared
    in the RowResult class. We also define it for the case that copy-elision
    was not applied.
  */

  RowResult(RowResult &&other)
    : BaseResult(std::move(static_cast<BaseResult&>(other)))
  {}

  RowResult(BaseResult &&init_)
    : BaseResult(std::move(init_))
  {}

  RowResult& operator=(BaseResult &&init_)
  {
    BaseResult::operator=(std::move(init_));
    return *this;
  }

  /// Retrun number of fields in each row.

  col_count_t getColumnCount() const;

  /// Return Column instance describing given result column.

  const Column& getColumn(col_count_t pos) const;

  /**
    Return meta-data for all result columns. The returned data
    can be stored in any STL container which can store Column
    objects.
  */

  internal::ColumnListInitializer getColumns() const
  {
    try {
      return internal::ColumnListInitializer(*this);
    }
    CATCH_AND_WRAP
  }

  /**
    Return current row and move to the next one in the sequence.

    If there are no more rows in this result, returns NULL.
  */

  Row fetchOne();

  /**
    Return all remaining rows

    Rows that have been fetched using fetchOne() will not be available when
    calling fetchAll()
   */

private:

  class Row_list_initializer
  {
  protected:

    typedef std::forward_list<Row> Cache;
    typedef std::forward_list<Row>::iterator Cache_iterator;

    Cache &m_cache;

    Row_list_initializer(std::forward_list<Row> &cache)
      : m_cache(cache)
    {}


  public:

    template <typename U>
    operator U()
    {
      return U(m_cache.begin(), m_cache.end());
    }

    operator std::forward_list<Row>()
    {
      return std::move(m_cache);
    }

    friend RowResult;
    friend DocResult;
  };


public:

  Row_list_initializer fetchAll();

  /**
     Returns number of rows available on RowResult to be fetched
   */

  uint64_t count();

protected:

  void check_result() const;

  friend class Task;
  friend DocResult;
};


template <class C>
inline
internal::ColumnListInitializer::operator C() const
{
  try {

    /*
      Note: It is assumed that C is an STL container which can store
      Column instances. They are added to the container using
      C::emplace_back() method.
    */

    C columns;

    for (col_count_t pos = 0; pos < m_impl.getColumnCount(); ++pos)
      columns.emplace_back(m_impl.getColumn(pos));

    return std::move(columns);
  }
  CATCH_AND_WRAP
}


inline
const Column&
internal::ColumnListInitializer::operator[](col_count_t pos) const
{
  try {
    return m_impl.getColumn(pos);
  }
  CATCH_AND_WRAP
}


/**
  %Result of SQL query or command.

  Such result can contain one or more results returned from a
  single command or query. When created, SqlResult instance gives
  access to the first result. Method `nextResult()` moves to the next
  result if there is any.

  @todo implement `nextResult()` and other methods specified by DevAPI.
*/

class SqlResult : public RowResult
{
public:

  SqlResult(SqlResult &&other)
    : RowResult(std::move(static_cast<BaseResult&>(other)))
  {}

  SqlResult(BaseResult &&init_)
    : RowResult(std::move(init_))
  {}

  SqlResult& operator=(BaseResult &&init_)
  {
    RowResult::operator=(std::move(init_));
    return *this;
  }


  /**
    Tels if current result contains rows.

    If this is the case, rows can be accessed using `RowResult` interface.
    Otherwise calling `RowResult` methods throws an error.
  */

  bool hasData() const;


  /**
    Move to next result if there is one.

    Returns true if next result is available, false if there are no more
    results in the reply. Calling `nextResult()` discards the current result
    and all the data it contains (if any).
  */

  bool nextResult();
};


// RowResult column meta-data
// --------------------------

/**
  List of types defined by DevAPI. For each type TTT in this list
  there is corresponding enumeration value Type::TTT.

  Note: The class name declared for each type is ignored for now
  - it is meant for future extensions.
*/

#define TYPE_LIST(X) \
  X(BIT,        BlobType)     \
  X(TINYINT,    IntegerType)  \
  X(SMALLINT,   IntegerType)  \
  X(MEDIUMINT,  IntegerType)  \
  X(INT,        IntegerType)  \
  X(BIGINT,     IntegerType)  \
  X(FLOAT,      NumericType)  \
  X(DECIMAL,    NumericType)  \
  X(DOUBLE,     NumericType)  \
  X(JSON,       Type)         \
  X(STRING,     StringType)   \
  X(BYTES,      BlobType)     \
  X(TIME,       Type)         \
  X(DATE,       Type)         \
  X(DATETIME,   Type)         \
  X(TIMESTAMP,  Type)         \
  X(SET,        StringType)   \
  X(ENUM,       StringType)   \
  X(GEOMETRY,   Type)         \

#undef TYPE_ENUM
#define TYPE_ENUM(T,X) T,

enum class Type : unsigned short
{
  TYPE_LIST(TYPE_ENUM)
};


#define TYPE_NAME(T,X) case Type::T: return #T;

inline
const char* typeName(Type t)
{
  switch (t)
  {
    TYPE_LIST(TYPE_NAME)
    default:
      THROW("Unkonwn type");
  }
}

inline
std::ostream& operator<<(std::ostream &out, Type t)
{
  return out << typeName(t);
}


/**
  Class providing meta-data for a single result column.
*/

class Column : public internal::Printable
{
public:

  string getSchemaName()  const;
  string getTableName()   const;
  string getTableLabel()  const;
  string getColumnName()  const;
  string getColumnLabel() const;

  Type getType()   const;

  unsigned long getLength() const;
  unsigned short getFractionalDigits() const;
  bool isNumberSigned() const;

  CharacterSet getCharacterSet() const;
  std::string getCharacterSetName() const
  {
    return characterSetName(getCharacterSet());
  }

  const CollationInfo& getCollation() const;
  std::string getCollationName() const
  {
    return getCollation().getName();
  }

  bool isPadded() const;

private:

  class Impl;
  std::shared_ptr<Impl> m_impl;
  virtual void print(std::ostream&) const;

  friend class Impl;

public:
  struct Access;
  friend struct Access;
};



// Document based results
// ----------------------


/**
  %Result of an operation that returns documents.
*/

class DocResult : public internal::BaseResult
{
  class Impl;
  Impl *m_doc_impl = NULL;

  void check_result() const;

public:

  DocResult()
  {}

  DocResult(DocResult &&other)
  {
    *this = std::move(static_cast<BaseResult&>(other));
  }

  DocResult(BaseResult &&init_)
  {
    *this = std::move(init_);
  }

  virtual ~DocResult();

  void operator=(BaseResult &&init_);


  /**
    Return current document and move to the next one in the sequence.

    If there are no more documents in this result returns null document.
  */

  DbDoc fetchOne();

  /**
    Return all remaining documents.

    Documents that have been fetched using fetchOne() will not be available when
    calling fetchAll()
   */

private:

  class Doc_list_initializer
    : RowResult::Row_list_initializer
  {

    // Struct used to iterate and convert Row to DbDoc
    struct Doc_iterator
        : Cache_iterator
    {

      typedef DbDoc                             value_type;
      typedef DbDoc*                            pointer;
      typedef DbDoc&                            reference;
      typedef Cache_iterator::difference_type   difference_type;
      typedef Cache_iterator::iterator_category iterator_category;


      Doc_iterator(const Doc_iterator &it)
        : Cache_iterator(it)
      {}

      Doc_iterator(Cache_iterator it)
        : Cache_iterator(it)
      {}

      Doc_iterator& operator=(const Doc_iterator &it)
      {
        Cache_iterator::operator=(it);
        return *this;
      }

      bool operator!=(const Doc_iterator& other)const noexcept
      {
        return (*static_cast<const Cache_iterator*>(this)) != other;
      }

      bool operator==(const Doc_iterator& other)const noexcept
      {
        return (*static_cast<const Cache_iterator*>(this)) == other;
      }

      Doc_iterator& operator++() noexcept
      {
        Cache_iterator::operator++();
        return *this;
      }

      Doc_iterator operator++(int) noexcept
      {
        Doc_iterator tmp(*this);
        Cache_iterator::operator++();
        return tmp;
      }

      DbDoc operator*() const noexcept;

    };


    Doc_list_initializer(const RowResult::Row_list_initializer &row_list)
      : RowResult::Row_list_initializer(row_list)
    {}


  public:

    template <typename U>
    operator U()
    {
      return U(Doc_iterator(m_cache.begin()), Doc_iterator(m_cache.end()));
    }


    friend DocResult;

  };

public:

  Doc_list_initializer fetchAll();

  /**
     Returns number of documents available on DocResult to be fetched.
   */

  uint64_t count();


  friend class Impl;
  friend class Task;
  friend class DbDoc;
};


}  // mysqlx

#endif
