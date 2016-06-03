/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * This code is licensed under the terms of the GPLv2
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

#include <mysql/cdk/mysqlx.h>
#include <mysql/cdk/protocol/mysqlx.h>

PUSH_SYS_WARNINGS
#include <iostream>
#include "auth_mysql41.h"
POP_SYS_WARNINGS

#include "delayed_op.h"

namespace cdk {
namespace mysqlx {


class error_category_server : public error_category
{
public:

  error_category_server() {}

  virtual const char* name() const { return "server"; }
  virtual std::string message(int) const { return "Server Error"; }
  virtual error_condition default_error_condition(int) const;
  virtual bool  equivalent(int, const error_condition&) const;

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
error_category_server::default_error_condition(int errc) const
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

bool error_category_server::equivalent(int code,
                                       const error_condition &ec) const
{
  try
  {
    return ec == default_error_condition(code);
  }
  catch (...)
  {
  }
  return false;
}


/*
   Authentication
*/


class SessionAuthInterface
{
public:

  virtual ~SessionAuthInterface() {}

  virtual const char* auth_method() = 0;
  virtual bytes auth_data() = 0;
  virtual bytes auth_response() = 0;
  virtual bytes auth_continue(bytes) = 0;
};


class AuthPlain
    : public SessionAuthInterface
{

  std::string m_data;

  public:

  AuthPlain( const ds::Options &options )
  {
    std::string user(options.user());  // convert to utf8 before sending


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
    return bytes((byte*)NULL, (byte*)NULL);
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

  const ds::Options &m_options;
  std::string m_cont_data;

public:
  AuthMysql41(const ds::Options &options)
    : m_options(options)

  {}


  const char* auth_method() { return "MYSQL41";}
  virtual bytes auth_data()
  {
    return bytes((byte*)NULL, (byte*)NULL);
  }

  virtual bytes auth_response()
  {
    return bytes((byte*)NULL, (byte*)NULL);
  }

  virtual bytes auth_continue(bytes data)
  {
    std::string user(m_options.user());
    std::string pass;
    if (m_options.password())
      pass = *m_options.password();

    m_cont_data = ::mysqlx::build_mysql41_authentication_response(std::string(data.begin(), data.end()),
                                                                  user,
                                                                  pass,
                                                                  "");

   return bytes((byte*)m_cont_data.c_str(), m_cont_data.size());
  }

private:

};


/*
   Class Session
*/


void Session::authenticate( const ds::Options &options)
{

  delete m_auth_interface;
  m_auth_interface = NULL;
  m_auth_interface = new AuthMysql41(options);

  start_authentication(m_auth_interface->auth_method(),
                       m_auth_interface->auth_data(),
                       m_auth_interface->auth_response());

  start_reading_auth_reply();
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
    delete m_auth_interface;
  }
  catch (...)
  {
  }
}


option_t Session::is_valid()
{
  wait();

  return m_isvalid;
}


option_t Session::check_valid()
{
  //TODO: contact server to check session

  return  is_valid() ? true : false;
}


void Session::close()
{
  m_reply_op_queue.clear();

  if (is_valid())
  {
    m_protocol.snd_Close().wait();
    //    TODO: Uncomment this line when srever implements Close OK reply message
    //    m_protocol.rcv_Reply(*this).wait();
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
  m_current_reply = NULL;
}


Reply_init& Session::sql(const string &stmt, Any_list *args)
{
  return set_command(new SndStmt(m_protocol, "sql", stmt, args));

}

Reply_init& Session::admin(const char *cmd, Any_list &args)
{
  if (!is_valid())
    throw_error("admin: invalid session");

  m_stmt.set_utf8(cmd);
  m_cmd.reset(new SndStmt(m_protocol, "xplugin", m_stmt, &args));
  return *this;
}

Reply_init& Session::coll_add(const Table_ref &coll, Doc_source &docs, const Param_source *param)
{
  return set_command(
    new SndInsertDocs(m_protocol, coll.schema()->name(), coll.name(), docs, param)
  );
}

Reply_init& Session::coll_remove(const Table_ref &coll,
                                 const Expression *expr,
                                 const Order_by *order_by,
                                 const Limit *lim,
                                 const Param_source *param)
{
  return set_command(
    new SndDelete(m_protocol, protocol::mysqlx::DOCUMENT,
                  coll.schema()->name(), coll.name(), expr,order_by, lim, param)
  );
}

Reply_init& Session::coll_find(const Table_ref &coll,
                               const Expression *expr,
                               const Expression::Document *proj,
                               const Order_by *order_by,
                               const Expr_list *group_by,
                               const Expression *having,
                               const Limit *lim,
                               const Param_source *param)
{
  return set_command(
    new SndFind<protocol::mysqlx::DOCUMENT>(m_protocol,
                coll.schema()->name(), coll.name(), expr,
                proj, order_by, group_by, having, lim, param)
  );
}

Reply_init& Session::coll_update(const api::Table_ref &coll,
                                 const Expression *expr,
                                 const Update_spec &us,
                                 const Order_by *order_by,
                                 const Limit *lim,
                                 const Param_source *param)
{
  return set_command(
    new SndUpdate<protocol::mysqlx::DOCUMENT>(m_protocol,
                                              coll.schema()->name(),
                                              coll.name(),
                                              expr,
                                              us,
                                              order_by,
                                              lim,
                                              param)
  );
}

Reply_init& Session::table_insert(const Table_ref &coll, Row_source &rows,
                                  const api::Columns *cols, const Param_source *param)
{
  return set_command(
    new SndInsertRows(m_protocol, coll.schema()->name(), coll.name(), rows, cols, param)
  );
}

Reply_init& Session::table_delete(const Table_ref &coll,
                                  const Expression *expr,
                                  const Order_by *order_by,
                                  const Limit *lim,
                                  const Param_source *param)
{
  return set_command(
    new SndDelete(m_protocol, protocol::mysqlx::TABLE,
                  coll.schema()->name(), coll.name(),
                  expr, order_by, lim, param)
  );
}

Reply_init& Session::table_select(const Table_ref &coll,
                                  const Expression *expr,
                                  const Projection *proj,
                                  const Order_by *order_by,
                                  const Expr_list *group_by,
                                  const Expression *having,
                                  const Limit *lim,
                                  const Param_source *param)
{
  return set_command(
    new SndFind<protocol::mysqlx::TABLE>(m_protocol,
                coll.schema()->name(), coll.name(), expr, proj,
                order_by, group_by, having, lim, param)
  );
}

Reply_init& Session::table_update(const api::Table_ref &coll,
                                  const Expression *expr,
                                  const Update_spec &us,
                                  const Order_by *order_by,
                                  const Limit *lim,
                                  const Param_source *param)
{
  return set_command(
    new SndUpdate<protocol::mysqlx::TABLE>(m_protocol,
                                           coll.schema()->name(),
                                           coll.name(),
                                           expr,
                                           us,
                                           order_by,
                                           lim,
                                           param)
  );
}


Reply_init &Session::set_command(Proto_op *cmd)
{
  if (!is_valid())
    throw_error("set_command: invalid session");

  m_cmd.reset(cmd);

  return *this;
}


void Session::auth_ok(bytes data)
{
  m_isvalid = true;
  delete m_auth_interface;
  m_auth_interface = NULL;
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
  delete m_auth_interface;
  m_auth_interface = NULL;
}


void Session::error(unsigned int code, short int severity,
                    sql_state_t sql_state, const string &msg)
{

  if (m_current_reply)
  {
    m_current_reply->m_da.add_entry(Severity::ERROR,
                                    new Server_error(code, sql_state, msg));
    m_current_reply->m_error = true;
  }
  else
  {
    m_da.add_entry(Severity::ERROR, new Server_error(code, sql_state, msg));
  }
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
  }
  else
  {
    m_da.add_entry(level, new Server_error(code, sql_state, msg));
  }
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

  m_col_metadata[pos].m_type = type;
}


void Session::col_content_type(col_count_t pos, unsigned short type)
{
  if (m_discard)
    return;

  m_col_metadata[pos].m_content_type = type;
}

// TODO: original name should be optional (pointer)

void Session::col_name(col_count_t pos,
                       const string &name, const string &original)
{
  if (m_discard)
    return;

  Col_metadata &md= m_col_metadata[pos];

  md.m_name= name;
  md.m_name_original = original;
  md.m_has_name_original= true;
}


void Session::col_table(col_count_t pos,
                        const string &table, const string &original)
{
  if (m_discard)
    return;

  Col_metadata &md= m_col_metadata[pos];

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

  Col_metadata &md= m_col_metadata[pos];

  md.m_table.m_has_schema= true;
  md.m_table.m_schema.m_name= schema;
  md.m_table.m_schema.m_catalog.m_name = catalog;
}


void Session::col_charset(col_count_t pos, charset_id_t cs)
{
  if (m_discard)
    return;

  m_col_metadata[pos].m_cs = cs;
}


void Session::col_length(col_count_t pos, uint32_t length)
{
  if (m_discard)
    return;

  m_col_metadata[pos].m_length = length;
}


void Session::col_decimals(col_count_t pos, unsigned short decimals)
{
  if (m_discard)
    return;

  m_col_metadata[pos].m_decimals = decimals;
}


void Session::col_flags(col_count_t pos, uint32_t flags)
{
  if (m_discard)
    return;

  m_col_metadata[pos].m_flags = flags;
}


void Session::rows_affected(row_count_t row_count)
{
  m_affected_rows = row_count;
}


void Session::last_insert_id(protocol::mysqlx::row_count_t row_count)
{
  m_last_insert_id = row_count;
}


void Session::cursor_close_ok()
{

}


void Session::stmt_close_ok()
{

}


void Session::execute_ok()
{
  /*
     All done!
  */

  m_executed = true;
}


void Session::send_cmd()
{
  m_executed = false;
  m_reply_op_queue.push_back(m_cmd);
  m_cmd.reset();
}


void Session::start_reading_row_set()
{
  m_col_metadata.clear();
  m_executed = false;
  m_reply_op_queue.push_back(
    shared_ptr<Proto_op>(new RcvMetaData(m_protocol, *this))
  );
}


Proto_op* Session::start_reading_row_data(Row_processor &prc)
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

  return NULL;
}


}} // cdk mysqlx




