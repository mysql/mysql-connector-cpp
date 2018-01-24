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

#include <mysql/cdk.h>
#include <mysqlx/xdevapi.h>

#include "impl.h"

#include <vector>
#include <sstream>
#include <iomanip>
#include <cctype>


/*
  Implementation of Result and Row interfaces.
*/

using namespace ::mysqlx;
using common::GUID;
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
  Column implementation.
*/


void internal::Column_detail::print(std::ostream &out) const
{
  if (!get_impl().m_schema_name.empty())
    out << "`" << get_impl().m_schema_name << "`.";
  string table_name = get_table_label();
  if (!table_name.empty())
    out << "`" << table_name << "`.";
  out << "`" << get_label() <<"`";
}


/*
  Method getType() translates CDK type/format info into
  DevAPI type information.

  Note: Expected to return values of Type enum constants.
*/

Type get_api_type(cdk::Type_info, const common::Format_info &);

unsigned internal::Column_detail::get_type() const
{
  return unsigned(get_api_type(get_impl().m_type, get_impl()));
}


Type get_api_type(cdk::Type_info type, const common::Format_info &fmt)
{
  switch (type)
  {
  case cdk::TYPE_BYTES:
    return Type::BYTES;

  case cdk::TYPE_DOCUMENT:
    return Type::JSON;

  case cdk::TYPE_STRING:
  {
    const common::Format_descr<cdk::TYPE_STRING> &fd
      = fmt.get<cdk::TYPE_STRING>();
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
    const common::Format_descr<cdk::TYPE_FLOAT> &fd
      = fmt.get<cdk::TYPE_FLOAT>();
    switch (fd.m_format.type())
    {
    case cdk::Format<cdk::TYPE_FLOAT>::DOUBLE:  return Type::DOUBLE;
    case cdk::Format<cdk::TYPE_FLOAT>::FLOAT:   return Type::FLOAT;
    case cdk::Format<cdk::TYPE_FLOAT>::DECIMAL: return Type::DECIMAL;
    default:
      THROW("Unrecognized float value encoding format");
    }
  }

  case cdk::TYPE_DATETIME:
  {
    const common::Format_descr<cdk::TYPE_DATETIME> &fd
      = fmt.get<cdk::TYPE_DATETIME>();
    switch (fd.m_format.type())
    {
    case cdk::Format<cdk::TYPE_DATETIME>::TIME:
      return Type::TIME;
    case cdk::Format<cdk::TYPE_DATETIME>::TIMESTAMP:
      return Type::TIMESTAMP;
    case cdk::Format<cdk::TYPE_DATETIME>::DATETIME:
      return fd.m_format.has_time() ? Type::DATETIME : Type::DATE;
    default:
      THROW("Unrecognized temporal value encoding format");
    }
  }

  case cdk::TYPE_GEOMETRY:
    return Type::GEOMETRY;

  case cdk::TYPE_XML:
  default: return Type::BYTES;
  }
}


string internal::Column_detail::get_name() const
{
  return get_impl().m_name;
}

string internal::Column_detail::get_label() const
{
  return get_impl().m_label;
}

string internal::Column_detail::get_schema_name() const
{
  return get_impl().m_schema_name;
}

string internal::Column_detail::get_table_name() const
{
  return get_impl().m_table_name;
}

string internal::Column_detail::get_table_label() const
{
  return get_impl().m_table_label;
}

unsigned long internal::Column_detail::get_length() const
{
  return get_impl().m_length;
}

unsigned short internal::Column_detail::get_decimals() const
{
  return get_impl().m_decimals;
}


bool internal::Column_detail::is_signed() const
{
  if (cdk::TYPE_INTEGER != get_impl().m_type)
    return false;

  const common::Format_descr<cdk::TYPE_INTEGER> &fd
    = get_impl().get<cdk::TYPE_INTEGER>();
  return !fd.m_format.is_unsigned();
}

bool internal::Column_detail::is_padded() const
{
  return get_impl().m_padded;
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


const CollationInfo& internal::Column_detail::get_collation() const
{
  try {

    switch (get_impl().m_type)
    {
    case cdk::TYPE_BYTES:
      return Collation<CharacterSet::binary>::bin;

    case cdk::TYPE_DOCUMENT:
      return Collation<CharacterSet::utf8>::general_ci;

    case cdk::TYPE_STRING:
    {
      return collation_from_id(get_impl().m_collation);
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

CharacterSet internal::Column_detail::get_charset() const
{
  // TODO: Better use cdk encoding format information
  //const Format_descr<cdk::TYPE_STRING> &fd = m_impl->get<cdk::TYPE_STRING>();
  return get_collation().getCharacterSet();
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


/*
  Decoding document values from raw bytes representation.

  Note: Conversions for other value types are handled by common::Value
  class.
*/

Value
Value::Access::mk(
  cdk::bytes data, common::Format_descr<cdk::TYPE_DOCUMENT>&
)
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


col_count_t internal::Row_detail::col_count() const
{
  return get_impl().col_count();
}


bytes internal::Row_detail::get_bytes(col_count_t pos) const
{
  cdk::bytes data = get_impl().m_data.at(pos).data();
  return mysqlx::bytes::Access::mk(data);
}


mysqlx::Value& internal::Row_detail::get_val(mysqlx::col_count_t pos)
{
  return get_impl().get(pos);
}


void internal::Row_detail::process_one(
  std::pair<Impl*,col_count_t> *data, const Value &val
)
{
  Impl *impl = data->first;
  col_count_t pos = (data->second)++;
  impl->set(pos, val);
}


/*
  Result_detail
  =============
*/


struct internal::Result_detail::Impl
  : public common::Result_impl<string>
{
public:

  using common::Result_impl<string>::Result_impl;
};


internal::Result_detail::Result_detail(common::Result_init &init)
{
  m_owns_impl = true;
  m_impl = new Impl(init);
}


internal::Result_detail::~Result_detail()
{
  try {
    if (m_owns_impl)
      delete m_impl;
  }
  catch(...) {}
}


auto internal::Result_detail::operator=(internal::Result_detail &&other)
-> Result_detail&
{
  if (m_impl && m_owns_impl)
    delete m_impl;

  m_impl = other.m_impl;

  if (!other.m_owns_impl)
    m_owns_impl = false;
  else
  {
    m_owns_impl = true;
    other.m_owns_impl = false;
  }

  return *this;
}


auto internal::Result_detail::get_impl() -> Impl&
{
  if (!m_impl)
    THROW("Invalid result set");
  return *m_impl;
}


void internal::Result_detail::check_result() const
{
  if (!get_impl().has_data())
    THROW("No result set");
}


unsigned
internal::Result_detail::get_warning_count() const
{
  return get_impl().get_warning_count();
}


Warning internal::Result_detail::get_warning(size_t pos)
{
  /*
    TODO: get_warning_count() does not take into account INFO entries, but
    here we look at all entries, including INFO ones.
  */

  if (!common::check_num_limits<unsigned>(pos))
    throw std::out_of_range("No diagnostic entry at position ...");

  auto &impl = get_impl();
  impl.load_diagnostics();
  auto *entry = impl.get_entry((unsigned)pos);
  if (!entry)
    throw std::out_of_range("No diagnostic entry at position ...");

  byte level = Warning::LEVEL_ERROR;

  switch (entry->severity())
  {
  case cdk::api::Severity::ERROR:   level = Warning::LEVEL_ERROR; break;
  case cdk::api::Severity::WARNING: level = Warning::LEVEL_WARNING; break;
  case cdk::api::Severity::INFO:    level = Warning::LEVEL_INFO; break;
  }

  // TODO: handle error category

  return Warning_detail(
    level,
    (uint16_t)entry->code().value(),
    std::wstring(entry->description())
  );
}


uint64_t internal::Result_detail::get_affected_rows() const
{
  return get_impl().get_affected_rows();
}

uint64_t internal::Result_detail::get_auto_increment() const
{
  return get_impl().get_auto_increment();
}


const GUID& internal::Result_detail::get_document_id() const
{
  if (get_impl().m_guids.size() == 0)
    throw Error("Can only be used on add operations.");
  if (get_impl().m_guids.size() > 1)
    throw Error("Multiple documents added... should use getDocumentIds()");

  return get_impl().m_guids.front();
}


auto internal::Result_detail::get_document_ids() const -> DocIdList
{
  if (get_impl().m_guids.size() == 0)
    throw Error("Can only be used on add operations.");

  return get_impl().m_guids;
}


bool internal::Result_detail::has_data() const
{
  return get_impl().has_data();
}

bool internal::Result_detail::next_result()
{
  return get_impl().next_result();
}


/*
  RowResult
  =========
*/


template<>
bool internal::Row_result_detail<Columns>::iterator_next()
{
  auto &impl = get_impl();
  const common::Row_data *row = impl.get_row();

  if (!row)
    return false;

  m_row = internal::Row_detail(
    std::make_shared<internal::Row_detail::Impl>(*row, impl.get_mdata())
  );

  return true;
}


template<>
col_count_t internal::Row_result_detail<Columns>::col_count() const
{
  return get_impl().get_col_count();
}


template<>
internal::Row_result_detail<Columns>::Row_result_detail(common::Result_init &init)
  : internal::Result_detail(init)
{
  auto &impl = get_impl();
  if (impl.next_result())
    m_cols.init(impl);
}


template<>
auto internal::Row_result_detail<Columns>::get_column(col_count_t pos) const
-> const Column&
{
  return m_cols.at(pos);
}

template<>
auto internal::Row_result_detail<Columns>::get_columns() const
-> const Columns&
{
  return m_cols;
}


template<>
void internal::Columns_detail<Column>::init(
  const internal::Result_detail::Impl &impl
)
{
  for (col_count_t pos = 0; pos < impl.get_col_count(); ++pos)
  {
    emplace_back(impl.get_column(pos));
  }
}


template<>
row_count_t internal::Row_result_detail<Columns>::row_count()
{
  auto cnt = get_impl().count();
  ASSERT_NUM_LIMITS(row_count_t, cnt);
  return (row_count_t)cnt;
}


/*
  DocResult
  =========
*/


bool internal::Doc_result_detail::iterator_next()
{
  const common::Row_data *row = get_impl().get_row();

  if (!row)
    return false;

  // @todo Avoid copying of document string.
  cdk::foundation::bytes data = row->at(0).data();
  m_cur_doc = DbDoc(std::string(data.begin(),data.end()-1));
  return true;
}


uint64_t internal::Doc_result_detail::count()
{
  return get_impl().count();
}

