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
  Implementation of mysqlx protocol API: session handling
  =======================================================
*/


#include "protocol.h"
#include "builders.h"


PUSH_PB_WARNINGS
#include "protobuf/mysqlx_session.pb.h"
#include "protobuf/mysqlx_crud.pb.h"
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


// Client-side API


struct Cap_builder : api::Any::Document::Processor
{
  Mysqlx::Connection::Capabilities *m_msg;
  Any_builder m_ab;

  Cap_builder() : m_msg(NULL)
  {}

  void reset(Mysqlx::Connection::CapabilitiesSet &msg)
  {
    m_msg = msg.mutable_capabilities();
  }

  Any_prc* key_val(const string &key)
  {
    Mysqlx::Connection::Capability *cap = m_msg->add_capabilities();
    cap->set_name(key);
    m_ab.reset(*cap->mutable_value());
    return &m_ab;
  }
};


Protocol::Op& Protocol::snd_CapabilitiesSet(const api::Any::Document& caps)
{
  Mysqlx::Connection::CapabilitiesSet msg;
  Cap_builder builder;
  builder.reset(msg);
  caps.process(builder);
  return get_impl().snd_start(msg, msg_type::cli_CapabilitiesSet);
}


Protocol::Op& Protocol::snd_AuthenticateStart(const char* mechanism,
                                              bytes data,
                                              bytes response)
{
  Mysqlx::Session::AuthenticateStart auth_start;

  auth_start.set_mech_name(mechanism);
  auth_start.set_auth_data((const void*)data.begin(),
                           data.size());

  auth_start.set_initial_response((const void*)response.begin(),
                                  response.size());
  return get_impl().snd_start(auth_start, msg_type::cli_AuthenticateStart);
}


Protocol::Op& Protocol::snd_AuthenticateContinue(bytes data)
{
  Mysqlx::Session::AuthenticateContinue auth_cont;

  auth_cont.set_auth_data((const void*)data.begin(), data.size());

  return get_impl().snd_start(auth_cont, msg_type::cli_AuthenticateContinue);
}


class Rcv_auth_base : public Op_rcv
{
public:

  Rcv_auth_base(Protocol_impl &proto) : Op_rcv(proto)
  {}

  void resume(Auth_processor &prc)
  {
    read_msg(prc);
  }

  Next_msg do_next_msg(msg_type_t type)
  {
    switch (type)
    {
    case msg_type::AuthenticateOk:
    case msg_type::AuthenticateContinue:
      return EXPECTED;
    default: return UNEXPECTED;
    }
  }

  template<class MSG, class PRC>
  void process_msg_with(MSG&, PRC&)
  {
    // TODO: better error description (message/processor type info)
    throw_error("Invalid processor used to process server reply");
  }
};


template<>
void Rcv_auth_base::process_msg_with(Mysqlx::Session::AuthenticateOk &msg,
                                Auth_processor &prc)
{
  bytes data((byte*)msg.auth_data().data(), msg.auth_data().length());
  prc.auth_ok(data);
}

template<>
void Rcv_auth_base::process_msg_with(Mysqlx::Session::AuthenticateContinue &msg,
                                Auth_processor &prc)
{
  bytes data((byte*)msg.auth_data().data(), msg.auth_data().length());
  prc.auth_continue(data);
}


class Rcv_auth : public Message_dispatcher<Rcv_auth_base>
{
public:

  Rcv_auth(Protocol_impl &impl) : Dispatcher(impl)
  {}

  void do_process_msg(msg_type_t type, Message &msg)
  {
    Dispatcher::process_msg_with(type, msg, *static_cast<Auth_processor*>(m_prc));
  }
};



Protocol::Op& Protocol::rcv_AuthenticateReply(Auth_processor &prc)
{
  return get_impl().rcv_start<Rcv_auth>(prc);
}


// Parsing and processing protocol notices.

template<>
void process_notice<notice_type::SessionStateChange>(
  const bytes &notice,
  SessionState_processor &prc
  )
{
  Mysqlx::Notice::SessionStateChanged msg;

  if (!msg.ParseFromString(std::string(notice.begin(), notice.end())))
    THROW("Could not parse notice payload");

  switch (msg.param())
  {
  case Mysqlx::Notice::SessionStateChanged::CLIENT_ID_ASSIGNED:
  {
    uint64_t id = msg.value().v_unsigned_int();
    assert(id < std::numeric_limits<unsigned long>::max());
    prc.client_id((unsigned long)id);
    break;
  }

  case Mysqlx::Notice::SessionStateChanged::ACCOUNT_EXPIRED:
    prc.account_expired();
    break;

  case Mysqlx::Notice::SessionStateChanged::CURRENT_SCHEMA:
    assert(msg.value().has_v_string());
    // NOTE: Assuming the reported schema name is in utf8 encoding
    prc.current_schema(msg.value().v_string().value());
    break;

  case Mysqlx::Notice::SessionStateChanged::ROWS_AFFECTED:
    assert(msg.value().has_v_unsigned_int());
    prc.row_stats(prc.ROWS_AFFECTED, msg.value().v_unsigned_int());
    break;

  case Mysqlx::Notice::SessionStateChanged::ROWS_FOUND:
    assert(msg.value().has_v_unsigned_int());
    prc.row_stats(prc.ROWS_FOUND, msg.value().v_unsigned_int());
    break;

  case Mysqlx::Notice::SessionStateChanged::ROWS_MATCHED:
    assert(msg.value().has_v_unsigned_int());
    prc.row_stats(prc.ROWS_MATCHED, msg.value().v_unsigned_int());
    break;

  case Mysqlx::Notice::SessionStateChanged::GENERATED_INSERT_ID:
    assert(msg.value().has_v_unsigned_int());
    prc.last_insert_id(msg.value().v_unsigned_int());
    break;

  case Mysqlx::Notice::SessionStateChanged::TRX_COMMITTED:
    prc.trx_event(prc.COMMIT);
    break;

  case Mysqlx::Notice::SessionStateChanged::TRX_ROLLEDBACK:
    prc.trx_event(prc.ROLLBACK);
    break;

  case Mysqlx::Notice::SessionStateChanged::PRODUCED_MESSAGE:
  default: break;
  }
}

template<>
void process_notice<notice_type::Warning>(
  const bytes &notice,
  Error_processor &prc
  )
{
  Mysqlx::Notice::Warning msg;

  if (!msg.ParseFromString(std::string(notice.begin(), notice.end())))
    THROW("Could not parse notice payload");

  short int level;

  switch (msg.level())
  {
  case Mysqlx::Notice::Warning::ERROR:   level = 2; break;
  case Mysqlx::Notice::Warning::WARNING: level = 1; break;
  case Mysqlx::Notice::Warning::NOTE:
  default:
    level = 0; break;
  }

  prc.error(msg.code(), level, sql_state_t(), msg.msg());
}


// Server-side API


Protocol::Op& Protocol_server::snd_AuthenticateContinue(bytes data)
{
  Mysqlx::Session::AuthenticateContinue auth_cont;
  auth_cont.set_auth_data(data.begin(), data.size());
  return get_impl().snd_start(auth_cont, msg_type::AuthenticateContinue);
}

Protocol::Op& Protocol_server::snd_AuthenticateOK(bytes data)
{
  Mysqlx::Session::AuthenticateOk msg_auth_ok;
  msg_auth_ok.set_auth_data(data.begin(), data.size());
  return get_impl().snd_start(msg_auth_ok, msg_type::AuthenticateOk);
}


class Rcv_init_base : public Op_rcv
{
public:

  Rcv_init_base(Protocol_impl &proto) : Op_rcv(proto)
  {}

  void resume(Init_processor &prc)
  {
    read_msg(prc);
  }

  Next_msg next_msg(msg_type_t type)
  {
    switch (type)
    {
    case msg_type::cli_AuthenticateStart:
    case msg_type::cli_AuthenticateContinue:
      return EXPECTED;
    default: return UNEXPECTED;
    }
  }

  template<class MSG, class PRC>
  void process_msg_with(MSG&, PRC&)
  {
    // TODO: better error description (message/processor type info)
    throw_error("Invalid processor used to process server reply");
  }
};


template<>
void Rcv_init_base::process_msg_with(Mysqlx::Session::AuthenticateStart &msg,
                                Init_processor &ip)
{
   bytes data((byte*)msg.auth_data().data(), msg.auth_data().length());
   bytes response((byte*)msg.initial_response().data(), msg.initial_response().length());
   ip.auth_start(msg.mech_name().c_str(), data, response);
}

template<>
void Rcv_init_base::process_msg_with(Mysqlx::Session::AuthenticateContinue &msg,
                                Init_processor &ip)
{
  bytes data((byte*)msg.auth_data().data(), msg.auth_data().length());
  ip.auth_continue(data);
}


class Rcv_init : public Message_dispatcher<Rcv_init_base>
{
public:

  Rcv_init(Protocol_impl &impl) : Dispatcher(impl)
  {}

  void process_msg(msg_type_t type, Message &msg)
  {
    Dispatcher::process_msg_with(type, msg, *static_cast<Init_processor*>(m_prc));
  }
};


Protocol::Op& Protocol_server::rcv_InitMessage(Init_processor &prc)
{
  return get_impl().rcv_start<Rcv_init>(prc);
}


}}}  // cdk::protocol::mysqlx

