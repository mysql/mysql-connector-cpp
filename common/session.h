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

#ifndef MYSQLX_COMMON_SESSION_INT_H
#define MYSQLX_COMMON_SESSION_INT_H

/*
  Internal implementations for public DevAPI classes.
*/

#include <mysql_common.h>
#include <mysql/cdk.h>


namespace mysqlx {
namespace common {

class Result_impl_base;
class Result_init;

/*
  Internal implementation for Session objects.

  TODO: Add transaction methods here?
*/

class Session_impl
{
public:

  using string = cdk::string;

  cdk::Session  m_sess;
  string        m_default_db;

  Session_impl(cdk::ds::Multi_source &ms)
    : m_sess(ms)
  {
    if (m_sess.get_default_schema())
      m_default_db = *m_sess.get_default_schema();
    if (!m_sess.is_valid())
      m_sess.get_error().rethrow();
  }

  Result_impl_base *m_current_result = nullptr;

  virtual ~Session_impl()
  {
    /*
      There should be no registered results when session implementation is
      deleted because:
      - each result has a shared pointer to session implementation,
      - session implementation is deleted only when the last result referring
        to it is deleted
      - results de-register themselves before being destroyed.
    */
    assert(!m_current_result);

    // TODO: rollback an on-going transaction, if any?
  }


  /*
    Result objects should register itself with the session and de-register
    when all result data is consumed (this is also the case when result object
    is deleted).
  */

  void register_result(Result_impl_base *result)
  {
    assert(!m_current_result);
    m_current_result = result;
  }

  void deregister_result(Result_impl_base *result)
  {
    if (result == m_current_result)
      m_current_result = nullptr;
  }

  /*
    Prepare session for sending new command. This caches the current result,
    if one is registered with session.
  */

  void prepare_for_cmd();

  unsigned long m_savepoint = 0;

  unsigned long next_savepoint()
  {
    return ++m_savepoint;
  }
};


}  // internal namespace
}  // mysqlx namespace


#endif
