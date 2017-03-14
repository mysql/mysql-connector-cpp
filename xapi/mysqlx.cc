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
#include <stdexcept>

#define SAFE_EXCEPTION_BEGIN(HANDLE, ERR) try { \
  if (HANDLE == NULL) return ERR;

#define SAFE_EXCEPTION_END(HANDLE, ERR) } \
  catch(const cdk::Error &cdkerr) \
  { \
    HANDLE->set_diagnostic(cdkerr.what(), cdkerr.code().value()); \
    return ERR; \
  } \
  catch(const Mysqlx_exception &mysqlx_ex) \
  { \
    HANDLE->set_diagnostic(mysqlx_ex); \
    return ERR; \
  } \
  catch (const std::logic_error &logicerr) \
  { \
    HANDLE->set_diagnostic(logicerr.what(), 0); \
    return ERR; \
  } \
  catch(...) \
  { \
    HANDLE->set_diagnostic("Unknown error!", MYSQLX_ERR_UNKNOWN); \
    return ERR; \
  }

#define SAFE_EXCEPTION_SILENT_END(ERR) } catch(...) { return ERR; }

#define HANDLE_SESSION_EXCEPTIONS catch(const cdk::Error &e) \
  { \
    if (out_error) \
    { \
      size_t cpy_len = strlen(e.what()) >= MYSQLX_MAX_ERROR_LEN - 1 ? \
                       MYSQLX_MAX_ERROR_LEN - 1 : strlen(e.what()); \
       memcpy(out_error, e.what(), cpy_len); \
       out_error[cpy_len] = '\0'; \
    } \
    if (err_code) \
      *err_code = e.code().value(); \
    if (sess) { delete sess; sess = NULL; } \
  } \
  catch(...) \
  { \
    if (out_error) \
    { \
      const char *unknown = "Unknown error"; \
      memcpy(out_error, unknown, strlen(unknown) + 1); \
    } \
    if (err_code) \
      *err_code = 0; \
    if (sess) { delete sess; sess = NULL; } \
  }

#define PARAM_NULL_EMPTY_CHECK(PARAM, HANDLE, ERR_MSG, ERR) if (!PARAM || !(*PARAM)) \
  { \
    HANDLE->set_diagnostic(ERR_MSG, 0); \
    return ERR; \
  }

#define PARAM_NULL_CHECK(PARAM, HANDLE, ERR_MSG, ERR) if (!PARAM) \
  { \
  HANDLE->set_diagnostic(ERR_MSG, 0); \
  return ERR; \
  }

#define OUT_BUF_CHECK(PARAM, HANDLE, ERR_MSG, ERR) if (!PARAM) \
  { \
  HANDLE->set_diagnostic(ERR_MSG, 0); \
  return ERR; \
  }

static mysqlx_session_t *
_get_session(const char *host, int port, const char *user,
             const char *password, const char *database, const char *conn_str,
             char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code,
             bool is_node_sess)
{
  mysqlx_session_t *sess = NULL;
  try
  {

    if(!conn_str)
    {
      std::string pwd(password ? password : "");
      std::string db(database ? database : "");

      // TODO: the default user has to be determined in a more flexible way
      sess = new mysqlx_session_t(host ? host : "localhost", port,
                                  user ? user : "root", password ? &pwd : NULL,
                                database ? &db : NULL , is_node_sess);
    }
    else
    {
      sess = new mysqlx_session_t(conn_str, is_node_sess);
    }

    if (!sess->is_valid())
    {
      const cdk::Error *err = sess->get_cdk_error();
      if (err)
        throw *err;
    }
  }
  HANDLE_SESSION_EXCEPTIONS
  return sess;
}


static mysqlx_session_t *
_get_session_opt(mysqlx_session_options_t *opt,
             char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code,
             bool is_node_sess)
{
  mysqlx_session_t *sess = NULL;
  try
  {

    if(!opt)
    {
      throw cdk::Error(0, "Session options structure not initialized");
    }

    sess = new mysqlx_session_t(opt, is_node_sess);

    if (!sess->is_valid())
    {
      const cdk::Error *err = sess->get_cdk_error();
      if (err)
        throw *err;
    }
  }
  HANDLE_SESSION_EXCEPTIONS
  return sess;
}

/*
  Establish the X session using string options provided as function parameters
*/
mysqlx_session_t * STDCALL
mysqlx_get_session(const char *host, int port, const char *user,
               const char *password, const char *database,
               char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code)
{
  return _get_session(host, port, user, password, database,
                      NULL, out_error, err_code, false);
}

/*
  Establish the X session using the connection string
*/
mysqlx_session_t * STDCALL
mysqlx_get_session_from_url(const char *conn_string,
                   char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code)
{
  return _get_session(NULL, 0, NULL, NULL, NULL,
                      conn_string, out_error, err_code, false);
}

/*
  Establish the X session using the options structure
*/
mysqlx_session_t * STDCALL
mysqlx_get_session_from_options(mysqlx_session_options_t *opt,
                   char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code)
{
  return _get_session_opt(opt, out_error, err_code, false);
}

/*
  Establish the node session using the options structure
*/
mysqlx_session_t * STDCALL
mysqlx_get_node_session_from_options(mysqlx_session_options_t *opt,
                   char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code)
{
  return _get_session_opt(opt, out_error, err_code, true);
}

/*
  Establish the node session using string options provided as function parameters
*/
mysqlx_session_t * STDCALL
mysqlx_get_node_session(const char *host, int port, const char *user,
               const char *password, const char *database,
               char out_error[MYSQLX_MAX_ERROR_LEN], int *err_code)
{
  return _get_session(host, port, user, password, database,
                      NULL, out_error, err_code, true);
}

/*
  Establish the node session using connection string
*/
mysqlx_session_t * STDCALL
mysqlx_get_node_session_from_url(const char *conn_string,
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
    statement handler containing the results and/or error
    NULL is not supposed to be returned even in case of error.
    It is very unlikely for this function to end with an error
    because it does not do any parsing, parameter checking etc.
*/
mysqlx_stmt_t * STDCALL
mysqlx_sql_new(mysqlx_session_t *sess, const char *query,
                 uint32_t length)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  return sess->sql_query(query, length);

  SAFE_EXCEPTION_END(sess, NULL)
}

/*
  Function for binding values for parametrized queries.

  PARAMETERS:
    stmt - pointer to CRUD
    ...  - variadic arguments that follow in sequences like
           type1, value1, type2, value2, ..., type_n, value_n, PARAM_END
           (PARAM_END marks the end of parameters list)

  RETURN:
    RESULT_OK - on success
    RESULT_ERROR - on error

  NOTE: Each new call resets the binds set by the previous call to
        mysqlx_stmt_t::sql_bind()
*/
int mysqlx_stmt_bind(mysqlx_stmt_t *stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)

  int res = RESULT_OK;
  va_list args;
  va_start(args, stmt);

  /*
    Processing of SQL parameters and parameters for other statement operations
    is different. Therefore, use two different bind functions depending on
    the operation type.
  */
  if (stmt->op_type() == OP_SQL)
    res = stmt->sql_bind(args);
  else
    res = stmt->param_bind(args);

  va_end(args);
  return res;

  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}


int mysqlx_set_insert_row(mysqlx_stmt_t *stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)

  int res = RESULT_OK;
  va_list args;
  va_start(args, stmt);
  // Just row data, no columns in args
  res = stmt->add_row(false, args);
  va_end(args);
  return res;

  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}

int mysqlx_set_insert_columns(mysqlx_stmt_t *stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)

  int res = RESULT_OK;
  va_list args;
  va_start(args, stmt);
  res = stmt->add_columns(args);
  va_end(args);
  return res;

  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}

int STDCALL
mysqlx_set_add_document(mysqlx_stmt_t *stmt, const char *json_doc)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)

  int res = RESULT_OK;
  res = stmt->add_document(json_doc);
  return res;

  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}


mysqlx_stmt_t * STDCALL
mysqlx_table_select_new(mysqlx_table_t *table)
{
  SAFE_EXCEPTION_BEGIN(table, NULL)
  return table->stmt_op(OP_SELECT);
  SAFE_EXCEPTION_END(table, NULL)
}

mysqlx_stmt_t * STDCALL
mysqlx_table_insert_new(mysqlx_table_t *table)
{
  SAFE_EXCEPTION_BEGIN(table, NULL)
  return table->stmt_op(OP_INSERT);
  SAFE_EXCEPTION_END(table, NULL)
}

mysqlx_stmt_t * STDCALL
mysqlx_table_update_new(mysqlx_table_t *table)
{
  SAFE_EXCEPTION_BEGIN(table, NULL)
  return table->stmt_op(OP_UPDATE);
  SAFE_EXCEPTION_END(table, NULL)
}

mysqlx_stmt_t * STDCALL
mysqlx_table_delete_new(mysqlx_table_t *table)
{
  SAFE_EXCEPTION_BEGIN(table, NULL)
  return table->stmt_op(OP_DELETE);
  SAFE_EXCEPTION_END(table, NULL)
}

mysqlx_stmt_t * STDCALL
mysqlx_collection_add_new(mysqlx_collection_t *collection)
{
  SAFE_EXCEPTION_BEGIN(collection, NULL)
  return collection->stmt_op(OP_ADD);
  SAFE_EXCEPTION_END(collection, NULL)
}

mysqlx_stmt_t * STDCALL
mysqlx_collection_modify_new(mysqlx_collection_t *collection)
{
  SAFE_EXCEPTION_BEGIN(collection, NULL)
  return collection->stmt_op(OP_MODIFY);
  SAFE_EXCEPTION_END(collection, NULL)
}

mysqlx_stmt_t * STDCALL
mysqlx_collection_remove_new(mysqlx_collection_t *collection)
{
  SAFE_EXCEPTION_BEGIN(collection, NULL)
  return collection->stmt_op(OP_REMOVE);
  SAFE_EXCEPTION_END(collection, NULL)
}

mysqlx_stmt_t * STDCALL
mysqlx_collection_find_new(mysqlx_collection_t *collection)
{
  SAFE_EXCEPTION_BEGIN(collection, NULL)
  return collection->stmt_op(OP_FIND);
  SAFE_EXCEPTION_END(collection, NULL)
}

int STDCALL mysqlx_set_where(mysqlx_stmt_t *stmt, const char *where_expr)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)
  return stmt->set_where(where_expr);
  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}

int STDCALL mysqlx_set_having(mysqlx_stmt_t *stmt, const char *having_expr)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)
  return stmt->set_having(having_expr);
  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}

int STDCALL mysqlx_set_group_by(mysqlx_stmt_t *stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)
  va_list args;
  int res = RESULT_OK;
  va_start(args, stmt);
  res = stmt->add_group_by(args);
  va_end(args);
  return res;
  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}

int STDCALL
mysqlx_set_limit_and_offset(mysqlx_stmt_t *stmt, uint64_t row_count,
                            uint64_t offset)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)
  return stmt->set_limit(row_count, offset);
  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}

/*
  Set ORDER BY clause for statement operation
  Operations supported by this function:
    SELECT, FIND, UPDATE, MODIFY, DELETE, REMOVE

  Calling it for INSERT or ADD will result in an error

  PARAMETERS:
    stmt - pointer to statement structure
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
int STDCALL mysqlx_set_order_by(mysqlx_stmt_t *stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, stmt);
  res = stmt->add_order_by(args);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}

int STDCALL mysqlx_set_items(mysqlx_stmt_t *stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, stmt);
  res = stmt->add_projections(args);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}

int STDCALL mysqlx_set_find_projection(mysqlx_stmt_t *stmt, const char *proj)
{
  /*
    The call to mysqlx_set_items will take care of exceptions
    and operations validity
  */
  return mysqlx_set_items(stmt, proj, PARAM_END);
}

/*
  Execute a statement.
  PARAMETERS:
    stmt - pointer to statement structure

  RETURN:  A MYSQL_RESULT handle that can be used to access results
           of the operation. Returned handle is valid until the CRUD
           handle is freed (when session is closed or explicitly with
           mysqlx_free()) or until another call to mysqlx_execute()
           on the same statement handle is made. It is also possible to close
           a RESULT hanlde and free all resources used by it earlier with
           mysqlx_result_free() call.

           On error NULL is returned.
*/
mysqlx_result_t * STDCALL mysqlx_execute(mysqlx_stmt_t *stmt)
{
  SAFE_EXCEPTION_BEGIN(stmt, NULL)

  if (!stmt || !stmt->session_valid())
    return NULL;

  try
  {
    return stmt->exec();
  }
  catch(Mysqlx_exception &ex)
  {
    if (ex.type() == Mysqlx_exception::MYSQLX_EXCEPTION_EXTERNAL)
    {
      /*
        For external server errors, the diagnostic info
        is obtained from the cdk::Reply instance
      */
      stmt->acquire_diag();
    }
    else
    {
      stmt->set_diagnostic(ex);
    }
  }
  return NULL;

  SAFE_EXCEPTION_END(stmt, NULL)
}

int STDCALL mysqlx_set_update_values(mysqlx_stmt_t *stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, stmt);
  res = stmt->add_table_update_values(args);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}

int STDCALL mysqlx_set_modify_set(mysqlx_stmt_t *stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, stmt);
  res = stmt->add_coll_modify_values(args, MODIFY_SET);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}

int STDCALL STDCALL mysqlx_set_modify_unset(mysqlx_stmt_t *stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, stmt);
  res = stmt->add_coll_modify_values(args, MODIFY_UNSET);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}


int STDCALL STDCALL mysqlx_set_modify_array_insert(mysqlx_stmt_t *stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, stmt);
  res = stmt->add_coll_modify_values(args, MODIFY_ARRAY_INSERT);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}


int STDCALL STDCALL mysqlx_set_modify_array_append(mysqlx_stmt_t *stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, stmt);
  res = stmt->add_coll_modify_values(args, MODIFY_ARRAY_APPEND);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}


int STDCALL STDCALL mysqlx_set_modify_array_delete(mysqlx_stmt_t *stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(stmt, RESULT_ERROR)

  va_list args;
  int res = RESULT_OK;

  va_start(args, stmt);
  res = stmt->add_coll_modify_values(args, MODIFY_ARRAY_DELETE);
  va_end(args);

  return res;

  SAFE_EXCEPTION_END(stmt, RESULT_ERROR)
}

/*
  Fetch one row from the result and advance to the next row
  PARAMETERS:
    res - pointer to the result structure

  RETURN: pointer to mysqlx_row_t or NULL if no more rows left
*/
mysqlx_row_t * STDCALL mysqlx_row_fetch_one(mysqlx_result_t *res)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->read_row();
  SAFE_EXCEPTION_END(res, NULL)
}

mysqlx_doc_t * STDCALL mysqlx_doc_fetch_one(mysqlx_result_t *res)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->read_doc();
  SAFE_EXCEPTION_END(res, NULL)
}

const char * STDCALL mysqlx_json_fetch_one(mysqlx_result_t *res, size_t *length)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->read_json(length);
  SAFE_EXCEPTION_END(res, NULL)
}

int STDCALL
mysqlx_doc_get_uint(mysqlx_doc_t *doc, const char *key, uint64_t *out)
{
  SAFE_EXCEPTION_BEGIN(doc, RESULT_ERROR)
  PARAM_NULL_EMPTY_CHECK(key, doc, MYSQLX_ERROR_MISSING_KEY_NAME_MSG, RESULT_ERROR)
  OUT_BUF_CHECK(out, doc, MYSQLX_ERROR_OUTPUT_BUFFER_NULL, RESULT_ERROR)
  *out = doc->get_uint(key);
  return RESULT_OK;
  SAFE_EXCEPTION_END(doc, RESULT_ERROR)
}

int STDCALL
mysqlx_doc_get_sint(mysqlx_doc_t *doc, const char *key, int64_t *out)
{
  SAFE_EXCEPTION_BEGIN(doc, RESULT_ERROR)
  PARAM_NULL_EMPTY_CHECK(key, doc, MYSQLX_ERROR_MISSING_KEY_NAME_MSG, RESULT_ERROR)
  OUT_BUF_CHECK(out, doc, MYSQLX_ERROR_OUTPUT_BUFFER_NULL, RESULT_ERROR)
  *out = doc->get_sint(key);
  return RESULT_OK;
  SAFE_EXCEPTION_END(doc, RESULT_ERROR)
}

int STDCALL
mysqlx_doc_get_float(mysqlx_doc_t *doc, const char *key, float *out)
{
  SAFE_EXCEPTION_BEGIN(doc, RESULT_ERROR)
  PARAM_NULL_EMPTY_CHECK(key, doc, MYSQLX_ERROR_MISSING_KEY_NAME_MSG, RESULT_ERROR)
  OUT_BUF_CHECK(out, doc, MYSQLX_ERROR_OUTPUT_BUFFER_NULL, RESULT_ERROR)
  *out = doc->get_float(key);
  return RESULT_OK;
  SAFE_EXCEPTION_END(doc, RESULT_ERROR)
}

int STDCALL
mysqlx_doc_get_double(mysqlx_doc_t *doc, const char *key, double *out)
{
  SAFE_EXCEPTION_BEGIN(doc, RESULT_ERROR)
  PARAM_NULL_EMPTY_CHECK(key, doc, MYSQLX_ERROR_MISSING_KEY_NAME_MSG, RESULT_ERROR)
  OUT_BUF_CHECK(out, doc, MYSQLX_ERROR_OUTPUT_BUFFER_NULL, RESULT_ERROR)
  *out = doc->get_double(key);
  return RESULT_OK;
  SAFE_EXCEPTION_END(doc, RESULT_ERROR)
}

static int
_doc_get_bytes(mysqlx_doc_t* doc, const char *key, uint64_t offset,
               void *buf, size_t *buf_len, mysqlx_data_type_t type)
{
  SAFE_EXCEPTION_BEGIN(doc, RESULT_ERROR)
  PARAM_NULL_EMPTY_CHECK(buf_len, doc, MYSQLX_ERROR_OUTPUT_BUFFER_ZERO, RESULT_ERROR)
  PARAM_NULL_EMPTY_CHECK(key, doc, MYSQLX_ERROR_MISSING_KEY_NAME_MSG, RESULT_ERROR)
  OUT_BUF_CHECK(buf, doc, MYSQLX_ERROR_OUTPUT_BUFFER_NULL, RESULT_ERROR)

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
  return RESULT_OK;

  SAFE_EXCEPTION_END(doc, RESULT_ERROR)
}

int STDCALL
mysqlx_doc_get_bytes(mysqlx_doc_t* doc, const char *key, uint64_t offset,
                     void *buf, size_t *buf_len)
{
  return _doc_get_bytes(doc, key, offset, buf, buf_len,
                        MYSQLX_TYPE_BYTES);
}

int STDCALL
mysqlx_doc_get_str(mysqlx_doc_t* doc, const char *key, uint64_t offset,
                     char *buf, size_t *buf_len)
{
  return _doc_get_bytes(doc, key, offset, (void*)buf, buf_len,
                        MYSQLX_TYPE_STRING);
}

bool STDCALL mysqlx_doc_key_exists(mysqlx_doc_t *doc, const char *key)
{
  SAFE_EXCEPTION_BEGIN(doc, false)
  PARAM_NULL_EMPTY_CHECK(key, doc, MYSQLX_ERROR_MISSING_KEY_NAME_MSG, false)
  return doc->key_exists(key);
  SAFE_EXCEPTION_END(doc, false)
}

uint16_t STDCALL mysqlx_doc_get_type(mysqlx_doc_t *doc, const char *key)
{
  SAFE_EXCEPTION_BEGIN(doc, MYSQLX_TYPE_UNDEFINED)
  PARAM_NULL_EMPTY_CHECK(key, doc, MYSQLX_ERROR_MISSING_KEY_NAME_MSG, RESULT_ERROR)
  return doc->get_type(key);
  SAFE_EXCEPTION_END(doc, MYSQLX_TYPE_UNDEFINED)
}

int STDCALL
mysqlx_store_result(mysqlx_result_t *result, size_t *num)
{
  SAFE_EXCEPTION_BEGIN(result, RESULT_ERROR)
  size_t row_num = result->store_result();
  if (num)
    *num = row_num;

  if (mysqlx_error(&result->get_crud()))
    return RESULT_ERROR;

  return RESULT_OK;
  SAFE_EXCEPTION_END(result, RESULT_ERROR)
}

#define CHECK_COLUMN_RANGE(COL, ROW) if (COL >= ROW->row_size()) \
{ \
  ROW->set_diagnostic(MYSQLX_ERROR_INDEX_OUT_OF_RANGE_MSG, \
                      MYSQLX_ERROR_INDEX_OUT_OF_RANGE); \
  return RESULT_ERROR; \
}

int STDCALL mysqlx_get_bytes(mysqlx_row_t* row, uint32_t col, uint64_t offset,
                             void *buf, size_t *buf_len)
{
  SAFE_EXCEPTION_BEGIN(row, RESULT_ERROR)
  PARAM_NULL_EMPTY_CHECK(buf_len, row, MYSQLX_ERROR_OUTPUT_BUFFER_ZERO, RESULT_ERROR)
  OUT_BUF_CHECK(buf, row, MYSQLX_ERROR_OUTPUT_BUFFER_NULL, RESULT_ERROR)

  CHECK_COLUMN_RANGE(col, row)
  cdk::bytes b = row->get_col_data(col);
  int rc = RESULT_OK;

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
  else
    rc = RESULT_MORE_DATA;

  memcpy(buf, b.begin() + offset, *buf_len);
  return rc;

  SAFE_EXCEPTION_END(row, RESULT_ERROR)
}

int STDCALL mysqlx_get_uint(mysqlx_row_t* row, uint32_t col, uint64_t *val)
{
  SAFE_EXCEPTION_BEGIN(row, RESULT_ERROR)
  OUT_BUF_CHECK(val, row, MYSQLX_ERROR_OUTPUT_BUFFER_NULL, RESULT_ERROR)

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

int STDCALL mysqlx_get_sint(mysqlx_row_t* row, uint32_t col, int64_t *val)
{
  SAFE_EXCEPTION_BEGIN(row, RESULT_ERROR)
  OUT_BUF_CHECK(val, row, MYSQLX_ERROR_OUTPUT_BUFFER_NULL, RESULT_ERROR)
  CHECK_COLUMN_RANGE(col, row)
  if (row->get_col_data(col).size() == 0)
    return RESULT_NULL;

  cdk::Codec<cdk::TYPE_INTEGER> codec(row->get_result().get_cursor()->format(col));
  codec.from_bytes(row->get_col_data(col), *val);
  return RESULT_OK;

  SAFE_EXCEPTION_END(row, RESULT_ERROR)
}

int STDCALL mysqlx_get_float(mysqlx_row_t* row, uint32_t col, float *val)
{
  SAFE_EXCEPTION_BEGIN(row, RESULT_ERROR)
  OUT_BUF_CHECK(val, row, MYSQLX_ERROR_OUTPUT_BUFFER_NULL, RESULT_ERROR)
  CHECK_COLUMN_RANGE(col, row)
  if (row->get_col_data(col).size() == 0)
    return RESULT_NULL;

  cdk::Codec<cdk::TYPE_FLOAT> codec(row->get_result().get_cursor()->format(col));
  codec.from_bytes(row->get_col_data(col), *val);
  return RESULT_OK;

  SAFE_EXCEPTION_END(row, RESULT_ERROR)
}

int STDCALL mysqlx_get_double(mysqlx_row_t* row, uint32_t col, double *val)
{
  SAFE_EXCEPTION_BEGIN(row, RESULT_ERROR)
  OUT_BUF_CHECK(val, row, MYSQLX_ERROR_OUTPUT_BUFFER_NULL, RESULT_ERROR)
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
uint32_t STDCALL mysqlx_column_get_count(mysqlx_result_t *res)
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
const char * STDCALL mysqlx_column_get_name(mysqlx_result_t *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->column_get_info_char(pos, mysqlx_result_t::COL_INFO_NAME);
  SAFE_EXCEPTION_END(res, NULL)
}

/*
  Get column original name
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: character string containing column original name
*/
const char * STDCALL mysqlx_column_get_original_name(mysqlx_result_t *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->column_get_info_char(pos, mysqlx_result_t::COL_INFO_ORIG_NAME);
  SAFE_EXCEPTION_END(res, NULL)
}

/*
  Get column table name
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: character string containing column table name
*/
const char * STDCALL mysqlx_column_get_table(mysqlx_result_t *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->column_get_info_char(pos, mysqlx_result_t::COL_INFO_TABLE);
  SAFE_EXCEPTION_END(res, NULL)
}

/*
  Get column original table name
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: character string containing column original table name
*/
const char * STDCALL mysqlx_column_get_original_table(mysqlx_result_t *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->column_get_info_char(pos, mysqlx_result_t::COL_INFO_ORIG_TABLE);
  SAFE_EXCEPTION_END(res, NULL)
}

/*
  Get column schema name
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: character string containing column schema name
*/
const char * STDCALL mysqlx_column_get_schema(mysqlx_result_t *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->column_get_info_char(pos, mysqlx_result_t::COL_INFO_SCHEMA);
  SAFE_EXCEPTION_END(res, NULL)
}

/*
  Get column catalog name
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: character string containing column catalog name
*/
const char * STDCALL mysqlx_column_get_catalog(mysqlx_result_t *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, NULL)
  return res->column_get_info_char(pos, mysqlx_result_t::COL_INFO_CATALOG);
  SAFE_EXCEPTION_END(res, NULL)
}

/*
  Get column type identifier
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: 16-bit unsigned int number with the column type identifier
*/
uint16_t STDCALL mysqlx_column_get_type(mysqlx_result_t *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, MYSQLX_TYPE_UNDEFINED)
  return (uint16_t)res->column_get_info_int(pos, mysqlx_result_t::COL_INFO_TYPE);
  SAFE_EXCEPTION_END(res, MYSQLX_TYPE_UNDEFINED)
}

/*
  Get column collation number
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: 16-bit unsigned int number with the column collation number
*/
uint16_t STDCALL mysqlx_column_get_collation(mysqlx_result_t *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, MYSQLX_COLLATION_UNDEFINED)
  return (uint16_t)res->column_get_info_int(pos, mysqlx_result_t::COL_INFO_COLLATION);
  SAFE_EXCEPTION_END(res, MYSQLX_COLLATION_UNDEFINED)
}

/*
  Get column length
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: 32-bit unsigned int number indicating the maximum data length
*/
uint32_t STDCALL mysqlx_column_get_length(mysqlx_result_t *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, 0)
  return res->column_get_info_int(pos, mysqlx_result_t::COL_INFO_LENGTH);
  SAFE_EXCEPTION_END(res, 0)
}

/*
  Get column precision
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: 16-bit unsigned int number of digits after the decimal point
*/
uint16_t STDCALL mysqlx_column_get_precision(mysqlx_result_t *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, 0)
  return (uint16_t)res->column_get_info_int(pos, mysqlx_result_t::COL_INFO_PRECISION);
  SAFE_EXCEPTION_END(res, 0)
}

/*
  Get column flags
  PARAMETERS:
    res - pointer to the result structure
    pos - zero-based column number

  RETURN: 32-bit unsigned int number containing column flags
*/
uint32_t STDCALL mysqlx_column_get_flags(mysqlx_result_t *res, uint32_t pos)
{
  SAFE_EXCEPTION_BEGIN(res, 0)
  return res->column_get_info_int(pos, mysqlx_result_t::COL_INFO_FLAGS);
  SAFE_EXCEPTION_END(res, 0)
}

/*
  Get number of rows affected by the last operation
  PARAMETERS:
    res - pointer to the result structure

  RETURN: 64-bit unsigned int number containing the number of affected rows
*/
uint64_t STDCALL mysqlx_get_affected_count(mysqlx_result_t *res)
{
  SAFE_EXCEPTION_BEGIN(res, 0)
  return res->get_affected_count();
  SAFE_EXCEPTION_END(res, 0)
}

/*
  Free the statement explicitly, otherwise it will be done automatically
  when session is closed
*/
void STDCALL mysqlx_free(mysqlx_stmt_t *stmt)
{
  if (stmt)
    stmt->get_session().reset_stmt(stmt);
}


int STDCALL mysqlx_next_result(mysqlx_result_t *res)
{
  SAFE_EXCEPTION_BEGIN(res, RESULT_ERROR)
  return res->next_result() ? RESULT_OK : RESULT_NULL;
  SAFE_EXCEPTION_END(res, RESULT_ERROR)
}

/*
  Free the result explicitly, otherwise it will be done automatically
  when statement handler is destroyed
*/
void STDCALL mysqlx_result_free(mysqlx_result_t *res)
{
  if (res && res->get_crud().set_result(NULL))
    delete res;
}

/*
  Closing the session.
  This function must be called by the user to prevent memory leaks.
*/
void STDCALL mysqlx_session_close(mysqlx_session_t *sess)
{
  if (sess)
  {
    try {
      delete sess;
    }
    catch (...)
    {
      // Ignore errors that might happen during session destruction.
    }
  }
}

int STDCALL
mysqlx_schema_create(mysqlx_session_t *sess, const char *schema)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->create_schema(schema);
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

int STDCALL
mysqlx_schema_drop(mysqlx_session_t *sess, const char *schema)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  PARAM_NULL_EMPTY_CHECK(schema, sess, MYSQLX_ERROR_MISSING_SCHEMA_NAME_MSG, RESULT_ERROR)
  sess->drop_object(schema, "", mysqlx_session_t::SCHEMA);
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

int STDCALL
mysqlx_table_drop(mysqlx_schema_t *schema, const char *table)
{
  SAFE_EXCEPTION_BEGIN(schema, RESULT_ERROR)
  PARAM_NULL_EMPTY_CHECK(table, schema, MYSQLX_ERROR_MISSING_TABLE_NAME_MSG, RESULT_ERROR)
  schema->get_session().drop_object(schema->get_name(), table,
                                  mysqlx_session_t::TABLE);
  return RESULT_OK;
  SAFE_EXCEPTION_END(schema, RESULT_ERROR)
}

int STDCALL
mysqlx_view_drop(mysqlx_schema_t *schema, const char *view)
{
  SAFE_EXCEPTION_BEGIN(schema, RESULT_ERROR)
  PARAM_NULL_EMPTY_CHECK(view, schema, MYSQLX_ERROR_MISSING_VIEW_NAME_MSG, RESULT_ERROR)
  schema->get_session().drop_object(schema->get_name(), view,
                                    mysqlx_session_t::VIEW);
  return RESULT_OK;
  SAFE_EXCEPTION_END(schema, RESULT_ERROR)
}

int STDCALL
mysqlx_collection_create(mysqlx_schema_t *schema, const char *collection)
{
  SAFE_EXCEPTION_BEGIN(schema, RESULT_ERROR)
  PARAM_NULL_EMPTY_CHECK(collection, schema, MYSQLX_ERROR_MISSING_COLLECTION_NAME_MSG, RESULT_ERROR)
  schema->get_session().admin_collection("create_collection",
                                         schema->get_name().data(),
                                         collection);
  return RESULT_OK;
  SAFE_EXCEPTION_END(schema, RESULT_ERROR)
}

int STDCALL
mysqlx_collection_drop(mysqlx_schema_t *schema, const char *collection)
{
  SAFE_EXCEPTION_BEGIN(schema, RESULT_ERROR)
  PARAM_NULL_EMPTY_CHECK(collection, schema, MYSQLX_ERROR_MISSING_COLLECTION_NAME_MSG, RESULT_ERROR)
  schema->get_session().drop_object(schema->get_name(), collection, mysqlx_session_t::COLLECTION);
  return RESULT_OK;
  SAFE_EXCEPTION_END(schema, RESULT_ERROR)
}

/*
  STMT will be unavailable outside the function, set session error
  to the upper level object and return
*/
#define SET_ERROR_FROM_STMT(OBJ, STMT, R) do { \
   mysqlx_error_t *err = STMT->get_error(); \
   if (err) \
      OBJ->set_diagnostic(err->message(), err->error_num()); \
    else \
      OBJ->set_diagnostic("Unknown error!", 0); \
    return R; \
  } while (0)

mysqlx_result_t * STDCALL mysqlx_sql(mysqlx_session_t *sess,
                                        const char *query,
                                        size_t query_len)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)

  mysqlx_stmt_t *stmt = sess->sql_query(query, query_len);
  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(sess, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

mysqlx_result_t * STDCALL mysqlx_sql_param(mysqlx_session_t *sess,
                                        const char *query,
                                        size_t query_len, ...)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  int rc = RESULT_OK;
  mysqlx_stmt_t *stmt;

  if ((stmt = sess->sql_query(query, query_len)) == NULL)
    return NULL;

  va_list args;
  va_start(args, query_len);
  rc = stmt->sql_bind(args);
  va_end(args);

  if (rc != RESULT_OK)
    SET_ERROR_FROM_STMT(sess, stmt, NULL);

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(sess, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

mysqlx_result_t * STDCALL
mysqlx_table_select(mysqlx_table_t *table, const char *criteria)
{
  SAFE_EXCEPTION_BEGIN(table, NULL)
  mysqlx_stmt_t *stmt;

  if ((stmt = table->stmt_op(OP_SELECT)) == NULL)
    return NULL;

  if (RESULT_OK != stmt->set_where(criteria))
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(table, NULL)
}

mysqlx_result_t * STDCALL
mysqlx_table_select_limit(mysqlx_table_t *table, const char *criteria,
                               uint64_t row_count, uint64_t offset, ...)
{
  SAFE_EXCEPTION_BEGIN(table, NULL)
  mysqlx_stmt_t *stmt;
  int rc = RESULT_OK;

  if ((stmt = table->stmt_op(OP_SELECT)) == NULL)
    return NULL;

  if (RESULT_OK != stmt->set_where(criteria))
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  if (RESULT_OK != stmt->set_limit(row_count, offset))
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  va_list args;
  va_start(args, offset);
  rc= stmt->add_order_by(args);
  va_end(args);

  if (rc != RESULT_OK)
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(table, NULL)
}

mysqlx_result_t * STDCALL
mysqlx_table_insert(mysqlx_table_t *table, ...)
{
  SAFE_EXCEPTION_BEGIN(table, NULL)
  mysqlx_stmt_t *stmt;
  int rc = RESULT_OK;

  if ((stmt = table->stmt_op(OP_INSERT)) == NULL)
    return NULL;

  va_list args;
  va_start(args, table);
  /*
    Parameters are triplets: <column name, value type, value>
  */
  rc= stmt->add_row(true, args);
  va_end(args);

  if (rc != RESULT_OK)
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(table, NULL)
}

mysqlx_result_t * STDCALL
mysqlx_table_update(mysqlx_table_t *table,
                        const char *criteria,
                        ...)
{
  SAFE_EXCEPTION_BEGIN(table, NULL)
  mysqlx_stmt_t *stmt;
  int rc = RESULT_OK;

  if ((stmt = table->stmt_op(OP_UPDATE)) == NULL)
    return NULL;

  if (RESULT_OK != stmt->set_where(criteria))
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  va_list args;
  va_start(args, criteria);
  /*
    Parameters are triplets: <column name, value type, value>
  */
  rc= stmt->add_table_update_values(args);
  va_end(args);

  if (rc != RESULT_OK)
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(table, NULL)
}

mysqlx_result_t * STDCALL
mysqlx_table_delete(mysqlx_table_t *table, const char *criteria)
{
  SAFE_EXCEPTION_BEGIN(table, NULL)
  mysqlx_stmt_t *stmt;

  if ((stmt = table->stmt_op(OP_DELETE)) == NULL)
    return NULL;

  if (RESULT_OK != stmt->set_where(criteria))
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(table, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(table, NULL)
}

mysqlx_result_t * STDCALL
mysqlx_collection_find(mysqlx_collection_t *collection, const char *criteria)
{
  SAFE_EXCEPTION_BEGIN(collection, NULL)
  mysqlx_stmt_t *stmt;

  if ((stmt = collection->stmt_op(OP_FIND)) == NULL)
    return NULL;

  if (RESULT_OK != stmt->set_where(criteria))
    SET_ERROR_FROM_STMT(collection, stmt, NULL);

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(collection, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(collection, NULL)
}

mysqlx_result_t * STDCALL
mysqlx_collection_add(mysqlx_collection_t *collection, ...)
{
  SAFE_EXCEPTION_BEGIN(collection, NULL)
  mysqlx_stmt_t *stmt;
  int rc = RESULT_OK;

  if ((stmt = collection->stmt_op(OP_ADD)) == NULL)
    return NULL;

  va_list args;
  va_start(args, collection);
  rc= stmt->add_multiple_documents(args);
  va_end(args);

  if (rc != RESULT_OK)
    SET_ERROR_FROM_STMT(collection, stmt, NULL);

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(collection, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(collection, NULL)
}

mysqlx_result_t * STDCALL
_mysqlx_collection_modify_exec(mysqlx_collection_t *collection,
                               const char *criteria,
                               mysqlx_modify_op modify_op, va_list args)
{
  SAFE_EXCEPTION_BEGIN(collection, NULL)
  mysqlx_stmt_t *stmt;
  int rc = RESULT_OK;

  if ((stmt = collection->stmt_op(OP_MODIFY)) == NULL)
    return NULL;

  if (RESULT_OK != stmt->set_where(criteria))
    SET_ERROR_FROM_STMT(collection, stmt, NULL);

  rc= stmt->add_coll_modify_values(args, modify_op);

  if (rc != RESULT_OK)
    SET_ERROR_FROM_STMT(collection, stmt, NULL);

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(collection, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(collection, NULL)
}

mysqlx_result_t * STDCALL
mysqlx_collection_modify_set(mysqlx_collection_t *collection,
                             const char *criteria, ...)
{
  mysqlx_result_t *res;
  va_list args;
  va_start(args, criteria);
  res = _mysqlx_collection_modify_exec(collection, criteria, MODIFY_SET, args);
  va_end(args);
  return res;
}

mysqlx_result_t * STDCALL
mysqlx_collection_modify_unset(mysqlx_collection_t *collection,
                               const char *criteria, ...)
{
  mysqlx_result_t *res;
  va_list args;
  va_start(args, criteria);
  res = _mysqlx_collection_modify_exec(collection, criteria, MODIFY_UNSET, args);
  va_end(args);
  return res;
}

mysqlx_result_t * STDCALL
mysqlx_collection_remove(mysqlx_collection_t *collection, const char*criteria)
{
  SAFE_EXCEPTION_BEGIN(collection, NULL)
  mysqlx_stmt_t *stmt;

  if ((stmt = collection->stmt_op(OP_REMOVE)) == NULL)
    return NULL;

  if (RESULT_OK != stmt->set_where(criteria))
    SET_ERROR_FROM_STMT(collection, stmt, NULL);

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(collection, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(collection, NULL)
}

mysqlx_result_t * STDCALL
mysqlx_get_tables(mysqlx_schema_t *schema,
                  const char *table_pattern,
                  int show_views)
{
  SAFE_EXCEPTION_BEGIN(schema, NULL)
  mysqlx_stmt_t *stmt;

  if ((stmt = schema->stmt_op(table_pattern ? table_pattern : "",
                              OP_ADMIN_LIST)) == NULL)
    return NULL;

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(schema, stmt, NULL);

  res->set_table_list_mask(FILTER_TABLE | (show_views ? FILTER_VIEW : 0));

  return res;
  SAFE_EXCEPTION_END(schema, NULL)
}

mysqlx_result_t * STDCALL
mysqlx_get_collections(mysqlx_schema_t *schema,
                       const char *col_pattern)
{
  SAFE_EXCEPTION_BEGIN(schema, NULL)
  mysqlx_stmt_t *stmt;

  if ((stmt = schema->stmt_op(col_pattern ? col_pattern : "",
                              OP_ADMIN_LIST)) == NULL)
    return NULL;

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(schema, stmt, NULL);

  res->set_table_list_mask(FILTER_COLLECTION);

  return res;
  SAFE_EXCEPTION_END(schema, NULL)
}

mysqlx_result_t * STDCALL
mysqlx_get_schemas(mysqlx_session_t *sess, const char *schema_pattern)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  mysqlx_stmt_t *stmt;
  /* This type of SQL is enabled for X session */
  if ((stmt = sess->sql_query("SHOW SCHEMAS LIKE ?",
       MYSQLX_NULL_TERMINATED, true)) == NULL)
    return NULL;

  if (mysqlx_stmt_bind(stmt, PARAM_STRING(schema_pattern ? schema_pattern : "%"),
                       PARAM_END) == RESULT_ERROR)
    SET_ERROR_FROM_STMT(sess, stmt, NULL);

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(sess, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(sess, NULL)
}

unsigned int STDCALL
mysqlx_result_warning_count(mysqlx_result_t *result)
{
  SAFE_EXCEPTION_BEGIN(result, 0)
  return (unsigned int)result->get_warning_count();
  SAFE_EXCEPTION_END(result, 0)
}

mysqlx_error_t * STDCALL
mysqlx_result_next_warning(mysqlx_result_t *result)
{
  SAFE_EXCEPTION_BEGIN(result, 0)
  return result->get_next_warning();
  SAFE_EXCEPTION_END(result, 0)
}

uint64_t STDCALL mysqlx_get_auto_increment_value(mysqlx_result_t *res)
{
  SAFE_EXCEPTION_BEGIN(res, 0)
  return res->get_auto_increment_value();
  SAFE_EXCEPTION_END(res, 0)
}

int STDCALL mysqlx_transaction_begin(mysqlx_session_t *sess)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->transaction_begin();
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

int STDCALL mysqlx_transaction_commit(mysqlx_session_t *sess)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->transaction_commit();
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

int STDCALL mysqlx_transaction_rollback(mysqlx_session_t *sess)
{
  SAFE_EXCEPTION_BEGIN(sess, RESULT_ERROR)
  sess->transaction_rollback();
  return RESULT_OK;
  SAFE_EXCEPTION_END(sess, RESULT_ERROR)
}

const char * STDCALL
mysqlx_fetch_doc_id(mysqlx_result_t *result)
{
  SAFE_EXCEPTION_BEGIN(result, NULL)
  return result->get_next_doc_id();
  SAFE_EXCEPTION_END(result, NULL)
}

int STDCALL
mysqlx_session_valid(mysqlx_session_t *sess)
{
  SAFE_EXCEPTION_BEGIN(sess, 0)
  return sess->is_valid();
  SAFE_EXCEPTION_END(sess, 0)
}

mysqlx_session_options_t * STDCALL
mysqlx_session_options_new()
{
  return new mysqlx_session_options_t();
}

void STDCALL
mysqlx_free_options(mysqlx_session_options_t *opt)
{
  if (opt)
    delete opt;
}

int STDCALL
mysqlx_session_option_set(mysqlx_session_options_t *opt, mysqlx_opt_type_t type, ...)
{
  SAFE_EXCEPTION_BEGIN(opt, RESULT_ERROR)
  int rc = RESULT_OK;
  unsigned int uint_data = 0;

  const char *char_data = NULL;

  va_list args;
  va_start(args, type);
  switch(type)
  {
    case MYSQLX_OPT_HOST:
      char_data = va_arg(args, char*);
      if (char_data == NULL)
      {
        opt->set_diagnostic("Host name cannot be NULL", 0);
        rc = RESULT_ERROR;
      }
      else
        opt->host(char_data);
    break;
    case MYSQLX_OPT_PORT:
      uint_data = va_arg(args, unsigned int);
      opt->port(uint_data);
    break;
    case MYSQLX_OPT_USER:
      char_data = va_arg(args, char*);
      if (char_data == NULL)
        char_data = "";
      opt->user(char_data);
    break;
    case MYSQLX_OPT_PWD:
      char_data = va_arg(args, char*);
      if (char_data == NULL)
        char_data = "";
      opt->password(char_data);
    break;
    case MYSQLX_OPT_DB:
      char_data = va_arg(args, char*);
      if (char_data == NULL)
        char_data = "";
      opt->set_database(char_data);
    break;
#ifdef WITH_SSL
    case MYSQLX_OPT_SSL_ENABLE:
      uint_data = va_arg(args, unsigned int);
      opt->set_tls(uint_data > 0);
    break;
    case MYSQLX_OPT_SSL_CA:
      char_data = va_arg(args, char*);
      opt->set_ssl_ca(char_data);
    break;
#else
    case MYSQLX_OPT_SSL_ENABLE:
    case MYSQLX_OPT_SSL_CA:
    case MYSQLX_OPT_SSL_CA_PATH:
      opt->set_diagnostic(
      "Can not create TLS session - this connector is built"
      " without TLS support.", 0
    );
    break;
#endif
    default:
      opt->set_diagnostic("Invalid option value", 0);
      rc = RESULT_ERROR;
  }
  va_end(args);

  return rc;
  SAFE_EXCEPTION_END(opt, RESULT_ERROR)
}

#define CHECK_OUTPUT_BUF(V, T) V = va_arg(args, T); \
if (V == NULL) \
{ \
   opt->set_diagnostic(MYSQLX_ERROR_OUTPUT_BUFFER_NULL, 0); \
   rc = RESULT_ERROR; \
   break; \
}

int STDCALL
mysqlx_session_option_get(mysqlx_session_options_t *opt, mysqlx_opt_type_t type, ...)
{
  SAFE_EXCEPTION_BEGIN(opt, RESULT_ERROR)
  int rc = RESULT_OK;
  unsigned int *uint_data = 0;

  char *char_data = NULL;

  va_list args;
  va_start(args, type);
  switch(type)
  {
    case MYSQLX_OPT_HOST:
      CHECK_OUTPUT_BUF(char_data, char*)
      strcpy(char_data, opt->get_host().data());
    break;
    case MYSQLX_OPT_PORT:
      CHECK_OUTPUT_BUF(uint_data, unsigned int*)
      *uint_data = opt->get_port();
    break;
    case MYSQLX_OPT_USER:
      CHECK_OUTPUT_BUF(char_data, char*)
      strcpy(char_data, opt->get_user().data());
    break;
    case MYSQLX_OPT_PWD:
      CHECK_OUTPUT_BUF(char_data, char*)
      strcpy(char_data, opt->get_password().data());
    break;
    case MYSQLX_OPT_DB:
      CHECK_OUTPUT_BUF(char_data, char*)
      strcpy(char_data, opt->get_db().data());
    break;
#ifdef WITH_SSL
    case MYSQLX_OPT_SSL_ENABLE:
      CHECK_OUTPUT_BUF(uint_data, unsigned int*)
      *uint_data = opt->get_tls().use_tls() ? 1 : 0;
    break;
    case MYSQLX_OPT_SSL_CA:
      CHECK_OUTPUT_BUF(char_data, char*)
      strcpy(char_data, opt->get_tls().get_ca().data());
    break;
#else
    case MYSQLX_OPT_SSL_ENABLE:
    case MYSQLX_OPT_SSL_CA:
    case MYSQLX_OPT_SSL_CA_PATH:
      opt->set_diagnostic(
      "Can not create TLS session - this connector is built"
      " without TLS support.", 0
    );
    break;
#endif
    default:
      opt->set_diagnostic("Invalid option value", 0);
      rc = RESULT_ERROR;
  }
  va_end(args);

  return rc;
  SAFE_EXCEPTION_END(opt, RESULT_ERROR)
}

mysqlx_schema_t * STDCALL
mysqlx_get_schema(mysqlx_session_t *sess, const char *schema_name,
                  unsigned int check)
{
  SAFE_EXCEPTION_BEGIN(sess, NULL)
  return &(sess->get_schema(schema_name, check > 0));
  SAFE_EXCEPTION_END(sess, NULL)
}

mysqlx_collection_t * STDCALL
mysqlx_get_collection(mysqlx_schema_t *schema, const char *col_name,
                      unsigned int check)
{
  SAFE_EXCEPTION_BEGIN(schema, NULL)
  return &(schema->get_collection(col_name, check > 0));
  SAFE_EXCEPTION_END(schema, NULL)
}

mysqlx_table_t * STDCALL
mysqlx_get_table(mysqlx_schema_t *schema, const char *tab_name,
                      unsigned int check)
{
  SAFE_EXCEPTION_BEGIN(schema, NULL)
  return &(schema->get_table(tab_name, check > 0));
  SAFE_EXCEPTION_END(schema, NULL)
}

mysqlx_error_t * STDCALL
mysqlx_error(void *obj)
{
  Mysqlx_diag_base *diag = (Mysqlx_diag_base*)obj;
  SAFE_EXCEPTION_BEGIN(diag, NULL)
  return diag->get_error();
  SAFE_EXCEPTION_SILENT_END(NULL)
}

const char * STDCALL
mysqlx_error_message(void *obj)
{
  mysqlx_error_t *error = mysqlx_error(obj);
  if (error)
  {
    const char *c = error->message();
    return c;
  }

  return NULL;
}

unsigned int STDCALL mysqlx_error_num(void *obj)
{
  mysqlx_error_t *error = mysqlx_error(obj);
  if (error)
    return error->error_num();

  return 0;
}

#define STMT_SELECT_FIND_CHECK(SCHEMA, STMT, RET) if (STMT->op_type() != OP_SELECT) { \
          SCHEMA->set_diagnostic(MYSQLX_ERROR_VIEW_INVALID_STMT_TYPE, 0); \
          return RET; \
        }

mysqlx_stmt_t *
_mysqlx_view_new(mysqlx_schema_t *schema, const char *name,
                 mysqlx_stmt_t *select_stmt, mysqlx_op_t op_type)
{
  SAFE_EXCEPTION_BEGIN(schema, NULL)
  PARAM_NULL_EMPTY_CHECK(name, schema, MYSQLX_ERROR_MISSING_VIEW_NAME_MSG, NULL)
  PARAM_NULL_CHECK(select_stmt, schema, MYSQLX_ERROR_HANDLE_NULL_MSG, NULL)
  STMT_SELECT_FIND_CHECK(schema, select_stmt, NULL)
  mysqlx_stmt_t *stmt = schema->stmt_op(name, op_type, select_stmt);
  return stmt;
  SAFE_EXCEPTION_END(schema, NULL)
}


mysqlx_stmt_t *
mysqlx_view_create_new(mysqlx_schema_t *schema, const char *name,
                       mysqlx_stmt_t *select_stmt)
{
  return _mysqlx_view_new(schema, name, select_stmt, OP_VIEW_CREATE);
}


mysqlx_stmt_t *
mysqlx_view_modify_new(mysqlx_schema_t *schema, const char *name,
                       mysqlx_stmt_t *select_stmt)
{
  return _mysqlx_view_new(schema, name, select_stmt, OP_VIEW_UPDATE);
}


mysqlx_stmt_t *
mysqlx_view_replace_new(mysqlx_schema_t *schema, const char *name,
                        mysqlx_stmt_t *select_stmt)
{
  return _mysqlx_view_new(schema, name, select_stmt, OP_VIEW_REPLACE);
}


PUBLIC_API mysqlx_result_t *
_mysqlx_view(mysqlx_schema_t *schema, const char *name,
             mysqlx_stmt_t *select_stmt,
             mysqlx_op_t op_type, va_list args)
{
  SAFE_EXCEPTION_BEGIN(schema, NULL)
  mysqlx_stmt_t *stmt;

  if ((stmt =_mysqlx_view_new(schema, name, select_stmt,
                              op_type)) == NULL)
    return NULL;

  stmt->set_view_properties(args);

  mysqlx_result_t *res = mysqlx_execute(stmt);
  if (res == NULL)
    SET_ERROR_FROM_STMT(schema, stmt, NULL);

  return res;
  SAFE_EXCEPTION_END(schema, NULL)
}

PUBLIC_API mysqlx_result_t *
mysqlx_view_create(mysqlx_schema_t *schema, const char *name,
                   mysqlx_stmt_t *select_stmt, ...)
{
  va_list args;
  va_start(args, select_stmt);
  mysqlx_result_t *res = _mysqlx_view(schema, name, select_stmt,
                                      OP_VIEW_CREATE, args);
  va_end(args);
  return res;
}

PUBLIC_API mysqlx_result_t *
mysqlx_view_replace(mysqlx_schema_t *schema, const char *name,
                    mysqlx_stmt_t *select_stmt, ...)
{
  va_list args;
  va_start(args, select_stmt);
  mysqlx_result_t *res = _mysqlx_view(schema, name, select_stmt,
                                      OP_VIEW_REPLACE, args);
  va_end(args);
  return res;
}

PUBLIC_API mysqlx_result_t *
mysqlx_view_modify(mysqlx_schema_t *schema, const char *name,
                   mysqlx_stmt_t *select_stmt, ...)
{
  va_list args;
  va_start(args, select_stmt);
  mysqlx_result_t *res = _mysqlx_view(schema, name, select_stmt,
                                      OP_VIEW_UPDATE, args);
  va_end(args);
  return res;
}

int mysqlx_set_view_algorithm(mysqlx_stmt_t *view_stmt, int algorithm)
{
  SAFE_EXCEPTION_BEGIN(view_stmt, RESULT_ERROR)
  view_stmt->set_view_algorithm(algorithm);
  return RESULT_OK;
  SAFE_EXCEPTION_END(view_stmt, RESULT_ERROR)
}

int mysqlx_set_view_security(mysqlx_stmt_t *view_stmt, int security)
{
  SAFE_EXCEPTION_BEGIN(view_stmt, RESULT_ERROR)
  view_stmt->set_view_security(security);
  return RESULT_OK;
  SAFE_EXCEPTION_END(view_stmt, RESULT_ERROR)
}

int mysqlx_set_view_check_option(mysqlx_stmt_t *view_stmt, int option)
{
  SAFE_EXCEPTION_BEGIN(view_stmt, RESULT_ERROR)
  view_stmt->set_view_check_option(option);
  return RESULT_OK;
  SAFE_EXCEPTION_END(view_stmt, RESULT_ERROR)
}

int mysqlx_set_view_definer(mysqlx_stmt_t *view_stmt, const char* user)
{
  SAFE_EXCEPTION_BEGIN(view_stmt, RESULT_ERROR)
  view_stmt->set_view_definer(user);
  return RESULT_OK;
  SAFE_EXCEPTION_END(view_stmt, RESULT_ERROR)
}

int mysqlx_set_view_columns(mysqlx_stmt_t *view_stmt, ...)
{
  SAFE_EXCEPTION_BEGIN(view_stmt, RESULT_ERROR)
  va_list args;
  va_start(args, view_stmt);
  view_stmt->set_view_columns(args);
  va_end(args);
  return RESULT_OK;
  SAFE_EXCEPTION_END(view_stmt, RESULT_ERROR)
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