#ifndef MYSQLXX_H
#define MYSQLXX_H

#include "common.h"

#include <vector>
#include <string>
#include <ostream>
#include <memory>
#include <string.h> // for memcpy

namespace cdk {

class Session;
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


/**
  Global unique identifiers for documents.

  TODO: Windows GUID type
*/


class GUID
{
  char m_data[32];

  void set(const char *data)
  {
    memcpy(m_data, data, sizeof(m_data));
    m_data[sizeof(m_data)-1]= '\0';
  }

  void set(const std::string &data) { set(data.c_str()); }

public:

  GUID()
  {
    m_data[0]= '\0';
  }

  template <typename T> GUID(T data) { set(data); }
  template<typename T>  GUID& operator=(T data) { set(data); return *this; }
  operator const char*() const { return m_data; }
  void generate();
};


inline
ostream& operator<<(ostream &out, const GUID &guid)
{
  out <<(const char*)guid;
  return out;
}


/*
  Accesing query results
  ======================
*/


class Result : nocopy
{
  class Impl;
  Impl  *m_impl;
  bool m_owns_impl;
  row_count_t  m_pos;

  Result(cdk::Reply*);
  Result(cdk::Reply*, const GUID&);
  void init(Result&&);

public:

  Result()
    : m_impl(NULL), m_owns_impl(true)
    , m_pos(0)
  {}

  Result(Result &&other) { init(std::move(other)); }
  virtual ~Result();

  Result& operator=(Result &&other)
  {
    init(std::move(other));
    return *this;
  }

  const GUID& getLastDocumentId() const;

  friend class Task;
  friend class RowResult;
  friend class DocResult;
  friend class NodeSession;

  struct Access;
  friend struct Access;
};


inline
void Result::init(Result &&init)
{
  m_pos= 0;
  m_impl= init.m_impl;
  if (!init.m_owns_impl)
    m_owns_impl= false;
  else
  {
    m_owns_impl= true;
    init.m_owns_impl= false;
  }
}


// Row based results
// -----------------


class Row : nocopy
{
public:

  virtual ~Row() {}

  virtual const string getString(col_count_t pos) =0;
  virtual bytes getBytes(col_count_t pos) =0;

  const string operator[](col_count_t pos)
  { return getString(pos); }
};


class RowResult : public Result
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
    : Result(std::move(static_cast<Result&>(other)))
  {}

  RowResult(Result &&init)
    : Result(std::move(init))
  {}

  col_count_t getColumnCount() const;
  Row* next();

  friend class Task;
};


// Document based results
// ----------------------


class DbDoc : nocopy
{
  virtual void print(ostream&) const =0;

  friend ostream& operator<<(ostream&, const DbDoc&);
};

inline
ostream& operator<<(ostream &out, const DbDoc &doc)
{
  doc.print(out);
  return out;
}


class DocResult : public Result
{
  class Impl;
  Impl *m_doc_impl;

public:

  DocResult(DocResult &&other)
    : m_doc_impl(NULL)
  {
    *this = std::move(static_cast<Result&>(other));
  }

  DocResult(Result &&init)
    : m_doc_impl(NULL)
  {
    *this = std::move(init);
  }

  virtual ~DocResult();

  void operator=(Result &&init);

  DbDoc& first();
  DbDoc* next();

  friend class Impl;
  friend class Task;
};


/*
  Results of CRUD operations
  ==========================
*/


class Task : nocopy
{
protected:

  class Impl;
  Impl  *m_impl;

  Task(Impl *impl) : m_impl(impl)
  {}

  Task(Task &&other) : m_impl(other.m_impl)
  { other.m_impl= NULL; }

public:

  virtual ~Task();
  bool is_completed();
  Result wait();
  void cont();

  friend class Impl;
  friend class Session;
  friend class NodeSession;
  friend class Result;
  friend class Collection;
  friend class Schema;

  struct Access;
  friend struct Access;
};


class Executable : public Task
{
public:

  Executable(Impl *impl) : Task(impl)
  {}

  Result execute()
  { return wait(); }

};


class Schema
{
  Session &m_sess;
  const string m_name;

public:

  Schema(Session &sess, const string &name)
    : m_sess(sess), m_name(name)
  {}

  Schema(Session&); // default schema of the session

  const string& getName() const { return m_name; }

  Collection createCollection(const string&, bool reuse= false);
  Collection getCollection(const string&, bool check_exists= false);

  friend class Collection;
  friend class Task;
};


/*
  Database objects
  ================
*/


class Collection
{
  Schema &m_schema;
  const string m_name;

public:

  Collection(Schema &sch, const string &name)
    : m_schema(sch), m_name(name)
  {}

  const string& getName() const { return m_name; }
  const Schema& getSchema() const { return m_schema; }

  Executable add(const string&); // add document given by json string
  Executable remove();
  Executable find();
  Executable find(const string&);

  friend class Task;
};


/*
  Session
  =======
*/

class Session : nocopy
{
protected:

  class Impl;
  Impl  *m_impl;

  cdk::Session& get_cdk_session();

public:

  /*
  Session(const char *host, unsigned short port,
          const string  &user,
          const char    *pwd =NULL);
  */

  Session(unsigned short port,
          const string  &user,
          const char    *pwd =NULL);
  virtual ~Session();

  Schema getSchema(const string&);

private:

  friend class Schema;
  friend class Collection;
  friend class Result;
  friend class Task;
};


class NodeSession: public Session
{
public:

/*
  NodeSession(const char* host, unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : Session(host, port, user, pwd)
  {}
*/

  NodeSession(unsigned short port,
              const string  &user,
              const char    *pwd =NULL)
   : Session(port, user, pwd)
  {}

  Result executeSql(const string &query);
};




class Error
{};

ostream& operator<<(ostream&, const Error&);

}  // mysqlx

#endif
