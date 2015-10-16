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
  Result implementation
  =====================
  This implementation stores all rows received from server in
  m_rows member. Done like this because cdk::Cursor::get_row() is
  not yet implemented.
*/

class BaseResult::Impl
  : public Row
  , public cdk::Row_processor
{
  typedef std::map<col_count_t,Buffer> Row_data;

  cdk::Reply  *m_reply;
  cdk::Cursor *m_cursor;
  std::map<row_count_t,Row_data> m_rows;
  row_count_t  m_pos;
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
      m_cursor= new cdk::Cursor(*m_reply);
      m_cursor->wait();
      // TODO: do it asynchronously
      read_rows();
      m_cursor->close();
      // Note: we keep cursor instance because it gives access to meta-data
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

  /*
    This class instance works as Row instance too. The
    m_pos member tells which row is being accessed.
  */

  Row* get_row(row_count_t pos)
  {
    if (!m_cursor)
      return NULL;
    if (pos >= m_rows.size())
      return NULL;
    m_pos= pos;
    return this;
  }

  // Row

  const mysqlx::string getString(mysqlx::col_count_t pos);
  mysqlx::bytes getBytes(mysqlx::col_count_t pos);
  Value get(mysqlx::col_count_t pos);

  // Row_processor

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


bytes Result::Impl::getBytes(mysqlx::col_count_t pos)
{
  return mysqlx::bytes::Access::mk(m_rows.at(m_pos).at(pos).data());
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


Value Result::Impl::get(mysqlx::col_count_t pos)
{
  using cdk::Format;
  using cdk::Codec;

  if (!m_cursor)
    throw "empty row";
  if (pos > m_cursor->col_count())
    throw "pos out of range";

  Row_data &rd = m_rows.at(m_pos);

  try {
    // will throw out_of_range exception if column at `pos` is NULL
    cdk::bytes data = rd.at(pos).data();

    switch (m_cursor->type(pos))
    {
    case cdk::TYPE_STRING:
      {
        Codec<cdk::TYPE_STRING> codec(m_cursor->format(pos));
        cdk::string str;
        codec.from_bytes(data, str);
        return Value(std::move(str));
      }

    case cdk::TYPE_INTEGER:
    {
      Codec<cdk::TYPE_INTEGER>  codec(m_cursor->format(pos));
      Format<cdk::TYPE_INTEGER> fmt(m_cursor->format(pos));

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

    case cdk::TYPE_FLOAT:
    {
      Format<cdk::TYPE_FLOAT> fmt(m_cursor->format(pos));

      // Note: DECIMAL format not yet supported by CDK

      if (fmt.FLOAT == fmt.type() || fmt.DOUBLE == fmt.type())
      {
        Codec<cdk::TYPE_FLOAT> codec(m_cursor->format(pos));
        double val;
        codec.from_bytes(data, val);
        return Value(val);
      }

      return Value::Access::mk_raw(data);
    }

    case cdk::TYPE_DOCUMENT:
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
  Get value stored in column `pos` in row `m_pos` as a string.

  For debugging purposes no real conversion to native C++ types
  is done, but instead the returned string has format:

    "<type>: <bytes>"

  where <type> is the type of the column as reported by CDK and <bytes>
  is a hex dump of the value. For STRING values we assume they are ascii
  strings and show as stings, NULL values are shown as "<null>".
*/

const mysqlx::string Result::Impl::getString(mysqlx::col_count_t pos)
{
  if (!m_cursor)
    throw "empty row";
  if (pos > m_cursor->col_count())
    throw "pos out of range";

  std::wstringstream out;

  // Add prefix with name of the CDK column type

#define TYPE_NAME(X)  case cdk::TYPE_##X: out <<#X <<": "; break;

  switch (m_cursor->type(pos))
  {
    CDK_TYPE_LIST(TYPE_NAME)
  default:
    out <<"UNKNOWN(" <<m_cursor->type(pos) <<"): "; break;
  }

  // Append value bytes

  Row_data &rd= m_rows.at(m_pos);

  try {
    // will throw out_of_range exception if column at `pos` is NULL
    cdk::bytes data= rd.at(pos).data();

    switch (m_cursor->type(pos))
    {
    case cdk::TYPE_STRING:
      // assume ascii string
      out <<'"' <<std::string(data.begin(), data.end()-1).c_str() <<'"';
      break;

    default:

      // Output other values as sequences of hex digits

      for(byte *ptr= data.begin(); ptr < data.end(); ++ptr)
      {
        out <<std::setw(2) <<std::setfill(L'0') <<std::hex <<(unsigned)*ptr;
      };
      break;
    }
  }
  catch (std::out_of_range&)
  {
    out <<"<null>";
  }

  return out.str();
}


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


Row* RowResult::fetchOne()
try {
  return m_impl->get_row(m_pos++);
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
try {
  return NULL != m_impl->m_cursor;
}
CATCH_AND_WRAP


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


DbDoc* DocResult::fetchOne()
try {
  m_doc_impl->next_doc();
  return (m_doc_impl->has_doc() ? &(m_doc_impl->m_doc) : NULL);
}
CATCH_AND_WRAP


