#include <mysqlx.h>
#include <mysql/cdk.h>

#include <boost/format.hpp>
#include <iostream>


using namespace ::mysqlx;

class XSession::Impl
{
  cdk::ds::TCPIP   m_ds;
  std::string      m_pwd;
  cdk::ds::Options m_opt;
  cdk::Session     m_sess;

  Impl(const char *host, unsigned short port,
       const string &user, const char *pwd =NULL)
    : m_ds(host, port)
    , m_pwd(pwd ? pwd : "")
    , m_opt(user, pwd ? &m_pwd : NULL)
    , m_sess(m_ds, m_opt)
  {
    if (!m_sess.is_valid())
      m_sess.get_error().rethrow();
  }

  friend class XSession;
};


XSession::XSession(const char *host, unsigned short port,
                   const string  &user,
                   const char    *pwd)
try {
  m_impl= new Impl(host, port, user, pwd);
}
CATCH_AND_WRAP


XSession::~XSession()
try {
  delete m_impl;
}
CATCH_AND_WRAP

cdk::Session& XSession::get_cdk_session()
{
  return m_impl->m_sess;
}

Schema XSession::getSchema(const string &name)
try {
  return Schema(*this, name);
}
CATCH_AND_WRAP


Result NodeSession::executeSql(const string &query)
try {
  cdk::Reply *r= new cdk::Reply(get_cdk_session().sql(query));
  r->wait();
  if (0 < r->entry_count())
    r->get_error().rethrow();
  return Result(r);
}
CATCH_AND_WRAP


/*
  Schema.createCollection()
  =========================
*/

class Create_args
  : public cdk::Any_list
  , public cdk::Any
{
  typedef cdk::Any Any;

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
    // NOTE: uses utf8
    ep.str((0 == m_pos ? m_schema : m_name)); 
  }
};


Collection Schema::getCollection(const string &name, bool /*check*/)
try {
  return Collection(*this, name);
}
CATCH_AND_WRAP

Collection Schema::createCollection(const string &name, bool reuse)
try {
  Create_args args(m_name, name);
  cdk::Reply r(m_sess.get_cdk_session().admin("create_collection", args));
  r.wait();
  if (0 < r.entry_count())
  {
    const cdk::Error &err= r.get_error();
    // 1050 = table already exists
    if (!reuse || cdk::server_error(1050) != err.code())
      err.rethrow();
  }
  return Collection(*this, name);
}
CATCH_AND_WRAP


string::string(const std::string &other)
  : std::wstring(cdk::string(other))
{}

string::string(const char *other)
  : std::wstring(cdk::string(other))
{}

string::operator const std::string() const
{
  return std::string(cdk::string(*this));
}

/*
string::operator const cdk::foundation::string&() const
{
  return cdk::string(*static_cast<const std::wstring*>(this));
}
*/

ostream& operator<<(ostream &out, const Error&)
{
  out <<"MYSQLX Error!";
  return out;
}

