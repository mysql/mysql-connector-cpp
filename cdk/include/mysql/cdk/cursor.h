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

#ifndef CDK_CURSOR_H
#define CDK_CURSOR_H

#include "common.h"
#include "api/cursor.h"
#include "api/mdata.h"
#include "reply.h"
#include "mysqlx.h"
//#include "foundation/codec.h"


namespace cdk {


class Cursor
  : public api::Cursor<Traits>
  , public api::Meta_data<Traits>
{

  mysqlx::Cursor m_impl;

public:

  Cursor(Reply &r) : m_impl(r.m_impl)
  {}

  // Cursor interface

  void get_rows(Row_processor& rp)
  { m_impl.get_rows(rp); }
  void get_rows(Row_processor& rp, row_count_t limit)
  { m_impl.get_rows(rp, limit); }
  bool get_row(Row_processor& rp) { return m_impl.get_row(rp); }
  void close() { m_impl.close(); }

  // Meta_data interface

  col_count_t col_count() const
  { return m_impl.col_count(); }
  Type_info   type(col_count_t pos)     { return m_impl.type(pos); }
  Format_info format(col_count_t pos)   { return m_impl.format(pos); }
  Column_info col_info(col_count_t pos) { return m_impl.col_info(pos); }

  // Async_op interface

  bool is_completed() const { return m_impl.is_completed(); }
  const cdk::api::Event_info* get_event_info() const
  { return m_impl.get_event_info(); }

private:

  // Async_op

  bool do_cont() { return m_impl.cont(); }
  void do_wait() { return m_impl.wait(); }
  void do_cancel() { return m_impl.cancel(); }

};

}

#endif
