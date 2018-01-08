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

#include <mysql/cdk.h>

#include "result.h"
#include "session.h"

#include <vector>
#include <sstream>
#include <iomanip>
#include <cctype>


/*
  Implementation of result and row objects and conversion of raw bytes
  into values.
*/

using namespace ::mysqlx::common;


/*
  Decoding raw bytes into values
  ==============================

  Overloads of convert() defined below handle conversion of raw representation
  of values of different CDK types into Value object. A format descriptor is
  used to learn about the raw encoding format and perform conversion to the
  correct type using a corresponding codec object.
*/


Value
mysqlx::common::convert(cdk::bytes data, Format_descr<cdk::TYPE_STRING> &fd)
{
  /*
    String encoding has artificial 0x00 byte appended at the end to
    distinguish the empty string from the null value. We skip
    the trailing 0x00 byte to get just the raw bytes that encode the string.
  */

  cdk::bytes raw(data.begin(), data.end() - 1);

  // If this string value is in fact a SET, then return it as raw bytes.

  if (fd.m_format.is_set())
    return { raw.begin(), raw.size() };

  auto &codec = fd.m_codec;
  cdk::string str;
  codec.from_bytes(raw, str);
  return std::move(str);
}


Value
mysqlx::common::convert(cdk::bytes data, Format_descr<cdk::TYPE_INTEGER> &fd)
{
  auto &codec = fd.m_codec;
  auto &fmt = fd.m_format;

  if (fmt.is_unsigned())
  {
    uint64_t val;
    codec.from_bytes(data, val);
    return Value(val);
  }
  else
  {
    int64_t val;
    codec.from_bytes(data, val);
    return Value(val);
  }
}


Value
mysqlx::common::convert(cdk::bytes data, Format_descr<cdk::TYPE_FLOAT> &fd)
{
  auto &fmt = fd.m_format;

  if (fmt.FLOAT == fmt.type())
  {
    float val;
    fd.m_codec.from_bytes(data, val);
    return Value(val);
  }

  // For other formats (DOUBLE, DECIMAL), try storing in double
  // TODO: exact representation for DECIMAL values?
  {
    double val;
    fd.m_codec.from_bytes(data, val);
    return Value(val);
  }
}


Value
mysqlx::common::convert(cdk::bytes data, Format_descr<cdk::TYPE_DOCUMENT>&)
{
  if (0 == data.size())
    return Value();

  /*
    Note: Here we do not look into format description and blindly assume
    that document is represented as a JSON string.

    Otherwise, implementation that would not assume what underlying
    representation is used for documnets should use a Codec to decode
    the raw bytes and build a representation of the documnent to be
    stored in the Value instance.
  */

  // trim initial space

  unsigned i;
  for (i = 0; i < data.size() && std::isspace(*(data.begin() + i)); ++i);

  std::string json(data.begin() + i, data.end()-1);

  return Value::Access::mk_json(json);
}



/*
  Result implementation
  =====================
*/


Result_impl_base::Result_impl_base(Result_init &init)
  : m_sess(init.get_session()), m_reply(init.get_reply())
{
  // Note: init.get_reply() can be NULL in the case of ignored server error
  m_sess->register_result(this);
  init.init_result(*this);
}


Result_impl_base::~Result_impl_base()
{
  try {
    if (m_sess)
      m_sess->deregister_result(this);
  }
  catch (...)
  {}

  // Note: Cursor must be deleted before reply.
  delete m_cursor;
  delete m_reply;
}


bool Result_impl_base::next_result()
{
  /*
    Note: closing cursor discards previous rset. Only then
    we can move to the next rset (if any).
  */

  if (m_pending_rows)
  {
    assert(m_cursor);
    m_cursor->close();
  }

  // Prepare for reading (next) result

  delete m_cursor;
  m_cursor = nullptr;
  m_mdata.reset();
  clear_cache();
  m_pending_rows = false;
  clear_diagnostics();
  m_inited = true;


  if (!m_reply)
    return false;

  // Wait for the cdk reply object to become ready.

  m_reply->wait();

  if (0 < m_reply->entry_count())
    m_reply->get_error().rethrow();

  if (!m_reply->has_results())
  {
    m_sess->deregister_result(this);
    return false;
  }

  // Result has row data - create cursor to access it

  m_cursor = new cdk::Cursor(*m_reply);

  // Wait for cursor to fetch result meta-data and copy it to local storage.

  m_cursor->wait();
  m_mdata.reset(fetch_meta_data(*m_cursor));

  m_pending_rows = true;

  return true;
}


const Row_data* Result_impl_base::get_row()
{
  // TODO: Session parameter for cache prefetch size

  if (!load_cache(1024))
    return nullptr;

  assert(!m_row_cache.empty());

  m_row = m_row_cache.front();
  m_row_cache.pop_front();
  m_row_cache_size--;
  return &m_row;
}


/*
  Returns true if there are some rows in the cache after returning from
  the call. If cache is empty when this method is called, it loads
  prefetch_size rows into the cache. If prefetch_size is 0, it loads
  all remaining rows into the cache (even if cache currently contains some
  rows).
*/

bool Result_impl_base::load_cache(row_count_t prefetch_size)
{
  if (!m_inited)
    next_result();

  if (!m_row_cache.empty() && 0 != prefetch_size)
    return true;

  if (!m_pending_rows)
    return false;

  /*
    Note: if cache is not empty then m_cache_it correctly points at the last
    element in the cache.
  */

  if (m_row_cache.empty())
    m_cache_it = m_row_cache.before_begin();

  // Initiate row reading operation

  if (0 < prefetch_size)
    m_cursor->get_rows(*this, prefetch_size);
  else
    m_cursor->get_rows(*this);  // this reads all remaining rows

  // Wait for it to complete

  m_cursor->wait();

  /*
    Cleanup after reading all rows.
  */

  if (!m_pending_rows)
  {
    m_cursor->close();
    m_sess->deregister_result(this);
    load_diagnostics();
  }

  return !m_row_cache.empty();
}


//  Row_processor interface implementation


size_t Result_impl_base::field_begin(col_count_t pos, size_t size)
{
  //m_row.insert(std::pair<col_count_t, Buffer>(pos, Buffer()));
  m_row.emplace(pos, Buffer());
  // FIX
  return size;
}

size_t Result_impl_base::field_data(col_count_t pos, bytes data)
{
  m_row[(unsigned)pos].append(data);
  // FIX
  return data.size();
}

void Result_impl_base::row_end(row_count_t)
{
  if (!m_row_filter(m_row))
    return;

  m_cache_it = m_row_cache.emplace_after(m_cache_it, std::move(m_row));
  m_row_cache_size++;
}

void Result_impl_base::end_of_data()
{
  m_pending_rows = false;
}


// Handle diagnostic information.


void Result_impl_base::load_diagnostics()
{
  assert(m_reply);

  /*
    Flag m_all_warnings tells if all warnings for this result have
    been collected in m_warnings. If this is the case then there is
    nothing to do.

    Otherwise we copy currently available warnings to m_warnings and
    check if complete reply has been processed (m_reply->has_results()
    returns false). In that case we can set m_all_warnings to true,
    because we know that no more warnings will be reported. Otherwise
    the flag remains false and we will re-load warnings on a next call.
    This way newly reported warnings (if any) will land in m_warnings
    list.

    Note: A better handling of warnings would be with asynchronous
    notifications about new warnings which would be appended to m_warnings
    list. But this is not yet implemented in CDK.
  */

  if (m_diag_ready)
    return;

  if (!m_reply->has_results())
    m_diag_ready = true;

  Diagnostic_arena::clear();

  for (auto &it = m_reply->get_entries(cdk::api::Severity::WARNING); it.next();)
  {
    auto &entry = it.entry();
    add_entry(entry.severity(), entry.get_error().clone());
  }
}
