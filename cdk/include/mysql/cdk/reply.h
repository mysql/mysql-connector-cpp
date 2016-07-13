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

#ifndef CDK_REPLY_H
#define CDK_REPLY_H

#include "common.h"
#include "api/reply.h"
#include "mysqlx.h"


namespace cdk {

class Session;
class Cursor;

class Reply
  : public api::Reply<Traits>
{
protected:

  mysqlx::Reply m_impl;
  typedef mysqlx::Reply_init& Initializer;

public:

  Reply()
  {}

  Reply(Initializer _init)
  {
    m_impl= _init;
  }

  Reply& operator=(Initializer _init)
  {
    m_impl= _init;
    return *this;
  }

  // Reply interface

  bool has_results() { return m_impl.has_results(); }
  void skip_result() { m_impl.skip_result(); }
  row_count_t affected_rows() { return m_impl.affected_rows(); }
  row_count_t last_insert_id() { return m_impl.last_insert_id(); }
  void discard() { m_impl.discard(); }

  // Diagnostics interface

  unsigned int entry_count(Severity::value level=Severity::ERROR)
  { return m_impl.entry_count(level); }

  Diagnostic_iterator& get_entries(Severity::value level=Severity::ERROR)
  { return m_impl.get_entries(level); }

  const Error& get_error()
  { return m_impl.get_error(); }

  // Async_op interface

  bool is_completed() const { return m_impl.is_completed(); }

private:

  // Async_op

  bool do_cont() { return m_impl.cont(); }
  void do_wait() { return m_impl.wait(); }
  void do_cancel() { return m_impl.cancel(); }
  const cdk::api::Event_info* get_event_info() const
  { return m_impl.get_event_info(); }


  friend class Session;
  friend class Cursor;
};

}

#endif
