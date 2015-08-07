#include <mysqlxx.h>
#include <vector>
#include <sstream>
#include <iomanip>

/*
  Implementation of Result and Row interfaces.
*/

using cdk::byte;
using cdk::string;
typedef cdk::Traits::col_count_t col_count_t;


/*
  Convenience wrapper around std container that is used
  to store incoming raw bytes sequence.
*/

class Buffer
{
  std::vector<byte> m_impl;

public:

  void append(cdk::bytes data)
  {
    m_impl.insert(m_impl.end(), data.begin(), data.end());
  }

  size_t size() const { return m_impl.size(); }

  cdk::bytes data() const
  {
    return cdk::bytes((byte*)m_impl.data(), m_impl.size());
  }
};


namespace mysqlx {


/*
  This implementation stores all rows received from server in
  m_rows member. Done like this because cdk::Cursor::get_row() is
  not yet implemented.
*/

class Result::Impl
  : public Row
  , public cdk::Row_processor
{
  typedef std::map<col_count_t,Buffer> Row_data;

  cdk::Cursor *m_cur;
  std::map<row_count_t,Row_data> m_rows;
  row_count_t  m_pos;

  Impl() : m_cur(NULL)
  {}

  /*
    Read all rows and store raw data in m_rows.
  */

  void read_rows(cdk::Cursor&);

  /*
    This class instance works as Row instance too. The
    m_pos member tells which row is being accessed.
  */

  Row* get_row(row_count_t pos)
  {
    if (!m_cur)
      return NULL;
    if (pos >= m_rows.size())
      return NULL;
    m_pos= pos;
    return this;
  }

  // Row

  const string getString(col_count_t pos);

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
  friend class ::mysqlx::Result;
};


/*
  Get value stored in column `pos` in row `m_pos` as a string.

  For debugging purposes no real conversion to native C++ types
  is done, but instead the returned string has format:

    "<type>: <bytes>"

  where <type> is the type of the column as reported by CDK and <bytes>
  is a hex dump of the value. For STRING values we assume they are ascii
  strings and show as stings, NULL values are shown as "<null>".
*/

const string Result::Impl::getString(col_count_t pos)
{
  if (!m_cur)
    throw "empty row";
  if (pos > m_cur->col_count())
    throw "pos out of range";

  std::stringstream out;

  // Add prefix with name of the CDK column type

#define TYPE_NAME(X)  case cdk::TYPE_##X: out <<#X <<": "; break;

  switch (m_cur->type(pos))
  {
    CDK_TYPE_LIST(TYPE_NAME)
  default:
    out <<"UNKNOWN(" <<m_cur->type(pos) <<"): "; break;
  }

  // Append value bytes

  Row_data &rd= m_rows.at(m_pos);

  try {
    // will throw out_of_range exception if column at `pos` is NULL
    cdk::bytes data= rd.at(pos).data();

    switch (m_cur->type(pos))
    {
    case cdk::TYPE_STRING:
      // assume ascii string
      out <<'"' <<std::string(data.begin(), data.end()-1) <<'"';
      break;

    default:

      // Output other values as sequences of hex digits

      for(byte *ptr= data.begin(); ptr < data.end(); ++ptr)
      {
        out <<std::setw(2) <<std::setfill('0') <<std::hex <<(unsigned)*ptr;
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


void Result::Impl::read_rows(cdk::Cursor &cur)
{
  m_cur= &cur;
  m_cur->get_rows(*this);
  m_cur->wait();
}



size_t Result::Impl::field_begin(col_count_t pos, size_t)
{
  Row_data &rd= m_rows[m_pos];
  rd.insert(std::pair<col_count_t,Buffer>(pos, Buffer()));
  return 1024;
}

size_t Result::Impl::field_data(col_count_t pos, bytes data)
{
  m_rows[(unsigned)m_pos][(unsigned)pos].append(data);
  return 1024;
}



Result::Result(Session::Result_init init)
  : m_reply(init)
  , m_cursor(NULL)
  , m_pos(0)
{
  if (m_reply.has_results())
  {
    m_impl= new Impl();
    m_cursor= new cdk::Cursor(m_reply);
    m_cursor->wait();
    // TODO: do it asynchronously
    m_impl->read_rows(*m_cursor);
    m_cursor->close();
    // Note: we keep cursor instance because it gives access to meta-data
  }
}


Result::~Result()
{
  delete m_cursor;
  delete m_impl;
}


Row* Result::next()
{
  return m_impl->get_row(m_pos++);
}


}  // mysqlx
