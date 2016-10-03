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

#include <stdio.h>
#include <mysqlx.h>

/* Error processing macros */
#define CRUD_CHECK(C, S) if (!C) \
  { \
    printf("\nError! %s", mysqlx_error_message(mysqlx_session_error(S))); \
    return -1; \
  }

#define RESULT_CHECK(R, C) if (!R) \
  { \
    printf("\nError! %s", mysqlx_error_message(mysqlx_crud_error(C))); \
    return -1; \
  }

#define IS_OK(R, C)  if (R != RESULT_OK) \
  { \
    printf("\nError! %s", mysqlx_error_message(mysqlx_crud_error(C))); \
    return -1; \
  }

#ifdef _WIN32
#define format_64 "[%s: %I64d] "
#else
#define format_64 "[%s: %lld] "
#endif

int main(int argc, const char* argv[])
{
  MYSQLX_SESSION *sess;
  MYSQLX_ERROR *error = NULL;
  MYSQLX_CRUD *crud;
  MYSQLX_RESULT *res;
  MYSQLX_ROW *row;

  const char *host = "127.0.0.1";
  const char *user = "dbs";
  const char *passw = "dbspass";
  const char *database = "";
  int port = 33060;

  char conn_error[MYSQLX_MAX_ERROR_LEN];
  int conn_err_code;

  int64_t v_sint = -17;
  uint64_t v_uint = 101;
  float v_float = 3.31f;
  double v_double = 1.7E+08;
  const char *v_str = "just some text";

  int rc = 0;
  int row_num = 0;

  /*
    Connect and create session.
  */
  sess = mysqlx_get_session_s(host, port, user, passw, database, conn_error,
                              &conn_err_code);
  if (!sess)
  {
    printf("\nError! %s. Error Code: %d", conn_error, conn_err_code);
    return -1;
  }

  printf("\nConnected...");

  /* Drop test table if exists */
  crud = mysqlx_sql_query(sess, 
                          "DROP TABLE IF EXISTS cc_crud_test.crud_placeholder_test",
                          MYSQLX_NULL_TERMINATED);
  CRUD_CHECK(crud, sess);

  /* Execute the query */
  res = mysqlx_crud_execute(crud);
  RESULT_CHECK(res, crud);

  /* Create a test table */
  crud = mysqlx_sql_query(sess, 
                          "CREATE TABLE cc_crud_test.crud_placeholder_test " \
                          "(sint BIGINT, uint BIGINT UNSIGNED, flv FLOAT," \
                          "dbv DOUBLE, strv VARCHAR(255))",
                          MYSQLX_NULL_TERMINATED);
  CRUD_CHECK(crud, sess);

  /* Execute the query */
  res = mysqlx_crud_execute(crud);
  RESULT_CHECK(res, crud);
  printf("\nTable created...");

  /* Do insert as a plain SQL with parameters */
  crud = mysqlx_sql_query(sess, 
                          "INSERT INTO cc_crud_test.crud_placeholder_test " \
                          "(sint, uint, flv, dbv, strv) VALUES (?,?,?,?,?)",
                          MYSQLX_NULL_TERMINATED);
  CRUD_CHECK(crud, sess);

  /* Provide the parameter values */
  rc = mysqlx_crud_bind(crud, PARAM_SINT(v_sint),
                              PARAM_UINT(v_uint), 
                              PARAM_FLOAT(v_float),
                              PARAM_DOUBLE(v_double),
                              PARAM_STRING(v_str),
                              PARAM_END);
  IS_OK(rc, crud);

  /* Execute the query */
  res = mysqlx_crud_execute(crud);
  RESULT_CHECK(res, crud);

  crud = mysqlx_table_insert_new(sess, "cc_crud_test",
                                 "crud_placeholder_test");
  
  /* Change values to have some differences in inserted rows */
  v_sint = -232;
  v_uint = 789;
  v_float = 99.34f;
  v_double = 2.84532E-5;
  v_str = "some more text";

  rc = mysqlx_set_insert_row(crud, PARAM_SINT(v_sint),
                             PARAM_UINT(v_uint), 
                             PARAM_FLOAT(v_float),
                             PARAM_DOUBLE(v_double),
                             PARAM_STRING(v_str),
                             PARAM_END);
  IS_OK(rc, crud);

  /* Execute the query */
  res = mysqlx_crud_execute(crud);
  RESULT_CHECK(res, crud);

  printf("\nRows inserted...");

  /* Read the rows we have just inserted */
  crud = mysqlx_table_select_new(sess, "cc_crud_test", "crud_placeholder_test");
  CRUD_CHECK(crud, sess);

  /* Add WHERE clause */
  rc = mysqlx_set_select_where(crud, "(sint < 10) AND (UINT > 100)");
  IS_OK(rc, crud);

  /* Limit the number of rows */
  rc = mysqlx_set_select_limit_and_offset(crud, 500, 0);
  IS_OK(rc, crud);

  res = mysqlx_crud_execute(crud);
  RESULT_CHECK(res, crud);

  printf("\n\nReading Rows:");
  while (row = mysqlx_row_fetch_one(res))
  {
    int64_t v_sint2 = 0;
    uint64_t v_uint2 = 0;
    float v_float2 = 0;
    double v_double2 = 0;
    char v_str2[256];
    const char *col_name;
    size_t buf_len = sizeof(v_str2);

    printf("\nRow # %d: ", ++row_num);

    IS_OK(mysqlx_get_sint(row, 0, &v_sint2), crud);
    col_name = mysqlx_column_get_name(res, 0);
    printf(format_64, col_name, v_sint2);

    IS_OK(mysqlx_get_uint(row, 1, &v_uint2), crud);
    col_name = mysqlx_column_get_name(res, 1);
    printf(format_64, col_name, v_uint2);

    IS_OK(mysqlx_get_float(row, 2, &v_float2), crud);
    col_name = mysqlx_column_get_name(res, 2);
    printf("[%s: %f]", col_name, v_float2);

    IS_OK(mysqlx_get_double(row, 3, &v_double2), crud);
    col_name = mysqlx_column_get_name(res, 3);
    printf("[%s: %f]", col_name, v_double2);

    IS_OK(mysqlx_get_bytes(row, 4, 0, v_str2, &buf_len), crud);
    col_name = mysqlx_column_get_name(res, 4);
    printf("[%s: %s [%d bytes]]", col_name, v_str2, buf_len);
  }

  mysqlx_session_close(sess);
  printf("\nSession closed");
}
