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

#ifndef CDK_FOUNDATION_CONNECTION_YASSL_H
#define CDK_FOUNDATION_CONNECTION_YASSL_H

#include "connection_tcpip.h"
#include "stream.h"
#include "error.h"


namespace cdk {
namespace foundation {
namespace connection {


class TLS
  : public TCPIP_base
  , opaque_impl<TLS>
{
public:

  class Options;

  TLS(TCPIP_base* tcpip,
      const Options& Opts);


  class Read_op;
  class Read_some_op;
  class Write_op;
  class Write_some_op;

private:
  TCPIP_base::Impl& get_base_impl();
};


class TLS::Options
{
public:

  /*
    Note: Normally m_use_tls should be always true: using TLS options object
    implies an intent to have TLS connection. A TLS::Options object with
    m_use_tls set to false is only used to disable TLS connection inside
    TCPIP::Options object. The TCPIP::Options object holds an instance
    of TLS::Options. Calling TCPIP::Options::set_tls(false) will alter this
    internal TLS::Options instance so that m_use_tls is false and then the
    TCPIP::Options object knows that TLS should not be used for the connection.
  */

  Options(bool use_tls = true)
    : m_use_tls(use_tls)
  {}

  void set_use_tls(bool use_tls) { m_use_tls = use_tls; }
  bool use_tls() const { return m_use_tls; }

  void set_key(const string &key) { m_key = key; }
  const std::string &get_key() const { return m_key; }

  void set_ca(const string &ca) { m_ca = ca; }
  void set_ca_path(const string &ca_path) { m_ca_path = ca_path; }

  const std::string &get_ca() const { return m_ca; }
  const std::string &get_ca_path() const { return m_ca_path; }

protected:

  bool m_use_tls;
  std::string m_key;
  std::string m_ca;
  std::string m_ca_path;
};


class TLS::Read_op : public TCPIP_base::IO_op
{
public:
  Read_op(TLS &conn, const buffers &bufs, time_t deadline = 0);

  virtual bool do_cont();
  virtual void do_wait();

private:
  TLS& m_tls;
  unsigned int m_currentBufferIdx;
  size_t m_currentBufferOffset;

  bool common_read();
};


class TLS::Read_some_op : public TCPIP_base::IO_op
{
public:
  Read_some_op(TLS &conn, const buffers &bufs, time_t deadline = 0);

  virtual bool do_cont();
  virtual void do_wait();

private:
  TLS& m_tls;

  bool common_read();
};


class TLS::Write_op : public TCPIP_base::IO_op
{
public:
  Write_op(TLS &conn, const buffers &bufs, time_t deadline = 0);

  virtual bool do_cont();
  virtual void do_wait();

private:
  TLS& m_tls;
  unsigned int m_currentBufferIdx;
  size_t m_currentBufferOffset;

  bool common_write();
};


class TLS::Write_some_op : public TCPIP_base::IO_op
{
public:
  Write_some_op(TLS &conn, const buffers &bufs, time_t deadline = 0);

  virtual bool do_cont();
  virtual void do_wait();

private:
  TLS& m_tls;

  bool common_write();
};


} // namespace connection
} // namespace foundation
} // namespace cdk

#endif // CDK_FOUNDATION_CONNECTION_YASSL_H
