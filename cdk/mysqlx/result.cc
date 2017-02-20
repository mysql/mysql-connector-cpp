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

#include <mysql/cdk/mysqlx.h>
#include <iostream>


namespace cdk {
namespace mysqlx {


/*
   Class Reply
*/


Reply& Reply::operator=(Reply_init& _init)
{
  //terminate last reply
  wait();

  discard();

  init(_init);

  return *this;
}


Reply::~Reply()
{
  discard();
}


void Reply::init(Reply_init &init)
{
  m_error = false;
  m_da.clear();
  m_session = &init;

  init.register_reply(this);

  m_session->send_cmd();
  m_session->start_reading_result();
}


void Reply::close_cursor()
{
  if (NULL == m_session)
    return;

  assert(this == m_session->m_current_reply);

  if (m_session->m_current_cursor)
    m_session->m_current_cursor->close();
}


void Reply::discard()
{
  // TODO: workaround whenever there is no way to cancel a protocol command

  if (NULL == m_session)
    return;

  assert(this == m_session->m_current_reply);

  if (m_session->m_current_cursor)
    throw_error("Cursor in usage!");

  m_session->m_discard = true;

  while(has_results())
  {
    skip_result();
  }

  m_session->m_discard = false;
  m_session->deregister_reply(this);
  m_session = NULL;
}


bool Reply::has_results()
{
  if (NULL == m_session)
    return false;

  assert(this == m_session->m_current_reply);

  // If we hit error, do not continue.

  if (entry_count() > 0)
    return false;

  wait();

  // If server reported error, we do not have any results.

  if (entry_count() > 0)
    return false;

  return m_session->m_has_results;
}


void Reply::skip_result()
{
  if (NULL == m_session)
    throw_error("Session not initialized");

  assert(this == m_session->m_current_reply);

  if (entry_count() > 0)
    return;

  if (m_session->m_current_cursor)
    throw_error("Cursor in usage!");

  /*
    If there is next row set, cursor instance
    will discard it upon destruction.
  */

  if (has_results())
  {
    Cursor c(*this);
  }
}


// Async_op


bool Reply::is_completed() const
{
  if (!m_session)
    return true;

  assert(this == m_session->m_current_reply);

  if (!m_session->m_reply_op_queue.empty())
    return false;

  return true;
}


bool Reply::do_cont()
{
  if (!m_session)
    return true;

  assert(this == m_session->m_current_reply);

  if (m_session->m_reply_op_queue.empty())
    return true;

  if (m_error)
  {
    m_session->m_reply_op_queue.clear();
    return true;
  }

  bool done;

  try
  {
    done = m_session->m_reply_op_queue.front()->cont();
  }
  catch (...)
  {
    m_session->m_reply_op_queue.clear();
    throw;
  }

  if (done)
    m_session->m_reply_op_queue.pop_front();

  return false;
};


void Reply::do_wait()
{
  while (m_session && !m_session->m_reply_op_queue.empty())
  {
    assert(this == m_session->m_current_reply);

    if (m_error)
    {
      m_session->m_reply_op_queue.clear();
      return;
    }

    try
    {
      m_session->m_reply_op_queue.front()->wait();
    }
    catch (...)
    {
      m_session->m_reply_op_queue.clear();
      throw;
    }

    m_session->m_reply_op_queue.pop_front();
  }
}


void Reply::do_cancel()
{
  //TODO: implement when protocol supports cancel
  //  m_protocol_op->cancel();
}


const cdk::api::Event_info* Reply::get_event_info() const
{
  if (!m_session->m_reply_op_queue.empty())
    return m_session->m_reply_op_queue.front()->waits_for();

  return NULL;
}


/*
   Class Cursor
*/


Cursor::Cursor(Reply &reply)
  : m_session(reply.get_session())
  , m_closed(false)
  , m_rows_op(NULL)
  , m_row_prc(NULL)
  , m_rows_limit(0)
  , m_limited(false)
  , m_more_rows(false)
{

  if (m_session.m_current_cursor)
  {
    THROW("Only one cursor for now");
  }

  if (!reply.has_results())
  {
    if (reply.entry_count() > 0)
      reply.get_error().rethrow();
    else
      // TODO: better error
      throw_error("No results when creating cursor");
  }

  Mdata_storage *mdata = m_session.m_col_metadata.release();
  assert(mdata);
  m_metadata.reset(mdata);

  m_more_rows = true;

  m_session.m_has_results = false;
  m_session.m_current_cursor = this;
  m_session.m_discard = false;
}


Cursor::~Cursor()
{
  close();
}


void Cursor::internal_get_rows(mysqlx::Row_processor& rp)
{
  if (m_closed)
    throw_error("get_rows: Closed cursor");

  //wait previous get_rows();
  if (m_rows_op)
    m_rows_op->wait();

  if (!m_more_rows)
  {
    m_rows_op = NULL;
    m_row_prc = NULL;
    rp.end_of_data();
    return;
  }

  m_rows_op = m_session.start_reading_row_data(*this);
  m_row_prc = &rp;

}

void Cursor::get_rows(mysqlx::Row_processor& rp)
{
  internal_get_rows(rp);
  m_limited = false;
}

void Cursor::get_rows(mysqlx::Row_processor& rp, row_count_t limit)
{
  internal_get_rows(rp);
  m_rows_limit = limit;
  m_limited = true;
}



bool Cursor::get_row(mysqlx::Row_processor& rp)
{
  get_rows(rp, 1);
  wait();
  return m_rows_limit  == 0;
}


void Cursor::close()
{
  if (this == m_session.m_current_cursor)
  {
    // Discard remaining rows

    while(m_rows_op || m_more_rows)
    {
      if (m_rows_op)
      {
        m_row_prc = NULL;
        m_session.m_discard = true;
        m_rows_op->wait();
        m_rows_op = NULL;
        m_session.m_discard = false;
      }

      if (m_more_rows)
      {
        m_rows_op = m_session.start_reading_row_data(*this);
      }
    }

    m_session.m_current_cursor = NULL;
  }

  m_closed = true;
}


// Meta data

/*
  Types used by X protocol must be mapped to generic type/format
  info used by CDK, as defined by cdk::Type_info and  cdk::Format_info
  types.

  TODO: Encoding formats.
*/

Type_info Cursor::type(col_count_t pos) const
{
  typedef protocol::mysqlx::col_type  col_type;
  typedef mysqlx::content_type        content_type;

  const Col_metadata &md= get_metadata(pos);

  switch (md.m_type)
  {
  case col_type::SINT:
  case col_type::UINT:
    return TYPE_INTEGER;

  case col_type::FLOAT:
  case col_type::DOUBLE:
  case col_type::DECIMAL:
    return TYPE_FLOAT;

  case col_type::TIME:
  case col_type::DATETIME:
    return TYPE_DATETIME;

  case col_type::BYTES:
    switch (md.m_content_type)
    {
    case content_type::JSON: return TYPE_DOCUMENT;
    case content_type::GEOMETRY: return TYPE_GEOMETRY;
    case content_type::XML: return TYPE_XML;
    default: return md.m_cs != BINARY_CS_ID ? TYPE_STRING : TYPE_BYTES;
    }

  case col_type::SET:
  case col_type::ENUM:
    return TYPE_STRING;

  default:
    // TODO: correctly handle all X types (BIT)
    return TYPE_BYTES;
  }
}


const Format_info& Cursor::format(col_count_t pos) const
{
  return get_metadata(pos);
}


const Col_metadata& Cursor::get_metadata(col_count_t pos) const
{
  if (!m_metadata)
    THROW("Attempt to get metadata from unitialized cursor");
  Mdata_storage::const_iterator it = m_metadata->find(pos);
  if (it == m_metadata->end())
    // TODO: Report nice error if no metadata present
    THROW("No meta-data for requested column");
  return it->second;
}

// Async_op


bool Cursor::is_completed() const
{
  if (NULL == m_rows_op)
    return true;

  return m_rows_op->is_completed();
}


bool Cursor::do_cont()
{
  if (m_closed)
    throw_error("do_cont: Closed cursor");

  if (m_rows_op)
    m_rows_op->cont();

  return is_completed();
}


void Cursor::do_wait()
{
  if (is_completed())
    return;

  if (m_closed)
    throw_error("wait: Closed cursor");

  if (m_rows_op)
  {
    m_rows_op->wait();
    assert(is_completed());
  }
}


void Cursor::do_cancel()
{
  //same as closed for now
  close();
}


const cdk::api::Event_info* Cursor::get_event_info() const
{
  if (!m_closed && m_rows_op)
    return m_rows_op->waits_for();
  return NULL;
}

/*
   Row_processor (cdk::protocol::mysqlx::Row_processor)
*/


bool Cursor::row_begin(row_count_t row)
{
  if (m_row_prc)
    return m_row_prc->row_begin(row);
  return false;
}


void Cursor::row_end(row_count_t row)
{
  if (m_row_prc)
  {
    m_row_prc->row_end(row);
    if (m_limited)
      --m_rows_limit;
  }
}


void Cursor::col_null(col_count_t pos)
{
  if (m_row_prc)
    m_row_prc->field_null(pos);
}


void Cursor::col_unknown(col_count_t /*pos*/, int /*fmt*/)
{
  //TODO: How to match this cdk::mysqlx::Row_processor vs cdk::protocol::mysqlx::Row_processor
  //      Ignore for now
}


size_t Cursor::col_begin(col_count_t pos, size_t data_len)
{
  if (m_row_prc)
    return m_row_prc->field_begin(pos, data_len);
  return 0;
}


size_t Cursor::col_data(col_count_t pos, bytes data)
{
  if (m_row_prc)
    return m_row_prc->field_data(pos, data);
  return 0;
}


void Cursor::col_end(col_count_t pos, size_t /*data_len*/)
{
  // TODO: data_len
  if (m_row_prc)
    m_row_prc->field_end(pos);
}


void Cursor::done(bool eod, bool more)
{
  if (m_row_prc)
    m_row_prc->end_of_data();

  m_more_rows = false;

  if (more)
  {
    // TODO: Normally start_reading_result() accepts OK message from
    // server, but here it should not.
    m_session.start_reading_result();
  }
  else if (eod)
  {
    m_session.start_reading_stmt_reply();
  }

  m_rows_op = NULL;

}


bool Cursor::message_end()
{
  return m_row_prc && m_limited ? 0 < m_rows_limit : true;
}


}} // cdk mysqlx




