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

/*
  Implementation of mysqlx protocol API: SQL statement execution
  ==============================================================
*/


#include "protocol.h"
#include "builders.h"

PUSH_PB_WARNINGS
#include "protobuf/mysqlx_sql.pb.h"
POP_PB_WARNINGS


using namespace cdk::foundation;
using namespace google::protobuf;
using namespace cdk::protocol::mysqlx;


/*
  Implementation of Protocol methods using the internal implementation.
*/

namespace cdk {
namespace protocol {
namespace mysqlx {


/*
  To use Array_builder template to create a builder for storing statement
  arguments inside Mysqlx::Sql::StmtExecute message, define Arr_msg_traits<>
  for this type of message. The traits tell that adding new element to
  the array means adding a new argument inside StmtExecute message.
*/

template<>
struct Arr_msg_traits<Mysqlx::Sql::StmtExecute>
{
  typedef Mysqlx::Sql::StmtExecute Array;
  typedef Mysqlx::Datatypes::Any   Msg;

  static Msg& add_element(Array &arr)
  {
    return *arr.add_args();
  }
};


Protocol::Op& Protocol::snd_StmtExecute(const char *ns,
                                        const string &stmt,
                                        const api::Any_list *args)
{
  Mysqlx::Sql::StmtExecute stmt_exec;

  if (ns)
    stmt_exec.set_namespace_(ns);

  stmt_exec.set_stmt(stmt);

  if (args)
  {
    Array_builder<Any_builder, Mysqlx::Sql::StmtExecute> args_builder;
    args_builder.reset(stmt_exec);
    args->process(args_builder);
  }

  return get_impl().snd_start(stmt_exec, msg_type::cli_StmtExecute);
}


Protocol::Op& Protocol_server::snd_StmtExecuteOk()
{
  Mysqlx::Sql::StmtExecuteOk ok;
  return get_impl().snd_start(ok, msg_type::StmtExecuteOk);
}

#if 0

Protocol::Op& Protocol::snd_PrepareStmt(stmt_id_t id, const string &stmt)
{
/*
  Mysqlx::Sql::PrepareStmt prep_stmt;

  prep_stmt.set_stmt_id(id);
  prep_stmt.set_stmt(stmt);

  return get_impl().snd_message(prep_stmt, msg_type::cli_PrepareStmt);
*/

  // Code is commented out because of missing prepare statements in .proto files
  throw_error("Not implemented");

  // this is to keep the compiler happy
  return get_impl().snd_message(Mysqlx::Sql::StmtExecute(), msg_type::cli_StmtExecute);
}


// TODO: param_row should be supplied as well
Protocol::Op& Protocol::snd_PreparedStmtExecute(stmt_id_t id, cursor_id_t cid)
{
/*
  Mysqlx::Sql::PreparedStmtExecute prep_stmt_ex;

  prep_stmt_ex.set_stmt_id(id);
  prep_stmt_ex.set_cursor_id(cid);

  return get_impl().snd_message(prep_stmt_ex, msg_type::cli_PreparedStmtExecute);
*/

  // Code is commented out because of missing prepare statements in .proto files
  throw_error("Not implemented");

  // this is to keep the compiler happy
  return get_impl().snd_message(Mysqlx::Sql::StmtExecute(), msg_type::cli_StmtExecute);
}


Protocol::Op& Protocol::snd_CursorClose(cursor_id_t cid)
{
/*
  Mysqlx::Sql::CursorClose cur_close;
  cur_close.set_cursor_id(cid);

  return get_impl().snd_message(cur_close, msg_type::cli_CursorClose);
*/

  // Code is commented out because of missing prepare statements in .proto files
  throw_error("Not implemented");

  // this is to keep the compiler happy
  return get_impl().snd_message(Mysqlx::Sql::StmtExecute(), msg_type::cli_StmtExecute);
}


Protocol::Op& Protocol::snd_PreparedStmtClose(stmt_id_t id)
{
/*
  Mysqlx::Sql::PreparedStmtClose stmt_close;
  stmt_close.set_stmt_id(id);

  return get_impl().snd_message(stmt_close, msg_type::cli_PreparedStmtClose);
*/

  // Code is commented out because of missing prepare statements in .proto files
  throw_error("Not implemented");

  // this is to keep the compiler happy
  return get_impl().snd_message(Mysqlx::Sql::StmtExecute(), msg_type::cli_StmtExecute);

}

#endif


}}}  // cdk::protocol::mysqlx


namespace cdk {
namespace protocol {
namespace mysqlx {


// Commented out because of protocol changes

#if 0

template<>
void Protocol::Impl::process_msg_with(Mysqlx::Sql::CursorFetchDone &msg,
                                 Stmt_processor &prc)
{
  throw_error("Invalid rcv_StmtReply() after snd_StmtExecute() that"
              " produced results");
}

template<>
void Protocol::Impl::process_msg_with(Mysqlx::Sql::PrepareStmtOk &msg,
                                 Stmt_processor &prc)
{
  prc.prepare_ok();
}
*/

template<>
void Protocol::Impl::process_msg_with(Mysqlx::Sql::PreparedStmtExecuteOk &msg,
                                 Stmt_processor &prc)
{
  prc.rows_affected(msg.rows_affected());
  prc.last_insert_id(msg.last_insert_id());
  prc.execute_ok();
}

template<>
void Protocol::Impl::process_msg_with(Mysqlx::Ok &msg,
                                 Stmt_processor &prc)
{
  prc.stmt_close_ok();
}


template<>
void Protocol::Impl::process_msg_with(Mysqlx::Sql::CursorCloseOk &msg,
                                 Stmt_processor &prc)
{
  prc.cursor_close_ok();
}

#endif


}}}  // cdk::protocol::mysqlx

