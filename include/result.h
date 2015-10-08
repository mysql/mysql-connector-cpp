#ifndef MYSQLX_RESULT_H
#define MYSQLX_RESULT_H

/**
  @file
  DevAPI classes used to access query results.
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
class DbDoc;
class DocResult;

class Task;
class Executable;


class BaseResult : nocopy
{
  class Impl;
  Impl  *m_impl;
  bool m_owns_impl;
  row_count_t  m_pos;

  BaseResult(cdk::Reply*);
  BaseResult(cdk::Reply*, const GUID&);

protected:

  BaseResult()
    : m_impl(NULL), m_owns_impl(true)
    , m_pos(0)
  {}

  BaseResult& operator=(BaseResult &&other)
  {
    init(std::move(other));
    return *this;
  }

  void init(BaseResult&&);

public:

  BaseResult(BaseResult &&other) { init(std::move(other)); }
  virtual ~BaseResult();

  friend class NodeSession;
  friend class Result;
  friend class RowResult;
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


class Result : public BaseResult
{
public:

  Result& operator=(BaseResult &&other)
  {
    init(std::move(other));
    return *this;
  }

  const GUID& getLastDocumentId() const;

  //friend class Task;
  //friend class RowResult;
  //friend class DocResult;
  //friend class NodeSession;
};



// Row based results
// -----------------


class Row : nocopy
{
public:

  virtual ~Row() {}

  virtual const string getString(col_count_t pos) =0;
  virtual bytes getBytes(col_count_t pos) =0;
  virtual Value get(col_count_t) = 0;

  const Value operator[](col_count_t pos)
  { return get(pos); }
};


class RowResult : public BaseResult
{

public:

  /*
    Note: Even though we have RowResult(Result&&) constructor below,
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

  col_count_t getColumnCount() const;
  Row* fetchOne();

  friend class Task;
};


// Document based results
// ----------------------


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

  DbDoc* fetchOne();

  friend class Impl;
  friend class Task;
  friend class DbDoc;
};


}  // mysqlx

#endif
