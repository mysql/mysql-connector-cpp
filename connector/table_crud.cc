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

  friend class mysqlx::TableInsert;
};


namespace mysqlx {

template <>
struct Crud_impl<TableInsert>
{
  typedef Op_table_insert type;
};

} // mysqlx


void TableInsert::prepare(Table &table)
{
  Statement::Access::reset_task(*this, new Op_table_insert(table));
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
  lp.list_begin();

  for (unsigned pos = 0; pos < m_cur_row->colCount(); ++pos)
  {
    Value_expr ve((*m_cur_row)[pos], Parser_mode::TABLE);
    ve.process_if(safe_prc(lp)->list_el());
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
  {}

  friend class mysqlx::TableSelect;
};


namespace mysqlx {

template <>
struct Crud_impl<TableSelect>
{
  typedef Op_table_select type;
};

} // mysqlx


void TableSelect::prepare(Table &table)
{
  Statement::Access::reset_task(*this, new Op_table_select(table));
}


void TableSelect::add_proj(const mysqlx::string& projection)
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
  typedef std::map<string, internal::ExprValue> SetValues;

  Table_ref m_table;
  string m_where;
  std::unique_ptr<parser::Expression_parser> m_expr;
  std::unique_ptr<parser::Table_field_parser> m_table_field;
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
    }
    else
    {
      ++m_set_it;
    }

    bool more = m_set_it != m_set_values.end();

    if (more)
     m_table_field.reset(new parser::Table_field_parser(m_set_it->first));

    return more;
  }

  void process(cdk::Update_spec::Processor &prc) const override
  {
    prc.column(*this);

    Value_expr val_prc(m_set_it->second, parser::Parser_mode::TABLE);
    val_prc.process_if(prc.set(m_table_field->path()));
  }


  //  cdk::api::Column_ref

  const string name() const override
  {
    return m_table_field->name();
  }

  const cdk::api::Table_ref* table() const override
  {
    return m_table_field->table();
  }

public:
  Op_table_update(Table &table)
    : Op_sort(table)
    , m_table(table)
  {}


  friend class mysqlx::TableUpdate;
};


namespace mysqlx {

template <>
struct Crud_impl<TableUpdate>
{
  typedef Op_table_update type;
};

} // mysqlx


void TableUpdate::prepare(Table &table)
{
  Statement::Access::reset_task(*this, new Op_table_update(table));
}


TableUpdate& TableUpdate::set(const mysqlx::string& field,
                              internal::ExprValue val)
{
  get_impl(this).m_set_values[field] = std::move(val);
  return *this;
}


internal::TableSort<false>& TableUpdate::where(const mysqlx::string &expr)
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
  {}


  friend class mysqlx::TableRemove;
};


namespace mysqlx {

template <>
struct Crud_impl<TableRemove>
{
  typedef Op_table_remove type;
};

} // mysqlx


void TableRemove::prepare(Table &table)
{
  Statement::Access::reset_task(*this, new Op_table_remove(table));
}


internal::TableSort<false>& TableRemove::where(const mysqlx::string& where)
{
  get_impl(this).m_where = where;
  return *this;
}
