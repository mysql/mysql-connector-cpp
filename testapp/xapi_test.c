/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
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
#include <mysql_xapi.h>

/* Error processing macros */
#define CRUD_CHECK(C, S) if (!C) \
  { \
    printf("\nError! %s", mysqlx_error_message(S)); \
    return -1; \
  }

#define RESULT_CHECK(R, C) if (!R) \
  { \
    printf("\nError! %s", mysqlx_error_message(C)); \
    return -1; \
  }

#define IS_OK(R, C)  if (R != RESULT_OK) \
  { \
    printf("\nError! %s", mysqlx_error_message(C)); \
    return -1; \
  }

#ifdef _WIN32
#define format_64 "[%s: %I64d] "
#else
#define format_64 "[%s: %lld] "
#endif

int main(int argc, const char* argv[])
{
  mysqlx_session_t  *sess;
  mysqlx_stmt_t     *crud;
  mysqlx_result_t   *res;
  mysqlx_row_t      *row;
  mysqlx_schema_t   *db;
  mysqlx_table_t    *table;

  const char   *url = (argc > 1 ? argv[1] : "mysqlx://root@127.0.0.1");

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

    We use node session to be able to execute SQL queries.
  */

  sess = mysqlx_get_node_session_from_url(url, conn_error, &conn_err_code);
  if (!sess)
  {
    printf("\nError! %s. Error Code: %d", conn_error, conn_err_code);
    return -1;
  }

  printf("\nConnected...");

  /* Drop test table if exists */

  res = mysqlx_sql(sess,
                   "DROP TABLE IF EXISTS test.crud_placeholder_test",
                   MYSQLX_NULL_TERMINATED);
  RESULT_CHECK(res, sess);

  /* Create a test table */

  res = mysqlx_sql(sess,
                   "CREATE TABLE test.crud_placeholder_test " \
                   "(sint BIGINT, uint BIGINT UNSIGNED, flv FLOAT," \
                   "dbv DOUBLE, strv VARCHAR(255))",
                   MYSQLX_NULL_TERMINATED);
  RESULT_CHECK(res, sess);
  printf("\nTable created...");

  /* Do insert as a plain SQL with parameters */

  crud = mysqlx_sql_new(sess,
                        "INSERT INTO test.crud_placeholder_test " \
                        "(sint, uint, flv, dbv, strv) VALUES (?,?,?,?,?)",
                        MYSQLX_NULL_TERMINATED);
  CRUD_CHECK(crud, sess);

  /* Provide the parameter values */

  rc = mysqlx_stmt_bind(crud, PARAM_SINT(v_sint),
                              PARAM_UINT(v_uint),
                              PARAM_FLOAT(v_float),
                              PARAM_DOUBLE(v_double),
                              PARAM_STRING(v_str),
                              PARAM_END);
  IS_OK(rc, crud);

  /* Execute the query */

  res = mysqlx_execute(crud);
  RESULT_CHECK(res, crud);

  /*
    Query table using CRUD operations.
  */

  db = mysqlx_get_schema(sess, "test", 1);
  RESULT_CHECK(db, sess);

  table = mysqlx_get_table(db, "crud_placeholder_test", 1);
  RESULT_CHECK(table, db);

  crud = mysqlx_table_insert_new(table);

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

  res = mysqlx_execute(crud);
  RESULT_CHECK(res, crud);

  printf("\nRows inserted...");

  /* Read the rows we have just inserted, limit to 500 rows, no sorting. */

  res = mysqlx_table_select_limit(table,
          "(sint < 10) AND (UINT > 100)", 500, 0, PARAM_END);
  RESULT_CHECK(res, table);

  printf("\n\nReading Rows:");
  while ((row = mysqlx_row_fetch_one(res)))
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
    printf(format_64, col_name, (long long int)v_sint2);

    IS_OK(mysqlx_get_uint(row, 1, &v_uint2), crud);
    col_name = mysqlx_column_get_name(res, 1);
    printf(format_64, col_name, (long long int)v_uint2);

    IS_OK(mysqlx_get_float(row, 2, &v_float2), crud);
    col_name = mysqlx_column_get_name(res, 2);
    printf("[%s: %f]", col_name, v_float2);

    IS_OK(mysqlx_get_double(row, 3, &v_double2), crud);
    col_name = mysqlx_column_get_name(res, 3);
    printf("[%s: %f]", col_name, v_double2);

    IS_OK(mysqlx_get_bytes(row, 4, 0, v_str2, &buf_len), crud);
    col_name = mysqlx_column_get_name(res, 4);
    printf("[%s: %s [%u bytes]]", col_name, v_str2, (unsigned)buf_len);
  }

  mysqlx_session_close(sess);
  printf("\nSession closed");
  return 0;
}
