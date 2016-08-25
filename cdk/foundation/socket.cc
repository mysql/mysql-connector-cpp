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

#include <mysql/cdk/foundation/socket.h>
#include <mysql/cdk/foundation/opaque_impl.i>
#include "connection_tcpip_base.h"


class Socket_conn_impl
  : public cdk::foundation::connection::TCPIP::Impl
{
public:
  unsigned short m_port;
  Socket_conn_impl(unsigned short port);
  void do_connect();
};

IMPL_TYPE(cdk::foundation::Socket::Connection, Socket_conn_impl);
IMPL_PLAIN(cdk::foundation::Socket::Connection);

Socket_conn_impl::Socket_conn_impl(unsigned short port)
  : m_port(port)
{}


void Socket_conn_impl::do_connect()
{
  m_sock = cdk::foundation::connection::detail::listen_and_accept(m_port);
}


namespace cdk {
namespace foundation {


Socket::Connection::Connection(const Socket &sock)
  : connection::TCPIP("", sock.m_port)
  , opaque_impl<Socket::Connection>(NULL, sock.m_port)
{}

Socket::Connection::Impl& Socket::Connection::get_base_impl()
{
  return opaque_impl<Socket::Connection>::get_impl();
}

void Socket::Connection::do_wait()
{
  connect();
}

bool Socket::Connection::is_completed() const
{
  return TCPIP_base::get_base_impl().is_open();
}


}} // sdk::foundation

