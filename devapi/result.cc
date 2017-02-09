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
  Handling column meta-data information
  =====================================

  Meta-data for result columns is provided by CDK cursor object which implements
  cdk::Meta_data interface. This information is read from cursor
  in BaseResult::Impl::init() method and is stored in m_mdata member of type
  Meta_data.

  Meta_data class contains a map from column positions to instances of Column
  class. Each Column instance can store meta-data information for a single
  column. The Column instances are created in the Meta_data constructor which
  reads meta-data information from cdk::Meta_data interface and adds Column
  objects using add() methods.

  The CDK meta-data for a single column consists of:

  - CDK type constant (cdk::Type_info)
  - encoding format information (cdk::Format_info)
  - additional column information (cdk::Column_info)

  The first two items describe the type and encoding of the values that appear
  in the result in the corresponding column. Additional column information
  is mainly the column name etc.

  Classes Format_descr<TTT> and Format_info are used to store type and encoding
  format information for each column. For CDK type TTT, class Format_descr<TTT>
  stores encoding format descriptor (instance of cdk::Format<TTT> class) and
  encoder/decoder for the values (instance of cdk::Codec<TTT> class)
  (Note: for some types TTT there is no codec or no format descriptor). Class
  Format_info is a variant class that can store Format_descr<TTT> values
  for different TTT.

  Class Column::Impl extends Format_info with additional storage for
  the cdk::Column_info data (column name etc). Class Column uses information
  stored in Column::Impl to implement the DevAPI Column interface.

  Using meta-data to decode result values
  ---------------------------------------

  Meta-data information stored in m_mdata member of RowResult::Impl class
  is used to interpret raw bytes returned by CDK in the reply to a query.
  This interpretation is done by Row::Impl class whose instance is created,
  for example, in RowResult::fetchOne() passing the metad-data information
  to Row::Impl constructor. This meta-data is used in Row::get() method, which
  first checks the type of the value and then uses get<TTT>() method which
  calls appropriate convert() method after extracting encoding format
  information from the meta-data.

  Presenting meta-data via DevAPI Column interface
  ------------------------------------------------

  This is done by the Column class, using column meta-data stored in
  the Column::Impl instance. The type and encoding format information must
  be translated to types defined by DevAPI. This translation happens
  in Column::getType() method. For example, a CDK column of type FLOAT can
  be reported as DevAPI type FLOAT, DOUBLE or DECIMAL, depending on the
  encoding format that was reported by CDK. Additional encoding information
  is exposed via other DevAPI Column methods such as isNumberSigned().
  The information from cdk::Column_info interface is extracted and stored
  in Column::Impl class by store_info() method. Then it is exposed by relevant
  DevAPI Column methods.
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
  Phony Format_descr<> structure used for raw bytes and values
  of types which we do not process in any way (but present as
  raw bytes).
*/

template <>
struct Format_descr<cdk::TYPE_BYTES>
{};

/*
  Note: we do not decode temporal values yet, thus there is
  no codec in Format_descr class.
*/

template<>
struct Format_descr<cdk::TYPE_DATETIME>
{
  cdk::Format<cdk::TYPE_DATETIME> m_format;

  Format_descr(const cdk::Format_info &fi)
    : m_format(fi)
  {}
};


/*
  Note: For GEOMETRY and XML types we do not decode the values.
  Also, CDK does not provide any encoding format information -
  GEOMETRY uses some unspecified MySQL intenral representation
  format and XML format is well known.
*/

template<>
struct Format_descr<cdk::TYPE_GEOMETRY>
{
  Format_descr(const cdk::Format_info &)
  {}
};

template<>
struct Format_descr<cdk::TYPE_XML>
{
  Format_descr(const cdk::Format_info &)
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
  Format_descr<cdk::TYPE_DOCUMENT>,
  Format_descr<cdk::TYPE_BYTES>,
  Format_descr<cdk::TYPE_DATETIME>,
  Format_descr<cdk::TYPE_GEOMETRY>,
  Format_descr<cdk::TYPE_XML>
> Format_info_base;

struct Format_info
  : public Format_info_base
{
  cdk::Type_info m_type;

  template <cdk::Type_info T>
  Format_info(const Format_descr<T> &fd)
    : Format_info_base(fd), m_type(T)
  {}

  Format_info(cdk::Type_info type)
    : Format_info_base(Format_descr<cdk::TYPE_BYTES>())
    , m_type(type)
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
  Helper class to construct Column instances and access
  non-public members.
*/

struct Column::Access
{
  static const Format_info& get_format(const Column&);

  template <cdk::Type_info T>
  static Column mk(const cdk::Column_info&, const Format_descr<T>&);

  static Column mk_raw(const cdk::Column_info&, cdk::Type_info);
};


/*
  Meta_data holds type and format information for all columns in
  a result. An instance is filled given information provided by
  cdk::Meta_data interface.
*/

struct Meta_data
  : private std::map<cdk::col_count_t, Column>
{
  Meta_data(cdk::Meta_data&);

  col_count_t col_count() const { return m_col_count;  }

  const Format_info& get_format(cdk::col_count_t pos) const
  {
    return Column::Access::get_format(get_column(pos));
  }

  cdk::Type_info get_type(cdk::col_count_t pos) const
  {
    return get_format(pos).m_type;
  }

  const Column& get_column(cdk::col_count_t pos) const
  {
    return at(pos);
  }

private:

  cdk::col_count_t  m_col_count;


  /*
    Add to this Meta_data instance information about column
    at position `pos`. The type and format information is given
    by cdk::Format_info object, aaditional column meta-data by
    cdk::Column_info object.
  */
  template<cdk::Type_info T>
  void add(cdk::col_count_t pos,
           const cdk::Column_info &ci, const cdk::Format_info &fi)
  {
    emplace(pos, Column::Access::mk<T>(ci, fi));
  }


  /*
    Add raw column information (whose values are presented as
    raw bytes).
  */

  void add_raw(cdk::col_count_t pos,
               const cdk::Column_info &ci, cdk::Type_info type)
  {
    emplace(pos, Column::Access::mk_raw(ci, type));
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

class Column::Impl : public Format_info
{
public:

  string m_name;
  string m_label;
  string m_table_name;
  string m_table_label;
  string m_schema_name;

  unsigned long m_length;
  unsigned short m_decimals;
  cdk::collation_id_t m_collation;

  template <typename T>
  Impl(const T &init) : Format_info(init)
  {}

  void store_info(const cdk::Column_info &ci)
  {
    m_name = ci.orig_name();
    m_label = ci.name();

    if (ci.table())
    {
      m_table_name = ci.table()->orig_name();
      m_table_label = ci.table()->name();

      if (ci.table()->schema())
        m_schema_name = ci.table()->schema()->name();
    }

    m_collation = ci.collation();
    m_length = ci.length();
    assert(ci.decimals() < std::numeric_limits<short unsigned>::max());
    m_decimals = static_cast<short unsigned>(ci.decimals());
  }

};


template <cdk::Type_info T>
Column Column::Access::mk(const cdk::Column_info &ci, const Format_descr<T> &fd)
{
  Column col;
  col.m_impl = std::make_shared<Column::Impl>(fd);
  col.m_impl->store_info(ci);
  return std::move(col);
}

Column Column::Access::mk_raw(const cdk::Column_info &ci, cdk::Type_info type)
{
  Column col;
  col.m_impl = std::make_shared<Column::Impl>(type);
  col.m_impl->store_info(ci);
  return std::move(col);
}

const Format_info& Column::Access::get_format(const Column &c)
{
  return *c.m_impl.get();
}


void Column::print(std::ostream &out) const
{
  if (!m_impl->m_schema_name.empty())
    out << "`" << m_impl->m_schema_name << "`.";
  string table_name = getTableLabel();
  if (!table_name.empty())
    out << "`" << table_name << "`.";
  out << "`" << getColumnLabel() <<"`";
}


/*
  Implementation of DevAPI Column interface.
*/


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


  // Convert raw bytes to Value using given encoding format description.

  template<cdk::Type_info T>
  const Value convert(cdk::bytes, Format_descr<T>&) const;


  friend Row;
  friend RowResult;
  friend SqlResult;
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
    throw;
  }
  CATCH_AND_WRAP
}


Value& Row::get(mysqlx::col_count_t pos)
{
  if (!m_impl)
    throw out_of_range("Accesing field of a null Row instance");

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
  CATCH_AND_WRAP
}


Value& Row::set(col_count_t pos, const Value &val)
{
  try {
    if (!m_impl)
      m_impl = std::make_shared<Impl>();

    Impl &impl = get_impl();

    impl.m_vals.emplace(pos, val);

    if (pos + 1 > impl.m_col_count)
      impl.m_col_count = pos + 1;

    return impl.m_vals.at(pos);
  }
  CATCH_AND_WRAP
}


void Row::clear()
{
  try { m_impl.reset(); }
  CATCH_AND_WRAP
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

  // trim initial space

  unsigned i;
  for (i = 0; i < data.size() && std::isspace(*(data.begin() + i)); ++i);

  std::string json(data.begin() + i, data.end());

  if ('{' == *(data.begin() + i))
    return Value::Access::mk_doc(json);

  return Value::Access::mk_from_json(json);
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
  std::vector<GUID>           m_guid;
  bool                        m_cursor_closed = false;

  Impl(cdk::Reply *r)
    : m_reply(r)
  {
    init();
  }

  Impl(cdk::Reply *r, const std::vector<GUID> &guids)
    : m_reply(r), m_guid(guids)
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
      delete m_cursor;
      m_cursor_closed = false;
      m_cursor = new cdk::Cursor(*m_reply);
      m_cursor->wait();
      // copy meta-data information from cursor
      m_mdata = std::make_shared<Meta_data>(*m_cursor);
    }
  }

  bool next_result()
  {
    /*
      Note: closing cursor discards previous rset. Only then
      we can move to the next rset (if any).
    */

    if (m_cursor)
      m_cursor->close();

    if (!m_reply || !m_reply->has_results())
      return false;

    init();
    return true;
  }

  virtual ~Impl()
  {
    // Note: Cursor must be deleted before reply.
    delete m_cursor;
    delete m_reply;
  }

  /*
    Discard the CDK reply object owned by the implementation. This
    is called when the corresponding session is about to be closed
    and the reply object will be no longer valid.
  */

  void discard_reply()
  {
    if (!m_reply)
      return;

    delete m_reply;
    m_reply = NULL;
  }

  /*
    Read next row from the cursor. Returns NULL if there are no
    more rows. Throws exeption if this result has no data.
  */

  const Row_data *get_row();


  cdk::row_count_t get_affected_rows() const
  {
    if (!m_reply)
      THROW("Attempt to get affected rows count on empty result");
    return m_reply->affected_rows();
  }

  cdk::row_count_t get_auto_increment() const
  {
    if (!m_reply)
      THROW("Attempt to get auto increment value on empty result");
    return m_reply->last_insert_id();
  }

  unsigned get_warning_count() const
  {
    if (!m_reply)
      THROW("Attempt to get warning count for empty result");
    const_cast<Impl*>(this)->load_warnings();
    return m_reply->entry_count(cdk::api::Severity::WARNING);
  }

  std::vector<Warning> m_warnings;
  bool m_all_warnings = false;

  Warning get_warning(unsigned pos) const
  {
    return m_warnings.at(pos);
  }

  void load_warnings();


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

  friend internal::BaseResult;
  friend mysqlx::Result;
  friend mysqlx::RowResult;
  friend mysqlx::SqlResult;
};


const Row_data* Result::Impl::get_row()
{
  if (!m_cursor)
    THROW("Attempt to read row from empty result");

  if (m_cursor_closed)
    return NULL;

  /*
    TODO: Row cache for better I/O performance (read several rows at once)
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


struct Warning::Access
{
  static Warning mk(Level level, uint16_t code, const string &msg)
  {
    return Warning(level, code, msg);
  }
};

void Result::Impl::load_warnings()
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
    Warning::Level level = Warning::INFO;

    switch (entry.severity())
    {
    case cdk::api::Severity::ERROR: level = Warning::ERROR; break;
    case cdk::api::Severity::WARNING: level = Warning::WARNING; break;
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
  BaseResult
  ==========
*/


internal::BaseResult::BaseResult(XSession_base *sess,
                                 cdk::Reply *r)
{
  try {
    m_owns_impl = true;
    m_impl= new Impl(r);
    m_sess = sess;
    m_sess->register_result(this);
  }
  CATCH_AND_WRAP
}

internal::BaseResult::BaseResult(XSession_base *sess,
                                 cdk::Reply *r,
                                 const std::vector<GUID> &guids)
{
  try {
    m_owns_impl = true;
    m_impl= new Impl(r,guids);
    m_sess = sess;
    m_sess->register_result(this);
  }
  CATCH_AND_WRAP
}


internal::BaseResult::~BaseResult()
{
  try {
    if (m_sess && m_sess->m_impl)
      m_sess->deregister_result(this);
  }
  catch (...) {}

  try {
    if (m_owns_impl)
      delete m_impl;
  }
  catch(...) {}
}


void mysqlx::internal::BaseResult::init(mysqlx::internal::BaseResult &&init_)
{
  if (m_impl && m_owns_impl)
    delete m_impl;

  m_pos = 0;
  m_impl = init_.m_impl;
  if (!init_.m_owns_impl)
    m_owns_impl = false;
  else
  {
    m_owns_impl = true;
    init_.m_owns_impl = false;
  }

  m_sess = init_.m_sess;

  //On empty results, m_sess is NULL, so don't do anything with it!
  if (m_sess)
  {
    // first deregister init result, since it registered itself on ctor
    // otherwise it would trigger cache, and we are moving Result object
    m_sess->deregister_result(&init_);
    m_sess->register_result(this);
  }

}


const internal::BaseResult::Impl&
internal::BaseResult::get_impl() const
{
  try {
    if (!m_impl)
      // TODO: Better error
      throw Error("Attempt to use null result instance");
    return *m_impl;
  }
  CATCH_AND_WRAP
}


/*
  This method is called when the result object is deregistered from
  the session (so that it is no longer the active result of that
  session).

  We do cleanups here to make the result object independent from the
  session. Derived classes should cache pending results so that they
  can be accessed without the session.
*/

void internal::BaseResult::deregister_notify()
{
  assert(m_impl);

  // Let derived object do its own cleanup
  deregister_cleanup();

  // Discard CDK reply object which is about to be invalidated.
  m_impl->discard_reply();

  m_sess = NULL;
}


unsigned
internal::BaseResult::getWarningCount() const
{
  return get_impl().get_warning_count();
}

Warning internal::BaseResult::getWarning(unsigned pos)
{
  get_impl().load_warnings();
  return get_impl().get_warning(pos);
}

internal::List_initializer<internal::BaseResult>
internal::BaseResult::getWarnings()
{
  get_impl().load_warnings();
  return List_initializer<BaseResult>(*this);
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

Row RowResult::fetchOne()
{
  if (m_cache)
  {
    if (m_row_cache_size == 0)
      return Row();

    Row r = std::move(m_row_cache.front());
    m_row_cache.pop_front();
    m_row_cache_size--;
    return r;
  }
  try {
    Impl &impl = get_impl();
    const Row_data *row = impl.get_row();

    if (!row)
      return Row();

    return Row(std::make_shared<Row::Impl>(*row, impl.m_mdata));
  }
  CATCH_AND_WRAP
}

uint64_t RowResult::count()
{
  if (!m_cache)
    try {

    m_cache = true;
    Impl &impl = get_impl();

    auto it = m_row_cache.before_begin();

    for(const Row_data *row = impl.get_row();
        row != nullptr;
        row = impl.get_row())
    {
      ++m_row_cache_size;
      it = m_row_cache.insert_after(it,
                                    Row(std::make_shared<Row::Impl>(*row,
                                                                    impl.m_mdata)
                                        )
                                    );
    }
  }
  CATCH_AND_WRAP

  return m_row_cache_size;
}


void RowResult::check_result() const
{
  if (!get_impl().m_cursor)
    THROW("No result set");
}


col_count_t RowResult::getColumnCount() const
{
  try {
    check_result();
    return m_impl->m_cursor->col_count();
  }
  CATCH_AND_WRAP
}


const Column& RowResult::getColumn(col_count_t pos) const
{
  try {
    check_result();
    return m_impl->m_mdata->get_column(pos);
  }
  CATCH_AND_WRAP
}


bool mysqlx::SqlResult::hasData() const
{
  try {
    return NULL != get_impl().m_cursor;
  }
  CATCH_AND_WRAP
}

bool mysqlx::SqlResult::nextResult()
{
  try {
    if (get_impl().next_result())
    {
      clear_cache();
      return true;
    };
    return false;
  }
  CATCH_AND_WRAP
}


/*
  DocResult
  =========
*/

DocResult::DocResult(internal::BaseResult &&other)
{
  m_doc_impl = new Impl(RowResult(std::move(other)));
}


void DocResult::operator=(DocResult &&other)
{
  try {
    delete m_doc_impl;
    m_doc_impl = other.m_doc_impl;
    other.m_doc_impl = NULL;
  }
  CATCH_AND_WRAP
}


DocResult::~DocResult()
{
  delete m_doc_impl;
}


void DocResult::check_result() const
{
  if (!m_doc_impl)
    // TODO: Better error
    throw Error("Attempt to use null result instance");
  m_doc_impl->check_result();
}


DbDoc DocResult::fetchOne()
{
  try {
    check_result();
    return m_doc_impl->get_next_doc();
  }
  CATCH_AND_WRAP
}

uint64_t DocResult::count()
{
  return m_doc_impl->count_docs();
}

