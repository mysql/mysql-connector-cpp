/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C is licensed under the terms of the GPLv2
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

#include <mysqlx/common.h>
#include <mysqlx/xapi.h>
#include "mysqlx_cc_internal.h"
#include <algorithm>
#include <string>

using std::string;


mysqlx_session_struct::mysqlx_session_struct(
  mysqlx_session_options_struct *opt
)
{
  cdk::ds::Multi_source ds;
  opt->get_data_source(ds);
  m_impl = std::make_shared<common::Session_impl>(ds);
}


mysqlx_session_struct::mysqlx_session_struct(
  const std::string &host, unsigned short port,
  const string &usr, const std::string *pwd,
  const std::string *db
)
  : mysqlx_session_struct(mysqlx_session_options_struct(host, port, usr, pwd, db))
{}

mysqlx_session_struct::mysqlx_session_struct(
  const std::string &conn_str
)
  : mysqlx_session_struct(mysqlx_session_options_struct(conn_str))
{}


mysqlx_stmt_struct*
mysqlx_session_struct::sql_query(const char *query_utf8, uint32_t length)
{
  if (!query_utf8 || !(*query_utf8))
    throw Mysqlx_exception("Query is empty");

  if (length == MYSQLX_NULL_TERMINATED)
    length = (uint32_t)strlen(query_utf8);

  std::string query(query_utf8, length);
  return new_stmt<OP_SQL>(cdk::string(query));  // note: UTF8 conversion
}


mysqlx_error_struct * mysqlx_session_struct::get_last_error()
{
  cdk::Session &sess = get_session();

  // Return session errors from CDK first
  if (sess.entry_count())
  {
      m_error.set(&sess.get_error());
  }
  else if (!m_error.message() && !m_error.error_num())
    return NULL;

  return &m_error;
}


const cdk::Error * mysqlx_session_struct::get_cdk_error()
{
  if (get_session().entry_count())
    return &get_session().get_error();

  return NULL;
}


void mysqlx_session_struct::reset_diagnostic()
{
  m_error.reset();
}


void mysqlx_session_struct::transaction_begin()
{
  // Note: the internal implementation object handles registered results etc.
  stmt_traits<OP_TRX_BEGIN>::Impl stmt(m_impl);
  stmt.execute();
}

void mysqlx_session_struct::transaction_commit()
{
  stmt_traits<OP_TRX_COMMIT>::Impl stmt(m_impl);
  stmt.execute();
}

void mysqlx_session_struct::transaction_rollback()
{
  stmt_traits<OP_TRX_ROLLBACK>::Impl stmt(m_impl);
  stmt.execute();
}


using cdk::foundation::connection::TLS;


TLS::Options::SSL_MODE uint_to_ssl_mode(unsigned int mode)
{
  switch (mode)
  {
    case SSL_MODE_DISABLED:
      return TLS::Options::SSL_MODE::DISABLED;
    case SSL_MODE_REQUIRED:
      return TLS::Options::SSL_MODE::REQUIRED;
    case SSL_MODE_VERIFY_CA:
      return TLS::Options::SSL_MODE::VERIFY_CA;
    case SSL_MODE_VERIFY_IDENTITY:
      return TLS::Options::SSL_MODE::VERIFY_IDENTITY;
    default:
      assert(false);
      // Quiet compile warnings
      return TLS::Options::SSL_MODE::DISABLED;
  }
}

unsigned int ssl_mode_to_uint(TLS::Options::SSL_MODE mode)
{
  switch (mode)
  {
    case TLS::Options::SSL_MODE::DISABLED:
      return SSL_MODE_DISABLED;
    case TLS::Options::SSL_MODE::REQUIRED:
      return SSL_MODE_REQUIRED;
    case TLS::Options::SSL_MODE::VERIFY_CA:
      return SSL_MODE_VERIFY_CA;
    case TLS::Options::SSL_MODE::VERIFY_IDENTITY:
      return SSL_MODE_VERIFY_IDENTITY;
    default:
      assert(false);
      // Quiet compile warnings
      return 0;
  }
}


const char* opt_name(mysqlx_opt_type_t opt)
{
  using mysqlx::common::Settings_impl;
  using Option = Settings_impl::Option;
  return Settings_impl::option_name(Option(opt));
}

const char* ssl_mode_name(mysqlx_ssl_mode_t m)
{
  using mysqlx::common::Settings_impl;
  using SSL_mode = Settings_impl::SSL_mode;
  return Settings_impl::ssl_mode_name(SSL_mode(m));
}


struct Error_bad_option : public Mysqlx_exception
{
  Error_bad_option()
    : Mysqlx_exception("Unrecognized connection option")
  {}

  Error_bad_option(const std::string &opt) : Error_bad_option()
  {
    m_message += ": " + opt;
  }

  Error_bad_option(unsigned int opt) : Error_bad_option()
  {
    std::ostringstream buf;
    buf << opt;
    m_message += " (" + buf.str() + ")";
  }
};

struct Error_dup_option : public Mysqlx_exception
{
  Error_dup_option(mysqlx_opt_type_t opt)
  {
    m_message = "Option ";
    m_message += opt_name(opt);
    m_message += " defined twice";
  }
};

struct Error_bad_mode : public Mysqlx_exception
{
  Error_bad_mode(const std::string &m)
  {
    m_message = "Unrecognized ssl-mode: " + m;
  }
};

struct Error_ca_mode : public Mysqlx_exception
{
  Error_ca_mode()
    : Mysqlx_exception("The ssl-ca option is not compatible with ssl-mode")
  {}

  Error_ca_mode(mysqlx_ssl_mode_t m) : Error_ca_mode()
  {
    m_message += " ";
    m_message += ssl_mode_name(m);
  }

  Error_ca_mode(TLS::Options::SSL_MODE m)
    : Error_ca_mode(mysqlx_ssl_mode_t(ssl_mode_to_uint(m)))
  {}
};
