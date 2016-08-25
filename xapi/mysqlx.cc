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
#include <stdlib.h>
#include <string.h>
#include <iostream>

#define SAFE_EXCEPTION_BEGIN(HANDLE, ERR) try { \
  if (HANDLE == NULL) return ERR;

#define SAFE_EXCEPTION_END(HANDLE, ERR) } \
  catch(const cdk::Error &cdkerr) \
  { \
    HANDLE->set_diagnostic(cdkerr.what(), cdkerr.code().value()); \
    return ERR; \
  } \
  catch(const MYSQLX_EXCEPTION &mysqlx_ex) \
  { \
    HANDLE->set_diagnostic(mysqlx_ex); \
    return ERR; \
  } \
  catch(...) \
  { \
    HANDLE->set_diagnostic("Unknown error!", MYSQLX_ERR_UNKNOWN); \
    return ERR; \
  }

static MYSQLX_SESSION *
_get_session(const char *host, int port, const char *user,
             const char *password, const char *database, const char *conn_str,
             char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code,
             bool is_node_sess)
{
  MYSQLX_SESSION *sess = NULL;
  try
  {
    
    if(!conn_str)
    {
      std::string pwd(password ? password : "");
      std::string db(database ? database : "");
      // Temporarily set empty string for the default database
      sess = new MYSQLX_SESSION(host, port, user, password ? &pwd : NULL,
                                database ? &db : NULL , is_node_sess);
    }
    else
      sess = new MYSQLX_SESSION(conn_str, is_node_sess);

    if (!sess->is_valid())
    {
      const cdk::Error *err = sess->get_cdk_error();
      if (err)
        throw *err;
    }
  }
  catch(const cdk::Error &e)
  {
    if (out_error)
    {
      size_t cpy_len = strlen(e.what()) >= MYSQLX_MAX_ERROR_LEN - 1 ?
                       MYSQLX_MAX_ERROR_LEN - 1 : strlen(e.what());

       memcpy(out_error, e.what(), cpy_len);
       out_error[cpy_len] = '\0';
    }

    if (err_code)
      *err_code = e.code().value();
    if (sess)
    {
      delete sess;
      sess = NULL;
    }
  }
  catch(...)
  {
    if (out_error)
    {
      const char *unknown = "Unknown error";
      // Copy string termination byte too
      memcpy(out_error, unknown, strlen(unknown) + 1);
    }

    if (err_code)
      *err_code = 0;

    if (sess)
    {
      delete sess;
      sess = NULL;
    }
  }
  return sess;
}

/*
  Establish the X session using string options provided as function parameters
*/
MYSQLX_SESSION * STDCALL
mysqlx_get_session_s(const char *host, int port, const char *user,
               const char *password, const char *database,
               char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code)
{
  return _get_session(host, port, user, password, database,
                      NULL, out_error, err_code, false);
}

/*
  Establish the X session using the connection string
*/
MYSQLX_SESSION * STDCALL
mysqlx_get_session(const char *conn_string,
                   char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code)
{
  return _get_session(NULL, 0, NULL, NULL, NULL,
                      conn_string, out_error, err_code, false);
}

/*
  Establish the node session using string options provided as function parameters
*/
MYSQLX_SESSION * STDCALL
mysqlx_get_node_session_s(const char *host, int port, const char *user,
               const char *password, const char *database,
               char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code)
{
  return _get_session(host, port, user, password, database,
                      NULL, out_error, err_code, true);
}

/*
  Establish the node session using connection string
*/
MYSQLX_SESSION * STDCALL
mysqlx_get_node_session(const char *conn_string,
               char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code)
{
  return _get_session(NULL, 0, NULL, NULL, NULL,
                      conn_string, out_error, err_code, true);
}

/*
  Execute a plain SQL query (supports parameters and placeholders)
  PARAMETERS:
    sess - pointer to the current session
    query - SQL query
    length - length of the query

  RETURN:
    CRUD handler containing the results and/or error
    NULL is not supposed to be returned even in case of error.
    It is very unlikely for this function to end with the error
    because it does not do any parsing, parameters checking etc.
*/
MYSQLX_CRUD * STDCALL
mysqlx_sql_query(MYSQLX_SESSION *sess, const char *query,
                 uint32_t length)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  return sess->sql_query(query, length);

  SAFE_EXCEPTION_END(sess, NULL)
}

/*
  Function for binding values for parametrized queries.

  PARAMETERS:
    crud - pointer to CRUD
    ...  - variadic arguments that follow in sequences like
           type1, value1, type2, value2, ..., type_n, value_n, PARAM_END
           (PARAM_END marks the end of parameters list)

  RETURN:
    RESULT_OK - on success
    RESULT_ERROR - on error

  NOTE: Each new call resets the binds set by the previous call to
        MYSQLX_CRUD::sql_bind()
*/
int mysqlx_crud_bind(MYSQLX_CRUD *crud, ...)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  int res = RESULT_OK;
  va_list args;
  va_start(args, crud);

  /*
    Processing of SQL parameters and parameters for other CRUD operations
    is different. Therefore, use two different bind functions depending on
    the operation type.
  */
  if (crud->op_type() == OP_SQL)
    res = crud->sql_bind(args);
  else
    res = crud->param_bind(args);

  va_end(args);
  return res;

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}


int mysqlx_set_insert_row(MYSQLX_CRUD *crud, ...)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  int res = RESULT_OK;
  va_list args;
  va_start(args, crud);
  // Just row data, no columns in args
  res = crud->add_row(false, args);
  va_end(args);
  return res;

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}

int mysqlx_set_insert_columns(MYSQLX_CRUD *crud, ...)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  int res = RESULT_OK;
  va_list args;
  va_start(args, crud);
  res = crud->add_columns(args);
  va_end(args);
  return res;

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}

int STDCALL
mysqlx_set_add_document(MYSQLX_CRUD *crud, const char *json_doc)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  int res = RESULT_OK;
  res = crud->add_document(json_doc);
  return res;

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}


MYSQLX_CRUD * STDCALL
mysqlx_table_select_new(MYSQLX_SESSION *sess, const char *schema,
                        const char *table)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  return sess->crud_op(schema, table, OP_SELECT);

  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_CRUD * STDCALL
mysqlx_table_insert_new(MYSQLX_SESSION *sess, const char *schema,
                        const char *table)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  return sess->crud_op(schema, table, OP_INSERT);

  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_CRUD * STDCALL
mysqlx_table_update_new(MYSQLX_SESSION *sess, const char *schema,
                        const char *table)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  return sess->crud_op(schema, table, OP_UPDATE);

  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_CRUD * STDCALL
mysqlx_table_delete_new(MYSQLX_SESSION *sess, const char *schema,
                        const char *table)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  return sess->crud_op(schema, table, OP_DELETE);

  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_CRUD * STDCALL
mysqlx_collection_add_new(MYSQLX_SESSION *sess,
                      const char *schema,
                      const char *collection)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  return sess->crud_op(schema, collection, OP_ADD);

  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_CRUD * STDCALL
mysqlx_collection_modify_new(MYSQLX_SESSION *sess,
                      const char *schema,
                      const char *collection)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  return sess->crud_op(schema, collection, OP_MODIFY);

  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_CRUD * STDCALL
mysqlx_collection_remove_new(MYSQLX_SESSION *sess,
                      const char *schema,
                      const char *collection)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  return sess->crud_op(schema, collection, OP_REMOVE);

  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_CRUD * STDCALL
mysqlx_collection_find_new(MYSQLX_SESSION *sess,
                       const char *schema,
                       const char *collection)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  return sess->crud_op(schema, collection, OP_FIND);

  SAFE_EXCEPTION_END(sess, NULL)
}

int STDCALL mysqlx_set_where(MYSQLX_CRUD *crud, const char *where_expr)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  return crud->set_where(where_expr);

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}

int STDCALL
mysqlx_set_limit_and_offset(MYSQLX_CRUD *crud, uint64_t row_count,
                            uint64_t offset)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  return crud->set_limit(row_count, offset);

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}

/*
  Set ORDER BY clause for CRUD operation
  Operations supported by this function:
    SELECT, FIND, UPDATE, MODIFY, DELETE, REMOVE

  Calling it for INSERT or ADD will result in an error

  PARAMETERS:
    crud - pointer to CRUD structure
    ... - variable parameters list consisting of (expression, direction) pairs
          terminated by PARAM_END:

          expr_1, direction_1, ..., expr_n, direction_n, PARAM_END
          (PARAM_END marks the end of parameters list)
          Each expression computes value used to sort
          the rows/documents in ascending or descending order,
          as determined by direction constant
          (list the direction enum names).

  RETURN:
    RESULT_OK - on success
    RESULT_ERROR - on error

  NOTE: this function is not supposed to be used directly.
        For SELECT operation the user code should use
        mysqlx_set_select_xxxx() macros that map the
        corresponding mysqlx_set_xxxx() functions.
        This way the unsupported operations will not be used.

        Eeach call of this function replaces previously set ORDER BY
*/
int STDCALL mysqlx_set_order_by(MYSQLX_CRUD *crud, ...)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, crud);
  res = crud->add_order_by(args);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}

int STDCALL mysqlx_set_items(MYSQLX_CRUD *crud, ...)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, crud);
  res = crud->add_projections(args);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}

int STDCALL mysqlx_set_find_projection(MYSQLX_CRUD *crud, const char *proj)
{
  /*
    The call to mysqlx_set_items will take care of exceptions
    and operations validity
  */
  return mysqlx_set_items(crud, proj, PARAM_END);
}

/*
  Execute a CRUD statement.
  PARAMETERS:
    crud - pointer to CRUD structure

  RETURN:  A MYSQL_RESULT handle that can be used to access results
           of the operation. Returned handle is valid until the CRUD
           handle is freed (when session is closed or explicitly with
           mysqlx_crud_free()) or until another call to mysqlx_crud_execute()
           on the same CRUD handle is made. It is also possible to close
           a RESULT hanlde and free all resources used by it earlier with
           mysqlx_result_free() call.

           On error NULL is returned.
*/
MYSQLX_RESULT * STDCALL mysqlx_crud_execute(MYSQLX_CRUD *crud)
{
  SAFE_EXCEPTION_BEGIN(crud, NULL)

  if (!crud || !crud->session_valid())
    return NULL;

  try
  {
    return crud->exec();
  }
  catch(MYSQLX_EXCEPTION &ex)
  {
    if (ex.type() == MYSQLX_EXCEPTION::MYSQLX_EXCEPTION_EXTERNAL)
    {
      /*
        For external server errors, the diagnostic info
        is obtained from the cdk::Reply instance
      */
      crud->acquire_diag();
    }
    else
    {
      crud->set_diagnostic(ex);
    }
  }
  return NULL;

  SAFE_EXCEPTION_END(crud, NULL)
}

int STDCALL STDCALL mysqlx_set_update_values(MYSQLX_CRUD *crud, ...)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, crud);
  res = crud->add_table_update_values(args);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}

int STDCALL mysqlx_set_modify_set(MYSQLX_CRUD *crud, ...)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, crud);
  res = crud->add_coll_modify_values(args, MODIFY_SET);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}

int STDCALL STDCALL mysqlx_set_modify_unset(MYSQLX_CRUD *crud, ...)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, crud);
  res = crud->add_coll_modify_values(args, MODIFY_UNSET);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}


int STDCALL STDCALL mysqlx_set_modify_array_insert(MYSQLX_CRUD *crud, ...)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, crud);
  res = crud->add_coll_modify_values(args, MODIFY_ARRAY_INSERT);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}


int STDCALL STDCALL mysqlx_set_modify_array_append(MYSQLX_CRUD *crud, ...)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, crud);
  res = crud->add_coll_modify_values(args, MODIFY_ARRAY_APPEND);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}


int STDCALL STDCALL mysqlx_set_modify_array_delete(MYSQLX_CRUD *crud, ...)
{
  SAFE_EXCEPTION_BEGIN(crud, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, crud);
  res = crud->add_coll_modify_values(args, MODIFY_ARRAY_DELETE);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(crud, RESULT_ERROR)
}

/*
  Fetch one row from the result and advance to the next row
  PARAMETERS:
    res - pointer to the result structure

  RETURN: pointer to MYSQLX_ROW or NULL if no more rows left
*/
MYSQLX_ROW * STDCALL mysqlx_row_fetch_one(MYSQLX_RESULT *res)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)

  return res->read_row();

  SAFE_EXCEPTION_END(res, NULL)
}

MYSQLX_DOC * STDCALL mysqlx_doc_fetch_one(MYSQLX_RESULT *res)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)

  return res->read_doc();

  SAFE_EXCEPTION_END(res, NULL)
}

const char * STDCALL mysqlx_json_fetch_one(MYSQLX_RESULT *res, size_t *length)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)

  return res->read_json(length);

  SAFE_EXCEPTION_END(res, NULL)
}

int STDCALL
mysqlx_doc_get_uint(MYSQLX_DOC *doc, const char *key, uint64_t *out)
{
  SAFE_EXCEPTION_BEGIN(doc, RESULT_ERROR)

  try
  {
    *out = doc->get_uint(key);
  }
  catch(MYSQLX_EXCEPTION &ex)
  {
    doc->set_diagnostic(ex);
    return RESULT_ERROR;
  }
  return RESULT_OK;

  SAFE_EXCEPTION_END(doc, RESULT_ERROR)
}

int STDCALL
mysqlx_doc_get_sint(MYSQLX_DOC *doc, const char *key, int64_t *out)
{
  SAFE_EXCEPTION_BEGIN(doc, RESULT_ERROR)

  try
  {
    *out = doc->get_sint(key);
  }
  catch(MYSQLX_EXCEPTION &ex)
  {
    doc->set_diagnostic(ex);
    return RESULT_ERROR;
  }
  return RESULT_OK;

  SAFE_EXCEPTION_END(doc, RESULT_ERROR)
}

int STDCALL
mysqlx_doc_get_float(MYSQLX_DOC *doc, const char *key, float *out)
{
  SAFE_EXCEPTION_BEGIN(doc, RESULT_ERROR)

  try
  {
    *out = doc->get_float(key);
  }
  catch(MYSQLX_EXCEPTION &ex)
  {
    doc->set_diagnostic(ex);
    return RESULT_ERROR;
  }
  return RESULT_OK;

  SAFE_EXCEPTION_END(doc, RESULT_ERROR)
}

int STDCALL
mysqlx_doc_get_double(MYSQLX_DOC *doc, const char *key, double *out)
{
  SAFE_EXCEPTION_BEGIN(doc, RESULT_ERROR)

  try
  {
    *out = doc->get_double(key);
  }
  catch(MYSQLX_EXCEPTION &ex)
  {
    doc->set_diagnostic(ex);
    return RESULT_ERROR;
  }
  return RESULT_OK;

  SAFE_EXCEPTION_END(doc, RESULT_ERROR)
}

static int
_doc_get_bytes(MYSQLX_DOC* doc, const char *key, uint64_t offset,
               void *buf, size_t *buf_len, MYSQLX_DATA_TYPE type)
{
  SAFE_EXCEPTION_BEGIN(doc, RESULT_ERROR)

  try
  {
    switch (type)
    {
      case MYSQLX_TYPE_BYTES:
      {
        cdk::bytes b = doc->get_bytes(key);

        if (offset >= b.size())
        {
          // Report 0 bytes written in the buffer and do nothing else
          *buf_len = 0;
          return RESULT_OK;
        }

        if (b.size() == 0)
          return RESULT_NULL;

        if (b.size() - offset < *buf_len)
          *buf_len = b.size() - (size_t)offset;
        memcpy(buf, b.begin() + offset, *buf_len);
      }
      break;

      case MYSQLX_TYPE_STRING:
      {
        std::string str = doc->get_string(key);
        char *cbuf = (char *)buf;

        if (str.length() - offset + 1 < *buf_len)
          *buf_len = str.length() - (size_t)offset + 1;

        memcpy(cbuf, str.data() + offset, *buf_len);
        cbuf[*buf_len - 1] = '\0';
      }
      break;

      default:
        doc->set_diagnostic("Data cannot be converted!", 0);
        return RESULT_ERROR;
    }
  }
  catch(MYSQLX_EXCEPTION &ex)
  {
    doc->set_diagnostic(ex);
    return RESULT_ERROR;
  }
  return RESULT_OK;

  SAFE_EXCEPTION_END(doc, RESULT_ERROR)
}

int STDCALL
mysqlx_doc_get_bytes(MYSQLX_DOC* doc, const char *key, uint64_t offset,
                     void *buf, size_t *buf_len)
{
  return _doc_get_bytes(doc, key, offset, buf, buf_len,
                        MYSQLX_TYPE_BYTES);
}

int STDCALL
mysqlx_doc_get_str(MYSQLX_DOC* doc, const char *key, uint64_t offset,
                     char *buf, size_t *buf_len)
{
  return _doc_get_bytes(doc, key, offset, (void*)buf, buf_len,
                        MYSQLX_TYPE_STRING);
}

bool STDCALL mysqlx_doc_key_exists(MYSQLX_DOC *doc, const char *key)
{
  return doc->key_exists(key);
}

uint16_t STDCALL mysqlx_doc_get_type(MYSQLX_DOC *doc, const char *key)
{
  SAFE_EXCEPTION_BEGIN(doc, MYSQLX_TYPE_UNDEFINED)
    return doc->get_type(key);
  SAFE_EXCEPTION_END(doc, MYSQLX_TYPE_UNDEFINED)
}

int STDCALL
mysqlx_store_result(MYSQLX_RESULT *result, size_t *num)
{
  SAFE_EXCEPTION_BEGIN(result, RESULT_ERROR)
  *num = result->store_result();
  if (mysqlx_crud_error(&result->get_crud()))
    return RESULT_ERROR;

  return RESULT_OK;
  SAFE_EXCEPTION_END(result, RESULT_ERROR)
}

#define CHECK_COLUMN_RANGE(COL, ROW) if (COL >= ROW->row_size()) \
{ \
  row->set_diagnostic(MYSQLX_ERROR_INDEX_OUT_OF_RANGE_MSG, \
                      MYSQLX_ERROR_INDEX_OUT_OF_RANGE); \
  return RESULT_ERROR; \
}

int STDCALL mysqlx_get_bytes(MYSQLX_ROW* row, uint32_t col, uint64_t offset,
                             void *buf, size_t *buf_len)
{
  SAFE_EXCEPTION_BEGIN(row, RESULT_ERROR)

  CHECK_COLUMN_RANGE(col, row)
  cdk::bytes b = row->get_col_data(col);

  if (b.size() == 0)
    return RESULT_NULL;

  if (offset >= b.size())
  {
    // Report 0 bytes written in the buffer and do nothing else
    *buf_len = 0;
    return RESULT_OK;
  }

  if (b.size() - offset < *buf_len)
    *buf_len = b.size() - (size_t)offset;
  memcpy(buf, b.begin() + offset, *buf_len);
  return RESULT_OK;

  SAFE_EXCEPTION_END(row, RESULT_ERROR)
}

int STDCALL mysqlx_get_uint(MYSQLX_ROW* row, uint32_t col, uint64_t *val)
{
  SAFE_EXCEPTION_BEGIN(row, RESULT_ERROR)

  uint64_t v;

  CHECK_COLUMN_RANGE(col, row)
  if (row->get_col_data(col).size() == 0)
    return RESULT_NULL;

  cdk::Codec<cdk::TYPE_INTEGER> codec(row->get_result().get_cursor()->format(col));
  codec.from_bytes(row->get_col_data(col), v);
  *val = v;
  return RESULT_OK;

  SAFE_EXCEPTION_END(row, RESULT_ERROR)
}

int STDCALL mysqlx_get_sint(MYSQLX_ROW* row, uint32_t col, int64_t *val)
{
  SAFE_EXCEPTION_BEGIN(row, RESULT_ERROR)

  CHECK_COLUMN_RANGE(col, row)
  if (row->get_col_data(col).size() == 0)
    return RESULT_NULL;

  cdk::Codec<cdk::TYPE_INTEGER> codec(row->get_result().get_cursor()->format(col));
  codec.from_bytes(row->get_col_data(col), *val);
  return RESULT_OK;

  SAFE_EXCEPTION_END(row, RESULT_ERROR)
}

int STDCALL mysqlx_get_float(MYSQLX_ROW* row, uint32_t col, float *val)
{
  SAFE_EXCEPTION_BEGIN(row, RESULT_ERROR)

  CHECK_COLUMN_RANGE(col, row)
  if (row->get_col_data(col).size() == 0)
    return RESULT_NULL;

  cdk::Codec<cdk::TYPE_FLOAT> codec(row->get_result().get_cursor()->format(col));
  codec.from_bytes(row->get_col_data(col), *val);
  return RESULT_OK;

  SAFE_EXCEPTION_END(row, RESULT_ERROR)
}

int STDCALL mysqlx_get_double(MYSQLX_ROW* row, uint32_t col, double *val)
{
  SAFE_EXCEPTION_BEGIN(row, RESULT_ERROR)

  CHECK_COLUMN_RANGE(col, row)
    if (row->get_col_data(col).size() == 0)
      return RESULT_NULL;

  cdk::Codec<cdk::TYPE_FLOAT> codec(row->get_result().get_cursor()->format(col));
  codec.from_bytes(row->get_col_data(col), *val);
  return RESULT_OK;

  SAFE_EXCEPTION_END(row, RESULT_ERROR)
}

/*
  Get the number of columns in the result
  PARAMETERS:
    res - pointer to the result structure

  RETURN: the number of columns
*/
uint32_t STDCALL mysqlx_column_get_count(MYSQLX_RESULT *res)
{
  SAFE_EXCEPTION_BEGIN(res, 0)
  cdk::Cursor *pcursor;
  if ((pcursor = res->get_cursor()) == NULL)
    return 0;

  return pcursor->col_count();
  SAFE_EXCEPTION_END(res, 0)
}

/*
  Get column name
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: character string containing column name
*/
const char * STDCALL mysqlx_column_get_name(MYSQLX_RESULT *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->column_get_info_char(pos, MYSQLX_RESULT::COL_INFO_NAME);
  SAFE_EXCEPTION_END(res, NULL)
}

/*
  Get column original name
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: character string containing column original name
*/
const char * STDCALL mysqlx_column_get_original_name(MYSQLX_RESULT *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->column_get_info_char(pos, MYSQLX_RESULT::COL_INFO_ORIG_NAME);
  SAFE_EXCEPTION_END(res, NULL)
}

/*
  Get column table name
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: character string containing column table name
*/
const char * STDCALL mysqlx_column_get_table(MYSQLX_RESULT *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->column_get_info_char(pos, MYSQLX_RESULT::COL_INFO_TABLE);
  SAFE_EXCEPTION_END(res, NULL)
}

/*
  Get column original table name
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: character string containing column original table name
*/
const char * STDCALL mysqlx_column_get_original_table(MYSQLX_RESULT *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->column_get_info_char(pos, MYSQLX_RESULT::COL_INFO_ORIG_TABLE);
  SAFE_EXCEPTION_END(res, NULL)
}

/*
  Get column schema name
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: character string containing column schema name
*/
const char * STDCALL mysqlx_column_get_schema(MYSQLX_RESULT *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->column_get_info_char(pos, MYSQLX_RESULT::COL_INFO_SCHEMA);
  SAFE_EXCEPTION_END(res, NULL)
}

/*
  Get column catalog name
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: character string containing column catalog name
*/
const char * STDCALL mysqlx_column_get_catalog(MYSQLX_RESULT *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->column_get_info_char(pos, MYSQLX_RESULT::COL_INFO_CATALOG);
  SAFE_EXCEPTION_END(res, NULL)
}

/*
  Get column type identifier
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: 16-bit unsigned int number with the column type identifier
*/
uint16_t STDCALL mysqlx_column_get_type(MYSQLX_RESULT *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, MYSQLX_TYPE_UNDEFINED)
  return (uint16_t)res->column_get_info_int(pos, MYSQLX_RESULT::COL_INFO_TYPE);
  SAFE_EXCEPTION_END(res, MYSQLX_TYPE_UNDEFINED)
}

/*
  Get column collation number
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: 16-bit unsigned int number with the column collation number
*/
uint16_t STDCALL mysqlx_column_get_collation(MYSQLX_RESULT *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, MYSQLX_COLLATION_UNDEFINED)
  return (uint16_t)res->column_get_info_int(pos, MYSQLX_RESULT::COL_INFO_COLLATION);
  SAFE_EXCEPTION_END(res, MYSQLX_COLLATION_UNDEFINED)
}

/*
  Get column length
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: 32-bit unsigned int number indicating the maximum data length
*/
uint32_t STDCALL mysqlx_column_get_length(MYSQLX_RESULT *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, 0)
  return res->column_get_info_int(pos, MYSQLX_RESULT::COL_INFO_LENGTH);
  SAFE_EXCEPTION_END(res, 0)
}

/*
  Get column precision
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: 16-bit unsigned int number of digits after the decimal point
*/
uint16_t STDCALL mysqlx_column_get_precision(MYSQLX_RESULT *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, 0)
  return (uint16_t)res->column_get_info_int(pos, MYSQLX_RESULT::COL_INFO_PRECISION);
  SAFE_EXCEPTION_END(res, 0)
}

/*
  Get column flags
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: 32-bit unsigned int number containing column flags
*/
uint32_t STDCALL mysqlx_column_get_flags(MYSQLX_RESULT *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, 0)
  return res->column_get_info_int(pos, MYSQLX_RESULT::COL_INFO_FLAGS);
  SAFE_EXCEPTION_END(res, 0)
}

/*
  Get number of rows affected by the last operation
  PARAMETERS:
    res - pointer to the result structure

  RETURN: 64-bit unsigned int number containing the number of affected rows
*/
uint64_t STDCALL mysqlx_get_affected_count(MYSQLX_RESULT *res)
{
  SAFE_EXCEPTION_BEGIN(res, 0)
  return res->get_affected_count();
  SAFE_EXCEPTION_END(res, 0)
}

/*
  Free the CRUD explicitly, otherwise it will be done automatically
  when session is closed
*/
void STDCALL mysqlx_crud_free(MYSQLX_CRUD *crud)
{
  if (crud)
    crud->get_session().reset_crud(crud);
}


int STDCALL mysqlx_next_result(MYSQLX_RESULT *res)
{
  SAFE_EXCEPTION_BEGIN(res, RESULT_ERROR)
  return res->next_result() ? RESULT_OK : RESULT_NULL;
  SAFE_EXCEPTION_END(res, RESULT_ERROR)
}

/*
  Free the result explicitly, otherwise it will be done automatically
  when CRUD handler is destroyed
*/
void STDCALL mysqlx_result_free(MYSQLX_RESULT *res)
{
  if (res && res->get_crud().set_result(NULL))
    delete res;
}

/*
  Closing the session.
  This function must be called by the user to prevent memory leaks.
*/
void STDCALL mysqlx_session_close(MYSQLX_SESSION *sess)
{
  if (sess) delete sess;
}

MYSQLX_ERROR * STDCALL mysqlx_crud_error(MYSQLX_CRUD *crud)
{
  SAFE_EXCEPTION_BEGIN(crud, NULL)
  return crud->get_last_error();
  SAFE_EXCEPTION_END(crud, NULL)
}

MYSQLX_ERROR * STDCALL mysqlx_session_error(MYSQLX_SESSION *sess)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  return sess->get_last_error();
  SAFE_EXCEPTION_END(sess, NULL)
}

const char * STDCALL mysqlx_crud_error_message(MYSQLX_CRUD *crud)
{
  MYSQLX_ERROR *err = mysqlx_crud_error(crud);
  if (err)
    return err->message();

  return NULL;
}

const char * STDCALL mysqlx_error_message(MYSQLX_ERROR *error)
{
  if (error)
    return error->message();

  return NULL;
}


unsigned int STDCALL mysqlx_error_num(MYSQLX_ERROR *error)
{
  if (error)
    return error->error_num();

  return 0;
}

int STDCALL
mysqlx_schema_create(MYSQLX_SESSION *sess, const char *schema)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->create_schema(schema);
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

int STDCALL
mysqlx_schema_drop(MYSQLX_SESSION *sess, const char *schema)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->drop_object(schema, NULL, MYSQLX_SESSION::SCHEMA);
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

int STDCALL
mysqlx_table_drop(MYSQLX_SESSION *sess, const char *schema,
                  const char *table)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->drop_object(schema, table, MYSQLX_SESSION::TABLE);
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

int STDCALL
mysqlx_view_drop(MYSQLX_SESSION *sess, const char *schema,
                 const char *view)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->drop_object(schema, view, MYSQLX_SESSION::VIEW);
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

int STDCALL
mysqlx_collection_create(MYSQLX_SESSION *sess,
                      const char *schema, const char *collection)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->admin_collection("create_collection", schema, collection);
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

int STDCALL
mysqlx_collection_drop(MYSQLX_SESSION *sess,
                      const char *schema, const char *collection)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->drop_object(schema, collection, MYSQLX_SESSION::COLLECTION);
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

/*
  CRUD will be unavailable outside the function, set session error
  and return
*/
#define SET_SESSION_ERROR_FROM_CRUD(S, C, R) do { \
   MYSQLX_ERROR *err = C->get_last_error(); \
   if (err) \
      S->set_diagnostic(err->message(), err->error_num()); \
    else \
      S->set_diagnostic("Unknown error!", 0); \
    return R; \
  } while (0)

MYSQLX_RESULT * STDCALL mysqlx_sql_exec(MYSQLX_SESSION *sess,
                                        const char *query,
                                        size_t query_len)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  MYSQLX_CRUD *crud = sess->sql_query(query, query_len);
  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL mysqlx_sql_exec_param(MYSQLX_SESSION *sess,
                                        const char *query,
                                        size_t query_len, ...)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  int rc = RESULT_OK;
  MYSQLX_CRUD *crud;

  if ((crud = sess->sql_query(query, query_len)) == NULL)
    return NULL;

  va_list args;
  va_start(args, query_len);
  rc = crud->sql_bind(args);
  va_end(args);
  
  if (rc != RESULT_OK)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL
mysqlx_table_select_exec(MYSQLX_SESSION *sess, const char *schema,
                        const char *table, const char *criteria)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  MYSQLX_CRUD *crud;

  if ((crud = sess->crud_op(schema, table, OP_SELECT)) == NULL)
    return NULL;

  if (RESULT_OK != crud->set_where(criteria))
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL
mysqlx_table_select_exec_limit(MYSQLX_SESSION *sess, const char *schema,
                               const char *table, const char *criteria,
                               uint64_t row_count, uint64_t offset, ...)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  MYSQLX_CRUD *crud;
  int rc = RESULT_OK;

  if ((crud = sess->crud_op(schema, table, OP_SELECT)) == NULL)
    return NULL;

  if (RESULT_OK != crud->set_where(criteria))
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  if (RESULT_OK != crud->set_limit(row_count, offset))
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  va_list args;
  va_start(args, offset);
  rc= crud->add_order_by(args);
  va_end(args);

  if (rc != RESULT_OK)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL
mysqlx_table_insert_exec(MYSQLX_SESSION *sess,
                        const char *schema,
                        const char *table, ...)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  MYSQLX_CRUD *crud;
  int rc = RESULT_OK;

  if ((crud = sess->crud_op(schema, table, OP_INSERT)) == NULL)
    return NULL;

  va_list args;
  va_start(args, table);
  /*
    Parameters are triplets: <column name, value type, value>
  */
  rc= crud->add_row(true, args);
  va_end(args);

  if (rc != RESULT_OK)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL
mysqlx_table_update_exec(MYSQLX_SESSION *sess,
                        const char *schema,
                        const char *table,
                        const char *criteria,
                        ...)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  MYSQLX_CRUD *crud;
  int rc = RESULT_OK;

  if ((crud = sess->crud_op(schema, table, OP_UPDATE)) == NULL)
    return NULL;

  if (RESULT_OK != crud->set_where(criteria))
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  va_list args;
  va_start(args, criteria);
  /*
    Parameters are triplets: <column name, value type, value>
  */
  rc= crud->add_table_update_values(args);
  va_end(args);

  if (rc != RESULT_OK)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL
mysqlx_table_delete_exec(MYSQLX_SESSION *sess, const char *schema,
                        const char *table, const char *criteria)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  MYSQLX_CRUD *crud;

  if ((crud = sess->crud_op(schema, table, OP_DELETE)) == NULL)
    return NULL;

  if (RESULT_OK != crud->set_where(criteria))
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL
mysqlx_collection_find_exec(MYSQLX_SESSION *sess, const char *schema,
                        const char *collection, const char *criteria)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  MYSQLX_CRUD *crud;

  if ((crud = sess->crud_op(schema, collection, OP_FIND)) == NULL)
    return NULL;

  if (RESULT_OK != crud->set_where(criteria))
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL
mysqlx_collection_add_exec(MYSQLX_SESSION *sess, const char *schema,
                        const char *collection, ...)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  MYSQLX_CRUD *crud;
  int rc = RESULT_OK;

  if ((crud = sess->crud_op(schema, collection, OP_ADD)) == NULL)
    return NULL;

  va_list args;
  va_start(args, collection);
  rc= crud->add_multiple_documents(args);
  va_end(args);

  if (rc != RESULT_OK)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL
_mysqlx_collection_modify_exec(MYSQLX_SESSION *sess, const char *schema,
                        const char *collection, const char *criteria,
                        MYSQLX_MODIFY_OP modify_op, va_list args)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  MYSQLX_CRUD *crud;
  int rc = RESULT_OK;

  if ((crud = sess->crud_op(schema, collection, OP_MODIFY)) == NULL)
    return NULL;

  if (RESULT_OK != crud->set_where(criteria))
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  rc= crud->add_coll_modify_values(args, modify_op);

  if (rc != RESULT_OK)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL
mysqlx_collection_modify_set_exec(MYSQLX_SESSION *sess, const char *schema,
                                  const char *collection, const char *criteria,
                                  ...)
{
  MYSQLX_RESULT *res;
  va_list args;
  va_start(args, criteria);
  res = _mysqlx_collection_modify_exec(sess, schema, collection, criteria,
                                 MODIFY_SET, args);
  va_end(args);
  return res;
}

MYSQLX_RESULT * STDCALL
mysqlx_collection_modify_unset_exec(MYSQLX_SESSION *sess, const char *schema,
                                  const char *collection, const char *criteria,
                                  ...)
{
  MYSQLX_RESULT *res;
  va_list args;
  va_start(args, criteria);
  res = _mysqlx_collection_modify_exec(sess, schema, collection, criteria,
                                 MODIFY_UNSET, args);
  va_end(args);
  return res;
}

MYSQLX_RESULT * STDCALL
mysqlx_collection_remove_exec(MYSQLX_SESSION *sess, const char *schema,
                              const char *collection, const char*criteria)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  MYSQLX_CRUD *crud;

  if ((crud = sess->crud_op(schema, collection, OP_REMOVE)) == NULL)
    return NULL;

  if (RESULT_OK != crud->set_where(criteria))
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL
mysqlx_get_tables(MYSQLX_SESSION *sess,
                  const char *schema,
                  const char *table_pattern,
                  int show_views)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  MYSQLX_CRUD *crud;

  if ((crud = sess->crud_op(schema, table_pattern ? table_pattern : "",
                            OP_ADMIN_LIST)) == NULL)
    return NULL;

  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  res->set_table_list_mask(FILTER_TABLE | (show_views ? FILTER_VIEW : 0));

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL
mysqlx_get_collections(MYSQLX_SESSION *sess,
                  const char *schema,
                  const char *col_pattern)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  MYSQLX_CRUD *crud;

  if ((crud = sess->crud_op(schema, col_pattern ? col_pattern : "",
                            OP_ADMIN_LIST)) == NULL)
    return NULL;

  MYSQLX_RESULT *res = mysqlx_crud_execute(crud);
  if (res == NULL)
    SET_SESSION_ERROR_FROM_CRUD(sess, crud, NULL);

  res->set_table_list_mask(FILTER_COLLECTION);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

MYSQLX_RESULT * STDCALL
  mysqlx_get_schemas(MYSQLX_SESSION *sess, const char *schema_pattern)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  return mysqlx_sql_exec_param(sess, "SHOW SCHEMAS LIKE ?",
                               MYSQLX_NULL_TERMINATED,
                               PARAM_STRING(schema_pattern ? schema_pattern : "%"),
                               PARAM_END);

  SAFE_EXCEPTION_END(sess, NULL)
}

unsigned int STDCALL
mysqlx_result_warning_count(MYSQLX_RESULT *result)
{
  SAFE_EXCEPTION_BEGIN(result, 0)
  return (unsigned int)result->get_warning_count();
  SAFE_EXCEPTION_END(result, 0)
}

MYSQLX_ERROR * STDCALL
mysqlx_result_next_warning(MYSQLX_RESULT *result)
{
  SAFE_EXCEPTION_BEGIN(result, 0)
  return result->get_next_warning();
  SAFE_EXCEPTION_END(result, 0)
}

uint64_t STDCALL mysqlx_get_auto_increment_value(MYSQLX_RESULT *res)
{
  SAFE_EXCEPTION_BEGIN(res, 0)
  return res->get_auto_increment_value();
  SAFE_EXCEPTION_END(res, 0)
}

int STDCALL mysqlx_transaction_begin(MYSQLX_SESSION *sess)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->transaction_begin();
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

int STDCALL mysqlx_transaction_commit(MYSQLX_SESSION *sess)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->transaction_commit();
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

int STDCALL mysqlx_transaction_rollback(MYSQLX_SESSION *sess)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->transaction_rollback();
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

const char * STDCALL
mysqlx_fetch_doc_id(MYSQLX_RESULT *result)
{
  SAFE_EXCEPTION_BEGIN(result, NULL)
  return result->get_next_doc_id();
  SAFE_EXCEPTION_END(result, NULL)
}

#ifdef _WIN32
BOOL WINAPI DllMain(
  _In_ HINSTANCE,
  _In_ DWORD,
  _In_ LPVOID
)
{
  return true;
}
#endif

// TODO: add implementations for other mysqlx_xxxxxx functions