/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#include <mysql/cdk/foundation.h>
#include <mysql/cdk/mysqlx.h>
#include <mysql/cdk/protocol/mysqlx.h>

PUSH_SYS_WARNINGS_CDK
#include <iostream>
#include "auth_hash.h"
POP_SYS_WARNINGS_CDK

#include "delayed_op.h"

namespace cdk {
namespace mysqlx {

/*
  A structure to check if xplugin we are connecting supports a
  specific field
*/
struct Proto_field_checker : public cdk::protocol::mysqlx::api::Expectations
{
  cdk::bytes m_data;
  cdk::protocol::mysqlx::Protocol &m_proto;

  Proto_field_checker(cdk::protocol::mysqlx::Protocol &proto) :
    m_proto(proto)
  {}

  struct Check_reply_prc : cdk::protocol::mysqlx::Reply_processor
  {
    unsigned int m_code = 0;

    void error(unsigned int code, short int,
               cdk::protocol::mysqlx::sql_state_t, const string &)
    {
      m_code = code;
    }

    void ok(string)
    {
      m_code = 0;
    }
  };

  void process(Processor &prc) const
  {
    prc.list_begin();
    prc.list_el()->set(FIELD_EXISTS, m_data);
    prc.list_end();
  }

  /*
  This method sets the expectation and returns
  the field flag if it is supported, otherwise 0 is returned.
  */
  uint64_t is_supported(Protocol_fields::value v)
  {
    switch (v)
    {
    case Protocol_fields::ROW_LOCKING:
      // Find=17, locking=12
      m_data = bytes("17.12");
      break;
    case Protocol_fields::UPSERT:
      // Insert=18, upsert=6
      m_data = bytes("18.6");
      break;
    case Protocol_fields::PREPARED_STATEMENTS:
      m_data = bytes("40");
      break;
    case Protocol_fields::KEEP_OPEN:
      m_data = bytes("6.1");
      break;
    default:
      return 0;
    }
    m_proto.snd_Expect_Open(*this, false).wait();

    Check_reply_prc prc;
    m_proto.rcv_Reply(prc).wait();
    uint64_t ret = prc.m_code == 0 ? (uint64_t)v : 0;

    if (prc.m_code == 0 || prc.m_code == 5168)
    {
      /*
      The expectation block needs to be closed if no error
      or expectation failed error (5168)
      */
      m_proto.snd_Expect_Close().wait();
      m_proto.rcv_Reply(prc).wait();
    }
    return ret;
  }
};


class error_category_server : public foundation::error_category_base
{
public:

  error_category_server() {}

  virtual const char* name() const NOEXCEPT { return "server"; }
  virtual std::string message(int) const NOEXCEPT { return "Server Error"; }
  virtual error_condition do_default_error_condition(int) const;
  virtual bool  do_equivalent(int, const error_condition&) const;

};

const error_category& server_error_category()
{
  static const error_category_server instance;
  return instance;
}

error_code server_error(int code)
{
  return error_code(code, server_error_category());
}


error_condition
error_category_server::do_default_error_condition(int errc) const
{
  switch (errc)
  {
  case 0:
    return errc::no_error;
  // TODO: Define appropriate error conditions for server errors.
  default:
    THROW("No error condition defined for server error");
    //return errc::operation_not_permitted;
  }
}

bool error_category_server::do_equivalent(int code,
                                       const error_condition &ec) const
{
  try
  {
    return ec == default_error_condition(code);
  }
  catch (...)
  {
    return false;
  }
}


/*
   Authentication
*/


class AuthPlain
    : public SessionAuthInterface
{

  std::string m_data;

  public:

  AuthPlain( const Session::Options &options )
  {
    std::string user(options.user());  // convert to utf8 before sending

    // TODO: Check if this is correct way to sepcify default schema

    if (options.database())
      m_data.append(*options.database());

    m_data.push_back('\0'); // authz
    m_data.append(user).push_back('\0'); // authc
    if (options.password())
      m_data.append(*options.password()); // pass

  }

  const char* auth_method() { return "PLAIN";}

  virtual bytes auth_data()
  {
    return bytes((byte*)m_data.c_str(), m_data.size());
  }

  virtual bytes auth_response()
  {
    return bytes((byte*)nullptr, (byte*)nullptr);
  }

  virtual bytes auth_continue(bytes)
  {
    THROW("Unexpected auth continuation");
  }
};

class AuthMysql41
    : public SessionAuthInterface
{

protected:

  std::string m_user;
  std::string m_pass;
  std::string m_db;

  std::string m_cont_data;

public:
  AuthMysql41(const Session::Options &options)
    : m_user(options.user())
  {
    if (options.password())
      m_pass = *options.password();
    if (options.database())
      m_db = *options.database();
  }


  const char* auth_method() { return "MYSQL41";}

  virtual bytes auth_data()
  {
    return bytes((byte*)nullptr, (byte*)nullptr);
  }

  virtual bytes auth_response()
  {
    return bytes((byte*)nullptr, (byte*)nullptr);
  }

  virtual bytes auth_continue(bytes data)
  {
    m_cont_data = ::mysqlx::build_mysql41_authentication_response(std::string(data.begin(), data.end()),
                                                                  m_user, m_pass, m_db);

   return bytes((byte*)m_cont_data.c_str(), m_cont_data.size());
  }

private:

};

class AuthExternal
  : public SessionAuthInterface
{

  std::string m_data;

public:

  AuthExternal(const Session::Options &options)
  {
    if (options.database())
      m_data.append(*options.database());
  }

  const char* auth_method() { return "EXTERNAL"; }

  virtual bytes auth_data()
  {
    return bytes((byte*)m_data.c_str(), m_data.size());
  }

  virtual bytes auth_response()
  {
    return bytes((byte*)nullptr, (byte*)nullptr);
  }

  virtual bytes auth_continue(bytes)
  {
    THROW("Unexpected auth continuation");
  }
};


class AuthSha256Memory
    : public SessionAuthInterface
{

protected:

  std::string m_user;
  std::string m_pass;
  std::string m_db;

  std::string m_cont_data;

public:
  AuthSha256Memory(const Session::Options &options)
    : m_user(options.user())
  {
    if (options.password())
      m_pass = *options.password();
    if (options.database())
      m_db = *options.database();
  }

  const char* auth_method() { return "SHA256_MEMORY";}

  virtual bytes auth_data()
  {
    return bytes((byte*)nullptr, (byte*)nullptr);
  }

  virtual bytes auth_response()
  {
    return bytes((byte*)nullptr, (byte*)nullptr);
  }

  virtual bytes auth_continue(bytes data)
  {
    m_cont_data = ::mysqlx::build_sha256_authentication_response(std::string(data.begin(), data.end()),
                                                                  m_user, m_pass, m_db);

    return bytes((byte*)m_cont_data.c_str(), m_cont_data.size());
  }

private:

};


/*
   Class Session
*/

void Session::send_connection_attr(const Options &options)
{

  struct Attr_converter
      : cdk::protocol::mysqlx::api::Any::Document
      , ds::Attr_processor
  {
    Attr_converter(const ds::Session_attributes* attr)
      :m_attr(attr)
    {}

    const ds::Session_attributes * m_attr;
    Processor::Any_prc::Doc_prc *m_attr_prc;

    void process(Processor &prc) const override
    {
      auto *self  = const_cast<Attr_converter*>(this);
      prc.doc_begin();
      self->m_attr_prc = prc.key_val("session_connect_attrs")->doc();
      self->m_attr_prc->doc_begin();
      m_attr->process(*self);
      self->m_attr_prc->doc_end();
      prc.doc_end();
    }

    void attr(const string &key, const string &val) override
    {
      m_attr_prc->key_val(key)->scalar()->str(bytes(val));
    }

  } ;

  if (options.attributes())
  {
    m_protocol.snd_CapabilitiesSet(Attr_converter(options.attributes())).wait();

    struct Check_reply_prc : cdk::protocol::mysqlx::Reply_processor
    {
      string m_msg;
      unsigned int m_code = 0;
      cdk::protocol::mysqlx::sql_state_t m_sql_state;
      void error(unsigned int code, short int,
                 cdk::protocol::mysqlx::sql_state_t state, const string &msg) override
      {
        m_code = code;
        m_sql_state = state;
        m_msg = msg;
      }

      void ok(string) override
      {}
    };

    Check_reply_prc prc;

    m_protocol.rcv_Reply(prc).wait();

    if(prc.m_code != 0 &&    prc.m_code != 5002)
    {
      //code: 5002
      //msg: "Capability \'session_connect_attrs\' doesn\'t exist"
      throw Server_error(prc.m_code, prc.m_sql_state, prc.m_msg);
    }

  }
}


void Session::send_auth()
{
  start_authentication(m_auth_interface->auth_method(),
                       m_auth_interface->auth_data(),
                       m_auth_interface->auth_response());

  start_reading_auth_reply();
}

void Session::do_authenticate(const Options &options,
                              int original_am,
                              bool  secure_conn)
{

  m_auth_interface.reset();

  using cdk::ds::mysqlx::Protocol_options;

  auto am = original_am;
  if (Protocol_options::DEFAULT == am)
    am = secure_conn ? Protocol_options::PLAIN : Protocol_options::MYSQL41;

  switch (am)
  {
    case Protocol_options::MYSQL41:
      m_auth_interface.reset(new AuthMysql41(options));
    break;
    case Protocol_options::PLAIN:
      m_auth_interface.reset(new AuthPlain(options));
    break;
    case Protocol_options::EXTERNAL:
      m_auth_interface.reset(new AuthExternal(options));
    break;
  case Protocol_options::SHA256_MEMORY:
    m_auth_interface.reset(new AuthSha256Memory(options));
  break;
    case Protocol_options::DEFAULT:
      assert(false);  // should not happen
    default:
      THROW("Unknown authentication method");
  }

  send_auth();

  if (!is_valid())
  {
    if (Protocol_options::DEFAULT == original_am && !secure_conn)
    {
      //Cleanup Diagnostic_area
      clear_errors();

      /*
        Second attempt does not throw errors. If auth fails, it will always
        throw below error
      */
      try{
      do_authenticate(options, Protocol_options::SHA256_MEMORY, secure_conn);
      } catch(...)
      {}

      if (!m_isvalid)
      {
        throw_error("Authentication failed using MYSQL41 and SHA256_MEMORY, "
                      "check username and password or try a secure connection");
      }
    }
  }
}

void Session::authenticate(const Options &options, bool  secure_conn)
{
  do_authenticate(options, options.auth_method(),secure_conn);
}


Session::~Session()
{
  //TODO: add timeout to close session!
  try
  {
    close();
  }
  catch (...)
  {
  }

  try
  {
    m_auth_interface.reset();
  }
  catch (...)
  {
  }
}


option_t Session::is_valid()
{
  wait();
  // TODO: should errors be thrown here, if any?
  return m_isvalid;
}


void Session::check_protocol_fields()
{
  wait();
  if (0 < entry_count())
    get_error().rethrow();
  if (m_proto_fields == UINT64_MAX)
  {
    Proto_field_checker field_checker(m_protocol);
    m_proto_fields = 0;
    /* More fields checks will be added here */
    m_proto_fields |= field_checker.is_supported(Protocol_fields::ROW_LOCKING);
    m_proto_fields |= field_checker.is_supported(Protocol_fields::UPSERT);
    m_proto_fields |= field_checker.is_supported(Protocol_fields::PREPARED_STATEMENTS);
    m_proto_fields |= field_checker.is_supported(Protocol_fields::KEEP_OPEN);
  }
}

bool Session::has_prepared_statements()
{
  check_protocol_fields();
  return (m_proto_fields & Protocol_fields::PREPARED_STATEMENTS) != 0;
}

void Session::set_has_prepared_statements(bool x)
{
  if (x)
    m_proto_fields |= Protocol_fields::PREPARED_STATEMENTS;
  else
    m_proto_fields &= ~Protocol_fields::PREPARED_STATEMENTS;
}

bool Session::has_keep_open()
{
  check_protocol_fields();
  return (m_proto_fields & Protocol_fields::KEEP_OPEN) != 0;
}


option_t Session::check_valid()
{
  //TODO: contact server to check session

  return  is_valid() ? true : false;
}

void Session::reset()
{
  check_protocol_fields(); // This will be used for lazy checks
  clear_errors();

  m_reply_op_queue.clear();

  if (is_valid())
  {
    m_protocol.snd_SessionReset(has_keep_open()).wait();

    m_protocol.rcv_Reply(*this).wait();

    if (!has_keep_open())
    {
      m_isvalid = false;
      send_auth();  // Re-authenticate for servers not supporting keep-open
    }
  }
}


void Session::close()
{
  m_reply_op_queue.clear();

  if (is_valid())
  {
    m_protocol.snd_ConnectionClose().wait();
    m_protocol.rcv_Reply(*this).wait();
  }
  m_isvalid = false;

}

void Session::register_reply(Reply *reply)
{
  // Complete previous reply

  if (m_current_reply)
  {
    m_current_reply->close_cursor();
    m_current_reply->discard();
  }
  m_current_reply = reply;
}

void Session::deregister_reply(Reply *reply)
{
  // TODO: Should reply be discared here?
  m_current_reply = nullptr;
}


Reply_init& Session::sql(uint32_t stmt_id,const string &stmt, Any_list *args)
{
  return set_command(
        new SndStmt(m_protocol, stmt_id, "sql", stmt, args));
}

void Session::Doc_args::process(Processor &prc) const
{
  Safe_prc<Any_list::Processor> sprc(prc);
  sprc->list_begin();
  if (m_doc)
    m_doc->process_if(sprc->list_el()->doc());
  sprc->list_end();
}


Reply_init& Session::admin(const char *cmd, const cdk::Any::Document &args)
{

  if (!is_valid())
    throw_error("admin: invalid session");

  m_cmd_args.m_doc = &args;

  m_stmt.set_utf8(cmd);
  return set_command(new SndStmt(m_protocol, 0, "mysqlx", m_stmt, &m_cmd_args));
}


/*
  Note: current implementation of transaction operations simply uses
  relevant SQL statements. Eventually we need something more fancy
  which will work well in a distributed environment.
*/

void Session::begin()
{
  Reply r(sql(0, "START TRANSACTION", nullptr));
  r.wait();
  if (r.entry_count() > 0)
    r.get_error().rethrow();
}

void Session::commit()
{
  Reply r(sql(0, "COMMIT", nullptr));
  r.wait();
  if (r.entry_count() > 0)
    r.get_error().rethrow();
}

void Session::rollback(const string &savepoint)
{
  string qry = "ROLLBACK";
  if (!savepoint.empty())
    qry += " TO `" + savepoint + "`";
  Reply r(sql(0, qry, nullptr));
  r.wait();
  if (r.entry_count() > 0)
    r.get_error().rethrow();
}

void Session::savepoint_set(const string &savepoint)
{
  // TODO: some chars in savepoint name need to be quotted.
  string qry = u"SAVEPOINT `" + savepoint + u"`";
  Reply r(sql(0, qry, nullptr));
  r.wait();
  if (r.entry_count() > 0)
    r.get_error().rethrow();
}

void Session::savepoint_remove(const string &savepoint)
{
  string qry = "RELEASE SAVEPOINT `" + savepoint + "`";
  Reply r(sql(0, qry, nullptr));
  r.wait();
  if (r.entry_count() > 0)
    r.get_error().rethrow();
}


Reply_init& Session::prepared_execute(uint32_t stmt_id,
                                      const Limit *lim,
                                      const Param_source *param
                                      )
{
    return set_command(
                new SndPrepareExecute(m_protocol, stmt_id, lim, param)
                );
}

Reply_init& Session::prepared_execute(uint32_t stmt_id,
                                      const cdk::Any_list *list
                                     )
{
    return set_command(
                new SndPrepareExecute(m_protocol, stmt_id, list)
                );
}


Reply_init& Session::prepared_deallocate(uint32_t stmt_id)
{
    return set_command(new SndPrepareDeallocate(m_protocol, stmt_id));
}


Reply_init& Session::coll_add(const Table_ref &coll,
                              Doc_source &docs,
                              const Param_source *param,
                              bool upsert)
{
  return set_command(
    new SndInsertDocs(m_protocol, 0, coll, docs, param, upsert)
  );
}

Reply_init& Session::coll_remove(uint32_t stmt_id,
                                 const Table_ref &coll,
                                 const Expression *expr,
                                 const Order_by *order_by,
                                 const Limit *lim,
                                 const Param_source *param)
{
  return set_command(
        new SndDelete<protocol::mysqlx::DOCUMENT>(
          m_protocol, stmt_id,
          coll, expr,order_by, lim, param)
        );
}

Reply_init& Session::coll_find(uint32_t stmt_id,
                               const Table_ref &coll,
                               const View_spec *view,
                               const Expression *expr,
                               const Expression::Document *proj,
                               const Order_by *order_by,
                               const Expr_list *group_by,
                               const Expression *having,
                               const Limit *lim,
                               const Param_source *param,
                               const Lock_mode_value lock_mode,
                               const Lock_contention_value lock_contention)
{
  if (lock_mode != Lock_mode_value::NONE &&
      !(m_proto_fields & Protocol_fields::ROW_LOCKING))
    throw_error("Row locking is not supported by this version of the server");

  SndFind<protocol::mysqlx::DOCUMENT> *find
      =  new SndFind<protocol::mysqlx::DOCUMENT>(
           m_protocol, stmt_id, coll, expr, proj,
           order_by,group_by, having, lim, param, lock_mode, lock_contention
           );

  if (view)
    return set_command(new SndViewCrud<protocol::mysqlx::DOCUMENT>(*view, find));

  return set_command(find);
}


Reply_init& Session::coll_update(uint32_t stmt_id,
                                 const api::Table_ref &coll,
                                 const Expression *expr,
                                 const Update_spec &us,
                                 const Order_by *order_by,
                                 const Limit *lim,
                                 const Param_source *param)
{
  return set_command(
        new SndUpdate<protocol::mysqlx::DOCUMENT>(
          m_protocol, stmt_id, coll, expr, us, order_by, lim, param)
        );
}

Reply_init& Session::table_insert(uint32_t stmt_id,
                                  const Table_ref &coll,
                                  Row_source &rows,
                                  const api::Columns *cols,
                                  const Param_source *param)
{
  return set_command(
        new SndInsertRows(m_protocol, stmt_id, coll, rows, cols, param)
        );
}

Reply_init& Session::table_delete(uint32_t stmt_id,
                                  const Table_ref &coll,
                                  const Expression *expr,
                                  const Order_by *order_by,
                                  const Limit *lim,
                                  const Param_source *param)
{
  return set_command(
        new SndDelete<protocol::mysqlx::TABLE>(
          m_protocol, stmt_id, coll, expr, order_by, lim, param)
        );
}

Reply_init& Session::table_select(uint32_t stmt_id,
                                  const Table_ref &coll,
                                  const View_spec *view,
                                  const Expression *expr,
                                  const Projection *proj,
                                  const Order_by *order_by,
                                  const Expr_list *group_by,
                                  const Expression *having,
                                  const Limit *lim,
                                  const Param_source *param,
                                  const Lock_mode_value lock_mode,
                                  const Lock_contention_value lock_contention)
{
  if (lock_mode != Lock_mode_value::NONE &&
      !(m_proto_fields & Protocol_fields::ROW_LOCKING))
    throw_error("Row locking is not supported by this version of the server");


  auto* select_cmd =
      new SndFind<protocol::mysqlx::TABLE>(
        m_protocol, stmt_id, coll, expr, proj, order_by,
        group_by, having, lim, param, lock_mode, lock_contention
        );

  if (view)
    return set_command(
          new SndViewCrud<protocol::mysqlx::TABLE>(*view, select_cmd)
          );

  return set_command(select_cmd);
}

Reply_init& Session::table_update(uint32_t stmt_id,
                                  const api::Table_ref &coll,
                                  const Expression *expr,
                                  const Update_spec &us,
                                  const Order_by *order_by,
                                  const Limit *lim,
                                  const Param_source *param)
{
  return set_command(
        new SndUpdate<protocol::mysqlx::TABLE>(
          m_protocol, stmt_id, coll, expr, us, order_by, lim, param)
        );

}


Reply_init& Session::view_drop(const api::Table_ref &view, bool check_existence)
{
  return set_command(
    new SndDropView(m_protocol, view, check_existence)
  );
}


Reply_init &Session::set_command(cdk::mysqlx::Proto_prepare_op *cmd)
{
  m_cmd.reset(cmd);

  m_prepare_prepare = m_cmd->prepare_prepare();

  if (!is_valid())
    throw_error("set_command: invalid session");

  return *this;
}


void Session::auth_ok(bytes data)
{
  m_isvalid = true;
}


void Session::auth_continue(bytes data)
{
  start_authentication_continue(m_auth_interface->auth_continue(data));
  start_reading_auth_reply();
}


void Session::auth_fail(bytes data)
{
  add_diagnostics(Severity::ERROR, error_code(cdkerrc::auth_failure),
                  std::string(data.begin(), data.end()));
  m_isvalid = false;
  m_auth_interface.reset();
}


void Session::notice(unsigned int type, short int scope, bytes payload)
{
  using namespace protocol::mysqlx;

  switch (type)
  {
  case notice_type::Warning:
    process_notice<notice_type::Warning>(
      payload,
      *static_cast<Reply_processor*>(this)
    );
    return;

  case notice_type::SessionStateChange:
    if (notice_scope::LOCAL != scope)
      return;
    process_notice<notice_type::SessionStateChange>(payload, *this);
    return;

  default: return;
  }
}


void Session::error(unsigned int code, short int severity,
                    sql_state_t sql_state, const string &msg)
{
  Severity::value level;
  switch (severity)
  {
  case 0: level = Severity::INFO; break;
  case 1: level = Severity::WARNING; break;
  case 2:
  default:
    level = Severity::ERROR; break;
  }
  add_diagnostics(level, code, sql_state, msg);
}


void Session::add_diagnostics(Severity::value level, error_code code,
                              const string &msg)
{
  if (m_current_reply)
  {
    m_current_reply->m_da.add_entry(level, new Error(code, msg));
  }
  else
  {
    m_da.add_entry(level, new Error(code, msg));
  }
}


void Session::add_diagnostics(Severity::value level, unsigned code,
                              sql_state_t sql_state, const string &msg)
{
  if (m_current_reply)
  {
    m_current_reply->m_da.add_entry(level, new Server_error(code, sql_state, msg));
    if (Severity::ERROR == level)
        m_current_reply->error();
  }
  else
  {
    m_da.add_entry(level, new Server_error(code, sql_state, msg));
  }
}


void Session::add_diagnostics(Severity::value level,
                              const Error *e,
                              bool report_to_reply)
{
  if (m_current_reply)
  {
    m_current_reply->m_da.add_entry(level, e);
    if (Severity::ERROR == level && report_to_reply)
        m_current_reply->error();
  }
  else
  {
    m_da.add_entry(level, e);
  }
}

void Session::ok(string)
{}

void Session::Prepare_processor::error(
      unsigned int code,
      short severity,
      protocol::mysqlx::Error_processor::sql_state_t sql_state,
      const protocol::mysqlx::Processor_base::string &msg)
{
  Severity::value level;
  switch (severity)
  {
  case 0: level = Severity::INFO; break;
  case 1: level = Severity::WARNING; break;
  case 2:
  default:
    level = Severity::ERROR; break;
  }

  m_session->add_diagnostics(
        level,
        new Server_prepare_error(code, sql_state, msg),
        false);
}


void Session::col_count(col_count_t nr_cols)
{
  //When all columns metadata arrived...
  m_nr_cols = nr_cols;
  m_has_results = m_nr_cols != 0;

  if (!m_has_results)
    start_reading_stmt_reply();

}


void Session::col_type(col_count_t pos, unsigned short type)
{
  if (m_discard)
    return;

  (*m_col_metadata)[pos].m_type = type;
}


void Session::col_content_type(col_count_t pos, unsigned short type)
{
  if (m_discard)
    return;

  (*m_col_metadata)[pos].m_content_type = type;
}

// TODO: original name should be optional (pointer)

void Session::col_name(col_count_t pos,
                       const string &name, const string &original)
{
  if (m_discard)
    return;

  Col_metadata &md= (*m_col_metadata)[pos];

  md.m_name= name;
  md.m_name_original = original;
  md.m_has_name_original= true;
}


void Session::col_table(col_count_t pos,
                        const string &table, const string &original)
{
  if (m_discard)
    return;

  Col_metadata &md= (*m_col_metadata)[pos];

  md.m_has_table= true;
  md.m_table.m_name= table;
  md.m_table.m_name_original = original;
  md.m_table.m_has_name_original= true;
}


// TODO: catalog is optional - should be a pointer?

void Session::col_schema(col_count_t pos,
                         const string &schema, const string &catalog)
{
  if (m_discard)
    return;

  Col_metadata &md= (*m_col_metadata)[pos];

  md.m_table.m_has_schema= true;
  md.m_table.m_schema.m_name= schema;
  md.m_table.m_schema.m_catalog.m_name = catalog;
}


void Session::col_collation(col_count_t pos, collation_id_t cs)
{
  if (m_discard)
    return;

  (*m_col_metadata)[pos].m_cs = cs;
}


void Session::col_length(col_count_t pos, uint32_t length)
{
  if (m_discard)
    return;

  (*m_col_metadata)[pos].m_length = length;
}


void Session::col_decimals(col_count_t pos, unsigned short decimals)
{
  if (m_discard)
    return;

  (*m_col_metadata)[pos].m_decimals = decimals;
}


void Session::col_flags(col_count_t pos, uint32_t flags)
{
  if (m_discard)
    return;

  (*m_col_metadata)[pos].m_flags = flags;
}


void Session::execute_ok()
{
  // All done!
  m_executed = true;
}


/*
  Processing session state change notices.
*/

void Session::client_id(unsigned long val)
{
  m_id = val;
}

void Session::account_expired()
{
  m_expired = true;
}

void Session::current_schema(const string &val)
{
  m_cur_schema = val;
}

void Session::last_insert_id(insert_id_t val)
{
  m_stmt_stats.last_insert_id = val;
}

void Session::row_stats(row_stats_t stats, row_count_t val)
{
  switch (stats)
  {
  case ROWS_AFFECTED: m_stmt_stats.rows_affected = val; return;
  case    ROWS_FOUND: m_stmt_stats.rows_found = val;    return;
  case  ROWS_MATCHED: m_stmt_stats.rows_matched = val;  return;
  }
}

void Session::generated_document_id(const std::string& id)
{
  if (m_current_reply)
  {
    m_current_reply->m_generated_ids.push_back(id);
  }
}

void Session::send_cmd()
{
  m_executed = false;
  m_reply_op_queue.push_back(m_cmd);
  m_cmd.reset();
  m_stmt_stats.clear();
}

void Session::start_reading_result()
{
  if (m_prepare_prepare)
  {
    //Reade PreparePrepare reply using the prepare_processor.
    m_reply_op_queue.push_back(
      shared_ptr<Proto_op>(new RcvReply(m_protocol, m_prepare_prc))
    );
    m_prepare_prepare = false;
  }

  m_col_metadata.reset(new Mdata_storage());
  m_executed = false;
  m_reply_op_queue.push_back(
    shared_ptr<Proto_op>(new RcvMetaData(m_protocol, *this))
  );

}


Proto_op*
Session::start_reading_row_data(protocol::mysqlx::Row_processor &prc)
{
  return &m_protocol.rcv_Rows(prc);
}


void Session::start_reading_stmt_reply()
{
  m_reply_op_queue.push_back(
    shared_ptr<Proto_op>(new RcvStmtReply(m_protocol, *this))
  );
}


void Session::start_authentication(const char* mechanism,
                                   bytes data,
                                   bytes response)
{
  m_op_queue.push_back(
    shared_ptr<Proto_op>(
      new SndAuthStart(m_protocol, mechanism, data, response)
    )
  );
}


void Session::start_authentication_continue(bytes data)
{
  m_op_queue.push_back(
    shared_ptr<Proto_op>(new SndAuthContinue(m_protocol, data))
  );
}


void Session::start_reading_auth_reply()
{
  m_op_queue.push_back(
    shared_ptr<Proto_op>(new RcvAuthReply(m_protocol, *this))
  );
}


bool Session::is_completed() const
{
  if (!m_op_queue.empty())
    return false;

  return true;
}


bool Session::do_cont()
{
  if (m_op_queue.empty())
    return true;

  bool done;

  try
  {
    done = m_op_queue.front()->cont();
  }
  catch (...)
  {
    m_op_queue.pop_front();
    throw;
  }

  if (done)
    m_op_queue.pop_front();

  return false;
};


void Session::do_wait()
{
  while (!is_completed())
  {
    try
    {
      m_op_queue.front()->wait();
    }
    catch (...)
    {
      m_op_queue.pop_front();
      throw;
    }

    m_op_queue.pop_front();
  }
}


void Session::do_cancel()
{
  while (!m_op_queue.empty())
  {
    try
    {
      m_op_queue.front()->cancel();
    }
    catch (...)
    {
      m_op_queue.pop_front();
      throw;
    }

    m_op_queue.pop_front();
  }
}

const cdk::api::Event_info* Session::get_event_info() const
{
  if (!m_op_queue.empty())
    return m_op_queue.front()->waits_for();

  return nullptr;
}


}} // cdk mysqlx




