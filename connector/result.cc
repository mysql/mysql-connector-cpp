/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
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
  Row_data m_data;
  std::shared_ptr<Meta_data> m_mdata;

  Impl(const Row_data&, std::shared_ptr<Meta_data>&);

  bytes get_bytes(col_count_t pos) const
  {
    return mysqlx::bytes::Access::mk(m_data.at(pos).data());
  }

  // Get value of field at given position after converting to Value.

  template<cdk::Type_info T>
  const Value get(col_count_t pos) const
  {
    const Format_info &fi = m_mdata->get_format(pos);
    return convert(m_data.at(pos).data(), fi.get<T>());
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


void Row::init(Impl *impl)
{
  m_impl = impl;
  m_owns_impl = true;
}


Row::~Row()
{
  /*
     Note: catch wrapper removed to avoid compile warnings --
     add back if Impl destructor can throw any exceptions.
  */
//  try {
    if (m_owns_impl)
      delete m_impl;
//  }
//  CATCH_AND_WRAP
}


const Row::Impl& Row::get_impl() const
{
  if (!m_impl)
    throw "Attempt to use null Row instance";
  return *m_impl;
}


bytes Row::getBytes(col_count_t pos) const
{
  return get_impl().get_bytes(pos);
}


struct Value::Access
{
  static Value mk_raw(const cdk::bytes data)
  {
    Value ret;
    ret.m_type = Value::RAW;
    ret.m_str.assign(data.begin(), data.end());
    return std::move(ret);
  }

  static Value mk_doc(const string &json)
  {
    Value ret;
    ret.m_type = Value::DOCUMENT;
    ret.m_doc = DbDoc(json);
    return std::move(ret);
  }
};


const Value Row::get(mysqlx::col_count_t pos) const
{
  const Impl &impl = get_impl();

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
      return Value::Access::mk_raw(data);
    }
  }
  catch (std::out_of_range&)
  {
    // NULL value
    return Value();
  }
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
  This implementation stores all rows received from server in
  m_rows member.

  @todo Non-buffering implementation which reads single rows
  from cursor.
*/

class BaseResult::Impl
  : public cdk::Row_processor
{
  cdk::Reply  *m_reply;
  cdk::Cursor *m_cursor;
  std::map<row_count_t,Row_data> m_rows;
  std::shared_ptr<Meta_data>     m_mdata;
  GUID         m_guid;

  Impl(cdk::Reply *r)
    : m_reply(r)
    , m_cursor(NULL)
  {
    init();
  }

  Impl(cdk::Reply *r, const GUID &guid)
    : m_reply(r), m_cursor(NULL), m_guid(guid)
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
      // TODO: do it asynchronously
      read_rows();
      m_cursor->close();
      m_pos = 0;

      // copy meta-data information from cursor
      m_mdata = std::make_shared<Meta_data>(*m_cursor);
    }
  }

  ~Impl()
  {
    delete m_reply;
    delete m_cursor;
  }

  /*
    Read all rows and store raw data in m_rows.
  */

  void read_rows();

  // Row_processor

  row_count_t m_pos;

  bool row_begin(row_count_t pos)
  {
    m_pos= pos;
    m_rows.insert(std::pair<row_count_t,Row_data>(pos, Row_data()));
    return true;
  }
  void row_end(row_count_t) {}

  size_t field_begin(col_count_t pos, size_t);
  void   field_end(col_count_t) {}
  void   field_null(col_count_t) {}
  size_t field_data(col_count_t pos, bytes);
  void   end_of_data() {}

  friend class Row_builder;
  friend class BaseResult;
  friend class Result;
  friend class RowResult;
  friend class SqlResult;
};


void Result::Impl::read_rows()
{
  if (!m_cursor)
    return;
  m_cursor->get_rows(*this);
  m_cursor->wait();
}



size_t Result::Impl::field_begin(col_count_t pos, size_t)
{
  Row_data &rd= m_rows[m_pos];
  rd.insert(std::pair<col_count_t,Buffer>(pos, Buffer()));
  return 1024;
}

size_t Result::Impl::field_data(col_count_t pos, bytes data)
{
  m_rows[(unsigned)m_pos][(unsigned)pos].append(mysqlx::bytes::Access::mk(data));
  return 1024;
}


/*
  Result
  ======
*/

#include <iostream>

BaseResult::BaseResult(cdk::Reply *r)
try {
  m_owns_impl = true;
  m_impl= new Impl(r);
}
CATCH_AND_WRAP


BaseResult::BaseResult(cdk::Reply *r, const GUID &guid)
try {
  m_owns_impl = true;
  m_impl= new Impl(r,guid);
}
CATCH_AND_WRAP


BaseResult::~BaseResult()
try {
  if (m_owns_impl)
    delete m_impl;
}
CATCH_AND_WRAP


BaseResult::Impl& BaseResult::get_impl()
try {
  if (!m_impl)
    throw "Attempt to use null result instance";
  return *m_impl;
}
CATCH_AND_WRAP

const GUID& Result::getLastDocumentId() const
try {
  if (!m_impl)
    throw "Empty result";
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
  cdk::row_count_t pos = impl.m_pos;

  if (!impl.m_cursor)
    throw "Attempt to fetch row from result without any data";
  if (pos >= impl.m_rows.size())
    return Row();

  impl.m_pos++;
  return Row(new Row::Impl(impl.m_rows.at(pos), impl.m_mdata));
}
CATCH_AND_WRAP


col_count_t RowResult::getColumnCount() const
try {
  if (!m_impl)
    throw "Empty result";
  if (!m_impl->m_cursor)
    throw "No result set";
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


