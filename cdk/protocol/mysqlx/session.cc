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

