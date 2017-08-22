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

#include <mysql/cdk/foundation/common.h>
PUSH_SYS_WARNINGS
#include "../extra/yassl/include/openssl/ssl.h"
POP_SYS_WARNINGS
#include <mysql/cdk/foundation/error.h>
#include <mysql/cdk/foundation/connection_yassl.h>
#include <mysql/cdk/foundation/opaque_impl.i>
#include "socket_detail.h"
#include "connection_tcpip_base.h"


static const char* tls_ciphers_list="DHE-RSA-AES256-SHA:DHE-RSA-AES128-SHA:"
                                    "AES128-RMD:DES-CBC3-RMD:DHE-RSA-AES256-RMD:"
                                    "DHE-RSA-AES128-RMD:DHE-RSA-DES-CBC3-RMD:"
                                    "AES256-SHA:RC4-SHA:RC4-MD5:DES-CBC3-SHA:"
                                    "DES-CBC-SHA:EDH-RSA-DES-CBC3-SHA:"
                                    "EDH-RSA-DES-CBC-SHA:AES128-SHA:AES256-RMD:";
static const char* tls_cipher_blocked= "!aNULL:!eNULL:!EXPORT:!LOW:!MD5:!DES:!RC2:!RC4:!PSK:!SSLv3:";


static void throw_yassl_error_msg(const char* msg)
{
  throw cdk::foundation::Error(cdk::foundation::cdkerrc::tls_error,
                               std::string("yaSSL: ") + msg);
}

static void throw_yassl_error()
{
  char buffer[512];

  yaSSL::ERR_error_string_n(yaSSL::ERR_get_error(), buffer, sizeof(buffer));

  throw_yassl_error_msg(buffer);
}


/*
  Implementation of TLS connection class.
*/


class connection_TLS_impl
  : public ::cdk::foundation::connection::Socket_base::Impl
{
public:
  connection_TLS_impl(cdk::foundation::connection::Socket_base* tcpip,
                      cdk::foundation::connection::TLS::Options options)
    : m_tcpip(tcpip)
    , m_tls(NULL)
    , m_tls_ctx(NULL)
    , m_options(options)
  {}

  ~connection_TLS_impl()
  {
    if (m_tls)
    {
      yaSSL::SSL_shutdown(m_tls);
      yaSSL::SSL_free(m_tls);
    }

    if (m_tls_ctx)
      yaSSL::SSL_CTX_free(m_tls_ctx);

    delete m_tcpip;
  }

  void do_connect();

  void verify_server_cert();

  cdk::foundation::connection::Socket_base* m_tcpip;
  yaSSL::SSL* m_tls;
  yaSSL::SSL_CTX* m_tls_ctx;
  cdk::foundation::connection::TLS::Options m_options;
};


void connection_TLS_impl::do_connect()
{
  if (m_tcpip->is_closed())
    m_tcpip->connect();

  if (m_tls || m_tls_ctx)
  {
    // TLS handshake already established, exit.
    return;
  }

  try
  {
    yaSSL::SSL_METHOD* method = yaSSL::TLSv1_1_client_method();

    if (!method)
      throw_yassl_error();

    m_tls_ctx = SSL_CTX_new(method);
    if (!m_tls_ctx)
      throw_yassl_error();


    std::string cipher_list;
    cipher_list.append(tls_cipher_blocked);
    cipher_list.append(tls_ciphers_list);

    SSL_CTX_set_cipher_list(m_tls_ctx, cipher_list.c_str());

    if (m_options.ssl_mode()
        >=
        cdk::foundation::connection::TLS::Options::SSL_MODE::VERIFY_CA
        )
    {
      SSL_CTX_set_verify(m_tls_ctx, yaSSL::SSL_VERIFY_PEER , NULL);

      int errNr = SSL_CTX_load_verify_locations(
                    m_tls_ctx,
                    m_options.get_ca().c_str(),
                    m_options.get_ca_path().empty()
                    ? NULL : m_options.get_ca_path().c_str());

      switch(errNr)
      {
        case yaSSL::SSL_BAD_FILE:
          throw_yassl_error_msg("error opening ca file");
        case yaSSL::SSL_BAD_PATH:
          throw_yassl_error_msg("bad ca_path");
        case yaSSL::SSL_BAD_STAT:
          throw_yassl_error_msg("bad file permissions inside ca_path");
        default:
          break;
      }

    }
    else
    {
      SSL_CTX_set_verify(m_tls_ctx, yaSSL::SSL_VERIFY_NONE, 0);
    }

    m_tls = yaSSL::SSL_new(m_tls_ctx);
    if (!m_tls)
      throw_yassl_error();

    unsigned int fd = m_tcpip->get_fd();

    cdk::foundation::connection::detail::set_nonblocking(fd, false);

    yaSSL::SSL_set_fd(m_tls, static_cast<yaSSL::YASSL_SOCKET_T>(fd));

    if(yaSSL::SSL_connect(m_tls) != yaSSL::SSL_SUCCESS)
      throw_yassl_error();

    if (m_options.ssl_mode()
        ==
        cdk::foundation::connection::TLS::Options::SSL_MODE::VERIFY_IDENTITY
        )
      verify_server_cert();

  }
  catch (...)
  {
    if (m_tls)
    {
      yaSSL::SSL_shutdown(m_tls);
      yaSSL::SSL_free(m_tls);
      m_tls = NULL;
    }

    if (m_tls_ctx)
    {
      yaSSL::SSL_CTX_free(m_tls_ctx);
      m_tls_ctx = NULL;
    }

    throw;
  }
}


/*
  Class used to safely delete allocated X509 cert.
  This way, no need to test cert on each possible return/throw.
*/
class safe_cert
{
  yaSSL::X509* m_cert;

public:
  safe_cert(yaSSL::X509 *cert = NULL)
    : m_cert(cert)
  {}

  ~safe_cert()
  {
    if (m_cert)
      yaSSL::X509_free(m_cert);
  }

  operator bool()
  {
    return m_cert != NULL;
  }

  safe_cert& operator = (yaSSL::X509 *cert)
  {
    m_cert = cert;
    return *this;
  }

  safe_cert& operator = (safe_cert& cert)
  {
    m_cert = cert.m_cert;
    cert.m_cert = NULL;
    return *this;
  }

  operator yaSSL::X509 *() const
  {
    return m_cert;
  }
};


void connection_TLS_impl::verify_server_cert()
{
  safe_cert server_cert;
  char *cn= NULL;
  int cn_loc= -1;
  yaSSL::ASN1_STRING *cn_asn1= NULL;
  yaSSL::X509_NAME_ENTRY *cn_entry= NULL;
  yaSSL::X509_NAME *subject= NULL;


  server_cert = SSL_get_peer_certificate(m_tls);

  if (!server_cert)
  {
    throw_yassl_error_msg("Could not get server certificate");
  }

  if (yaSSL::X509_V_OK != SSL_get_verify_result(m_tls))
  {
    throw_yassl_error_msg("Failed to verify the server certificate");
  }
  /*
    We already know that the certificate exchanged was valid; the SSL library
    handled that. Now we need to verify that the contents of the certificate
    are what we expect.
  */

  /*
   Some notes for future development
   We should check host name in alternative name first and then if needed check in common name.
   Currently yssl doesn't support alternative name.
   openssl 1.0.2 support X509_check_host method for host name validation, we may need to start using
   X509_check_host in the future.
  */

  subject= X509_get_subject_name((yaSSL::X509 *) server_cert);
  // Find the CN location in the subject
  cn_loc= X509_NAME_get_index_by_NID(subject, NID_commonName, -1);
  if (cn_loc < 0)
  {
    throw_yassl_error_msg("Failed to get CN location in the certificate subject");
  }

  // Get the CN entry for given location
  cn_entry= X509_NAME_get_entry(subject, cn_loc);
  if (cn_entry == NULL)
  {
    throw_yassl_error_msg("Failed to get CN entry using CN location");
  }

  // Get CN from common name entry
  cn_asn1 = X509_NAME_ENTRY_get_data(cn_entry);
  if (cn_asn1 == NULL)
  {
    throw_yassl_error_msg("Failed to get CN from CN entry");
  }

  cn= (char *) ASN1_STRING_data(cn_asn1);

  // There should not be any NULL embedded in the CN
  if ((size_t)ASN1_STRING_length(cn_asn1) != strlen(cn))
  {
    throw_yassl_error_msg("NULL embedded in the certificate CN");
  }


  if (!m_options.verify_cn(cn))
  {
    throw_yassl_error_msg("SSL certificate validation failure");
  }

}


IMPL_TYPE(cdk::foundation::connection::TLS, connection_TLS_impl);
IMPL_PLAIN(cdk::foundation::connection::TLS);


namespace cdk {
namespace foundation {
namespace connection {


TLS::TLS(Socket_base* tcpip,
         const TLS::Options &options)
  : opaque_impl<TLS>(NULL, tcpip, options)
{}


Socket_base::Impl& TLS::get_base_impl()
{
  return get_impl();
}


TLS::Read_op::Read_op(TLS &conn, const buffers &bufs, time_t deadline)
  : IO_op(conn, bufs, deadline)
  , m_tls(conn)
  , m_currentBufferIdx(0)
  , m_currentBufferOffset(0)
{
  connection_TLS_impl& impl = m_tls.get_impl();

  if (!impl.m_tcpip->get_base_impl().is_open())
    throw Error_eos();
}


bool TLS::Read_op::do_cont()
{
  return common_read();
}


void TLS::Read_op::do_wait()
{
  while (!is_completed())
    common_read();
}


bool TLS::Read_op::common_read()
{
  if (is_completed())
    return true;

  connection_TLS_impl& impl = m_tls.get_impl();

  const bytes& buffer = m_bufs.get_buffer(m_currentBufferIdx);
  byte* data =buffer.begin() + m_currentBufferOffset;
  int buffer_size = static_cast<int>(buffer.size() - m_currentBufferOffset);

  int result = yaSSL::SSL_read(impl.m_tls, data, buffer_size);

  if (result > 0)
  {
    m_currentBufferOffset += result;

    if (m_currentBufferOffset == buffer.size())
    {
      ++m_currentBufferIdx;

      if (m_currentBufferIdx == m_bufs.buf_count())
      {
        set_completed(m_bufs.length());
        return true;
      }
    }
  }

  return false;
}


TLS::Read_some_op::Read_some_op(TLS &conn, const buffers &bufs, time_t deadline)
  : IO_op(conn, bufs, deadline)
  , m_tls(conn)
{
  connection_TLS_impl& impl = m_tls.get_impl();

  if (!impl.m_tcpip->get_base_impl().is_open())
    throw Error_eos();
}


bool TLS::Read_some_op::do_cont()
{
  return common_read();
}


void TLS::Read_some_op::do_wait()
{
  while (!is_completed())
    common_read();
}


bool TLS::Read_some_op::common_read()
{
  if (is_completed())
    return true;

  connection_TLS_impl& impl = m_tls.get_impl();

  const bytes& buffer = m_bufs.get_buffer(0);

  int result = yaSSL::SSL_read(impl.m_tls, buffer.begin(), (int)buffer.size());

  if (result > 0)
  {
    set_completed(static_cast<size_t>(result));
    return true;
  }

  return false;
}


TLS::Write_op::Write_op(TLS &conn, const buffers &bufs, time_t deadline)
  : IO_op(conn, bufs, deadline)
  , m_tls(conn)
  , m_currentBufferIdx(0)
  , m_currentBufferOffset(0)
{
  connection_TLS_impl& impl = m_tls.get_impl();

  if (!impl.m_tcpip->get_base_impl().is_open())
    throw Error_no_connection();
}


bool TLS::Write_op::do_cont()
{
  return common_write();
}


void TLS::Write_op::do_wait()
{
  while (!is_completed())
    common_write();
}


bool TLS::Write_op::common_write()
{
  if (is_completed())
    return true;

  connection_TLS_impl& impl = m_tls.get_impl();

  const bytes& buffer = m_bufs.get_buffer(m_currentBufferIdx);
  byte* data = buffer.begin() + m_currentBufferOffset;
  int buffer_size = static_cast<int>(buffer.size() - m_currentBufferOffset);

  int result = yaSSL::SSL_write(impl.m_tls, data, buffer_size);

  if (result > 0)
  {
    m_currentBufferOffset += result;

    if (m_currentBufferOffset == buffer.size())
    {
      ++m_currentBufferIdx;

      if (m_currentBufferIdx == m_bufs.buf_count())
      {
        set_completed(m_bufs.length());
        return true;
      }
    }
  }

  return false;
}


TLS::Write_some_op::Write_some_op(TLS &conn, const buffers &bufs, time_t deadline)
  : IO_op(conn, bufs, deadline)
  , m_tls(conn)
{
  connection_TLS_impl& impl = m_tls.get_impl();

  if (!impl.m_tcpip->get_base_impl().is_open())
    throw Error_no_connection();
}


bool TLS::Write_some_op::do_cont()
{
  return common_write();
}


void TLS::Write_some_op::do_wait()
{
  while (!is_completed())
    common_write();
}


bool TLS::Write_some_op::common_write()
{
  if (is_completed())
    return true;

  connection_TLS_impl& impl = m_tls.get_impl();

  const bytes& buffer = m_bufs.get_buffer(0);

  int result = yaSSL::SSL_write(impl.m_tls, buffer.begin(), (int)buffer.size());

  if (result > 0)
  {
    set_completed(static_cast<size_t>(result));
    return true;
  }

  return false;
}


}  // namespace connection
}  // namespace foundation
}  // namespace cdk
