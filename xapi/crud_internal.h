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

/*
  CRUD implementation
  Copy constructor must be disabled for this class
*/

typedef struct mysqlx_session_struct mysqlx_session_t;
class Db_obj_ref;
class Limit;
class Order_by;
class Param_list;
class Row_source;

class Group_by_list : public cdk::Expr_list
{
  typedef std::vector<cdk::string> group_by_list_type;
  group_by_list_type m_group_by_list;
  parser::Parser_mode::value m_parser_mode;

public:

  Group_by_list() {}

  void set_parser_mode(parser::Parser_mode::value mode) { m_parser_mode = mode; }
  void add_group_by(const char *expr) { m_group_by_list.push_back(cdk::string(expr)); }
  void clear() { m_group_by_list.clear(); }
  void process(cdk::Expr_list::Processor& prc) const;
  cdk::Expr_list *get_list() { return m_group_by_list.size() ? this : NULL; }
};

typedef struct mysqlx_stmt_struct : public Mysqlx_diag
{
private:
  mysqlx_session_t &m_session;
  cdk::scoped_ptr<mysqlx_result_t> m_result;
  Db_obj_ref m_db_obj_ref;
  cdk::protocol::mysqlx::Data_model m_data_model;
  parser::Parser_mode::value m_parser_mode;
  mysqlx_op_t m_op_type;
  cdk::Reply m_reply;
  cdk::scoped_ptr<cdk::Expression> m_where;
  cdk::scoped_ptr<cdk::Expression> m_having;
  cdk::scoped_ptr<Limit> m_limit;
  cdk::scoped_ptr<Order_by> m_order_by;
  cdk::scoped_ptr<Projection_list> m_proj_list;
  Param_list m_param_list;
  Param_source m_param_source;
  Row_source m_row_source;
  Column_source m_col_source;
  Doc_source m_doc_source;
  Update_spec m_update_spec;
  Modify_spec m_modify_spec;
  cdk::string m_query;
  Group_by_list m_group_by_list;
  View_spec m_view_spec;

  int set_expression(cdk::scoped_ptr<cdk::Expression> &member, const char *val);

public:
  mysqlx_stmt_struct(mysqlx_session_t *session, const char *query, uint32_t length) :
                                     m_session(*session),
                                     m_op_type(OP_SQL),
                                     m_query(std::string(query,length))
  {
    init_data_model();
  }

  mysqlx_stmt_struct(mysqlx_session_t *session, const cdk::string &schema, const cdk::string &name,
                     mysqlx_op_t op_type) : m_session(*session),
                     m_db_obj_ref(schema, name),
                     m_op_type(op_type)
  {
    init_data_model();
  }

  mysqlx_stmt_struct(mysqlx_session_t *session, const cdk::string &schema, const cdk::string &name,
                     mysqlx_op_t op_type, mysqlx_stmt_struct *parent) : m_session(*session),
                     m_db_obj_ref(parent->get_ref()),
                     m_op_type(op_type),
                     m_view_spec(schema, name, op_type)
  {
    init_data_model();
    if (parent)
      copy_parent_data(parent);
  }

  /*
    Un-registering result.
    The parameter is not used.
    This method will be extended in the future to handle
    multiple results.
  */
  bool set_result(mysqlx_result_t *res)
  {
    m_result.reset(res);
    return m_result.get() != NULL;
  }

  void copy_parent_data(mysqlx_stmt_struct *parent);

  mysqlx_result_t *get_result() { return m_result.get(); }

  /*
    Get diagnostic info for the previously executed CRUD
    TODO: implement getting warnings and info using
          Diagnostic_iterator
  */
  void acquire_diag(cdk::foundation::api::Severity::value val =
                    cdk::foundation::api::Severity::ERROR);

  ~mysqlx_stmt_struct();

  /*
    Member function to init the data model and parser mode when
    CRUD is being created, so this could be re-used.
  */
  void init_data_model();

  /*
    Member function to init the internal CRUD state.
  */
  void init_crud();

  /*
    Execute a CRUD statement.
    RETURN: pointer to mysqlx_result_t, which is being allocated each time
            when this function is called. The old result is freed automatically.
            On error the function returns NULL

    NOTE: no need to free the result in the end cdk::scoped_ptr will
          take care of it
  */
  mysqlx_result_t *exec();

  int sql_bind(va_list args);
  int sql_bind(cdk::string s);

  int param_bind(va_list args);

  /*
    Return the operation type OP_SELECT, OP_INSERT, OP_UPDATE, OP_DELETE,
    OP_FIND, OP_ADD, OP_MODIFY, OP_REMOVE, OP_SQL
  */
  mysqlx_op_t op_type() { return m_op_type; }
  mysqlx_session_t &get_session() { return m_session; }
  const Db_obj_ref &get_ref() { return m_db_obj_ref; }

  int set_where(const char *where_expr);
  int set_limit(row_count_t row_count, row_count_t offset);
  int set_having(const char *having_expr);

  int add_order_by(va_list args);
  int add_row(bool get_columns, va_list args);
  int add_columns(va_list args);
  int add_document(const char *json_doc);
  int add_multiple_documents(va_list args);
  int add_projections(va_list args);
  int add_table_update_values(va_list args);
  int add_coll_modify_values(va_list args, mysqlx_modify_op op);
  int add_group_by(va_list args);

  // Clear the list of ORDER BY items
  void clear_order_by();

  // Return the session validity state
  bool session_valid();
  bool is_view_op();

  void set_view_algorithm(int algorithm);
  void set_view_security(int security);
  void set_view_check_option(int option);
  void set_view_definer(const char* user);
  void set_view_columns(va_list args);
  void set_view_properties(va_list args);

  friend class Group_by_list;
} mysqlx_stmt_t;