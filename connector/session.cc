#include <mysqlxx.h>
#include <mysql/cdk/mysqlx/session.h>

#include <iostream>

using namespace ::std;

namespace mysqlx {

class Executable::Impl : nocopy
{
protected:

  Session &m_sess;

  Impl(Session &sess) : m_sess(sess)
  {}

  typedef Executable::Result_init Result_init;

  virtual Result_init execute() =0;

  friend class Executable;
};


Executable::~Executable()
{
  delete m_impl;
}

Executable::Result_init Executable::execute()
{
  return m_impl->execute();
}


class Executable::Op_collection_add : public Executable::Impl
{
  Collection &m_coll;
  const string m_json;

  Op_collection_add(Collection &coll, const string &json)
    : Impl(coll.m_schema.m_sess)
    , m_coll(coll), m_json(json)
  {}

  Result_init execute()
  {
    Result_init r= m_sess.add_document(m_coll.getSchema().getName(), m_coll.getName(), m_json);
    r->wait();
    if (0 < r->entry_count())
      r->get_error().rethrow();
    return r;
  }

  friend class Collection;
};


Schema Session::getSchema(const string &name)
{
  return Schema(*this, name);
}


class Create_args
  : public cdk::Any_list
  , public cdk::Any
{
  unsigned m_pos;
  const string &m_schema;
  const string &m_name;

public:

  Create_args(const string &schema, const string &name)
    : m_schema(schema), m_name(name)
  {}

  unsigned count() const { return 2; }
  const Any& get(unsigned pos) const
  {
    const_cast<Create_args*>(this)->m_pos= pos;
    return *this;
  }

  void process(Any::Processor &ep) const
  {
    const std::string str(0 == m_pos ? m_schema : m_name); // NOTE: conversion to UTF8
    // TODO: Add utf8 CS id
    ep.str(cdk::bytes((byte*)str.data(), str.size())); 
  }
};

Session::Result_init
Session::create_collection(const string &schema,
                           const string &name,
                           bool reuse)
{
  cout <<"Creating collection `" <<name <<"`"
       <<" in schema `" <<schema <<"`"
       <<endl;
  Create_args args(schema, name);
  cdk::Reply *r= new cdk::Reply(m_sess.admin("create_collection", args));
  r->wait();
  if (0 < r->entry_count())
  {
    const cdk::Error &err= r->get_error();
    // 1050 = table already exists
    if (!reuse || cdk::server_error(1050) != err.code())
      err.rethrow();
  }
  //r->discard();
  return Result_init(r);
}


class Schema_ref : public cdk::api::Schema_ref
{
  const string &m_name;

  const string& name() const { return m_name; }

public:

  Schema_ref(const string &name) : m_name(name) {}
};

class Table_ref : public cdk::api::Table_ref
{
  Schema_ref m_schema;
  const string &m_name;

  const string& name() const { return m_name; }
  const cdk::api::Schema_ref* schema() const { return &m_schema; }

public:

  Table_ref(const string &schema, const string &name)
    : m_schema(schema), m_name(name)
  {}
};

Session::Result_init
Session::add_document(const string &schema,
                      const string &name,
                      const string &json)
{
  cout <<"Adding document " <<json
       <<" to collection `" <<name <<"`"
       <<" in schema `" <<schema <<"`"
       <<endl;
  Table_ref doc(schema, name);
  cdk::Reply *r= new cdk::Reply(m_sess.coll_add(doc, json));
  return r;
}


Collection Schema::createCollection(const string &name, bool reuse)
{
  Result res(m_sess.create_collection(m_name, name, reuse));
  return Collection(*this, name);
}

Executable Collection::add(const string &json)
{
  return Executable(new Executable::Op_collection_add(*this, json));
}


ostream& operator<<(ostream &out, const Error&)
{
  out <<"MYSQLX Error!";
  return out;
}

}  // mysqlx
