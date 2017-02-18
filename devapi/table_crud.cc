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

#include <mysql_devapi.h>
#include <uuid_gen.h>

#include <time.h>
#include <sstream>
#include <forward_list>
#include <boost/format.hpp>
#include <list>
#include <bitset>

#include "impl.h"

using namespace mysqlx;
using parser::Parser_mode;


// --------------------------------------------------------------------

/*
  Table insert
  ============
*/

/*
  Internal implementation for table CRUD insert operation.

  Building on top of Op_sort<> this class implements remaining
  methods of TableInsert_impl. It stores columns and rows specified
  for the operation and presents them through cdk::Row_source and
  cdk::api::Columns interfaces.

  Overriden method Op_base::send_command() sends table insert command
  to the CDK session.
*/

class Op_table_insert
    : public Op_sort<
        internal::TableInsert_impl,
        Parser_mode::TABLE
      >
    , public cdk::Row_source
    , public cdk::api::Columns
{
  using string = cdk::string;
  using Row_list = std::forward_list < Row >;
  using Col_list = std::forward_list < string >;


  Table_ref m_table;
  Row_list  m_rows;
  Row_list::const_iterator m_cur_row = m_rows.cbegin();
  Row_list::iterator m_row_end = m_rows.before_begin();
  Col_list m_cols;
  Col_list::iterator m_col_end = m_cols.before_begin();

public:

  Op_table_insert(Table &tbl)
    : Op_sort(tbl)
    , m_table(tbl)
  {}

  Op_table_insert(const Op_table_insert &other)
    : Op_sort(other)
    , m_table(other.m_table)
    , m_rows(other.m_rows)
    , m_cols(other.m_cols)
  {}

  Executable_impl* clone() const override
  {
    return new Op_table_insert(*this);
  }

  void add_column(const mysqlx::string &column) override
  {
    m_col_end = m_cols.emplace_after(m_col_end, column);
  }

  Row& new_row() override
  {
    m_row_end = m_rows.emplace_after(m_row_end);
    return *m_row_end;
  }

  void add_row(const Row &row) override
  {
    m_row_end = m_rows.emplace_after(m_row_end, row);
  }

private:

  // Executable

  bool m_started;

  cdk::Reply* send_command() override
  {
    // Do nothing if no rows were specified.

    if (m_rows.empty())
      return NULL;

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

  friend mysqlx::TableInsert;
};


void TableInsert::prepare(Table &table)
{
  m_impl.reset(new Op_table_insert(table));
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
  Table select
  ============
*/

/*
  Internal implementation for table CRUD select operation.

  This implementation is built from Op_select<> and Op_projection<>
  templates. It overrides Op_base::send_command() to send table select
  command to the CDK session.
*/

class Op_table_select
  : public Op_select<
      Op_projection<
        internal::TableSelect_impl,
        parser::Parser_mode::TABLE
      >,
      parser::Parser_mode::TABLE
    >
{
  //typedef cdk::string string;

  Table_ref m_table;
  const cdk::View_spec *m_view = nullptr;

  cdk::Reply* send_command() override
  {
    return
        new cdk::Reply(get_cdk_session().table_select(
                          m_table,
                          m_view,           // view spec
                          get_where(),
                          get_tbl_proj(),
                          get_order_by(),
                          get_group_by(),
                          get_having(),
                          get_limit(),
                          get_params()
                       ));
  }

  void set_view(const cdk::View_spec *view)
  {
    m_view = view;
  }


  Executable_impl* clone() const override
  {
    return new Op_table_select(*this);
  }

public:

  Op_table_select(Table &table)
    : Op_select(table)
    , m_table(table)
  {}



  friend mysqlx::TableSelect;
  friend mysqlx::internal::Op_ViewCreateAlter;
};


void TableSelect::prepare(Table &table)
{
  m_impl.reset(new Op_table_select(table));
}


// --------------------------------------------------------------------

/*
  Table update
  ============
*/

/*
  Internal implementation for table CRUD select operation.

  This implementation is built from Op_select<> and Op_projection<>
  templates and it implements the `add_set` method of TableUpdate_impl
  implemantation interface. Update requests are stored in m_set_values
  member and presented to CDK via cdk::Update_spec interface.

  It overrides Op_base::send_command() to send table update command
  to the CDK session.
*/

class Op_table_update
    : public Op_select<
        Op_projection<
          internal::TableUpdate_impl,
          parser::Parser_mode::TABLE
        >,
        parser::Parser_mode::TABLE
      >
    , public cdk::Update_spec
    , public cdk::api::Column_ref
{
  //typedef cdk::string string;
  typedef std::map<mysqlx::string, internal::ExprValue> SetValues;

  Table_ref m_table;
  std::unique_ptr<parser::Table_field_parser> m_table_field;
  SetValues m_set_values;
  SetValues::const_iterator m_set_it;

  Executable_impl* clone() const override
  {
    return new Op_table_update(*this);
  }

  void add_set(const mysqlx::string &field, internal::ExprValue &&val) override
  {
    m_set_values[field] = std::move(val);
  }

  cdk::Reply* send_command() override
  {
    m_set_it = m_set_values.end();

    return
        new cdk::Reply(get_cdk_session().table_update(
                        m_table,
                        get_where(),
                        *this,
                        get_order_by(),
                        get_limit(),
                        get_params()
                      ));
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

    val_prc.process_if(
      prc.set(m_table_field->has_path() ? m_table_field.get() : NULL)
    );
  }


  //  cdk::api::Column_ref

  const cdk::string name() const override
  {
    return m_table_field->name();
  }

  const cdk::api::Table_ref* table() const override
  {
    return m_table_field->table();
  }


public:

  Op_table_update(Table &table)
    : Op_select(table)
    , m_table(table)
  {}

  Op_table_update(const Op_table_update &other)
    : Op_select(other)
    , m_table(other.m_table)
    , m_set_values(other.m_set_values)
  {}


  friend mysqlx::TableUpdate;
};


void TableUpdate::prepare(Table &table)
{
  m_impl.reset(new Op_table_update(table));
}


// --------------------------------------------------------------------

/*
  Table remove
  ============
*/

/*
  Internal implementation for table CRUD remove operation.

  This implementation is built using Op_select<> and Op_sort<>
  templates. It overrides Op_base::send_command() to send table remove
  command to the CDK session.
*/

class Op_table_remove
    : public Op_select<
        Op_sort<
          internal::TableRemove_impl,
          parser::Parser_mode::TABLE
        >,
        parser::Parser_mode::TABLE
      >
{
  typedef cdk::string string;

  Table_ref m_table;

  Executable_impl* clone() const override
  {
    return new Op_table_remove(*this);
  }


  cdk::Reply* send_command() override
  {
    return
        new cdk::Reply(get_cdk_session().table_delete(
                          m_table,
                          get_where(),
                          get_order_by(),
                          get_limit(),
                          get_params()
                      ));
  }

public:

  Op_table_remove(Table &table)
    : Op_select(table)
    , m_table(table)
  {}



  friend mysqlx::TableRemove;
};


void TableRemove::prepare(Table &table)
{
  m_impl.reset(new Op_table_remove(table));
}


// --------------------------------------------------------------------

/*
  ViewCreateAlter
  ===============
*/

namespace mysqlx{
namespace internal {

class Op_ViewCreateAlter
  : public Op_base<mysqlx::internal::View_impl>
  , cdk::View_spec
  , Table_ref
{
public:
  typedef cdk::View_spec::op_type op_type;

private:

  op_type m_op_type;
  std::unique_ptr<TableSelect> m_table_select;
  std::vector<mysqlx::string>  m_columns;

  CheckOption    m_check_option;
  SQLSecurity    m_security;
  Algorithm      m_algorithm;
  mysqlx::string m_definer;

  enum view_option { CHECK, SECURITY, ALGORITHM, DEFINER, LAST};
  std::bitset<LAST>  m_opts_mask;

  Executable_impl* clone() const override
  {
    return new Op_ViewCreateAlter(*this);
  }

  Op_ViewCreateAlter(const Op_ViewCreateAlter& other)
    : Op_base(other)
    , Table_ref(other)
    , m_op_type      (other.m_op_type     )
    , m_columns      (other.m_columns     )
    , m_check_option (other.m_check_option)
    , m_security     (other.m_security    )
    , m_algorithm    (other.m_algorithm   )
    , m_definer      (other.m_definer     )
    , m_opts_mask    (other.m_opts_mask   )
  {
    if (other.m_table_select.get() != NULL)
    {
      m_table_select.reset(new TableSelect(*other.m_table_select.get()));
      static_cast<Op_table_select*>(m_table_select->get_impl())->set_view(this);
    }
  }

public:

  Op_ViewCreateAlter(Schema &sch, const mysqlx::string &name, op_type replace)
    : Op_base< mysqlx::internal::View_impl >(sch.getSession())
    ,  Table_ref(sch.getName(), name)
    , m_op_type(replace)
  {}

  void with_check_option(CheckOption option) override
  {
    m_check_option = option;
    m_opts_mask.set(CHECK);
  }

  void defined_as(TableSelect &&select) override
  {
    m_table_select.reset(new TableSelect(std::move(select)));
    static_cast<Op_table_select*>(m_table_select->get_impl())->set_view(this);
  }

  void definer(const mysqlx::string &user) override
  {
    m_definer = user;
    m_opts_mask.set(DEFINER);
  }

  void security(SQLSecurity security) override
  {
    m_security = security;
    m_opts_mask.set(SECURITY);
  }

  void algorithm(Algorithm algorythm) override
  {
    m_algorithm = algorythm;
    m_opts_mask.set(ALGORITHM);
  }

  void add_columns(const mysqlx::string &name) override
  {
    m_columns.push_back(name);
  }

  cdk::Reply* send_command() override
  {
    if (m_table_select.get() == NULL)
      throw_error("Unexpected empty TableSelect");

    cdk::Reply *ret =
      static_cast<Op_table_select*>(m_table_select->get_impl())->send_command();

    return ret;

  }

  /*
     cdk::View_spec Processor
  */

  void process( cdk::View_spec::Processor &prc) const override
  {
    prc.name(*this, m_op_type);

    if (m_columns.size() != 0)
    {
      auto list_columns = prc.columns();
      if (list_columns)
      {
        list_columns->list_begin();
        for (auto column : m_columns)
        {
          list_columns->list_el()->val(column);
        }
        list_columns->list_end();
      }
    }

    auto options = prc.options();

    if (options)
    {

      if (m_opts_mask.test(DEFINER))
        options->definer(m_definer);

      if (m_opts_mask.test(ALGORITHM))
        switch (m_algorithm)
        {
          case Algorithm::MERGE:
            options->algorithm(cdk::api::View_algorithm::MERGE);
            break;
          case Algorithm::TEMPTABLE:
            options->algorithm(cdk::api::View_algorithm::TEMPTABLE);
            break;
          case Algorithm::UNDEFINED:
            options->algorithm(cdk::api::View_algorithm::UNDEFINED);
            break;
        };

      if (m_opts_mask.test(CHECK))
        switch(m_check_option)
        {
          case CheckOption::CASCADED:
            options->check(cdk::api::View_check::CASCADED);
            break;
          case CheckOption::LOCAL:
            options->check(cdk::api::View_check::LOCAL);
            break;
        };

      if (m_opts_mask.test(SECURITY))
        switch(m_security)
        {
          case SQLSecurity::DEFINER:
            options->security(cdk::api::View_security::DEFINER);
            break;
          case SQLSecurity::INVOKER:
            options->security(cdk::api::View_security::INVOKER);
            break;
        };
    }

  }
};

}} // namespace mysqlx::internal

namespace mysqlx {

ViewCreate::ViewCreate(Schema &sch, const string &name, bool replace)
{
  m_impl.reset(
        new internal::Op_ViewCreateAlter(sch,
                                         name,
                                         replace ?
                                           internal::Op_ViewCreateAlter::op_type::REPLACE :
                                           internal::Op_ViewCreateAlter::op_type::CREATE )
        );
}



ViewAlter::ViewAlter(Schema &sch, const string &name)
{
  m_impl.reset(
        new internal::Op_ViewCreateAlter(sch,
                                         name,
                                         internal::Op_ViewCreateAlter::op_type::UPDATE)
        );
}

} // namespace mysqlx



/*
   ViewDrop
   ========
*/

namespace mysqlx {
namespace internal{

class Op_ViewDrop
    : public Op_base<ViewDrop_impl>
    ,  public Table_ref
{
  bool m_checkExistence = true;

public:

  Op_ViewDrop(Schema &sch, const string &name)
    : Op_base<ViewDrop_impl>(sch.getSession())
    , Table_ref(sch.getName(), name)
  {}

  void if_exists() override
  {
    m_checkExistence = false;
  }

  Executable_impl* clone() const override
  {
    return new Op_ViewDrop(*this);
  }

  cdk::Reply* send_command() override
  {
    return new cdk::Reply(get_cdk_session().view_drop(*this,m_checkExistence));
  }
};

} // namespace internal

ViewDrop::ViewDrop(Schema &sch, const string &name)
{
  m_impl.reset(new internal::Op_ViewDrop(sch, name));
}

} // namespace mysqlx



// ---------------------------------------------------------------------
