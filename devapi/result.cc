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
#include <mysql_devapi.h>

#include "impl.h"

#include <vector>
#include <sstream>
#include <iomanip>
#include <cctype>


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

  static bytes mk(const cdk::bytes &data)
  { return bytes(data.begin(), data.end()); }
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
    const cdk::Column_info &ci = md.col_info(pos);

    switch (ti)
    {
    case cdk::TYPE_STRING:    add<cdk::TYPE_STRING>(pos, ci, fi);   break;
    case cdk::TYPE_INTEGER:   add<cdk::TYPE_INTEGER>(pos, ci, fi);  break;
    case cdk::TYPE_FLOAT:     add<cdk::TYPE_FLOAT>(pos, ci, fi);    break;
    case cdk::TYPE_DOCUMENT:  add<cdk::TYPE_DOCUMENT>(pos, ci, fi); break;
    case cdk::TYPE_DATETIME:  add<cdk::TYPE_DATETIME>(pos, ci, fi); break;
    case cdk::TYPE_GEOMETRY:  add<cdk::TYPE_GEOMETRY>(pos, ci, fi); break;
    case cdk::TYPE_XML:       add<cdk::TYPE_XML>(pos, ci, fi); break;
    default:
      add_raw(pos, ci, ti);
      break;
    }
  }
}


/*
  Column implementation.
*/


void Column::print(std::ostream &out) const
{
  if (!m_impl->m_schema_name.empty())
    out << "`" << m_impl->m_schema_name << "`.";
  string table_name = getTableLabel();
  if (!table_name.empty())
    out << "`" << table_name << "`.";
  out << "`" << getColumnLabel() <<"`";
}


string Column::getSchemaName()  const
{
  assert(m_impl);
  return m_impl->m_schema_name;
}

string Column::getTableName()   const
{
  assert(m_impl);
  return m_impl->m_table_name;
}

string Column::getTableLabel()  const
{
  assert(m_impl);
  return m_impl->m_table_label;
}

string Column::getColumnName()  const
{
  assert(m_impl);
  return m_impl->m_name;
}

string Column::getColumnLabel() const
{
  assert(m_impl);
  return m_impl->m_label;
}

unsigned long Column::getLength() const
{
  assert(m_impl);
  return m_impl->m_length;
}

unsigned short Column::getFractionalDigits() const
{
  assert(m_impl);
  return m_impl->m_decimals;
}


/*
  Method getType() translates CDK type/format info into
  DevAPI type information.
*/

Type Column::getType()   const
{
  assert(m_impl);

  try {
    switch (m_impl->m_type)
    {
    case cdk::TYPE_BYTES:
      return Type::BYTES;

    case cdk::TYPE_DOCUMENT:
      return Type::JSON;

    case cdk::TYPE_STRING:
    {
      const Format_descr<cdk::TYPE_STRING> &fd = m_impl->get<cdk::TYPE_STRING>();
      if (fd.m_format.is_enum())
        return Type::ENUM;
      if (fd.m_format.is_set())
        return Type::SET;
      return Type::STRING;
    }

    case cdk::TYPE_INTEGER:
      /*
        TODO: Report more precise DevAPI type (TINYINT etc) based
        on CDK type and encoding format information.
      */
      return Type::INT;

    case cdk::TYPE_FLOAT:
    {
      const Format_descr<cdk::TYPE_FLOAT> &fd = m_impl->get<cdk::TYPE_FLOAT>();
      switch (fd.m_format.type())
      {
      case cdk::Format<cdk::TYPE_FLOAT>::DOUBLE:  return Type::DOUBLE;
      case cdk::Format<cdk::TYPE_FLOAT>::FLOAT:   return Type::FLOAT;
      case cdk::Format<cdk::TYPE_FLOAT>::DECIMAL: return Type::DECIMAL;
      }
    }

    case cdk::TYPE_DATETIME:
    {
      const Format_descr<cdk::TYPE_DATETIME> &fd = m_impl->get<cdk::TYPE_DATETIME>();
      switch (fd.m_format.type())
      {
      case cdk::Format<cdk::TYPE_DATETIME>::TIME:
        return Type::TIME;
      case cdk::Format<cdk::TYPE_DATETIME>::TIMESTAMP:
        return Type::TIMESTAMP;
      case cdk::Format<cdk::TYPE_DATETIME>::DATETIME:
        return fd.m_format.has_time() ? Type::DATETIME : Type::DATE;
      }
    }

    case cdk::TYPE_GEOMETRY:
      return Type::GEOMETRY;

    case cdk::TYPE_XML:
    default: return Type::BYTES;
    }
  }
  CATCH_AND_WRAP
}


bool Column::isNumberSigned() const
{
  assert(m_impl);

  try {
    if (cdk::TYPE_INTEGER != m_impl->m_type)
      return false;

    const Format_descr<cdk::TYPE_INTEGER> &fd = m_impl->get<cdk::TYPE_INTEGER>();
    return !fd.m_format.is_unsigned();
  }
  CATCH_AND_WRAP
}

bool Column::isPadded() const
{
  // TODO (see pad_with format info)
  return false;
}


/*
  Handling character set and collation information
  -----------------------------------------------

  This information is obtained from format descriptor for columns of CDK
  STRING type. Format descriptor gives the MySQL collation id as given by
  the server. Function collation_from_charset_id() returns CollationInfo
  constant corresponding to given collation id. This CollationInfo instance
  can be then used to get collation name and the corresponding charcater
  set.
*/

#define CS_SWITCH(CS)  COLLATIONS_##CS(COLL_SWITCH)

#define COLL_SWITCH(CS,ID,COLL,CASE) \
  case ID: return Collation<CharacterSet::CS>::COLL_CONST_NAME(COLL,CASE);

const CollationInfo& collation_from_id(cdk::collation_id_t id)
{
  switch (id)
  {
    CDK_CS_LIST(CS_SWITCH)
  default:
    THROW("Unknown collation id");
  }
}


const CollationInfo& Column::getCollation() const
{
  try {
    assert(m_impl);

    switch (m_impl->m_type)
    {
    case cdk::TYPE_BYTES:
      return Collation<CharacterSet::binary>::bin;

    case cdk::TYPE_DOCUMENT:
      return Collation<CharacterSet::utf8>::general_ci;

    case cdk::TYPE_STRING:
    {
      return collation_from_id(m_impl->m_collation);
    }

    case cdk::TYPE_INTEGER:
    case cdk::TYPE_FLOAT:
    case cdk::TYPE_DATETIME:
    default:
      THROW("No collation info for the type");
    }
  }
  CATCH_AND_WRAP
}

CharacterSet Column::getCharacterSet() const
{
  // TODO: Better use cdk encoding format information
  //const Format_descr<cdk::TYPE_STRING> &fd = m_impl->get<cdk::TYPE_STRING>();
  return getCollation().getCharacterSet();
}


/*
  Definitions of the CollationInfo constants describing all known collations
  as defined in mysqlx/collations.h.
*/

struct CollationInfo::Access
{
  enum coll_case {
    case_ci = CollationInfo::case_ci,
    case_cs = CollationInfo::case_cs,
    case_bin = CollationInfo::case_bin
  };

  static CollationInfo mk(CharacterSet _cs, unsigned _id, coll_case _case, const char *_name)
  {
    CollationInfo ci;
    ci.m_cs = _cs;
    ci.m_id = _id;
    ci.m_case = CollationInfo::coll_case(_case);
    ci.m_name = _name;
    return std::move(ci);
  }
};

#define COLL_DEFS(CS)  COLLATIONS_##CS(COLL_CONST_DEF)

#define COLL_CONST_DEF(CS,ID,COLL,CASE) \
const CollationInfo \
Collation<CharacterSet::CS>::COLL_CONST_NAME(COLL,CASE) = \
  CollationInfo::Access::mk(CharacterSet::CS, ID, \
    CollationInfo::Access::case_##CASE, \
    COLL_NAME_##CASE(CS,COLL));

#define COLL_NAME_bin(CS,COLL) #CS "_bin"
#define COLL_NAME_ci(CS,COLL)  #CS "_" #COLL "_ci"
#define COLL_NAME_cs(CS,COLL)  #CS "_" #COLL "_cs"

CDK_CS_LIST(COLL_DEFS)


/*
  Handling result data
  ====================
*/


/*
  Implementation of Row class
  ---------------------------
*/

// Convert raw bytes to Value using given encoding format description.

template<cdk::Type_info T>
static const Value convert(cdk::bytes, Format_descr<T>&);


/*
  Implementation for a single Row instance. It holds a copy of row
  raw data and a shared pointer to row set meta-data.

  Using meta-data information, it can decode raw bytes of each
  field into appropriate Value.
*/

class internal::Row_detail::Impl
{
public:

  Impl() {}
  Impl(const Row_data&, const std::shared_ptr<Meta_data>&);

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

  /*
    Get value of field at given position after converting to Value.
    @throws std::out_of_range if given column does not exist in the row.
  */

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


  friend Row;
  friend Row_detail;
  friend RowResult;
  friend SqlResult;
};


// Note: row data is copied

internal::Row_detail::Impl::Impl(
  const Row_data &data,
  const std::shared_ptr<Meta_data> &mdata
)
  : m_data(data), m_mdata(mdata)
{}


internal::Row_detail::Impl& internal::Row_detail::get_impl()
{
  if (!m_impl)
    THROW("Attempt to use null Row instance");
  return *m_impl;
}

void internal::Row_detail::ensure_impl()
{
  if (!m_impl)
    m_impl = std::make_shared<Impl>();
}


col_count_t Row::colCount() const
{
  try {
    const Impl &impl = get_impl();
    col_count_t cnt = (impl.m_mdata ? impl.m_mdata->col_count() : 0);
    return impl.m_col_count > cnt ? impl.m_col_count : cnt;
  }
  CATCH_AND_WRAP
}


bytes Row::getBytes(col_count_t pos) const
{
  try {
    return get_impl().get_bytes(pos);
  }
  catch (const std::out_of_range&)
  {
    /*
      Note: we want to throw out_of_range error as is, without wrapping
      it in mysqlx::Error
    */
    throw;
  }
  catch (...)
  {
    try { throw; }
    CATCH_AND_WRAP
  }
}


Value& Row::get(mysqlx::col_count_t pos)
{
  try {

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
      bytes data = getBytes(pos);

      switch (impl.m_mdata->get_type(pos))
      {
      case cdk::TYPE_STRING:    return impl.get<cdk::TYPE_STRING>(pos);
      case cdk::TYPE_INTEGER:   return impl.get<cdk::TYPE_INTEGER>(pos);
      case cdk::TYPE_FLOAT:     return impl.get<cdk::TYPE_FLOAT>(pos);
      case cdk::TYPE_DOCUMENT:  return impl.get<cdk::TYPE_DOCUMENT>(pos);

        /*
          TODO: Other "natural" conversions
          TODO: User-defined conversions (also to user-defined types)
        */

      case cdk::TYPE_BYTES:

        /*
          Note: in case of raw bytes, we trim the extra 0x00 byte added
          at the end by the protocol (to handle NULL values).
        */

        return set(pos, bytes(data.begin(), data.end() - 1));

      default:

        /*
          For all types for which we do not have a natural conversion
          to C++ type, we return raw bytes representing the value as
          returned by protocol.
        */

        return set(pos, data);
      }
    }
    catch (std::out_of_range&)
    {
      // set to NULL
      return set(pos, Value());
    }

  }
  catch (const std::out_of_range&)
  {
    throw;
  }
  catch (...)
  {
    try { throw; }
    CATCH_AND_WRAP
  }
}


void internal::Row_detail::process_one(
  std::pair<Impl*,col_count_t> *data, const Value &val
)
{
  Impl *impl = data->first;
  col_count_t pos = (data->second)++;

  impl->m_vals.emplace(pos, val);

  if (pos + 1 > impl->m_col_count)
    impl->m_col_count = pos + 1;
}


void Row::clear()
{
  try { m_impl.reset(); }
  CATCH_AND_WRAP
}


/*
  Conversions of raw value representation to Value objects.
*/

template<>
const Value
convert(
  cdk::bytes data,
  Format_descr<cdk::TYPE_STRING> &fd)
{
  /*
    String encoding has artificial 0x00 byte appended at the end to
    distinguish the empty string from the null value. We skip
    the trailing 0x00 byte to get just the raw bytes that encode the string.
  */

  cdk::bytes raw(data.begin(), data.end() - 1);

  // If this string value is in fact a SET, then return it as raw bytes.

  if (fd.m_format.is_set())
    return Value(bytes(raw.begin(), raw.end()));

  auto &codec = fd.m_codec;
  cdk::string str;
  codec.from_bytes(raw, str);
  return Value(std::move(str));
}


template<>
const Value
convert(
  cdk::bytes data,
  Format_descr<cdk::TYPE_INTEGER> &fd)
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
convert(
  cdk::bytes data,
  Format_descr<cdk::TYPE_FLOAT> &fd)
{
  auto &fmt = fd.m_format;

  // Note: DECIMAL format not yet supported by CDK

  if (fmt.FLOAT == fmt.type())
  {
    float val;
    fd.m_codec.from_bytes(data, val);
    return Value(val);
  }

  if (fmt.DOUBLE == fmt.type())
  {
    double val;
    fd.m_codec.from_bytes(data, val);
    return Value(val);
  }

  return bytes(data.begin(), data.end());
}


template<>
const Value
convert(
  cdk::bytes data,
  Format_descr<cdk::TYPE_DOCUMENT>&)
{
  /*
    Note: this assumes that document is represented as json string
    - thanks to this we can take benefit of lazy parsing.

    Otherwise, implementation that would not assume what underlying
    representation is used for documnets should use a Codec to decode
    the raw bytes and build a representation of the documnent to be
    stored in the Value instance.
  */

  // trim initial space

  unsigned i;
  for (i = 0; i < data.size() && std::isspace(*(data.begin() + i)); ++i);

  std::string json(data.begin() + i, data.end());

  if ('{' == *(data.begin() + i))
    return Value::Access::mk_doc(json);

  return Value::Access::mk_from_json(json);
}


/*
  Result implementation
  =====================
*/


void internal::Result_detail::Impl::init()
{
  /*
    Note: This method is called also when moving to a next rset in a multi-result
    reply. In that case we delete the cursor first to discard any pending data
    from the previous rset.
  */

  delete m_cursor;
  m_cursor = nullptr;

  /*
    Registering this result with session will de-register currently registered
    result (if any) and give it a chance to cache pending data.
  */

  assert(m_sess);
  m_sess->register_result(this);

  clear_cache();

  if (!m_reply)
    return;

  m_reply->wait();

  if (m_reply->entry_count() > 0)
    return;

  if (m_reply->has_results())
  {
    m_cursor_closed = false;
    m_cursor = new cdk::Cursor(*m_reply);
    m_cursor->wait();
    // copy meta-data information from cursor
    m_mdata = std::make_shared<Meta_data>(*m_cursor);
  }
}


internal::Result_detail::Impl::~Impl()
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


void internal::Result_detail::Impl::deregister()
{
  // cache remaining rows
  // TODO: handle multi rsets...
  count();
}


const Row_data* internal::Result_detail::Impl::get_row()
{
  if (m_cache)
  {
    if (m_row_cache_size == 0)
      return nullptr;

    m_row = m_row_cache.front();
    m_row_cache.pop_front();
    m_row_cache_size--;
    return &m_row;
  }

  if (!m_cursor)
    THROW("Attempt to read row from empty result");

  if (m_cursor_closed)
    return NULL;

  /*
    TODO: Row cache for better I/O performance (read several rows at once)
  */
  if (m_cursor->get_row(*this))
    return &m_row;

  /*
    Cleanup after reading all rows. Note
  */

  m_cursor->close();
  m_cursor_closed = true;

  return NULL;
}


auto internal::Result_detail::Impl::count() -> row_count_t
{
  // If cursor is NULL then this is result without any data

  if (!m_cursor)
    return 0;

  if (!m_cache)
  {
    auto it = m_row_cache.before_begin();

    for(const Row_data *row = get_row(); row != nullptr; row = get_row())
    {
      ++m_row_cache_size;
      it = m_row_cache.insert_after(it, *row);
    }
  }

  m_cache = true;
  return m_row_cache_size;
}


size_t internal::Result_detail::Impl::field_begin(col_count_t pos, size_t size)
{
  m_row.insert(std::pair<col_count_t, Buffer>(pos, Buffer()));
  // FIX
  return size;
}

size_t internal::Result_detail::Impl::field_data(col_count_t pos, bytes data)
{
  m_row[(unsigned)pos].append(mysqlx::bytes::Access::mk(data));
  // FIX
  return data.size();
}


struct Warning::Access
{
  static Warning mk(Level level, uint16_t code, const string &msg)
  {
    return Warning(level, code, msg);
  }
};


void internal::Result_detail::Impl::load_warnings()
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

  if (m_all_warnings)
    return;

  if (!m_reply->has_results())
    m_all_warnings = true;

  m_warnings.clear();

  auto &it = m_reply->get_entries(cdk::api::Severity::WARNING);

  while (it.next())
  {
    auto &entry = it.entry();
    uint16_t code = 0;
    Warning::Level level = Warning::LEVEL_INFO;

    switch (entry.severity())
    {
    case cdk::api::Severity::ERROR: level = Warning::LEVEL_ERROR; break;
    case cdk::api::Severity::WARNING: level = Warning::LEVEL_WARNING; break;
    default: break;
    }

    if (entry.code().category() == cdk::server_error_category())
    {
      int c = entry.code().value();
      assert(c >= 0 && (unsigned)c < std::numeric_limits<uint16_t>::max());
      code = (uint16_t)c;
    }

    m_warnings.emplace_back(
      Warning::Access::mk(
        level, code,
        entry.get_error().description()
        )
      );
  }
}


/*
  Result_detail
  =============
*/


internal::Result_detail::Result_detail(
  mysqlx::Session *sess,
  cdk::Reply *r)
{
  assert(sess);
  assert(sess->m_impl);

  m_owns_impl = true;
  m_impl= new Impl(sess->m_impl, r);
}

internal::Result_detail::Result_detail(
  mysqlx::Session *sess,
  cdk::Reply *r,
  const std::vector<GUID> &guids)
{
  assert(sess);
  assert(sess->m_impl);

  m_owns_impl = true;
  m_impl= new Impl(sess->m_impl, r, guids);
}


internal::Result_detail::~Result_detail()
{
  try {
    if (m_owns_impl)
      delete m_impl;
  }
  catch(...) {}
}


void mysqlx::internal::Result_detail::init(mysqlx::internal::Result_detail &&init_)
{
  if (m_impl && m_owns_impl)
    delete m_impl;

  m_impl = init_.m_impl;

  if (!init_.m_owns_impl)
    m_owns_impl = false;
  else
  {
    m_owns_impl = true;
    init_.m_owns_impl = false;
  }
}


auto internal::Result_detail::get_impl() -> Impl&
{
  if (!m_impl)
    THROW("Invalid result set");
  return *m_impl;
}


void internal::Result_detail::check_result() const
{
  if (!get_impl().m_cursor)
    THROW("No result set");
}


void internal::Result_detail::iterator_start()
{
  m_wpos = 0;
  m_at_begin = true;
}

bool internal::Result_detail::iterator_next()
{
  if (!m_at_begin)
    m_wpos++;
  m_at_begin = false;
  return m_wpos < get_impl().get_warning_count();
}

Warning internal::Result_detail::iterator_get()
{
  return get_impl().get_warning(m_wpos);
}


unsigned
internal::Result_detail::get_warning_count() const
{
  return get_impl().get_warning_count();
}

Warning internal::Result_detail::get_warning(unsigned pos)
{
  get_impl().load_warnings();
  return get_impl().get_warning(pos);
}


internal::List_initializer<internal::Result_detail>
internal::Result_detail::get_warnings()
{
  get_impl().load_warnings();
  return List_initializer<Result_detail>(*this);
};



/*
  Result
  ======
*/


uint64_t Result::getAffectedItemsCount() const
{
  try {
    return get_impl().get_affected_rows();
  } CATCH_AND_WRAP
}


uint64_t Result::getAutoIncrementValue() const
{
  try {
    return get_impl().get_auto_increment();
  } CATCH_AND_WRAP
}


const GUID& Result::getDocumentId() const
{
  if (get_impl().m_guid.size() == 0)
    throw Error("Can only be used on add operations.");
  if (get_impl().m_guid.size() > 1)
    throw Error("Multiple documents added... should use getDocumentIds()");

  return get_impl().m_guid.front();
}


internal::List_init<GUID> Result::getDocumentIds() const
{
  if (get_impl().m_guid.size() == 0)
    throw Error("Can only be used on add operations.");

  auto &guid = get_impl().m_guid;
  return std::forward_list<GUID>(guid.begin(), guid.end());
}


/*
  RowResult
  =========
*/


void internal::Row_result_detail::Columns_src::iterator_start()
{
  m_pos = 0;
  m_at_begin = true;
}

bool internal::Row_result_detail::Columns_src::iterator_next()
{
  if (!m_at_begin)
    m_pos++;
  m_at_begin = false;
  return m_pos < m_res_impl.get_col_count();
}

auto internal::Row_result_detail::Columns_src::iterator_get()
-> Column_detail
{
  return m_res_impl.get_column(m_pos);
}


auto internal::Row_result_detail::Columns::operator[](col_count_t pos) const
-> Column_detail
{
  return m_src.m_res_impl.get_column(pos);
}


Row internal::Row_result_detail::get_row()
{
  Impl &impl = get_impl();

  const Row_data *row = impl.get_row();

  if (!row)
    return Row();

  return internal::Row_detail(
    std::make_shared<internal::Row_detail::Impl>(*row, impl.m_mdata)
  );
}


col_count_t RowResult::getColumnCount() const
{
  return get_impl().get_col_count();
}


auto RowResult::getColumn(col_count_t pos) const
-> Column
{
  return internal::Column_detail(get_impl().get_column(pos));
}


uint64_t RowResult::count()
{
  return get_impl().count();
}

/*
  SqlResult
  =========
*/


bool SqlResult::hasData() const
{
  try {
    return get_impl().has_data();
  }
  CATCH_AND_WRAP
}


bool SqlResult::nextResult()
{
  try {
    return get_impl().next_result();
  }
  CATCH_AND_WRAP
}


uint64_t mysqlx::SqlResult::getAffectedRowsCount()
{
  try {
    return get_impl().get_affected_rows();
  }
  CATCH_AND_WRAP
}


uint64_t mysqlx::SqlResult::getAutoIncrementValue()
{
  try {
    return get_impl().get_auto_increment();
  }
  CATCH_AND_WRAP
}


/*
  DocResult
  =========
*/


DbDoc internal::Doc_result_detail::get_doc()
{
  const Row_data *row = get_impl().get_row();

  if (!row)
    return DbDoc();

  // @todo Avoid copying of document string.
  cdk::foundation::bytes data = row->at(0).data();
  return DbDoc(std::string(data.begin(),data.end()-1));
}


uint64_t internal::Doc_result_detail::count()
{
  return get_impl().count();
}

