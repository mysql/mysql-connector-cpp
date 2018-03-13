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
  const std::vector<std::string>& generated_ids() const
  { return m_impl.generated_ids(); }
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
