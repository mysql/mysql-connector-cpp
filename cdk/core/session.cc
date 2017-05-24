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


#include <mysql/cdk/session.h>
#include <mysql/cdk/mysqlx/session.h>


namespace cdk {


#ifdef WITH_SSL
struct TLS_processor : cdk::protocol::mysqlx::Reply_processor
{
  TLS_processor(cdk::connection::TLS::Options::SSL_MODE ssl_mode)
    : m_ssl_mode(ssl_mode)
  {}

  cdk::connection::TLS::Options::SSL_MODE m_ssl_mode;
  bool m_tls = true;

  void error(unsigned int code, short int severity,
    cdk::protocol::mysqlx::sql_state_t sql_state, const string &msg)
  {
    sql_state_t expected_state("HY000");

    if (code == 5001 &&
        severity == 2 &&
        expected_state == sql_state &&
        m_ssl_mode == cdk::connection::TLS::Options::SSL_MODE::PREFERRED)
    {
      m_tls = false;
    }
    else
    {
      throw Error(static_cast<int>(code), msg);
    }
  }
};
#endif // WITH_SSL


/*
  A calss that creates a session from given data source.

  Instances of this calss are callable objects which can be used as visitors
  for ds::MultiSource implementing in this case the failover logic.
*/
struct Session_builder
{
  cdk::api::Connection *m_conn = NULL;
  mysqlx::Session      *m_sess = NULL;
  const mysqlx::string *m_database = NULL;
  bool m_throw_errors = false;

  Session_builder(bool throw_errors = false)
    : m_throw_errors(throw_errors)
  {}

  /*
    Construct a session for a given data source, if possible.

    1. If session could be constructed returns true. In this case m_sess points
       at the newly created session and m_conn points at the connection object
       used for that session. Someone must take ownership of these objects.

    2. If a network error was detected while creating session, either throws
       error if m_throw_errors is true or returns false. In the latter case,
       if used as a visitor over list of data sources, it will be called again
       to try another data source.

    3. If a bail-out error was detected, throws that error.
  */

  bool operator() (const ds::TCPIP &ds, const ds::TCPIP::Options &options);
  bool operator() (const ds::TCPIP_old &ds, const ds::TCPIP_old::Options &options);
};


bool
Session_builder::operator() (
  const ds::TCPIP &ds,
  const ds::TCPIP::Options &options
)
{
  using foundation::connection::TCPIP;
  using foundation::connection::TCPIP_base;

  TCPIP* connection = new TCPIP(ds.host(), ds.port());
  try
  {
    connection->connect();
  }
  catch (...)
  {
    delete connection;
    if (m_throw_errors)
      rethrow_error();
    else
      return false;  // continue to next host if available
  }

  bool tls = false;

#ifdef WITH_SSL
  if (options.get_tls().ssl_mode() >
      cdk::connection::TLS::Options::SSL_MODE::DISABLED)
  {
    using foundation::connection::TLS;

    // Negotiate TLS capabilities.
    cdk::protocol::mysqlx::Protocol proto(*connection);

    struct : cdk::protocol::mysqlx::api::Any::Document
    {
      void process(Processor &prc) const
      {
        prc.doc_begin();
        cdk::safe_prc(prc)->key_val("tls")->scalar()->yesno(true);
        prc.doc_end();
      }
    } tls_caps;

    proto.snd_CapabilitiesSet(tls_caps).wait();

    struct : cdk::protocol::mysqlx::Reply_processor
    {
      void error(unsigned int code, short int /*severity*/,
        cdk::protocol::mysqlx::sql_state_t /*sql_state*/, const string &msg)
      {
        throw Error(static_cast<int>(code), msg);
      }
    } prc;


    tls = true;

    try {
      proto.rcv_Reply(prc).wait();
    }
    // Server doesn't allow TLS connection
    catch(const Error&)
    {
      if (options.get_tls().ssl_mode() !=
          cdk::connection::TLS::Options::SSL_MODE::PREFERRED)
        rethrow_error();

      tls = false;
    }

  }

  if (tls)
  {
    connection::TLS *tls_conn
      = new connection::TLS(connection, options.get_tls());

    // TODO: attempt failover if TLS-layer reports network error?
    tls_conn->connect();

    m_conn = tls_conn;
    m_sess = new mysqlx::Session(*tls_conn, options);
  }
  else
#endif
  {
    m_conn = connection;
    m_sess = new mysqlx::Session(*connection, options);
  }

  m_database = options.database();

  return true;
}

bool
Session_builder::operator() (
  const ds::TCPIP_old &ds,
  const ds::TCPIP_old::Options &options
)
{
  throw Error(cdkerrc::generic_error, "Not supported");
  return false;
}

Session::Session(ds::TCPIP &ds, const ds::TCPIP::Options &options)
  : m_session(NULL)
  , m_connection(NULL)
  , m_trans(false)
{
  Session_builder sb(true);  // throw errors if detected

  sb(ds, options);

  m_session = sb.m_sess;
  m_connection = sb.m_conn;
}


struct ds::Multi_source::Access
{
  template <class Visitor>
  static void visit(Multi_source &ds, Visitor &visitor)
  { ds.visit(visitor); }
};

Session::Session(ds::Multi_source &ds)
  : m_session(NULL)
  , m_connection(NULL)
  , m_trans(false)
{
  Session_builder sb;

  ds::Multi_source::Access::visit(ds, sb);

  m_session = sb.m_sess;
  m_database = sb.m_database;
  m_connection = sb.m_conn;
}


Session::~Session()
{
  if (m_trans)
    rollback();
  delete m_session;
  delete m_connection;
}


} //cdk
