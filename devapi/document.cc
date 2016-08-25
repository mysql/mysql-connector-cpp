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
#include <json_parser.h>

/**
  @file
  Implementation of DbDoc and related classes.
*/

#include "impl.h"

#include <vector>
#include <sstream>
#include <iomanip>
#include <memory>

using namespace ::mysqlx;

using std::endl;


// Value
// -----


void Value::print(ostream &out) const
{
  switch (m_type)
  {
  case VNULL: out << "<null>"; return;
  case UINT64: out << m_val._uint64_v; return;
  case INT64: out << m_val._int64_v; return;
  case DOUBLE: out << m_val._double_v; return;
  case FLOAT: out << m_val._float_v; return;
  case BOOL: out << (m_val._bool_v ? "true" : "false"); return;
  case STRING: out << (std::string)m_str; return;
  case DOCUMENT: out << m_doc; return;
  case RAW: out << "<" << m_raw.size() << " raw bytes>"; return;
  // TODO: print array contnets
  case ARRAY: out << "<array with " << elementCount() << " element(s)>"; return;
  default:  out << "<unknown value>"; return;
  }
}


// DbDoc implementation
// --------------------


DbDoc::DbDoc(const std::string &json)
  : m_impl(std::make_shared<Impl::JSONDoc>(json))
{}

DbDoc::DbDoc(const std::shared_ptr<Impl> &impl)
  : m_impl(impl)
{}


bool DbDoc::hasField(const Field &fld) const
{
  try {
    return m_impl && m_impl->has_field(fld);
  }
  CATCH_AND_WRAP
}

const Value& DbDoc::operator[](const Field &fld) const
{
  try {
    return m_impl->get(fld);
  }
  CATCH_AND_WRAP
}

void DbDoc::print(std::ostream &out) const
{
  try {
    if (m_impl)
      m_impl->print(out);
    else
      out << "{}";
  }
  CATCH_AND_WRAP
}

// JSON document
// -------------


/*
  JSON processor which builds document implementation adding
  key-value pairs to document's map.
*/

struct DbDoc::Impl::Builder
  : public cdk::JSON::Processor
  , public cdk::JSON::Processor::Any_prc
  , public cdk::JSON::Processor::Any_prc::Scalar_prc
{
  Map  &m_map;
  mysqlx::string m_key;

public:

  Builder(DbDoc::Impl &doc)
    : m_map(doc.m_map)
  {}

  // JSON processor (to build the docuemnt)

  void doc_begin()
  {
    m_map.clear();
  }

  void doc_end()
  {}

  cdk::JSON::Processor::Any_prc*
  key_val(const cdk::string &key)
  {
    m_key = key;
    // Return itself to process key value
    return this;
  }


  /*
    Builder for array values.
  */

  struct Arr_builder
    : cdk::JSON::Processor::Any_prc
    , cdk::JSON::Processor::Any_prc::List_prc
    , cdk::JSON::Processor::Any_prc::Scalar_prc
  {
    Value::Array *m_arr;


    // List processor (to build the list)

    void list_begin()
    {
      m_arr->clear();
    }

    void list_end() {}

    Element_prc* list_el()
    {
      // Return itself to process list element.
      return this;
    }


    // Any processor (to process elements of the list)

    // Handle sub-arrray element.

    std::unique_ptr<Arr_builder> m_arr_builder;

    List_prc* arr()
    {
      // Create array value.

      Value sub;
      sub.m_type = Value::ARRAY;
      sub.m_arr = std::make_shared<Value::Array>();

      // Create builder for the sub-array.

      m_arr_builder.reset(new Arr_builder());
      m_arr_builder->m_arr = sub.m_arr.get();

      // Append the sub-array to the main array.

      m_arr->emplace_back(sub);

      return m_arr_builder.get();
    }


    // Handle a document element

    std::unique_ptr<Builder> m_doc_builder;

    Doc_prc*  doc()
    {
      // Create document value and append it to the array.

      Value sub;
      sub.m_type = Value::DOCUMENT;
      sub.m_doc.m_impl = std::make_shared<DbDoc::Impl>();
      m_arr->emplace_back(sub);

      // Create builder for the document and return it as the processor.

      m_doc_builder.reset(new Builder(*sub.m_doc.m_impl));
      return m_doc_builder.get();
    }


    // Handle scalar values using itself as a processor.

    Scalar_prc* scalar()
    { return this; }


    // Sclar processor (to store scalar values in the list)

    void null() { m_arr->emplace_back(Value()); }
    void str(const cdk::string &val)
    {
      m_arr->emplace_back(Value(val));
    }
    void num(uint64_t val) { m_arr->emplace_back(val); }
    void num(int64_t val) { m_arr->emplace_back(val); }
    void num(float val) { m_arr->emplace_back(val); }
    void num(double val) { m_arr->emplace_back(val); }
    void yesno(bool val) { m_arr->emplace_back(val); }

  }
  m_arr_builder;


  cdk::JSON::Processor::Any_prc::List_prc*
  arr()
  {
    using mysqlx::Value;
    Value &arr = m_map[m_key];

    // Turn the value to one storing an array.

    arr.m_type = Value::ARRAY;
    arr.m_arr  = std::make_shared<Value::Array>();

    // Set up array builder for the new value.

    m_arr_builder.m_arr = arr.m_arr.get();
    return &m_arr_builder;
  }


  std::unique_ptr<Builder> m_doc_builder;

  cdk::JSON::Processor::Any_prc::Doc_prc*
  doc()
  {
    using mysqlx::Value;
    Value &sub = m_map[m_key];

    // Turn the value to one storing a document.

    sub.m_type = Value::DOCUMENT;
    sub.m_doc.m_impl = std::make_shared<DbDoc::Impl>();

    // Use another builder to build the sub-document.

    m_doc_builder.reset(new Builder(*sub.m_doc.m_impl));
    return m_doc_builder.get();
  }

  cdk::JSON::Processor::Any_prc::Scalar_prc*
  scalar()
  {
    return this;
  }

  /*
    Callbacks for scalar values store the value under
    key given by m_key.
  */

  void null() { m_map.emplace(m_key, Value()); }
  void str(const cdk::string &val)
  {
    m_map.emplace(m_key, Value(val));
  }
  void num(uint64_t val)  { m_map.emplace(m_key, val); }
  void num(int64_t val)   { m_map.emplace(m_key, val); }
  void num(float val)     { m_map.emplace(m_key, val); }
  void num(double val)    { m_map.emplace(m_key, val); }
  void yesno(bool val)    { m_map.emplace(m_key, val); }

};


void DbDoc::Impl::JSONDoc::prepare()
{
  if (m_parsed)
    return;

  cdk::Codec<cdk::TYPE_DOCUMENT> codec;
  Builder bld(*this);
  codec.from_bytes(cdk::bytes(m_json), bld);
  m_parsed = true;
}


/*
  Parse JSON string and build a corresponding Value.
*/

Value Value::Access::mk_from_json(const std::string &json)
{
  typedef parser::Any_parser<
    parser::JSON_scalar_parser,
    cdk::JSON_processor
  > Parser;

  // Create parser for the JSON string.

  parser::Tokenizer toks(json);
  toks.get_tokens();
  auto first = toks.begin();
  Parser parser(first, toks.end());

  /*
    Define builder which acts as JSON value processor and
    builds the corresponding Value object.
  */

  struct Builder :
    public cdk::JSON::Processor::Any_prc
    , cdk::JSON_processor
  {
    Value *m_val = NULL;

    // Any_prc

    Scalar_prc *scalar() { return this; }

    std::unique_ptr<DbDoc::Impl::Builder> m_doc_builder;

    Doc_prc    *doc()
    {
      m_val->m_type = DOCUMENT;
      m_doc_builder.reset(new DbDoc::Impl::Builder(*m_val->m_doc.m_impl));
      return m_doc_builder.get();
    }

    DbDoc::Impl::Builder::Arr_builder m_arr_builder;

    List_prc   *arr()
    {
      m_val->m_type = ARRAY;
      m_val->m_arr = std::make_shared<Array>();
      m_arr_builder.m_arr = m_val->m_arr.get();
      return &m_arr_builder;
    }

    // JSON_processor

    void null() {}

    void str(const cdk::string &val)
    {
      *m_val = (mysqlx::string)val;
    }

    void num(uint64_t val) { *m_val = val; }
    void num(int64_t val)  { *m_val = val; }
    void num(float val)    { *m_val = val; }
    void num(double val)   { *m_val = val; }
    void yesno(bool val)   { *m_val = val; }
  }
  builder;

  // Invoke parser to build the value.

  Value val;
  builder.m_val = &val;
  parser.process(builder);

  return std::move(val);
}


/*
  Iterating over document fields
  ------------------------------
  Iterator functionality is implemented by document implementation
  object in forms of these methods:

  - reset()  - restart iteration form the beginning,
  - next()   - move to next document field,
  - at_end() - true if all fields have been enumerated,
  - get_current_fld() - return current field in the sequence.

  Note: Since document implementation acts as an iterator, only one
  iterator can be used at a time. Creating new iterator will invalidate
  other iterators.

  Note: Iterator takes shared ownership of document implementation
  so it can be used even if original document was destroyed.
*/


DbDoc::Iterator DbDoc::begin()
{
  Iterator it;
  m_impl->reset();
  it.m_impl = m_impl;
  it.m_end = false;
  return std::move(it);
}

DbDoc::Iterator DbDoc::end()
{
  /*
    Iterator that points one-past-the-end-of-sequence has no
    real representation - we simply set m_end flag in it.
  */
  Iterator it;
  it.m_end = true;
  return std::move(it);
}


const Field& DbDoc::Iterator::operator*()
{
  if (m_end)
    THROW("dereferencing past-the-end iterator");
  return m_impl->get_current_fld();
}

DbDoc::Iterator& DbDoc::Iterator::operator++()
{
  // only non-end iterator can be incremented.
  if (!m_end)
    m_impl->next();
  return *this;
}

bool DbDoc::Iterator::operator==(const Iterator &other) const
{
  /*
    if this is end iterator, other is equal if it is also end
    iterator or it is at the end of sequence. And vice-versa.
  */

  if (m_end)
    return other.m_end || other.m_impl->at_end();

  if (other.m_end)
    return m_end || m_impl->at_end();

  /*
    Otherwise two iterators are equal if they use the same
    document implementation (but such two iterators should not
    be used at the same time).
  */

  return m_impl == other.m_impl;
}
