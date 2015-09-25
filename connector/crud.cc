#include <mysqlx.h>
#include <mysql/cdk.h>
#include <expr_parser.h>

#include <boost/format.hpp>

using namespace mysqlx;
using cdk::string;
using namespace parser;


void mysqlx::GUID::generate()
{
  // TODO: Use UUID generator
  static unsigned long seq_num= 1;
  std::string uuid((boost::format("uuid-%d") % seq_num++).str());
  memcpy(m_data, uuid.c_str(), sizeof(m_data));
  m_data[sizeof(m_data)-1]= '\0';
}


struct Result::Access
{
  template <typename A>
  static Result mk(A a) { return Result(a); }

  template <typename A, typename B>
  static Result mk(A a, B b) { return Result(a, b); }
};

struct Task::Access
{
  typedef Task::Impl Impl;
};


class Task::Impl : nocopy
{
protected:

  XSession &m_sess;
  cdk::Reply *m_reply;

  Impl(XSession &sess)
    : m_sess(sess), m_reply(NULL)
  {}
  Impl(Collection &coll)
    : m_sess(coll.m_schema.m_sess), m_reply(NULL)
  {}

  ~Impl() {}

  cdk::Session& get_cdk_session() { return m_sess.get_cdk_session(); }

  bool is_completed() { return m_reply->is_completed(); }
  void cont() { m_reply->cont(); }

  Result wait()
  {
    m_reply->wait();
    if (0 < m_reply->entry_count())
      m_reply->get_error().rethrow();
    return get_result();
  }

  virtual Result get_result()
  {
    return Result(m_reply);
  }

  friend class Task;
  friend class Executable;
};


// Implementation of Task API using internal implementation object

Task::~Task() try { delete m_impl; } CATCH_AND_WRAP

bool Task::is_completed()
try { return m_impl->is_completed(); } CATCH_AND_WRAP

Result Task::wait()
try { return m_impl->wait(); } CATCH_AND_WRAP

void Task::cont()
try { m_impl->cont(); } CATCH_AND_WRAP



class Schema_ref : public cdk::api::Schema_ref
{
  const cdk::string m_name;

  const cdk::string name() const { return m_name; }

public:

  Schema_ref(const mysqlx::string &name) : m_name(name) {}
  Schema_ref(const cdk::string &name) : m_name(name) {}
};

class Table_ref : public cdk::api::Table_ref
{
  Schema_ref m_schema;
  const cdk::string m_name;

public:

  const cdk::string name() const { return m_name; }
  const cdk::api::Schema_ref* schema() const { return &m_schema; }

  Table_ref(const Collection &coll)
    : m_schema(coll.getSchema().getName())
    , m_name(coll.getName())
  {}

  Table_ref(const cdk::string &schema, const cdk::string &name)
    : m_schema(schema), m_name(name)
  {}
};



/*
  Collection.add()
  ================
*/

class Op_collection_add
  : public Task::Access::Impl
  , public cdk::Expr_list
  , public cdk::Expression
  , public cdk::JSON::Processor
{
  typedef cdk::string string;

  Table_ref    m_coll;
  const string m_json;
  mysqlx::GUID  m_id;
  bool  m_generated_id;

  Op_collection_add(Collection &coll, const string &json)
    : Impl(coll)
    , m_coll(coll), m_json(json)
    , m_generated_id(true)
  {
    // Parse JSON string to find _id if defined.
    // TODO: Avoid parsing (if inserted document id is returned by server).

    cdk::Codec<cdk::TYPE_DOCUMENT> codec;
    codec.from_bytes(cdk::bytes(m_json), *this);
    if (m_generated_id)
      m_id.generate();

    // Issue coll_add statement where documents are described by list
    // of expressions defined by this instance.

    m_reply= new cdk::Reply(get_cdk_session().coll_add(m_coll, *this));
  }

  Result get_result()
  {
    return Result::Access::mk(m_reply, m_id);
  }

  // Expr_list

  unsigned count() const { return 1; }
  const cdk::Expression& get(unsigned) const
  { return *this; }

  void process(Expression::Processor &ep) const;

  // JSON::Processor

  void doc_begin() {}
  void doc_end() {}

  void key_doc(const string&, const Document&) {}

  void key_val(const string &key, const Value &val)
  {
    // look only at key '_id'
    if (key != string("_id"))
      return;
    // process '_id' value
    val.process(*this);
  }

  // JSON::Value::Processor

  void str(const string &val)
  {
    m_generated_id= false;
    m_id= val;
  }

  friend class mysqlx::Collection;
};


Executable Collection::add(const mysqlx::string &json)
try {
  return Executable(new Op_collection_add(*this, json));
}
CATCH_AND_WRAP


/*
  Class describing elements of expression:

    JSON_INSERT(<json>, '$._id', <id>)

  where <json> and <id> are given as constructor parameters.
*/

class Insert_id
  : public cdk::api::Table_ref
  , public cdk::Expr_list
  , public cdk::Expression
{
  typedef cdk::string string;

  const std::string m_json;
  const std::string &m_id;

  Insert_id(const string &json, const std::string &id)
    : m_json(json)  // note: conversion to utf-8
    , m_id(id)
  {}

  // Table_ref (function name)

  const cdk::api::Schema_ref* schema() const { return NULL; }
  const string name() const { return L"JSON_INSERT"; }

  // Expr_list (arguments)

  unsigned count() const { return 3; }

  unsigned m_pos;

  const Expression& get(unsigned pos) const
  {
    const_cast<Insert_id*>(this)->m_pos= pos;
    return *this;
  }

  using cdk::Expression::Processor;

  void process(Processor &ep) const
  {
    switch (m_pos)
    {
    case 0: ep.str(m_json); return;
    case 1: ep.str(L"$._id"); return;
    case 2: ep.str(m_id); return;
    default: throw "index out of range";
    }
  }

  friend class Op_collection_add;
};

/*
  Expression describing single document to be inserted.

  If document id was generated, then expression is a call of
  JSON_INSERT() function that adds generated id. Otherwise it
  is plain JSON string of the document.

  TODO:
  - Append '_id' field at the end of document, instead of using
    JSON_INSERT() (should be more efficient). If duplicate key is
    found in a document string, only the first occurence is taken
    into account.
*/

void Op_collection_add::process(Expression::Processor &ep) const
{
  if (m_generated_id)
  {
    std::string id(m_id);
    Insert_id expr(m_json, id);
    ep.call(expr, expr);
  }
  else
  {
    // TODO: ep.val(TYPE_DOCUMENT, json_format, cdk::bytes())
    ep.str(m_json);
  }
}


/*
  Collection.remove()
  ===================
*/


class Op_collection_remove : public Task::Access::Impl
{
  Table_ref m_coll;

  Op_collection_remove(Collection &coll)
    : Impl(coll)
    , m_coll(coll)
  {
    // TODO: selection criteria
    m_reply= new cdk::Reply(get_cdk_session().coll_remove(m_coll));
  }

  friend class mysqlx::Collection;
};

Executable Collection::remove()
try {
  return Executable(new Op_collection_remove(*this));
}
CATCH_AND_WRAP


/*
  Collection.find()
  =================
*/


class Op_collection_find
  : public Task::Access::Impl
{
  Table_ref m_coll;
  parser::Expr_parser m_expr;

  Op_collection_find(Collection &coll)
    : Impl(coll)
    , m_coll(coll)
  {
    m_reply= new cdk::Reply(get_cdk_session().coll_find(m_coll, NULL));
  }

  Op_collection_find(Collection &coll, const mysqlx::string &expr)
    : Impl(coll)
    , m_coll(coll)
    , m_expr(expr)
  {
    m_reply= new cdk::Reply(get_cdk_session().coll_find(m_coll, &m_expr));
  }

  friend class mysqlx::Collection;
};

Executable Collection::find()
try {
  return Executable(new Op_collection_find(*this));
}
CATCH_AND_WRAP

Executable Collection::find(const mysqlx::string &expr)
try {
  return Executable(new Op_collection_find(*this, expr));
}
CATCH_AND_WRAP

