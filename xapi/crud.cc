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
#include "mysqlx_cc_internal.h"

/*
  Member function to init the data model and parser mode when
  CRUD is being created, so this could be re-used.
*/
void mysqlx_stmt_t::init_data_model()
{
  /*
    Knowing the operation type is enough to determine the data model and
    the parser mode.
  */
  switch(m_op_type)
  {
    // TABLE Ops
    case OP_SELECT:
    case OP_INSERT:
    case OP_UPDATE:
    case OP_DELETE:
    case OP_SQL: // For plain SQL we assume TABLE mode
      m_data_model = cdk::protocol::mysqlx::TABLE;
      m_parser_mode = parser::Parser_mode::TABLE;
      break;

    // DOCUMENT Ops
    case OP_FIND:
    case OP_ADD:
    case OP_MODIFY:
    case OP_REMOVE:
      m_data_model = cdk::protocol::mysqlx::DOCUMENT;
      m_parser_mode = parser::Parser_mode::DOCUMENT;
      break;
    default:
      m_data_model = cdk::protocol::mysqlx::DEFAULT;
      m_parser_mode = parser::Parser_mode::TABLE;
    break;
  }
  m_group_by_list.set_parser_mode(m_parser_mode);
}

void mysqlx_stmt_t::copy_parent_data(mysqlx_stmt_struct *parent)
{
  if (parent->m_where.get())
    m_where.reset(new Expression_parser(*static_cast<parser::Expression_parser*>(parent->m_where.get())));

  if (parent->m_having.get())
    m_having.reset(new Expression_parser(*static_cast<parser::Expression_parser*>(parent->m_having.get())));

  if (parent->m_limit.get())
    m_limit.reset(new Limit(*parent->m_limit.get()));

  if (parent->m_order_by.get())
    m_order_by.reset(new Order_by(*parent->m_order_by.get()));

  if (parent->m_proj_list.get())
    m_proj_list.reset(new Projection_list(*parent->m_proj_list.get()));

    m_param_source = parent->m_param_source;
    m_group_by_list = parent->m_group_by_list;
}

void mysqlx_stmt_t::init_crud()
{
  m_session.reset_diagnostic();
}

/*
  Member function for binding values for parametrized SQL queries.
  This function should only be called by mysqlx_stmt_bind()

  PARAMETERS:
    args - variable list of parameters that follow as
           (mysqlx_data_type_t)type, value, ..., 0
           The list is closed by 0 value for type.

           NOTE: the list is already initialized in mysqlx_stmt_bind(),
                 so no need to call va_start()/va_end() in here.

  RETURN:
    RESULT_OK - on success
    RESULT_ERROR - on error

  NOTE: Each new call resets the binds set by the previous call to
        mysqlx_stmt_t::sql_bind()
*/
int mysqlx_stmt_t::sql_bind(va_list args)
{
  m_param_list.clear();
  // For variadic parameters mysqlx_data_type_t is used as value of void* pointer
  int64_t type = (int64_t)va_arg(args, void*);
  do
  {
    switch (type)
    {
      case MYSQLX_TYPE_NULL:
        m_param_list.add_null_value();
        break;
      case MYSQLX_TYPE_SINT:
        m_param_list.add_param_value(va_arg(args, int64_t));
        break;
      case MYSQLX_TYPE_UINT:
        m_param_list.add_param_value(va_arg(args, uint64_t));
        break;
      case MYSQLX_TYPE_FLOAT:
      case MYSQLX_TYPE_DOUBLE:
        // With variable parameters a float value is promoted to a double
        m_param_list.add_param_value(va_arg(args, double));
        break;
      case MYSQLX_TYPE_BOOL:
      {
        // With variable parameters a bool value is promoted to int
        m_param_list.add_param_value((bool)(va_arg(args, int) != 0));
      }
      break;
      case MYSQLX_TYPE_STRING:
        m_param_list.add_param_value(cdk::string(va_arg(args, char*)));
        break;
      case MYSQLX_TYPE_BYTES:
      {
        cdk::byte *cb = va_arg(args, cdk::byte*);
        m_param_list.add_param_value(cdk::bytes(cb, va_arg(args, size_t)));
      }
      break;
      default:
        set_diagnostic("Data type is not supported.", 0);
        return RESULT_ERROR;
    }
    type = (int64_t)va_arg(args, void*);
    // Spin in the loop until finding the end of the list or on an error
  } while (0 != (int)type);

  return RESULT_OK;
}

/*
  This function will not clear the parameter list
*/
int mysqlx_stmt_t::sql_bind(cdk::string s)
{
   m_param_list.add_param_value(s);
   return RESULT_OK;
}

/*
  Member function for binding values for parametrized CRUD queries.
  This function should only be called by mysqlx_stmt_bind()

  PARAMETERS:
    args - variable list of parameters.
           param_name, (mysqlx_data_type_t)type, value, ..., PARAM_END

           The list is closed by PARAM_END value

           NOTE: the list is already initialized in mysqlx_stmt_bind(),
                 so no need to call va_start()/va_end() in here.

  RETURN:
    RESULT_OK - on success
    RESULT_ERROR - on error

  NOTE: Each new call resets the binds set by the previous call to
        mysqlx_stmt_t::param_bind()
*/
int mysqlx_stmt_t::param_bind(va_list args)
{
  m_param_list.clear();
  m_param_source.clear();
  // For variadic parameters mysqlx_data_type_t is used as value of void* pointer
  char *param_name = 0;

  while((param_name = va_arg(args, char*)) != NULL)
  {
    int64_t type = (int64_t)va_arg(args, void*);
    switch (type)
    {
      case MYSQLX_TYPE_NULL:
          m_param_source.add_null_value(param_name);
        break;
      case MYSQLX_TYPE_SINT:
        m_param_source.add_param_value(param_name, va_arg(args, int64_t));
        break;
      case MYSQLX_TYPE_UINT:
        m_param_source.add_param_value(param_name, va_arg(args, uint64_t));
        break;
      case MYSQLX_TYPE_FLOAT:
      case MYSQLX_TYPE_DOUBLE:
        // With variable parameters a float value is promoted to a double
        m_param_source.add_param_value(param_name, va_arg(args, double));
        break;
      case MYSQLX_TYPE_BOOL:
      {
        // With variable parameters a bool value is promoted to int
        m_param_source.add_param_value(param_name, (bool)(va_arg(args, int) != 0));
      }
      break;
      case MYSQLX_TYPE_STRING:
        m_param_source.add_param_value(param_name, cdk::string(va_arg(args, char*)));
        break;
      case MYSQLX_TYPE_BYTES:
      {
        cdk::byte *cb = va_arg(args, cdk::byte*);
        size_t sz = va_arg(args, size_t);
        m_param_source.add_param_value(param_name, cdk::bytes(cb, sz));
      }
      break;
      default:
        set_diagnostic("Data type is not supported.", 0);
        return RESULT_ERROR;
    }
  }

  return RESULT_OK;
}

int mysqlx_stmt_t::add_columns(va_list args)
{
  if (m_op_type != OP_INSERT)
  {
    m_error.set("Wrong operation type. Only INSERT and ADD are supported.", 0);
    return RESULT_ERROR;
  }
  m_col_source.clear();
  const char *col_name = va_arg(args, char*);
  while (col_name)
  {
    m_col_source.add_column(col_name);
    col_name = va_arg(args, char*);
  }

  return RESULT_OK;

}

/*
  Member function for adding row values CRUD ADD.

  PARAMETERS:
    get_columns - flag indicating if the column information is present
	       inside args list
    args - variable list of parameters that contains row data, but
	       also can have column names (see get_columns parameter)

           NOTE: the list is already initialized in the upper level,
                 so no need to call va_start()/va_end() in here.

  RETURN:
    RESULT_OK - on success
    RESULT_ERROR - on error

  NOTE: Each new call resets the column and row values
*/
int mysqlx_stmt_t::add_row(bool get_columns, va_list args)
{
  if (m_op_type != OP_INSERT && m_op_type != OP_ADD)
  {
    m_error.set("Wrong operation type. Only INSERT and ADD are supported.", 0);
    return RESULT_ERROR;
  }

  m_row_source.add_new_row();

  // For variadic parameters mysqlx_data_type_t is used as value of void* pointer
  int64_t type;
  char *col_name = NULL;

  // Clearing columns for each column + row data call
  if (get_columns)
    m_col_source.clear();

  /*
    Getting column name only if the flag is set. Otherwise do not attempt it
    to avoid the stack corruption.
  */
  while((!get_columns || (col_name = va_arg(args, char*)) != NULL)
         && (type = (int64_t)va_arg(args, void*)) != 0)
  {

    if (get_columns)
    {
      m_col_source.add_column(col_name);
    }

    switch (type)
    {
      case MYSQLX_TYPE_NULL:
        m_row_source.add_null_value();
        break;
      case MYSQLX_TYPE_SINT:
        m_row_source.add_row_value(va_arg(args, int64_t));
        break;
      case MYSQLX_TYPE_UINT:
        m_row_source.add_row_value(va_arg(args, uint64_t));
        break;
      case MYSQLX_TYPE_FLOAT:
      case MYSQLX_TYPE_DOUBLE:
        // With variable parameters a float value is promoted to a double
        m_row_source.add_row_value(va_arg(args, double));
        break;
      case MYSQLX_TYPE_BOOL:
        // With variable parameters a bool value is promoted to int
        m_row_source.add_row_value((bool)(va_arg(args, int) != 0));
        break;
      case MYSQLX_TYPE_STRING:
        m_row_source.add_row_value(string(va_arg(args, char*)));
        break;
      case MYSQLX_TYPE_BYTES:
      {
        cdk::byte *cb = va_arg(args, cdk::byte*);
        m_row_source.add_row_value(cdk::bytes(cb, va_arg(args, size_t)));
      }
      break;
      default:
        m_error.set("Data type is not supported.", 0);
        return RESULT_ERROR;
    }
  }

  return RESULT_OK;
}

int mysqlx_stmt_t::add_projections(va_list args)
{
  char *item = NULL;
  if (m_op_type != OP_SELECT && m_op_type != OP_FIND)
  {
    m_error.set("Wrong operation type. Only SELECT and FIND are supported.", 0);
    return RESULT_ERROR;
  }
  /*
    Projections are allocated dynamically because they are optional
    and creating an empty non-working list affects other operations
  */
  m_proj_list.reset(new Projection_list(m_op_type));
  while ((item = (char*)va_arg(args, char*)) != NULL )
  {
    m_proj_list->add_value(item);
  }

  if (!m_proj_list->count())
    m_proj_list.release();

  return RESULT_OK;
}

int mysqlx_stmt_t::add_coll_modify_values(va_list args, mysqlx_modify_op modify_type)
{
  char *path = NULL;
  if (m_op_type != OP_MODIFY)
  {
    m_error.set("Wrong operation type. Only MODIFY is supported.", 0);
    return RESULT_ERROR;
  }

  while ((path = (char*)va_arg(args, char*)) != NULL )
  {
    if (modify_type == MODIFY_UNSET || modify_type == MODIFY_ARRAY_DELETE)
    {
      m_modify_spec.add_value(modify_type, path);
      continue;
    }

    int64_t val_type = (int64_t)va_arg(args, void*);

    switch (val_type)
    {
      case MYSQLX_TYPE_SINT:
        m_modify_spec.add_value(modify_type, path, false, va_arg(args, int64_t));
        break;
      case MYSQLX_TYPE_UINT:
        m_modify_spec.add_value(modify_type, path, false, va_arg(args, uint64_t));
        break;
      case MYSQLX_TYPE_FLOAT:
      case MYSQLX_TYPE_DOUBLE:
        // With variable parameters a float value is promoted to a double
        m_modify_spec.add_value(modify_type, path, false, va_arg(args, double));
        break;
      case MYSQLX_TYPE_BOOL:
        // With variable parameters a float value is promoted to int
        m_modify_spec.add_value(modify_type, path, false, (bool)(va_arg(args, int) != 0));
        break;
      case MYSQLX_TYPE_STRING:
        m_modify_spec.add_value(modify_type, path, false, string(va_arg(args, char*)));
        break;
      case MYSQLX_TYPE_EXPR:
        m_modify_spec.add_value(modify_type, path, true, string(va_arg(args, char*)));
        break;
      case MYSQLX_TYPE_BYTES:
      {
        cdk::byte *cb = va_arg(args, cdk::byte*);
        m_modify_spec.add_value(modify_type, path, false,
                                cdk::bytes(cb,
                                va_arg(args, size_t)));
      }
      break;
      case MYSQLX_TYPE_NULL:
        m_modify_spec.add_null_value(modify_type, path);
      break;
      default:
        m_error.set("Data type is not supported.", 0);
        return RESULT_ERROR;
    }
  }
  return RESULT_OK;
}

int mysqlx_stmt_t::add_table_update_values(va_list args)
{
  char *column = NULL;

  if (m_op_type != OP_UPDATE)
  {
    m_error.set("Wrong operation type. Only UPDATE is supported.", 0);
    return RESULT_ERROR;
  }
  while ((column = (char*)va_arg(args, char*)) != NULL )
  {
    int64_t data_type = (int64_t)va_arg(args, void*);
    switch(data_type)
    {
      case MYSQLX_TYPE_NULL:
        m_update_spec.add_null_value(column);
      break;
      case MYSQLX_TYPE_SINT:
        m_update_spec.add_value(column, va_arg(args, int64_t));
      break;
      case MYSQLX_TYPE_UINT:
        m_update_spec.add_value(column, va_arg(args, uint64_t));
      break;
      case MYSQLX_TYPE_FLOAT:
      case MYSQLX_TYPE_DOUBLE:
        m_update_spec.add_value(column, va_arg(args, double));
      break;
      case MYSQLX_TYPE_BOOL:
        m_update_spec.add_value(column, va_arg(args, int) != 0);
      break;
      case MYSQLX_TYPE_STRING:
        m_update_spec.add_value(column, string(va_arg(args, char*)));
      break;
      case MYSQLX_TYPE_BYTES:
      {
        // For bytes the code expects another parameter for length
        cdk::byte *cb = va_arg(args, cdk::byte*);
        m_update_spec.add_value(column, cdk::bytes(cb, va_arg(args, size_t)));
      }
      break;
      case MYSQLX_TYPE_EXPR:
        m_update_spec.add_expr(column, string(va_arg(args, char*)));
      break;
      default:
       m_error.set("Data type is not supported.", 0);
       return RESULT_ERROR;
    }
  }
  return RESULT_OK;
}

/*
  Execute a CRUD statement.
  RETURN: pointer to mysqlx_result_t, which is being allocated each time
          when this function is called. The old result is freed automatically.
          On error the function returns NULL

  NOTE: no need to free the result in the end cdk::scoped_ptr will
        take care of it
*/
mysqlx_result_t *mysqlx_stmt_t::exec()
{
  cdk::Session &sess = m_session.get_session();

  switch(m_op_type)
  {
    case OP_SELECT:
      m_reply = sess.table_select(m_db_obj_ref,
                                  NULL,               // view spec
                                  m_where.get(),
                                  m_proj_list.get(),
                                  m_order_by.get(),
                                  m_group_by_list.get_list(),
                                  m_having.get(),
                                  m_limit.get(),
                                  m_param_source.count() ? &m_param_source : NULL);
      break;
    case OP_VIEW_CREATE:
    case OP_VIEW_UPDATE:
    case OP_VIEW_REPLACE:
      m_reply = sess.table_select(m_db_obj_ref,
                                  &m_view_spec,
                                  m_where.get(),
                                  m_proj_list.get(),
                                  m_order_by.get(),
                                  m_group_by_list.get_list(),
                                  m_having.get(),
                                  m_limit.get(),
                                  m_param_source.count() ? &m_param_source : NULL);
      break;
    case OP_INSERT:
      {
        if (!m_row_source.row_count())
          throw Mysqlx_exception(Mysqlx_exception::MYSQLX_EXCEPTION_INTERNAL,
                                 0, "Missing row data for INSERT! Use mysqlx_set_insert_row()");
        Row_source *row_src = &m_row_source;
        m_reply = sess.table_insert(m_db_obj_ref, *row_src,
                                    m_col_source.count() ? &m_col_source : NULL,
                                    m_param_source.count() ? &m_param_source : NULL);
      }
      break;
    case OP_UPDATE:
        if (!m_update_spec.count())
          throw Mysqlx_exception(Mysqlx_exception::MYSQLX_EXCEPTION_INTERNAL,
                                 0, "Missing data for UPDATE! Use mysqlx_set_update_values()");

        m_reply = sess.table_update(m_db_obj_ref,
                                    m_where.get(),
                                    m_update_spec,
                                    m_order_by.get(),
                                    m_limit.get(),
                                    m_param_source.count() ? &m_param_source : NULL);
      break;
    case OP_DELETE:
      m_reply = sess.table_delete(m_db_obj_ref,
                                  m_where.get(),
                                  m_order_by.get(),
                                  m_limit.get(),
                                  m_param_source.count() ? &m_param_source : NULL);
      break;
    case OP_FIND:
      m_reply = sess.coll_find(m_db_obj_ref,
                               NULL,                // view spec
                               m_where.get(),
                               m_proj_list.get(),
                               m_order_by.get(),
                               m_group_by_list.get_list(),
                               m_having.get(),
                               m_limit.get(),
                               m_param_source.count() ? &m_param_source : NULL);
      break;
    case OP_ADD:
      if (!m_doc_source.count())
          throw Mysqlx_exception(Mysqlx_exception::MYSQLX_EXCEPTION_INTERNAL,
                                 0, "Missing JSON data for ADD! Use mysqlx_set_add_document()");
      m_reply = sess.coll_add(m_db_obj_ref,
                              m_doc_source,
                              m_param_source.count() ? &m_param_source : NULL);
      break;
    case OP_MODIFY:
      if (!m_modify_spec.count())
          throw Mysqlx_exception(Mysqlx_exception::MYSQLX_EXCEPTION_INTERNAL,
                                 0, "Missing data for MODIFY! Use mysqlx_set_modify_set(), " \
                                    "mysqlx_set_modify_unset(), mysqlx_set_modify_array_insert(), " \
                                    "mysqlx_set_modify_array_delete(), mysqlx_set_modify_array_append() " \
                                    "functions");
      m_reply = sess.coll_update(m_db_obj_ref,
                                 m_where.get(),
                                 m_modify_spec,
                                 m_order_by.get(),
                                 m_limit.get(),
                                 m_param_source.count() ? &m_param_source : NULL);
      break;
    case OP_REMOVE:
      m_reply = sess.coll_remove(m_db_obj_ref,
                                 m_where.get(),
                                 m_order_by.get(),
                                 m_limit.get(),
                                 m_param_source.count() ? &m_param_source : NULL);
      break;
    case OP_SQL:
      {
        Param_list *param_list = m_param_list.count() ? &m_param_list : NULL;
        m_reply = sess.sql(m_query, param_list);
      }
      break;
    case OP_ADMIN_LIST:
      {
        m_reply = sess.admin("list_objects", m_db_obj_ref);
      }
      break;
    default: // All other operations are not implemented
      return NULL;
  }
  m_result.reset(new mysqlx_result_t(*this, m_reply));

  m_reply.wait(); // wait for the operation to complete

  if (m_op_type == OP_ADD)
    m_result->copy_doc_ids(m_doc_source);

  m_col_source.clear();
  m_doc_source.clear();
  m_row_source.clear();
  m_update_spec.clear();
  m_modify_spec.clear();

  return m_result.get();
}

/*
  Set member to a CDK expression which results from parsing given string val
*/
int mysqlx_stmt_t::set_expression(cdk::scoped_ptr<cdk::Expression> &member,
                                  const char *val)
{
  if (!val || !val[0])
    return RESULT_OK;

  int res = RESULT_OK;
  try
  {
    member.reset(new Expression_parser(m_parser_mode, val));
    if (!member.get())
      res = RESULT_ERROR;
  }
  catch (...)
  {
    res = RESULT_ERROR;
    // TODO: Get exception details
  }
  return res;
}


/*
  Set WHERE for statement operation
  PARAMETERS:
    where_expr - character string containing WHERE clause,
                 which will be parsed as required

  RETURN:
    RESULT_OK - on success
    RESULT_ERROR - on error

  NOTE: each call to this function replaces previously set WHERE
*/
int mysqlx_stmt_t::set_where(const char *where_expr)
{
  if (m_op_type == OP_ADD || m_op_type == OP_INSERT)
    throw Mysqlx_exception(MYSQLX_ERROR_OP_NOT_SUPPORTED);
  return set_expression(m_where, where_expr);
}

/*
  Set HAVING for statement operation
  PARAMETERS:
    having_expr - character string containing HAVING clause,
                  which will be parsed as required

  RETURN:
    RESULT_OK - on success
    RESULT_ERROR - on error

  NOTE: each call to this function replaces previously set HAVING
*/
int mysqlx_stmt_t::set_having(const char *having_expr)
{
  if (m_op_type != OP_SELECT && m_op_type != OP_FIND)
    throw Mysqlx_exception(MYSQLX_ERROR_OP_NOT_SUPPORTED);
  return set_expression(m_having, having_expr);
}

/*
  Set LIMIT for CRUD operation
  PARAMETERS:
    row_count - the number of result rows to return
    offset - the number of rows to skip before starting counting

  RETURN:
    RESULT_OK - on success
    RESULT_ERROR - on error

  NOTE: each call to this function replaces previously set LIMIT
*/
int mysqlx_stmt_t::set_limit(row_count_t row_count, row_count_t offset)
{
  if (m_op_type == OP_ADD || m_op_type == OP_INSERT)
    throw Mysqlx_exception(MYSQLX_ERROR_OP_NOT_SUPPORTED);

  int res = RESULT_OK;
  try
  {
    m_limit.reset(new Limit(row_count, offset));
    if (!(m_limit.get()))
      res = RESULT_ERROR;
  }
  catch(...)
  {
    res = RESULT_ERROR;
    // TODO: Get exception details
  }
  return res;
}

/*
  Set one item in ORDER BY for CRUD operation
  PARAMETERS:
    order - the character string expression describing ONE item
    direction - sort direction

  RETURN:
    RESULT_OK - on success
    RESULT_ERROR - on error

  NOTE: each call to this function adds a new item to ORDER BY list
*/
int mysqlx_stmt_t::add_order_by(va_list args)
{
  char *item = NULL;
  mysqlx_sort_direction_t sort_direction;
  clear_order_by();
  do
  {
    item = va_arg(args, char*);
    if (item && *item)
    {
      // mysqlx_sort_direction_t is promoted to int
      sort_direction = (mysqlx_sort_direction_t)va_arg(args, int);
      // Initial setup is done only once
      if (!m_order_by.get())
        m_order_by.reset(new Order_by(m_parser_mode));

      if (m_order_by.get())
        m_order_by->add_item(item, (Sort_direction::value)sort_direction);
      else
      {
        set_diagnostic("Error creating ORDER BY instance", 0);
        return RESULT_ERROR;
      }
    }
  }
  while (item && *item);
  return RESULT_OK;
}

// Return the session validity state
bool mysqlx_stmt_t::session_valid()
{
  return m_session.is_valid();
}

void mysqlx_stmt_t::acquire_diag(cdk::foundation::api::Severity::value val)
{
  // Free and reset the previous error
  m_error.set(NULL);

  if (m_reply.entry_count(val))
  {
    // TODO: implement warnings and info using Diagnostic_iterator
    try
    {
      m_error.set(&m_reply.get_error());
    }
    catch(...)
    {
      // swallow the exception
    }
  }
}

void mysqlx_stmt_t::clear_order_by()
{
  if(m_order_by.get()) m_order_by->clear();
}

bool mysqlx_stmt_t::is_view_op()
{
  return (m_op_type == OP_VIEW_CREATE || m_op_type == OP_VIEW_UPDATE ||
          m_op_type == OP_VIEW_REPLACE);
}

void mysqlx_stmt_t::set_view_algorithm(int algorithm)
{
  if (!is_view_op())
    throw Mysqlx_exception(MYSQLX_ERROR_VIEW_TYPE_MSG);

  m_view_spec.set_algorithm(algorithm);
}

void mysqlx_stmt_t::set_view_security(int security)
{
  if (!is_view_op())
    throw Mysqlx_exception(MYSQLX_ERROR_VIEW_TYPE_MSG);

  m_view_spec.set_security(security);
}

void mysqlx_stmt_t::set_view_check_option(int option)
{
  if (!is_view_op())
    throw Mysqlx_exception(MYSQLX_ERROR_VIEW_TYPE_MSG);

  m_view_spec.set_check(option);
}

void mysqlx_stmt_t::set_view_definer(const char* user)
{
  if (!is_view_op())
    throw Mysqlx_exception(MYSQLX_ERROR_VIEW_TYPE_MSG);

  m_view_spec.set_definer(user);
}

void mysqlx_stmt_t::set_view_columns(va_list args)
{
  if (!is_view_op())
    throw Mysqlx_exception(MYSQLX_ERROR_VIEW_TYPE_MSG);

  m_view_spec.set_columns(args);
}

void mysqlx_stmt_t::set_view_properties(va_list args)
{
  int64_t view_option = 0;
  if (!is_view_op())
    throw Mysqlx_exception(MYSQLX_ERROR_VIEW_TYPE_MSG);

  /*
    The list of arguments is ending after the list of columns,
    so the function must stop getting more arguments
  */
  while (view_option != VIEW_OPTION_COLUMNS &&
         (view_option = (int64_t)va_arg(args, void*)) != 0)
  {
    switch (view_option)
    {
      case VIEW_OPTION_ALGORITHM:
        set_view_algorithm(va_arg(args, int));
      break;
      case VIEW_OPTION_SECURITY:
        set_view_security(va_arg(args, int));
      break;
      case VIEW_OPTION_CHECK_OPTION:
        set_view_check_option(va_arg(args, int));
      break;
      case VIEW_OPTION_DEFINER:
        set_view_definer(va_arg(args, const char*));
      break;
      case VIEW_OPTION_COLUMNS:
        set_view_columns(args);
      break;
      default:
        throw Mysqlx_exception("Wrong VIEW property");
    }
  }
}

mysqlx_stmt_t::~mysqlx_stmt_struct()
{
  try
  {
    if (m_result.get())
      m_result->close_cursor();
  } catch(...) {}

}

int mysqlx_stmt_t::add_document(const char *json_doc)
{
  int res = RESULT_OK;
  if (m_op_type != OP_ADD)
  {
    set_diagnostic("Wrong operation type. Only ADD is supported.", 0);
    return RESULT_ERROR;
  }

  if (!json_doc || !(*json_doc))
    throw Mysqlx_exception("Missing JSON data");
  /*
    This is done as a two-stage process add_new_doc()/add_doc_key_value()
    because in the future this function can support multiple arguments
  */
  m_doc_source.add_new_doc();
  try
  {
    m_doc_source.add_doc_value(json_doc);
  }
  catch (...)
  {
    // Something went wrong when parsing the JSON, remove the new doc
    m_doc_source.remove_last_row();
    throw;
  }

  return res;
}

int mysqlx_stmt_t::add_multiple_documents(va_list args)
{
  int rc = RESULT_OK;
  m_doc_source.clear();
  const char *json_doc;
  while ((json_doc = va_arg(args, char*)) != NULL && rc == RESULT_OK)
  {
    rc = add_document(json_doc);
  }
  return rc;
}

int mysqlx_stmt_t::add_group_by(va_list args)
{
  m_group_by_list.clear();
  if (m_op_type != OP_SELECT && m_op_type != OP_FIND)
  {
    set_diagnostic(MYSQLX_ERROR_OP_NOT_SUPPORTED, 0);
    return RESULT_ERROR;
  }

  const char *group_by;
  while ((group_by = va_arg(args, char*)) != NULL)
  {
    m_group_by_list.add_group_by(group_by);
  }
  return RESULT_OK;
}

void Group_by_list::process(cdk::Expr_list::Processor& prc) const
{
  prc.list_begin();

  for (group_by_list_type::const_iterator it = m_group_by_list.begin();
    it != m_group_by_list.end(); ++it)
  {
    parser::Expression_parser expr_parser(m_parser_mode, *it);
    expr_parser.process_if(prc.list_el());
  }
  prc.list_end();
}
