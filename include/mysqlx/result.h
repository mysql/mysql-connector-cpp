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

#ifndef MYSQLX_RESULT_H
#define MYSQLX_RESULT_H

/**
  @file
  Classes used to access query and command execution results.
*/


#include "common.h"
#include "document.h"


namespace cdk {

  class Reply;

}  // cdk


namespace mysqlx {

using std::ostream;

class Session;
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

class BaseResult : nocopy
{
  class Impl;
  Impl  *m_impl = NULL;
  bool m_owns_impl = false;
  row_count_t  m_pos = 0;

  BaseResult(cdk::Reply*);
  BaseResult(cdk::Reply*, const GUID&);

protected:
 BaseResult()
  {}

  BaseResult& operator=(BaseResult &&other)
  {
    init(std::move(other));
    return *this;
  }

  void init(BaseResult&&);

  Impl& get_impl();

public:

  BaseResult(BaseResult &&other) { init(std::move(other)); }
  virtual ~BaseResult();

  friend class NodeSession;
  friend class Result;
  friend class RowResult;
  friend class SqlResult;
  friend class DocResult;
  friend class Task;

  struct Access;
  friend struct Access;
};

inline
void BaseResult::init(BaseResult &&init)
{
  m_pos = 0;
  m_impl = init.m_impl;
  if (!init.m_owns_impl)
    m_owns_impl = false;
  else
  {
    m_owns_impl = true;
    init.m_owns_impl = false;
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

class Result : public BaseResult
{
public:

  /**
    Construct empty `Result` instance that can
    store results returned by `execute()` method.
  */

  Result& operator=(BaseResult &&other)
  {
    init(std::move(other));
    return *this;
  }

  /**
    Return id of the last document which the operation
    added to a collection.
  */

  const GUID& getLastDocumentId() const;

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

class Row : nocopy
{
  class Impl;
  Impl  *m_impl = NULL;
  bool  m_owns_impl = false;

  Impl& get_impl()
  { return const_cast<Impl&>(const_cast<const Row*>(this)->get_impl()); }
  const Impl& get_impl() const;

  Row(Impl *impl) { init(impl); }

  void init(Impl*);

  void init(Row &other)
  {
    init(other.m_impl);
    other.m_owns_impl = false;
  }

public:

  Row() {}
  Row(Row &&other) { init(other); }

  virtual ~Row();

  Row& operator=(Row &&other)
  {
    init(other);
    return *this;
  }

  /// Get raw bytes representing value of row field at position `pos`.
  bytes getBytes(col_count_t pos) const;

  /// Get value of row field at position `pos`.
  const Value get(col_count_t) const;

  /// Convenience operator equivalent to `get()`.
  const Value operator[](col_count_t pos) const
  { return get(pos); }

  /// Check if this row contains fields or is null.
  bool isNull() const { return NULL == m_impl; }
  operator bool() const { return !isNull(); }

  friend class RowResult;
};


/**
  %Result of an operation that returns rows.
*/

class RowResult : public BaseResult
{

public:

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

  RowResult(BaseResult &&init)
    : BaseResult(std::move(init))
  {}

  /// Retrun number of fields in each row.
  col_count_t getColumnCount() const;

  /**
    Return current row and move to the next one in the sequence.

    If there are no more rows in this result, returns NULL.
  */

  Row fetchOne();

  friend class Task;
};


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

  SqlResult(BaseResult &&init)
    : RowResult(std::move(init))
  {}

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


// Document based results
// ----------------------


/**
  %Result of an operation that returns documents.
*/

class DocResult : public BaseResult
{
  class Impl;
  Impl *m_doc_impl;

public:

  DocResult(DocResult &&other)
    : m_doc_impl(NULL)
  {
    *this = std::move(static_cast<BaseResult&>(other));
  }

  DocResult(BaseResult &&init)
    : m_doc_impl(NULL)
  {
    *this = std::move(init);
  }

  virtual ~DocResult();

  void operator=(BaseResult &&init);

  /**
    Return current document and move to the next one in the sequence.

    If there are no more documents in this result returns null document.
  */

  DbDoc fetchOne();

  friend class Impl;
  friend class Task;
  friend class DbDoc;
};


}  // mysqlx

#endif
