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


Session::Session(ds::TCPIP &ds, const ds::Options &options)
  : m_session(NULL)
  , m_connection(NULL)
  , m_trans(false)
{
  using foundation::connection::TCPIP;

  TCPIP *connection = new TCPIP(ds.host(), ds.port());
  try
  {
    connection->connect();
  }
  catch (...)
  {
    delete connection;
    rethrow_error();
  }

  m_connection= connection;
  m_session = new mysqlx::Session(*connection, options);
}


Session::~Session()
{
  if (m_trans)
    rollback();
  delete m_session;
  delete m_connection;
}


} //cdk
