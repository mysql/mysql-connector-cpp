/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C is licensed under the terms of the GPLv2
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

#include <mysql_xapi.h>
#include <iostream>
#include <iomanip>
#include "mysqlx_cc_internal.h"


/*
  Helper class for allocating the result buffer dynamically and keeping
  it as long as needed
*/
class Data_holder
{
private:
  mysqlx_data_type_t m_type;
  cdk::byte *m_buf;
  cdk::bytes m_bytes;
  size_t m_full_data_len, m_data_offset;

  void data_copy(cdk::bytes data)
  {
    memcpy(m_buf, m_data_offset + data.begin(), data.size());
    m_data_offset += data.size();
  }

  void init(cdk::bytes data)
  {
    m_data_offset = 0;
    data_copy(data);
  }

public:

  /* The default constructor will be used to store NULL value */
  Data_holder() : m_type(MYSQLX_TYPE_NULL),
                  m_buf(NULL)
  {}

  Data_holder(cdk::bytes data, size_t full_data_len) : m_type(MYSQLX_TYPE_BYTES),
    m_buf(new cdk::byte[full_data_len]), m_bytes(m_buf, full_data_len),
    m_full_data_len(full_data_len)
  {
    init(data);
  }

  Data_holder(cdk::bytes data, size_t full_data_len, mysqlx_data_type_t type) : m_type(type),
     m_buf(new cdk::byte[full_data_len]), m_bytes(m_buf, full_data_len),
     m_full_data_len(full_data_len)
  {
    init(data);
  }

  bool is_null()
  {
    return m_buf == NULL;
  }

  void append(cdk::bytes data)
  {
    data_copy(data);
  }

  cdk::bytes get_data()
  {
    return m_bytes;
  }

  ~Data_holder() { delete [] m_buf; }
};

void mysqlx_row_t::clear()
{
  for(std::vector<Data_holder*>::iterator it = m_row_data.begin();
      it != m_row_data.end(); ++it)
  {
    delete *it;
  }
  m_row_data.clear();
}

mysqlx_row_t::~mysqlx_row_struct()
{
  clear();
}

void mysqlx_row_t::add_field_null()
{
  m_row_data.push_back(new Data_holder());
}


void mysqlx_row_t::add_field_data(cdk::foundation::bytes data, size_t full_len)
{
  m_row_data.push_back(new Data_holder(data, full_len));
}

void mysqlx_row_t::append_field_data(cdk::col_count_t pos, cdk::bytes data)
{
  if(pos + 1 <= m_row_data.size())
    m_row_data[pos]->append(data);
}

cdk::bytes mysqlx_row_t::get_col_data(cdk::col_count_t pos) {
  return m_row_data[pos]->get_data();
}

mysqlx_doc_t::mysqlx_doc_struct(cdk::bytes data) : m_bytes(data),
m_json_doc(m_bytes)
{ }

void Value_item::process_any(cdk::Any::Processor &prc) const
{
  cdk::Value_processor *vprc = prc.scalar();
  if (vprc)
    process_val(*vprc);
}
/*
  This process method is supposed to take
  cdk::Value_processor and cdk::Expr_processor::Val_prc types
*/
void Value_item::process_val(cdk::Value_processor &prc) const
{
  switch (m_type)
  {
    case MYSQLX_TYPE_NULL:    prc.null(); break;
    case MYSQLX_TYPE_SINT:    prc.num(m_val.v_sint); break;
    case MYSQLX_TYPE_UINT:    prc.num(m_val.v_uint); break;
    case MYSQLX_TYPE_FLOAT:   prc.num(m_val.v_float); break;
    case MYSQLX_TYPE_DOUBLE:  prc.num(m_val.v_double); break;
    case MYSQLX_TYPE_BOOL:    prc.yesno(m_val.v_bool); break;
    case MYSQLX_TYPE_STRING:  prc.str(m_str); break;
    case MYSQLX_TYPE_BYTES:   prc.value(cdk::TYPE_BYTES, Blob_format_info(), m_bytes); break;
    default: // TODO: handle other types
    break;
  }
}

void Param_item::process(cdk::Any::Processor &prc) const
{
  process_any(prc);
}


void Row_item::process(cdk::Value_processor &prc) const
{
  process_val(prc);
}

void Row_item::generate_uuid()
{
  uuid::uuid_type uuid;

  /*
    Create a local copy of a document structure just to get _id
    if it was provided by a user inside JSON
  */
  mysqlx_doc_t doc(m_str.data());

  if (doc.key_exists("_id"))
  {
    if (MYSQLX_TYPE_STRING != doc.get_type("_id"))
      throw Mysqlx_exception("Document id must be a string");

    std::string str_id = doc.get_string("_id");
    if (str_id.length() > sizeof(uuid::uuid_type)* 2)
      throw Mysqlx_exception("Specified UUID is too long");
    m_uuid = str_id;
  }
  else
  {
    if (!doc.count())
      m_empty_doc = true; // do not add "," before _id

    mysqlx::generate_uuid(uuid);
    char buf[sizeof(uuid::uuid_type)* 2 + 1];
    const char digits[17] = { "0123456789ABCDEF" };

    for (size_t i = 0; i < sizeof(uuid); ++i)
    {
      buf[i * 2] = digits[((unsigned char)uuid[i]) & 0x0F];
      buf[i * 2 + 1] = digits[((unsigned char)uuid[i] >> 4)];
    }
    buf[sizeof(uuid::uuid_type)* 2] = 0; // put a string termination
    m_uuid = buf;
  }
}

// Process method for table projections
void Projection_list::process(cdk::Projection::Processor & prc) const
{
  prc.list_begin();
  for (Proj_vec::const_iterator it = m_list.begin();
        it != m_list.end(); ++it)
  {
    parser::Projection_parser parser(m_mode, *it);
    cdk::Projection::Processor::Element_prc *eprc = prc.list_el();
    if (eprc)
      parser.process(*eprc);
  }
  prc.list_end();
}

// Process method for document projections
void Projection_list::process(cdk::Expression::Document::Processor & prc) const
{
  // For documents we only have one entry in the list
  Expression_parser parser(parser::Parser_mode::DOCUMENT, m_list[0]);
  cdk::Expr_conv_base<Expr_to_doc_prc_converter,
    cdk::Expression,
    cdk::Expression::Document> spec;
  spec.reset(parser);
  spec.process(prc);
}

bool Source_base::do_next()
{
  if (m_row_num >= m_rows.size())
    return false;
  ++m_row_num;
  return true;
}


void Row_source::process(Processor &prc) const
{
  /*
    Row number was already increased in a preceding next() call and we cannot
    increment it here because the function is const
  */
  size_t row_index = m_row_num - 1;
  if (m_row_num > m_rows.size())
    return;

  prc.list_begin();

  for (Row_item_vec::const_iterator it = m_rows[row_index].begin();
        it != m_rows[row_index].end(); ++it)
  {
    cdk::Value_processor *vprc = cdk::safe_prc(prc)->list_el()->scalar()->val();
    if (vprc)
      it->process(*vprc);
  }

  prc.list_end();
}

bool Row_source::next()
{
  return do_next();
}

void Doc_source::process(Processor &prc) const
{
  size_t row_index = m_row_num - 1;
  if (m_row_num > m_rows.size())
    return;

  for (Row_item_vec::const_iterator it = m_rows[row_index].begin();
        it != m_rows[row_index].end(); ++it)
  {

    std::string s = it->get_string();
    s.erase(s.rfind('}'));
    std::stringstream sstream;
    if (!it->is_empty_doc())
      sstream << ", ";

    sstream << "\"_id\": \"" << it->get_uuid() << "\"}";
    s.append(sstream.str());
    cdk::bytes b = s;
    cdk::safe_prc(prc)->scalar()->val()->value(cdk::TYPE_DOCUMENT,
                                               JSON_format_info(),
                                               b);
  }
}

bool Doc_source::next()
{
  return do_next();
}

void Column_source::process(cdk::api::Columns::Processor& prc) const
{
  prc.list_begin();
  for (Column_list::const_iterator it = m_columns.begin();
    it != m_columns.end(); ++it)
  {
    cdk::safe_prc(prc)->list_el()->name(*it);
  }
  prc.list_end();
}

void Column_source::process(cdk::String_list::Processor& prc) const
{
  prc.list_begin();
  for (Column_list::const_iterator it = m_columns.begin();
       it != m_columns.end(); ++it)
  {
    cdk::safe_prc(prc)->list_el()->val(*it);
  }
  prc.list_end();
}


cdk::string Update_item::get_expr() const
{
  if (!m_is_expr)
    throw Mysqlx_exception("Item is not MYSQLX_TYPE_EXPR type");

  return get_string();
}

void Update_item::process(cdk::Expression::Processor &prc) const
{
  Expression_parser parser(parser::Parser_mode::TABLE, get_expr());
  parser.process(prc);
}

void Update_spec::add_null_value(cdk::string column)
{
  m_items.push_back(Update_item(column));
}

void Update_spec::process(Processor &prc) const
{
  const Update_item *it = get_cur_item();
  parser::Table_field_parser field(it->m_field);
  cdk::Doc_path *path = field.has_path() ? &field : NULL;
  prc.column(field);
  if (it->is_expr())
  {
    cdk::Update_processor::Expr_prc *eprc = prc.set(path);
    // Cannot use process_if() method because of specialized process() methods
    if (eprc)
      it->process(*eprc);
  }
  else
  {
    cdk::Expr_processor::Value_prc *vprc =
      cdk::safe_prc(prc)->set(path)->scalar()->val();
    // Cannot use process_if() method because of specialized process() methods
    if (vprc)
      it->process_val(*vprc);
  }
}


void Modify_item::process(cdk::Expression::Processor &prc) const
{
  Expression_parser parser(parser::Parser_mode::DOCUMENT, get_expr());
  parser.process(prc);
}

void Modify_spec::process(Processor &prc) const
{
  const Modify_item *it = get_cur_item();
  parser::Doc_field_parser field(it->m_field);

  switch (it->get_op_type())
  {
    case MODIFY_SET:
      if (it->is_expr())
      {
        cdk::Update_processor::Expr_prc *eprc = prc.set(&field);
        if (eprc)
          it->process(*eprc);
      }
      else
      {
        cdk::Expr_processor::Value_prc *vprc =
          cdk::safe_prc(prc)->set(&field)->scalar()->val();
        if (vprc)
          it->process_val(*vprc);
      }
    break;
    case MODIFY_ARRAY_INSERT:
      if (it->is_expr())
      {
        cdk::Update_processor::Expr_prc *eprc = prc.array_insert(&field);
        if (eprc)
          it->process(*eprc);
      }
      else
      {
        cdk::Expr_processor::Value_prc *vprc =
          cdk::safe_prc(prc)->array_insert(&field)->scalar()->val();
        if (vprc)
          it->process_val(*vprc);
      }
    break;
    case MODIFY_ARRAY_APPEND:
      if (it->is_expr())
      {
        cdk::Update_processor::Expr_prc *eprc = prc.array_append(&field);
        if (eprc)
          it->process(*eprc);
      }
      else
      {
        cdk::Expr_processor::Value_prc *vprc =
          cdk::safe_prc(prc)->array_append(&field)->scalar()->val();
        if (vprc)
          it->process_val(*vprc);
      }
    break;
    case MODIFY_ARRAY_DELETE:
    case MODIFY_UNSET:
      prc.remove(&field);
    break;
  }
}
