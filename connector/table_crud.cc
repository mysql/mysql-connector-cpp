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

#include <mysqlx.h>
#include <uuid_gen.h>

#include <time.h>
#include <sstream>
#include <forward_list>
#include <boost/format.hpp>
#include <list>

#include "impl.h"

using namespace mysqlx;
using cdk::string;
using namespace parser;


// --------------------------------------------------------------------

/*
  Table.insert()
  ==============
*/


class Op_table_insert
    : public Op_sort<Parser_mode::TABLE>
    , public cdk::Row_source
    , public cdk::api::Columns
    , public cdk::Format_info
{
  using string = cdk::string;
  using Row_list = std::forward_list < Row >;
  using Col_list = std::forward_list < string >;


  Table_ref m_table;
  Row_list  m_rows;
  Row_list::const_iterator m_cur_row;
  Row_list::iterator m_row_end;
  Col_list m_cols;
  Col_list::iterator m_col_end;

  Op_table_insert(Table &tbl)
    : Op_sort(tbl)
    , m_table(tbl)
  {}

  void reset()
  {
    m_rows.clear();
    m_cur_row = m_rows.cbegin();
    m_row_end = m_rows.before_begin();
    m_cols.clear();
    m_col_end = m_cols.before_begin();
  }

  // Task::Impl

  bool m_started;

  cdk::Reply* send_command() override
  {
    // Prepare iterators to make a pass through m_rows list.
    m_started = false;
    m_row_end = m_rows.end();

    return new cdk::Reply(
      get_cdk_session().table_insert(m_table,
                                     *this,
                                     m_cols.empty() ? nullptr : this,
                                     nullptr)
                         );
  }

  // Row_source (Iterator)

  bool next() override
  {
    if (!m_started)
      m_cur_row = m_rows.cbegin();
    else
      ++m_cur_row;

    m_started = true;
    return m_cur_row != m_row_end;
  }

  // Columns

  void process(cdk::api::Columns::Processor& prc) const override
  {
    prc.list_begin();
    for (auto el : m_cols)
    {
      cdk::safe_prc(prc)->list_el()->name(el);
    }
    prc.list_end();
  }

  // Row_source (Expr_list)

  void process(cdk::Expr_list::Processor &ep) const override;

  // Format_info

  bool for_type(cdk::Type_info) const override { return true; }
  void get_info(cdk::Format<cdk::TYPE_BYTES>&) const override {}
  using cdk::Format_info::get_info;

  friend class mysqlx::TableInsert;
};


namespace mysqlx {

template <>
struct Crud_impl<TableInsert>
{
  typedef Op_table_insert type;
};

}


void TableInsert::prepare(Table &table)
{
  Task::Access::reset(m_task, new Op_table_insert(table));
  get_impl(this).reset();
}


void TableInsert::add_column(const mysqlx::string &column)
{
  auto &impl = get_impl(this);
  impl.m_col_end = impl.m_cols.emplace_after(impl.m_col_end, column);
}

void TableInsert::add_column(mysqlx::string&& column)
{
  auto &impl = get_impl(this);
  impl.m_col_end = impl.m_cols.emplace_after(impl.m_col_end, std::move(column));
}

Row& TableInsert::add_row()
{
  auto &impl = get_impl(this);
  impl.m_row_end = impl.m_rows.emplace_after(impl.m_row_end);
  return *impl.m_row_end;
}

void TableInsert::add_row(const Row &row)
{
  auto &impl = get_impl(this);
  impl.m_row_end = impl.m_rows.emplace_after(impl.m_row_end, row);
}


void Op_table_insert::process(cdk::Expr_list::Processor &lp) const
{
  using Element_prc = cdk::Expr_list::Processor::Element_prc;

  lp.list_begin();

  for (unsigned pos = 0; pos < m_cur_row->colCount(); ++pos)
  {
    Element_prc::Scalar_prc::Value_prc *vprc;
    vprc = safe_prc(lp)->list_el()->scalar()->val();

    if (!vprc)
      continue;

    const Value &val = m_cur_row->get(pos);

    switch (val.getType())
    {
    case Value::VNULL:  vprc->null(); break;
    case Value::STRING: vprc->str((string)val); break;
    case Value::INT64:  vprc->num((int64_t)(int)val); break;
    case Value::UINT64: vprc->num((uint64_t)(unsigned)val); break;
    case Value::FLOAT:  vprc->num((float)val); break;
    case Value::DOUBLE: vprc->num((double)val); break;
      // TODO: handle other value types
    default:
      vprc->value(cdk::TYPE_BYTES, *this, Value::Access::get_bytes(val));
      break;
    }
  }

  lp.list_end();
}


// --------------------------------------------------------------------

/*
  Table.select()
  ==============
*/

class Op_table_select
    : public Op_sort<parser::Parser_mode::TABLE>
    , public Op_projection<parser::Parser_mode::TABLE>
{
  typedef cdk::string string;

  Table_ref m_table;
  string m_where;
  std::unique_ptr<parser::Expression_parser> m_expr;

  cdk::Reply* send_command() override
  {
    if (!m_where.empty())
      m_expr.reset(new parser::Expression_parser(Parser_mode::TABLE, m_where));

    return
        new cdk::Reply(get_cdk_session().table_select(
                                          m_table,
                                          m_expr.get(),
                                          has_projection() ? this : nullptr,
                                          m_order.empty() ? nullptr : this,
                                          nullptr,
                                          nullptr,
                                          m_has_limit ? this : nullptr,
                                          get_params()
                                                     )
                       );
  }

public:
  Op_table_select(Table &table)
    : Op_sort(table)
    , m_table(table)
  {
  }

  friend class mysqlx::TableSelect;
};


namespace mysqlx {

template <>
struct Crud_impl<TableSelect>
{
  typedef Op_table_select type;
};

}


void TableSelect::prepare(Table &table)
{
  Executable::Access::reset_task(*this, new Op_table_select(table));
}

void TableSelect::add_proj(const string& projection)
{
  get_impl(this).add_projection(projection);
}

internal::TableSort<true>& TableSelect::where(const mysqlx::string &expr)
{
  get_impl(this).m_where = expr;
  return *this;
}


// --------------------------------------------------------------------

/*
  Table.update()
  ==============
*/

class Op_table_update
    : public Op_sort<parser::Parser_mode::TABLE>
    , public cdk::Update_spec
    , public cdk::api::Column_ref
{
  typedef cdk::string string;
  typedef std::map<string,ExprValue> SetValues;

  Table_ref m_table;
  string m_where;
  std::unique_ptr<parser::Expression_parser> m_expr;
  SetValues m_set_values;
  SetValues::const_iterator m_set_it;

  cdk::Reply* send_command() override
  {
    if (!m_where.empty())
      m_expr.reset(new parser::Expression_parser(Parser_mode::TABLE, m_where));

    m_set_it = m_set_values.end();

    return
        new cdk::Reply(
          get_cdk_session().table_update(m_table,
                                         m_expr ? m_expr.get() : nullptr,
                                         *this,
                                         m_order.empty() ? nullptr : this,
                                         m_has_limit ? this : nullptr,
                                         get_params()
                                        )
                      );
  }


  // cdk::Update_spec

  virtual bool next() override
  {
    if (m_set_it == m_set_values.end())
    {
      m_set_it = m_set_values.begin();
      return m_set_it != m_set_values.end();
    }
    ++m_set_it;
    return m_set_it != m_set_values.end();
  }

  void process(cdk::Update_spec::Processor &prc) const override
  {
    prc.column(*this);

    Value_prc val_prc(m_set_it->second, parser::Parser_mode::TABLE);
    val_prc.process_if(prc.set(NULL));
  }

  //  cdk::api::Column_ref

  virtual const string name() const override
  {
    return m_set_it->first;
  }

  const Table_ref* table() const override
  {
    return NULL;
  }


public:
  Op_table_update(Table &table)
    : Op_sort(table)
    , m_table(table)
  {
  }


  friend class mysqlx::TableUpdate;
};


namespace mysqlx {

template <>
struct Crud_impl<TableUpdate>
{
  typedef Op_table_update type;
};

}


void TableUpdate::prepare(Table &table)
{
  Executable::Access::reset_task(*this, new Op_table_update(table));
}

TableUpdate& TableUpdate::set(const mysqlx::string& field, ExprValue val)
{
  get_impl(this).m_set_values[field] = std::move(val);
  return *this;
}

Executable& TableUpdate::where(const mysqlx::string &expr)
{
  get_impl(this).m_where = expr;
  return *this;
}


// --------------------------------------------------------------------

/*
  Table.remove()
  ==============
*/

class Op_table_remove
    : public Op_sort<parser::Parser_mode::TABLE>
{
  typedef cdk::string string;

  Table_ref m_table;
  string m_where;
  std::unique_ptr<parser::Expression_parser> m_expr;

  cdk::Reply* send_command() override
  {
    if (!m_where.empty())
      m_expr.reset(new parser::Expression_parser(Parser_mode::TABLE, m_where));

    return
        new cdk::Reply(
          get_cdk_session().table_delete(m_table,
                                         m_expr ? m_expr.get() : nullptr,
                                         m_order.empty() ? nullptr : this,
                                         m_has_limit ? this : nullptr,
                                         get_params()
                                        )
                      );
  }

public:

  Op_table_remove(Table &table)
    : Op_sort(table)
    , m_table(table)
  {
  }

  friend class mysqlx::TableRemove;
};


namespace mysqlx {

template <>
struct Crud_impl<TableRemove>
{
  typedef Op_table_remove type;
};

}


void TableRemove::prepare(Table &table)
{
  Executable::Access::reset_task(*this, new Op_table_remove(table));
}

internal::TableSort<false>& TableRemove::where(const mysqlx::string& where)
{
  get_impl(this).m_where = where;
  return *this;
}
