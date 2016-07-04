/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C++ is licensed under the terms of the GPLv2
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

#ifndef IMPL_FOUNDATION_CONNECTION_TCPIP_BASE_H
#define IMPL_FOUNDATION_CONNECTION_TCPIP_BASE_H

PUSH_SYS_WARNINGS
#include <sys/types.h>
POP_SYS_WARNINGS

#include "socket_detail.h"

namespace cdk {
namespace foundation {
namespace connection {


class TCPIP_base::Impl
{
  class Socket_system_initializer
  {
  public:
    Socket_system_initializer()
    {
      detail::initialize_socket_system();
    }

    ~Socket_system_initializer()
    {
      try
      {
        detail::uninitialize_socket_system();
      }
      catch (...)
      {
        // Ignoring errors in destructor.
      }
    }
  };

public:
  typedef detail::Socket socket;

  socket m_sock;

  Impl()
    : m_sock(detail::NULL_SOCKET)
  {
    // This will initialize socket system (e.g. Winsock) during construction of first CDK connection.
    static Socket_system_initializer initializer;
  }

  bool is_open() const
  {
    return m_sock != detail::NULL_SOCKET;
  }

  void close()
  {
    if (is_open())
    {
      try
      {
        detail::shutdown(m_sock, detail::SHUTDOWN_MODE_BOTH);
      }
      catch (...)
      {
      }

      detail::close(m_sock);
      m_sock = detail::NULL_SOCKET;
    }
  }

  std::size_t available() const
  {
    if (!is_open())
      return 0;

    try
    {
      return detail::bytes_available(m_sock);
    }
    catch (...)
    {
      // We couldn't establish if there's still data to be read. Assuming there isn't.
      return 0;
    }
  }

  bool has_space() const
  {
    if (!is_open())
      return false;
    return detail::select_one(m_sock, detail::SELECT_MODE_WRITE, false) > 0;
  }

  virtual ~Impl()
  {
    close();
  }

  virtual void do_connect() =0;
};


}}}  // cdk::foundation::connection

#endif
