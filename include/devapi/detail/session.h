#ifndef MYSQLX_DETAIL_SESSION_H
#define MYSQLX_DETAIL_SESSION_H

#include "../common.h"
#include "../result.h"
#include "../crud.h"

#include <set>

namespace cdk {
  class Session;
}

namespace mysqlx {

class Session;
class Schema;
class Table;
class Collection;

namespace internal {


/*
  Represents a database object

  Inherited by Schema, Table and Collection. Can't be used alone.
*/

class PUBLIC_API Db_object
{
  mysqlx::Session *m_sess;

  DLL_WARNINGS_PUSH
    string m_name;
  DLL_WARNINGS_POP

  Db_object(mysqlx::Session& sess, const string& name)
    : m_sess(&sess), m_name(name)
  {}

  virtual ~Db_object()
  {}

public:

  /**
     Get database object name
  */

  const string& getName() const { return m_name; }


  /**
    Get Session object
  */

  Session& getSession() { return *m_sess; }


  /**
     Get schema object
  */

  virtual const Schema& getSchema() const = 0;


  /**
     Check if database object exists

     Every check will contact server.
  */

  virtual bool existsInDatabase() const = 0;


  friend Schema;
  friend Table;
  friend Collection;
};



  struct SQL_statement;

  using SQL_statement_cmd = Executable<SqlResult, SQL_statement>;


  struct PUBLIC_API SQL_statement
    : public Bind_placeholders< SQL_statement_cmd >
  {
    SQL_statement(Session *, const string &query);

    SQL_statement(SQL_statement_cmd &other)
    {
      SQL_statement_cmd::operator=(other);
    }

    SQL_statement(SQL_statement_cmd &&other)
    {
      SQL_statement_cmd::operator=(std::move(other));
    }
  };


  struct PUBLIC_API Session_detail
  {
  protected:

    /*
      Note: Session implementation is shared with result objects because it
      must exists as long as result implementation exists. This means that
      even when session object is deleted, its implementation can still hang
      around.
    */

    struct INTERNAL Impl;
    std::shared_ptr<Impl>  m_impl = NULL;

    Session_detail *m_parent_session = NULL;
    std::set<Session_detail*>  m_child_sessions;

    Session_detail() {}

    virtual ~Session_detail()
    {
      try {
        if (m_impl)
          close();
      }
      catch (...) {}
    }

    Impl& get_impl()
    {
      if (!m_impl)
        THROW("Invalid session");
      return *m_impl;
    }

    INTERNAL cdk::Session& get_cdk_session();

    void close();

    void add_child(Session_detail *child)
    {
      assert(child);
      child->m_parent_session = this;
      m_child_sessions.insert(child);
    }

    void remove_child(Session_detail *child)
    {
      m_child_sessions.erase(child);
    }

    /*
      This notification is sent from parent session when it is closed.
    */
    void parent_close_notify()
    {
      if (m_parent_session)
        m_impl = NULL;
    }

    /*
      Do necessary cleanups before sending new command to the server.
    */
    void prepare_for_cmd();

    /// @cond IGNORED
    friend Result_detail::Impl;
    /// @endcond
  };

}  // internal namespace
}  // mysqlx namespace

#endif
