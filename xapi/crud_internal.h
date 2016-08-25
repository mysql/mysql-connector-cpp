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

typedef struct MYSQLX_SESSION_T MYSQLX_SESSION;
class Db_obj_ref;
class Limit;
class Order_by;
class Param_list;
class Row_source;

typedef struct MYSQLX_CRUD_T : public cdk::foundation::nocopy
{
private:
  MYSQLX_SESSION &m_session;
  MYSQLX_ERROR m_error;
  cdk::scoped_ptr<MYSQLX_RESULT> m_result; // TODO: multiple results
  Db_obj_ref m_db_obj_ref;
  cdk::protocol::mysqlx::Data_model m_data_model;
  parser::Parser_mode::value m_parser_mode;
  MYSQLX_OP m_op_type;
  cdk::Reply m_reply;
  cdk::scoped_ptr<cdk::Expression> m_where;
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

public:
  MYSQLX_CRUD_T(MYSQLX_SESSION *session, const char *query, uint32_t length) :
                                     m_session(*session),
                                     m_op_type(OP_SQL),
                                     m_query(std::string(query,length))
  {
    init_data_model();
  }

  MYSQLX_CRUD_T(MYSQLX_SESSION *session, const cdk::string &schema, const cdk::string &name,
                MYSQLX_OP op_type) : m_session(*session),
                                     m_db_obj_ref(schema, name),
                                     m_op_type(op_type)
  {
    init_data_model();
  }

  /*
    Un-registering result.
    The parameter is not used.
    This method will be extended in the future to handle
    multiple results.
  */
  bool set_result(MYSQLX_RESULT *res)
  {
    m_result.reset(res);
    return m_result.get() != NULL;
  }

  void set_diagnostic(const MYSQLX_EXCEPTION &ex)
  {
    m_error.set(ex);
  }

  void set_diagnostic(const char *msg, unsigned int num)
  {
    m_error.set(msg, num);
  }

  MYSQLX_RESULT *get_result() { return m_result.get(); }

  /*
    Get diagnostic info for the previously executed CRUD
    TODO: implement getting warnings and info using
          Diagnostic_iterator
  */
  void acquire_diag(cdk::foundation::api::Severity::value val =
                    cdk::foundation::api::Severity::ERROR);

  MYSQLX_ERROR *get_last_error()
  {
    if (m_error.message() || m_error.error_num())
      return &m_error;

    return NULL;
  }

  ~MYSQLX_CRUD_T();

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
    RETURN: pointer to MYSQLX_RESULT, which is being allocated each time
            when this function is called. The old result is freed automatically.
            On error the function returns NULL

    NOTE: no need to free the result in the end cdk::scoped_ptr will
          take care of it
  */
  MYSQLX_RESULT *exec();

  int sql_bind(va_list args);
  int param_bind(va_list args);

  /*
    Return the operation type OP_SELECT, OP_INSERT, OP_UPDATE, OP_DELETE,
    OP_FIND, OP_ADD, OP_MODIFY, OP_REMOVE, OP_SQL
  */
  MYSQLX_OP op_type() { return m_op_type; }
  MYSQLX_SESSION &get_session() { return m_session; }

  /*
    Set WHERE for CRUD operation
    PARAMETERS:
      where_expr - character string containing WHERE clause,
                   which will be parsed as required

    RETURN:
      RESULT_OK - on success
      RESULT_ERR - on error

    NOTE: each call to this function replaces previously set WHERE
  */
  int set_where(const char *where_expr);

  /*
    Set LIMIT for CRUD operation
    PARAMETERS:
      row_count - the number of result rows to return
      offset - the number of rows to skip before starting counting

    RETURN:
      RESULT_OK - on success
      RESULT_ERR - on error

    NOTE: each call to this function replaces previously set LIMIT
  */
  int set_limit(row_count_t row_count, row_count_t offset);

  int add_order_by(va_list args);
  int add_row(bool get_columns, va_list args);
  int add_columns(va_list args);
  int add_document(const char *json_doc);
  int add_multiple_documents(va_list args);
  int add_projections(va_list args);
  int add_table_update_values(va_list args);
  int add_coll_modify_values(va_list args, MYSQLX_MODIFY_OP op);

  // Clear the list of ORDER BY items
  void clear_order_by();

  // Return the session validity state
  bool session_valid();

} MYSQLX_CRUD;
