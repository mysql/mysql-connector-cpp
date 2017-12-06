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

#include <mysql_common.h>
#include <mysql/cdk.h>
#include <json_parser.h>

#include "op_impl.h"


using namespace ::mysqlx::common;


struct JSON_val_conv : public cdk::Converter<JSON_val_conv,
  cdk::JSON_processor,
  cdk::Value_processor>
{
  using string = cdk::string;

  void null()
  {
    if (m_proc)
      m_proc->null();
  }

  void str(const string& s)
  {
    if (m_proc)
      m_proc->str(s);
  }

  void num(uint64_t v)
  {
    if (m_proc)
      m_proc->num(v);
  }

  void num(int64_t v)
  {
    if (m_proc)
      m_proc->num(v);
  }

  void num(float v)
  {
    if (m_proc)
      m_proc->num(v);
  }

  void num(double v)
  {
    if (m_proc)
      m_proc->num(v);
  }

  void yesno(bool v)
  {
    if (m_proc)
      m_proc->yesno(v);
  }

};


struct Field_conv
  : public cdk::Converter<Field_conv,
  cdk::JSON::Processor::Any_prc,
  cdk::Any::Processor
  >
{
  using string = cdk::string;

  using List_conv = cdk::List_prc_converter<JSON_val_conv>;
  using Any_prc = cdk::JSON::Processor::Any_prc;
  using Doc_prc = Any_prc::Doc_prc;

  Any_prc::Scalar_prc* scalar() override
  {
    assert(false);
    return nullptr;
  }

  List_conv m_list_conv;

  Any_prc::List_prc* arr() override
  {
    assert(false);
    return nullptr;
  }

  struct Field_doc_conv
    : public cdk::Doc_prc_converter<JSON_val_conv>
  {
    using Doc_conv = cdk::Doc_prc_converter<JSON_val_conv>;
    bool m_has_required = false;

    Any_prc* key_val(const string &key) override
    {
      string field_name = key;
      // Do a key name replacement on 2nd level
      if (key == string("field"))
      {
        field_name = "member";
      }
      else if (key == string("required"))
      {
        m_has_required = true;
      }
      return Doc_conv::key_val(field_name);
    }

    void doc_begin() override
    {
      m_has_required = false;
    }

    void doc_end() override
    {
      if(m_proc)
      {
        if (!m_has_required)
        {
          // No "required" in "field"
          m_proc->key_val("required")->scalar()->yesno(false);
        }
        m_proc->doc_end();
      }
    }
  }
  m_doc_conv;

  Doc_prc* doc() override
  {
    auto *prc = m_proc->doc();
    if (!prc)
      return nullptr;
    m_doc_conv.reset(*prc);
    return &m_doc_conv;
  }
};


struct Field_list_conv
  : public cdk::Converter<Field_list_conv,
  cdk::JSON::Processor::Any_prc::List_prc,
  cdk::Any_list::Processor
  >
{
  Field_conv m_field_conv;

  Element_prc* list_el() override
  {
    auto *prc = m_proc->list_el();
    if (!prc)
      return nullptr;
    m_field_conv.reset(*prc);
    return &m_field_conv;
  }

  void list_begin() override
  {
    m_proc->list_begin();
  }

  void list_end() override
  {
    m_proc->list_end();
  }
};


struct Fields_conv
  : public cdk::Converter<Fields_conv,
  cdk::JSON::Processor::Any_prc,
  cdk::Any::Processor
  >
{
  Field_list_conv  m_arr_conv;

  List_prc* arr() override
  {
    auto *prc = m_proc->arr();
    if (!prc)
      return nullptr;
    m_arr_conv.reset(*prc);
    return &m_arr_conv;
  }

  Scalar_prc* scalar() override
  {
    /*
      TODO: Add scalar processing for indexes as this one with
            scalar value on 1st level

      "type": "SPATIAL",
      "fields": [ { "field": "$.coords", "type": "GEOJSON", "srid": 31287 } ]

    */
    return nullptr;
  }

  Doc_prc* doc() override
  {
    throw_error("Wrong index specification");
    return nullptr;
  }
};


struct Index_def_conv : public cdk::Converter<Index_def_conv,
  cdk::JSON::Processor,
  cdk::Any::Document::Processor>
{
  Fields_conv m_fields_conv;

  Prc_from::Any_prc* key_val(const string &key)
  {
    string field_name = key;
    // Do a key name replacement on 1st level
    if (key == string("fields"))
    {
      field_name = "constraint";
    }

    auto *aprc = m_proc->key_val(field_name);
    if (!aprc)
      return nullptr;
    m_fields_conv.reset(*aprc);
    return &m_fields_conv;
  }
};


struct Index_def
 : cdk::Expr_conv_base<Index_def_conv, cdk::JSON, cdk::Any::Document>
{
  parser::JSON_parser m_parser;

  Index_def(const cdk::string &def)
    : m_parser(def)
  {
    reset(m_parser);
  }
};


void Op_idx_create::process(cdk::Any::Document::Processor &prc) const
{
  prc.doc_begin();

  for (auto it : m_map)
  {
    Value_scalar val(it.second);
    val.process_if(prc.key_val(it.first));
  }

  // Remove this later
  safe_prc(prc)->key_val("unique")->scalar()->yesno(false);

  // Report remaining values based on JSON document given by user.

  Index_def idx_def(m_def);
  idx_def.process(prc);

  prc.doc_end();
}
