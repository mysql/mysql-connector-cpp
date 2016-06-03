/*
* Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
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

#include "socket_detail.h"
#include <mysql/cdk/foundation/error.h>
#include <mysql/cdk/foundation/connection_tcpip.h>


namespace cdk {
namespace foundation {
namespace connection {
namespace detail {


#ifdef _WIN32


/*
  error_category_winsock class
  ============================

  Used for handling Winsock errors.
*/

class error_category_winsock : public error_category
{
  error_category_winsock() {}

  const char* name() const { return "winsock"; }
  std::string message(int) const;

  error_condition default_error_condition(int code) const
  {
    switch (code)
    {
      case WSAEACCES: return errc::permission_denied;
      case WSAEADDRINUSE: return errc::address_in_use;
      case WSAEADDRNOTAVAIL: return errc::address_not_available;
      case WSAEAFNOSUPPORT: return errc::address_family_not_supported;
      case WSAEALREADY: return errc::connection_already_in_progress;
      case WSAEBADF: return errc::bad_file_descriptor;
      case WSAECONNABORTED: return errc::connection_aborted;
      case WSAECONNREFUSED: return errc::connection_refused;
      case WSAECONNRESET: return errc::connection_reset;
      case WSAEDESTADDRREQ: return errc::destination_address_required;
      case WSAEFAULT: return errc::bad_address;
      case WSAEHOSTUNREACH: return errc::host_unreachable;
      case WSAEINPROGRESS: return errc::operation_in_progress;
      case WSAEINTR: return errc::interrupted;
      case WSAEINVAL: return errc::invalid_argument;
      case WSAEISCONN: return errc::already_connected;
      case WSAEMFILE: return errc::too_many_files_open;
      case WSAEMSGSIZE: return errc::message_size;
      case WSAENAMETOOLONG: return errc::filename_too_long;
      case WSAENETDOWN: return errc::network_down;
      case WSAENETRESET: return errc::network_reset;
      case WSAENETUNREACH: return errc::network_unreachable;
      case WSAENOBUFS: return errc::no_buffer_space;
      case WSAENOPROTOOPT: return errc::no_protocol_option;
      case WSAENOTCONN: return errc::not_connected;
      case WSAENOTSOCK: return errc::not_a_socket;
      case WSAEOPNOTSUPP: return errc::operation_not_supported;
      case WSAEPROTONOSUPPORT: return errc::protocol_not_supported;
      case WSAEPROTOTYPE: return errc::wrong_protocol_type;
      case WSAETIMEDOUT: return errc::timed_out;
      case WSAEWOULDBLOCK: return errc::operation_would_block;
      default:
        throw_error(code, winsock_error_category());
        return errc::no_error;  // suppress copile warnings
    }

  }

  bool equivalent(int code, const error_condition &ec) const
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

  friend const error_category& winsock_error_category();
};


std::string error_category_winsock::message(int code) const
{
  std::string message;
  LPSTR buffer = NULL;

  // Note: on windows error codes are unsigned
  assert(code > 0);

  DWORD result = ::FormatMessageA(
    FORMAT_MESSAGE_ALLOCATE_BUFFER
    | FORMAT_MESSAGE_FROM_SYSTEM
    | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, static_cast<DWORD>(code),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPSTR)&buffer, 0, NULL
  );

  if (result == 0)
    message = "Unknown Winsock error";
  else
    message = buffer;

  ::LocalFree(buffer);

  return message;
}


const error_category& winsock_error_category()
{
  static const error_category_winsock instance;

  return instance;
}


#else // _WIN32


const int SOCKET_ERROR = -1;


#endif // _WIN32


/**
  Throws thread specific socket error.
*/
static void throw_socket_error()
{
#ifdef _WIN32
  throw_error(WSAGetLastError(), winsock_error_category());
#else
  throw_system_error();
#endif
}


/**
  Checks socket's state for errors. If an error is encountered, the appropriate
  exception is thrown.
*/
static void check_socket_error(Socket socket)
{
  int error = 0;
  socklen_t error_length = sizeof(error);

  if (::getsockopt(socket, SOL_SOCKET, SO_ERROR, (char *)&error, &error_length) != 0)
    throw_socket_error();

  if (error)
#ifdef _WIN32
    throw_error(error, winsock_error_category());
#else
    throw_error(error, system_error_category());
#endif
}


/**
  Puts a socket into non-blocking mode.
*/
static void set_nonblocking(Socket socket)
{
#ifdef _WIN32
  u_long set_nonblocking = 1;

  if (::ioctlsocket(socket, FIONBIO, &set_nonblocking) == SOCKET_ERROR)
    throw_socket_error();
#else
  int flags = ::fcntl(socket, F_GETFL, 0);

  if (flags >= 0)
  {
    if (::fcntl(socket, F_SETFL, flags | O_NONBLOCK) != 0)
      throw_socket_error();
  }
  else
  {
    throw_socket_error();
  }
#endif
}


void initialize_socket_system()
{
#ifdef _WIN32
  WSADATA wsa_data;
  WORD version_requested = MAKEWORD(2, 2);

  if (::WSAStartup(version_requested, &wsa_data) != 0)
    throw_error("Winsock initialization failed.");
#endif
}


void uninitialize_socket_system()
{
#ifdef _WIN32
  if (::WSACleanup() != 0)
    throw_socket_error();
#endif
}


Socket socket(bool nonblocking)
{
  Socket socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (socket != NULL_SOCKET)
  {
    int reuse_addr = 1;
    if (::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse_addr, sizeof(reuse_addr)) != 0)
      throw_socket_error();

    if (nonblocking)
    {
      try
      {
        set_nonblocking(socket);
      }
      catch (...)
      {
        close(socket);
        throw;
      }
    }
  }
  else
  {
    throw_socket_error();
  }

  return socket;
}


void close(Socket socket)
{
#ifdef _WIN32
  if (::closesocket(socket) != 0)
#else
  if (::close(socket) != 0)
#endif
  {
    throw_socket_error();
  }
}


void shutdown(Socket socket, Shutdown_mode mode)
{
#ifdef _WIN32
  const int SHUT_RD = SD_RECEIVE;
  const int SHUT_WR = SD_SEND;
  const int SHUT_RDWR = SD_BOTH;
#endif

  int sys_mode;

  switch(mode)
  {
    case SHUTDOWN_MODE_READ:
      sys_mode = SHUT_RD;
      break;
    case SHUTDOWN_MODE_WRITE:
      sys_mode = SHUT_WR;
      break;
    case SHUTDOWN_MODE_BOTH:
      sys_mode = SHUT_RDWR;
      break;
    default:
      THROW("Invalid socket shutdown mode.");
  }

  if (::shutdown(socket, sys_mode) != 0)
    throw_socket_error();
}


void connect(Socket socket, const char *host_name, unsigned short port)
{
  hostent *host = ::gethostbyname(host_name);

  // TODO: Configurable number of attempts

  for (short attempt = 0; !host && h_errno == TRY_AGAIN && attempt < 1; ++attempt)
    host = ::gethostbyname(host_name);

  if (!host)
  {
    // TODO: Better error messages which include host name

    switch (h_errno)
    {
    case HOST_NOT_FOUND: THROW("Host name not found in DNS");
    case NO_DATA: THROW("DNS returned no data when resolving host name");
    case NO_RECOVERY: THROW("DNS failure while resolving host name");
    case TRY_AGAIN:
    default: THROW("Could not resolve host name");
    }
  }

  sockaddr_in sock_addr = {};
  sock_addr.sin_family = AF_INET;
  sock_addr.sin_port = htons(port);
  memcpy(&sock_addr.sin_addr.s_addr, host->h_addr,
         static_cast<size_t>(host->h_length));

  int connect_result = ::connect(socket, (struct sockaddr *)&sock_addr, sizeof(sock_addr));

  if (connect_result == 0)
  {
    return;
  }
#ifdef _WIN32
  else if (connect_result == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
#else
  else if (connect_result == SOCKET_ERROR && errno == EINPROGRESS)
#endif
  {
    int select_result = select_one(socket, SELECT_MODE_WRITE, true);

    if (select_result < 0)
      throw_socket_error();
    else
      check_socket_error(socket);
  }
  else
  {
    throw_socket_error();
  }
}


Socket listen_and_accept(unsigned short port)
{
  Socket client = NULL_SOCKET;
  Socket acceptor = detail::socket(true);

  try
  {
    sockaddr_in serv_addr = {};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (::bind(acceptor, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ||
        ::listen(acceptor, 1) == SOCKET_ERROR)
    {
      throw_socket_error();
    }

    int select_result = select_one(acceptor, SELECT_MODE_READ, true);

    if (select_result > 0)
    {
      sockaddr_in cli_addr = {};
      socklen_t cli_addr_length = sizeof(cli_addr);

      client = ::accept(acceptor, (sockaddr *)&cli_addr, &cli_addr_length);

      if (client == NULL_SOCKET)
        throw_socket_error();
    }
    else if (select_result == 0)
    {
      check_socket_error(acceptor);
    }
    else
    {
      throw_socket_error();
    }

    detail::close(acceptor);
  }
  catch (...)
  {
    detail::close(acceptor);
    throw;
  }

  return client;
}


int select_one(Socket socket, Select_mode mode, bool wait)
{
  timeval zero_timeout = {};

DIAGNOSTIC_PUSH

#ifdef _WIN32
  // 4548 = expression has no effect
  // This warning is generated by FD_SET
  DISABLE_WARNING(4548)
#endif

  fd_set socket_set;
  FD_ZERO(&socket_set);
  FD_SET(socket, &socket_set);

  fd_set except_set;
  FD_ZERO(&except_set);
  FD_SET(socket, &except_set);

DIAGNOSTIC_POP

  int result = ::select(FD_SETSIZE,
    mode == SELECT_MODE_READ ? &socket_set : NULL,
    mode == SELECT_MODE_WRITE ? &socket_set : NULL,
    &except_set, wait ? NULL : &zero_timeout);

  if (result > 0 && FD_ISSET(socket, &except_set))
    check_socket_error(socket);

  return result;
}


size_t bytes_available(Socket socket)
{
  unsigned long bytes_available;

#ifdef _WIN32
  if (::ioctlsocket(socket, FIONREAD, &bytes_available) != 0)
#else
  if (::ioctl(socket, FIONREAD, &bytes_available) == -1)
#endif
  {
    throw_socket_error();
  }

  return bytes_available;
}


void recv(Socket socket, byte *buffer, size_t buffer_size)
{
  // TODO: Investigate if more efficient implementation is possible with ::recv() and MSG_WAITALL flag.

  if (buffer_size == 0)
    return;

  size_t bytes_received = 0;

  while (bytes_received != buffer_size)
    bytes_received += recv_some(socket, buffer + bytes_received, buffer_size - bytes_received, true);
}


void send(Socket socket, const byte *buffer, size_t buffer_size)
{
  if (buffer_size == 0)
    return;

  size_t bytes_sent = 0;

  while (bytes_sent != buffer_size)
    bytes_sent += send_some(socket, buffer + bytes_sent, buffer_size - bytes_sent, true);
}

#include <limits>

size_t recv_some(Socket socket, byte *buffer, size_t buffer_size, bool wait)
{
  if (buffer_size == 0)
    return 0;

  /*
    TODO: buffer size checks - throw error if passed buffer is bigger than
    some reasonable limit.
  */
  assert(buffer_size > 0);
  assert(buffer_size < (size_t)std::numeric_limits<int>::max());

  size_t bytes_received = 0;

  int select_result = select_one(socket, SELECT_MODE_READ, wait);

  if (select_result > 0)
  {
    int recv_result = ::recv(socket, reinterpret_cast<char *>(buffer),
                             static_cast<int>(buffer_size), 0);

    if (recv_result == 0)
    {
      throw connection::Error_eos();
    }
    else if (recv_result == SOCKET_ERROR)
    {
#ifdef _WIN32
      if (WSAGetLastError() == WSAEWOULDBLOCK)
#else
      if (errno == EAGAIN || errno == EWOULDBLOCK)
#endif
      {
        bytes_received = 0;
      }
      else
      {
        throw_socket_error();
      }
    }
    else
    {
      assert(recv_result > 0);
      bytes_received = static_cast<size_t>(recv_result);
    }
  }
  else if (select_result == 0)
  {
    return 0;
  }
  else
  {
    throw_socket_error();
  }

  return bytes_received;
}


size_t send_some(Socket socket, const byte *buffer, size_t buffer_size, bool wait)
{
  if (buffer_size == 0)
    return 0;

  /*
    TODO: buffer size checks - throw error if passed buffer is bigger than
    some reasonable limit.
  */
  assert(buffer_size > 0);
  assert(buffer_size < (size_t)std::numeric_limits<int>::max());

  size_t bytes_sent = 0;

  int select_result = select_one(socket, SELECT_MODE_WRITE, wait);

  if (select_result > 0)
  {
    int send_result = ::send(socket, reinterpret_cast<const char *>(buffer),
                             static_cast<int>(buffer_size), 0);

    if (send_result == SOCKET_ERROR)
    {
#ifdef _WIN32
      if (WSAGetLastError() == WSAEWOULDBLOCK)
#else
      if (errno == EAGAIN || errno == EWOULDBLOCK)
#endif
      {
        bytes_sent = 0;
      }
      else
      {
        throw_socket_error();
      }
    }
    else
    {
      assert(send_result >= 0);
      bytes_sent = static_cast<size_t>(send_result);
    }
  }
  else if (select_result == 0)
  {
    return 0;
  }
  else
  {
    throw_socket_error();
  }

  return bytes_sent;
}


}}}} // cdk::foundation::connection::detail
