/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
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
#include <mysqlx.h>

#include "impl.h"

#include <vector>
#include <sstream>
#include <iomanip>

#include <boost/variant.hpp>

/*
  Implementation of Result and Row interfaces.
*/

using namespace ::mysqlx;
using mysqlx::GUID;
using mysqlx::col_count_t;
using mysqlx::row_count_t;

using std::endl;

class bytes::Access
{
public:

  static bytes mk(byte *beg, size_t len)
  { return bytes(beg, beg+len); }

  static bytes mk(const cdk::bytes &data)
  { return bytes(data.begin(), data.end()); }
};

/*
  Convenience wrapper around std container that is used
  to store incoming raw bytes sequence.
*/

class Buffer
{
  std::vector<byte> m_impl;

public:

  void append(bytes data)
  {
    m_impl.insert(m_impl.end(), data.begin(), data.end());
  }

  size_t size() const { return m_impl.size(); }

  cdk::bytes data() const
  {
    return cdk::bytes((byte*)m_impl.data(), m_impl.size());
  }
};


/*
  Storage for column meta-data information
  ========================================

  To correctly decode raw bytes that represent values stored in
  a row we need type and encoding information provided by cdk.
  First a Type_info constant T and then:

  - encoding format information which is a value of type Fromat<T>
  - codec object of type Codec<T>.

  For convenience, encoding format information and codec are kept
  together in single Format_descr<T> structure.
*/

template <cdk::Type_info T>
struct Format_descr
{
  cdk::Format<T> m_format;
  cdk::Codec<T>  m_codec;

  Format_descr(const cdk::Format_info &fi)
    : m_format(fi), m_codec(fi)
  {}
};

template <>
struct Format_descr<cdk::TYPE_DOCUMENT>
{
  cdk::Format<cdk::TYPE_DOCUMENT> m_format;
  cdk::Codec<cdk::TYPE_DOCUMENT>  m_codec;

  Format_descr(const cdk::Format_info &fi)
    : m_format(fi)
  {}
};


/*
  Structure Format_info holds information about the type
  of a column (m_type) and about its encoding format in
  Format_descr<T> structure. Since C++ type of Format_descr<T>
  is different for each T, a boost::variant is used to store
  the appropriate Format_descr<T> value.
*/

typedef boost::variant <
  Format_descr<cdk::TYPE_STRING>,
  Format_descr<cdk::TYPE_INTEGER>,
  Format_descr<cdk::TYPE_FLOAT>,
  Format_descr<cdk::TYPE_DOCUMENT>
> Format_info_base;

struct Format_info
  : public Format_info_base
{
  cdk::Type_info m_type;

  template <cdk::Type_info T>
  Format_info(const Format_descr<T> &fd)
    : Format_info_base(fd), m_type(T)
  {}

  template <cdk::Type_info T>
  Format_descr<T>& get() const
  {
    /*
      Note: we cast away constness here, because using a codec can
      modify it, and thus the Format_descr<T> must be mutable.
    */
    return const_cast<Format_descr<T>&>(boost::get<Format_descr<T>>(*this));
  }
};


/*
  Meta_data holds type and format information for all columns in
  a row set. An instance is filled given information provided by
  cdk::Meta_data interface.
*/

struct Meta_data
  : private std::map<cdk::col_count_t, Format_info>
{
  Meta_data(cdk::Meta_data&);

  col_count_t col_count() const { return m_col_count;  }

  const Format_info& get_format(cdk::col_count_t pos) const
  {
    return at(pos);
  }

  cdk::Type_info get_type(cdk::col_count_t pos) const
  {
    return get_format(pos).m_type;
  }

private:

  cdk::col_count_t  m_col_count;

  /*
    Add to this Meta_data instance information about column
    at position `pos`. The type and format information is given
    by cdk::Format_info object.
  */
  template<cdk::Type_info T>
  void add(cdk::col_count_t pos, const cdk::Format_info &fi)
  {
    emplace(pos, Format_descr<T>(fi));
  }
};


/*
  Create Meta_data instance using information provided by
  cdk::Meta_data interface.
*/

Meta_data::Meta_data(cdk::Meta_data &md)
  : m_col_count(md.col_count())
{
  for (col_count_t pos = 0; pos < m_col_count; ++pos)
  {
    cdk::Type_info ti = md.type(pos);
    const cdk::Format_info &fi = md.format(pos);

    switch (ti)
    {
    case cdk::TYPE_STRING:    add<cdk::TYPE_STRING>(pos, fi);   break;
    case cdk::TYPE_INTEGER:   add<cdk::TYPE_INTEGER>(pos, fi);  break;
    case cdk::TYPE_FLOAT:     add<cdk::TYPE_FLOAT>(pos, fi);    break;
    case cdk::TYPE_DOCUMENT:  add<cdk::TYPE_DOCUMENT>(pos, fi); break;
    default:
      THROW("Add support for the rest of types.");
      break;
    }
  }
}


/*
  Data structure used to hold raw row data. It holds a Buffer with
  raw bytes for each non-null field of a row.
*/

typedef std::map<col_count_t, Buffer> Row_data;


/*
  Implementation for single Row instance. It holds a copy of row
  raw data and a shared pointer to row set meta-data.

  Using meta-data information, it can decode raw bytes of each
  field into appropriate Value.
*/

class Row::Impl
{
public:

  Impl() {}
  Impl(const Row_data&, std::shared_ptr<Meta_data>&);

private:

  Row_data m_data;
  std::shared_ptr<Meta_data> m_mdata;
  std::map<col_count_t, Value> m_vals;
  col_count_t m_col_count = 0;

  void clear()
  {
    m_data.clear();
    m_vals.clear();
    m_mdata.reset();
  }

  bytes get_bytes(col_count_t pos) const
  {
    return mysqlx::bytes::Access::mk(m_data.at(pos).data());
  }

  // Get value of field at given position after converting to Value.
  // @throws std::out_of_range if given column does not exist in the row.

  template<cdk::Type_info T>
  Value& get(col_count_t pos)
  {
    const Format_info &fi = m_mdata->get_format(pos);

    m_vals.emplace(
      pos,
      convert(m_data.at(pos).data(), fi.get<T>())
    );

    return m_vals.at(pos);
  }

  // Convert raw bytes to Value using given encoding format description.

  template<cdk::Type_info T>
  const Value convert(cdk::bytes, Format_descr<T>&) const;

  friend class Row;
  friend class RowResult;
  friend class SqlResult;
};


// Note: row data is copied

Row::Impl::Impl(const Row_data &data, std::shared_ptr<Meta_data> &mdata)
  : m_data(data), m_mdata(mdata)
{}


const Row::Impl& Row::get_impl() const
{
  if (!m_impl)
    THROW("Attempt to use null Row instance");
  return *m_impl;
}


col_count_t Row::colCount() const
{
  const Impl &impl = get_impl();
  col_count_t cnt = (impl.m_mdata ? impl.m_mdata->col_count() : 0);
  return impl.m_col_count > cnt ? impl.m_col_count : cnt;
}


bytes Row::getBytes(col_count_t pos) const
{
  return get_impl().get_bytes(pos);
}


Value& Row::get(mysqlx::col_count_t pos)
{
  Impl &impl = get_impl();

  /*
    First see if field value is already stored in
    m_vals array.
  */

  try {
    return impl.m_vals.at(pos);
  }
  catch (const std::out_of_range&)
  {
    /*
      If we have data from server (meta-data is set) then we convert
      it into the value below - otherwise we throw out_of_range error.
    */
    if (!impl.m_mdata)
      throw;
  }

  /*
    We have data from server - convert it into a value and store
    in m_vals.
  */

  try {
    // will throw out_of_range exception if column at `pos` is NULL
    cdk::bytes data = impl.m_data.at(pos).data();

    switch (impl.m_mdata->get_type(pos))
    {
    case cdk::TYPE_STRING:    return impl.get<cdk::TYPE_STRING>(pos);
    case cdk::TYPE_INTEGER:   return impl.get<cdk::TYPE_INTEGER>(pos);
    case cdk::TYPE_FLOAT:     return impl.get<cdk::TYPE_FLOAT>(pos);
    case cdk::TYPE_DOCUMENT:  return impl.get<cdk::TYPE_DOCUMENT>(pos);

      // TODO: Other "natural" conversions
      // TODO: User-defined conversions (also to user-defined types)

    default:
      return set(pos, Value::Access::mk_raw(data));
    }
  }
  catch (std::out_of_range&)
  {
    // set to NULL
    return set(pos, Value());
  }
}


Value& Row::set(col_count_t pos, const Value &val)
{
  if (!m_impl)
    m_impl = std::make_shared<Impl>();

  Impl &impl = get_impl();

  impl.m_vals.emplace(pos, val);

  if (pos + 1 > impl.m_col_count)
    impl.m_col_count = pos + 1;

  return impl.m_vals.at(pos);
}

void Row::clear()
{
  m_impl.reset();
}

/*
  Conversions of raw value representation to Value objects.

  Note: gcc complains if templates are not specialized in the same namespace
  in which they were declared.
*/

namespace mysqlx {

  template<>
  const Value
    Row::Impl::convert(cdk::bytes data, Format_descr<cdk::TYPE_STRING> &fd) const
  {
    auto &codec = fd.m_codec;
    cdk::string str;
    codec.from_bytes(data, str);
    return Value(std::move(str));
  }

  template<>
  const Value
    Row::Impl::convert(cdk::bytes data, Format_descr<cdk::TYPE_INTEGER> &fd) const
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

  template<>
  const Value
    Row::Impl::convert(cdk::bytes data, Format_descr<cdk::TYPE_FLOAT> &fd) const
  {
    auto &fmt = fd.m_format;

    // Note: DECIMAL format not yet supported by CDK

    if (fmt.FLOAT == fmt.type() || fmt.DOUBLE == fmt.type())
    {
      double val;
      fd.m_codec.from_bytes(data, val);
      return Value(val);
    }

    return Value::Access::mk_raw(data);
  }

  template<>
  const Value
    Row::Impl::convert(cdk::bytes data, Format_descr<cdk::TYPE_DOCUMENT>&) const
  {
    /*
    Note: this assumes that document is represented as json string
    - thanks to this we can take benefit of lazy parsing.

    Otherwise, implementation that would not assume what underlying
    representation is used for documnets should use a Codec to decode
    the raw bytes and build a representation of the documnent to be
    stored in the Value instance.
    */
    return Value::Access::mk_doc(std::string(data.begin(), data.end()));
  }

}


/*
  Result implementation
  =====================
*/

class internal::BaseResult::Impl
  : public cdk::Row_processor
{
  cdk::Reply  *m_reply = NULL;
  cdk::Cursor *m_cursor = NULL;
  Row_data     m_row;
  std::shared_ptr<Meta_data>  m_mdata;
  GUID         m_guid;
  bool         m_cursor_closed = false;

  Impl(cdk::Reply *r)
    : m_reply(r)
  {
    init();
  }

  Impl(cdk::Reply *r, const GUID &guid)
    : m_reply(r), m_guid(guid)
  {
    init();
  }

  void init()
  {
    if (!m_reply)
      return;

    m_reply->wait();

    if (m_reply->entry_count() > 0)
      return;

    if (m_reply->has_results())
    {
      m_cursor = new cdk::Cursor(*m_reply);
      m_cursor->wait();
      // copy meta-data information from cursor
      m_mdata = std::make_shared<Meta_data>(*m_cursor);
    }
  }

  virtual ~Impl()
  {
    // Note: Cursor must be deleted before reply.
    delete m_cursor;
    delete m_reply;
  }

  /*
    Read next row from the cursor. Returns NULL if there are no
    more rows. Throws exeption if this result has no data.
  */

  const Row_data *get_row();

  // Row_processor

  bool row_begin(row_count_t)
  {
    m_row.clear();
    return true;
  }
  void row_end(row_count_t) {}

  size_t field_begin(col_count_t pos, size_t);
  void   field_end(col_count_t) {}
  void   field_null(col_count_t) {}
  size_t field_data(col_count_t pos, bytes);
  void   end_of_data() {}

  friend class Row_builder;
  friend class internal::BaseResult;
  friend class mysqlx::Result;
  friend class mysqlx::RowResult;
  friend class mysqlx::SqlResult;
};


const Row_data* Result::Impl::get_row()
{
  if (!m_cursor)
    THROW("Attempt to read row from empty result");

  if (m_cursor_closed)
    return NULL;

  /*
    TODO: Row cache for better I/O performance (read several
    rows at once)
  */
  if (m_cursor->get_row(*this))
    return &m_row;

  m_cursor->close();
  m_cursor_closed = true;

  return NULL;
}



size_t Result::Impl::field_begin(col_count_t pos, size_t size)
{
  m_row.insert(std::pair<col_count_t, Buffer>(pos, Buffer()));
  // FIX
  return size;
}

size_t Result::Impl::field_data(col_count_t pos, bytes data)
{
  m_row[(unsigned)pos].append(mysqlx::bytes::Access::mk(data));
  // FIX
  return data.size();
}


/*
  Result
  ======
*/


internal::BaseResult::BaseResult(cdk::Reply *r)
try {
  m_owns_impl = true;
  m_impl= new Impl(r);
}
CATCH_AND_WRAP


internal::BaseResult::BaseResult(cdk::Reply *r, const GUID &guid)
try {
  m_owns_impl = true;
  m_impl= new Impl(r,guid);
}
CATCH_AND_WRAP


internal::BaseResult::~BaseResult()
try {
  if (m_owns_impl)
    delete m_impl;
}
CATCH_AND_WRAP


internal::BaseResult::Impl& internal::BaseResult::get_impl()
try {
  if (!m_impl)
    THROW("Attempt to use null result instance");
  return *m_impl;
}
CATCH_AND_WRAP

const GUID& Result::getLastDocumentId() const
try {
  if (!m_impl)
    THROW("Empty result");
  return m_impl->m_guid;
}
CATCH_AND_WRAP


/*
  RowResult
  =========
*/


Row RowResult::fetchOne()
try {
  Impl &impl = get_impl();
  const Row_data *row = impl.get_row();

  if (!row)
    return Row();

  return Row(std::make_shared<Row::Impl>(*row, impl.m_mdata));
}
CATCH_AND_WRAP


col_count_t RowResult::getColumnCount() const
try {
  if (!m_impl)
    THROW("Empty result");
  if (!m_impl->m_cursor)
    THROW("No result set");
  return m_impl->m_cursor->col_count();
}
CATCH_AND_WRAP


bool SqlResult::hasData() const
//try
{
  // Note: add try catch wrapper if the code here
  // can throw errors.
  return NULL != m_impl->m_cursor;
}
//CATCH_AND_WRAP


/*
  DocResult
  =========
*/

void DocResult::operator=(BaseResult &&init)
{
  BaseResult::operator=(std::move(init));
  delete m_doc_impl;
  m_doc_impl= new Impl(init);
}

DocResult::~DocResult()
{
  delete m_doc_impl;
}


DbDoc DocResult::fetchOne()
try {
  DbDoc doc = m_doc_impl->get_current_doc();
  m_doc_impl->next_doc();
  return std::move(doc);
}
CATCH_AND_WRAP


