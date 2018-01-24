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

#include <mysqlx/common.h>
#include <mysql/cdk.h>
#include <uuid_gen.h>
#include <uri_parser.h>

#include "settings.h"
#include "session.h"
#include "result.h"


using namespace ::mysqlx::common;
using TCPIP_options = cdk::ds::TCPIP::Options;
using TLS_options = TCPIP_options::TLS_options;


void Settings_impl::clear()
{
  m_data = Data();
}


void Settings_impl::set_from_uri(const std::string &uri)
{
  parser::URI_parser parser(uri);
  Setter set(*this);

  parser.process(set);
  set.commit();
}


TCPIP_options::auth_method_t get_auth(unsigned m)
{
  using DevAPI_type = Settings_impl::Auth_method;
  using CDK_type = TCPIP_options::auth_method_t;

  switch (DevAPI_type(m))
  {
#define AUTH_TO_CDK(X,N) \
  case DevAPI_type::X: return CDK_type::X;

    AUTH_METHOD_LIST(AUTH_TO_CDK)

  default:
    // Note: caller should ensure that argument has correct value
    assert(false);
  }

  return CDK_type(0); // quiet compiler warnings
}

TLS_options::SSL_MODE get_ssl_mode(unsigned m)
{
  using DevAPI_type = Settings_impl::SSL_mode;
  using CDK_type = TLS_options::SSL_MODE;

  switch (DevAPI_type(m))
  {
#define AUTH_TO_CDK(X,N) \
  case DevAPI_type::X: return CDK_type::X;

    SSL_MODE_LIST(AUTH_TO_CDK)

  default:
    // Note: caller should ensure that argument has correct value
    assert(false);
  }

  return CDK_type(0); // quiet compiler warnings
}


/*
  Initialize CDK connection options based on session settings.
  If socket is true, we are preparing options for a connection
  over Unix domain socket (and then encryption is not required by default).
*/

void prepare_options(
  Settings_impl &settings, bool socket, TCPIP_options &opts
)
{
  using Option = Settings_impl::Option;
  using SSL_mode = Settings_impl::SSL_mode;

  if (!settings.has_option(Option::USER))
    throw_error("USER option not defined");

  opts = TCPIP_options(
    string(settings.get(Option::USER).get_string()),
    settings.has_option(Option::PWD)
      ? &settings.get(Option::PWD).get_string() : nullptr
  );

  // Set basic options

  if (settings.has_option(Option::DB))
    opts.set_database(settings.get(Option::DB).get_string());

  // Set TLS options

  /*
    By default ssl-mode is REQUIRED. If ssl-mode was not explicitly set but
    ssl-ca was, then mode defaults to VERIFY_CA.
  */

  unsigned mode = unsigned(SSL_mode::REQUIRED);
  bool mode_set = false;

  if (settings.has_option(Option::SSL_MODE))
  {
    mode_set = true;
    mode = (unsigned)settings.get(Option::SSL_MODE).get_uint();
  }
  else if (settings.has_option(Option::SSL_CA))
  {
    mode_set = true;
    mode = unsigned(SSL_mode::VERIFY_CA);
  }

  if (socket && mode_set && mode >= unsigned(SSL_mode::REQUIRED))
  {
    throw_error("SSL connection over Unix domain socket requested.");
  }

  if (unsigned(SSL_mode::DISABLED) == mode)
  {
#ifdef WITH_SSL
    opts.set_tls(TLS_options::SSL_MODE::DISABLED);
#endif
  }
  else
#ifdef WITH_SSL
  {
    socket = true;  // so that PLAIN auth method is used below

    TLS_options tls_opt(get_ssl_mode(mode));
    if (settings.has_option(Option::SSL_CA))
      tls_opt.set_ca(settings.get(Option::SSL_CA).get_string());
    opts.set_tls(tls_opt);
  }
#endif

  // Set authentication options

  if (settings.has_option(Option::AUTH))
    opts.set_auth_method(get_auth(
      (unsigned)settings.get(Option::AUTH).get_uint()
    ));
  else
  {
    opts.set_auth_method(
      socket ? TCPIP_options::PLAIN : TCPIP_options::MYSQL41
    );
  }

}


/*
  Initialize CDK data source based on collected settings.
*/

void Settings_impl::get_data_source(cdk::ds::Multi_source &src)
{
  cdk::ds::TCPIP::Options opts;

  /*
    A single-host connection over Unix domain socket is considered secure.
    Otherwise SSL connection will be configured by default.
  */
  bool socket = m_data.m_sock && (1 == m_data.m_host_cnt);

  prepare_options(*this, socket, opts);

  // Build the list of hosts based on current settings.

  src.clear();

  // if priorities were not set explicitly, assign decreasing starting from 100
  int prio = m_data.m_user_priorities ? -1 : 100;

  /*
    Look for a priority after host/socket setting. If prio >= 0 then implicit
    priorities are used and in that case only sanity checks are done.
    Otherwise we expect that priority is explicitly given in the settings and
    throw error if this is not the case.
  */

  auto check_prio = [this](iterator &it, int &prio) {

    if (0 > prio)
    {
      if (it == end() || Option::PRIORITY != it->first)
        throw_error("No priority specified for host ...");
      // note: value of PRIORITY option is checked for validity
      prio = (int)it->second.get_uint();
      ++it;
    }

    assert(0 <= prio && prio <= 100);

    /*
      If there are more options, there should be no PRIORITY option
      at this point.
    */
    assert(it == end() || Option::PRIORITY != it->first);
  };

  /*
    This lambda is called when current option is HOST or PORT, to add (next)
    TCPIP host with optional priority to the data source.
  */

  auto add_host = [this, &src, &opts, check_prio](iterator &it, int prio) {

    string host("localhost");
    unsigned short  port = DEFAULT_MYSQLX_PORT;

    if (Option::PORT == it->first)
    {
      assert(0 == m_data.m_host_cnt);
    }
    else
    {
      assert(Option::HOST == it->first);
      host = it->second.get_string();
      ++it;
    }

    // Look for PORT

    if (it != end() && Option::PORT == it->first)
    {
      port = (unsigned short)it->second.get_uint();
      ++it;
    }

    check_prio(it, prio);

#ifdef WITH_SSL

    /*
      Set expected CN if ssl mode is VERIFY_IDENTITY. We expect CN to be
      the host name given by user when creating the session.
    */

    if (TLS_options::SSL_MODE::VERIFY_IDENTITY == opts.get_tls().ssl_mode())
    {
      TLS_options tls = opts.get_tls();
      tls.set_cn(host);
      opts.set_tls(tls);
    }
#endif

    src.add(cdk::ds::TCPIP(host, port), opts, (unsigned short)prio);
  };

  /*
    This lambda is called when current option is SOCKET to add Unix socket
    source to the list.
  */

#ifdef _WIN32
  auto add_socket = [](iterator, int) {
    throw_error("Unix socket connections not supported on Windows platform.");
  };
#else
  auto add_socket = [this, &src, &opts, check_prio](iterator &it, int prio) {

    assert(Option::SOCKET == it->first);

    string socket = it->second.get_string();
    ++it;

    check_prio(it, prio);

    src.add(cdk::ds::Unix_socket(socket),
      (cdk::ds::Unix_socket::Options&)opts,
      (unsigned short)prio);

  };
#endif

  /*
    Go through options and look for ones which define connections.
  */

  for (auto it = begin(); it != end();)
  {
    switch (it->first)
    {
    case Option::HOST:
      add_host(it, prio--); break;

    case Option::SOCKET:
      add_socket(it, prio--); break;

    /*
      Note: if m_host_cnt > 0 then a HOST setting must be before PORT setting,
      so the case above should cover that HOST/PORT pair.
    */
    case Option::PORT:
      assert(0 == m_data.m_host_cnt);
      add_host(it, prio--);
      break;

    default:
      ++it;
    }
  }

  assert(0 < src.size());
}


// ---------------------------------------------------------------------------


void Session_impl::prepare_for_cmd()
{
  if (m_current_result)
    m_current_result->store();
  m_current_result = nullptr;
}


// ---------------------------------------------------------------------------

void mysqlx::common::GUID::generate()
{
  using namespace uuid;

  static const char *hex_digit = "0123456789ABCDEF";
  uuid_type uuid;

  generate_uuid(uuid);

  for (unsigned i = 0; i < sizeof(uuid) && 2*i < sizeof(m_data); ++i)
  {
    m_data[2*i] = hex_digit[uuid[i] >> 4];
    m_data[2 * i + 1] = hex_digit[uuid[i] % 16];
  }
}


