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

#include <stdio.h>
#include <string.h>
#include "test.h"

TEST_F(xapi, store_result_select)
{
  SKIP_IF_NO_XPLUGIN

  MYSQLX_CRUD *crud;
  MYSQLX_RESULT *res;
  MYSQLX_ROW *row;
  size_t row_num = 0, col_num = 0;
  /*
    Query produces 3 rows in one result-set
  */
  const char * query = "SELECT 100 as col_1, 'abc' as col_2, 9.8765E+2 "\
                       "UNION SELECT 200, 'def', 4.321E+1 " \
                       "UNION SELECT 300, 'ghi', 2.468765E+3";

  AUTHENTICATE();

  RESULT_CHECK(crud = mysqlx_sql_query(get_session(), query, strlen(query)));
  CRUD_CHECK(res = mysqlx_crud_execute(crud), crud);

  col_num = mysqlx_column_get_count(res);
  EXPECT_EQ(3, col_num);

  EXPECT_EQ(RESULT_OK, mysqlx_store_result(res, &row_num));
  EXPECT_EQ(3, row_num);

  while ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    int64_t col1 = 0;
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &col1));

    char col2[32];
    size_t col2_len = sizeof(col2);
    EXPECT_EQ(RESULT_OK, mysqlx_get_bytes(row, 1, 0, col2, &col2_len));

    double col3 = 0;
    EXPECT_EQ(RESULT_OK, mysqlx_get_double(row, 2, &col3));

    cout << "ROW DATA: " << col1 << " " << col2 << " " << col3 << endl;

  }
  cout << "DONE" << endl;

}

TEST_F(xapi, store_result_find)
{
  SKIP_IF_NO_XPLUGIN

  MYSQLX_CRUD *crud;
  MYSQLX_RESULT *res;
  size_t row_num = 0, json_len = 0;
  int i;
  const char *c;
  const char *json_add[3] = {
    "{ \"key_1\": 1, \"key_2\": \"abc 1\"}",
    "{ \"key_1\": 2, \"key_2\": \"def 2\"}",
    "{ \"key_1\": 3, \"key_2\": \"ghi 3\"}"
  };
  const char *json_result[3] = { "", "", "" };

  AUTHENTICATE();

  exec_sql("DROP DATABASE IF EXISTS cc_ddl_test");
  exec_sql("CREATE DATABASE cc_ddl_test");

  EXPECT_EQ(RESULT_OK, mysqlx_collection_create(get_session(), "cc_ddl_test", "store_result_test"));

  RESULT_CHECK(crud = mysqlx_collection_add_new(get_session(), "cc_ddl_test", "store_result_test"));
  for (i = 0; i < 3; ++i)
  {
    EXPECT_EQ(RESULT_OK, mysqlx_set_add_document(crud, json_add[i]));
    printf("\nJSON FOR ADD %d [ %s ]", i + 1, json_add[i]);
  }
  CRUD_CHECK(res = mysqlx_crud_execute(crud), crud);

  // Expect error for an operation without the resulting data
  EXPECT_EQ(RESULT_ERROR, mysqlx_store_result(res, &row_num));
  printf("\n Expected error: %s", mysqlx_crud_error_message(crud));

  RESULT_CHECK(crud = mysqlx_collection_find_new(get_session(), "cc_ddl_test", "store_result_test"));
  EXPECT_EQ(RESULT_OK, mysqlx_set_find_order_by(crud, "key_1", SORT_ORDER_ASC, PARAM_END));
  CRUD_CHECK(res = mysqlx_crud_execute(crud), crud);

  EXPECT_EQ(RESULT_OK, mysqlx_store_result(res, &row_num));
  EXPECT_EQ(3, row_num);

  i = 0;
  while ((c = mysqlx_json_fetch_one(res, &json_len)) != NULL)
  {
    if (c)
    {
      json_result[i] = c;
      printf("\n[json: %s]", json_result[i]);
    }
    ++i;
  }

  EXPECT_EQ(3, i);

  /*
    With buffered result all buffers are retained.
    Therefore json_result[i] should have their own values.
    Note: with unbuffered result the previously fetched JSON
          string becomes invalid or points to a next JSON string.
  */
  EXPECT_TRUE(strstr(json_result[0], "\"key_1\": 1, \"key_2\": \"abc 1\"") != NULL);
  EXPECT_TRUE(strstr(json_result[1], "\"key_1\": 2, \"key_2\": \"def 2\"") != NULL);
  EXPECT_TRUE(strstr(json_result[2], "\"key_1\": 3, \"key_2\": \"ghi 3\"") != NULL);
}


TEST_F(xapi, next_result)
{
  SKIP_IF_NO_XPLUGIN

  MYSQLX_CRUD *crud;
  MYSQLX_RESULT *res;
  MYSQLX_ROW *row;
  size_t row_num = 0, col_num = 0;

  const char * drop_db = "DROP DATABASE IF EXISTS cc_api_test";
  const char * create_db = "CREATE DATABASE cc_api_test";
  const char * create_proc = "CREATE DEFINER = CURRENT_USER PROCEDURE cc_api_test.test_proc()\n" \
    "BEGIN\n" \
    "SELECT 100 as col_1, 'abc' as col_2, 9.8765E+2;"\
    "SELECT 200, 'def', 4.321E+1;" \
    "SELECT 300, 'ghi', 2.468765E+3;" \
    "END;";

  const char * query = "CALL cc_api_test.test_proc()";
  authenticate();

  exec_sql(drop_db);
  exec_sql(create_db);
  exec_sql(create_proc);

  RESULT_CHECK(crud = mysqlx_sql_query(get_session(), query, strlen(query)));
  CRUD_CHECK(res = mysqlx_crud_execute(crud), crud);

  do
  {
    col_num = mysqlx_column_get_count(res);
    EXPECT_EQ(3, col_num);
    EXPECT_EQ(RESULT_OK, mysqlx_store_result(res, &row_num));
    EXPECT_EQ(1, row_num);

    while ((row = mysqlx_row_fetch_one(res)) != NULL)
    {
      int64_t col1 = 0;
      EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &col1));

      char col2[32];
      size_t col2_len = sizeof(col2);
      EXPECT_EQ(RESULT_OK, mysqlx_get_bytes(row, 1, 0, col2, &col2_len));

      double col3 = 0;
      EXPECT_EQ(RESULT_OK, mysqlx_get_double(row, 2, &col3));

       cout << "ROW DATA: " << col1 << " " << col2 << " " << col3 << endl;
    }

  } while (mysqlx_next_result(res) == RESULT_OK);

  cout << "DONE" << endl;
}

TEST_F(xapi, warnings_test)
{
  SKIP_IF_NO_XPLUGIN

  MYSQLX_CRUD *crud;
  MYSQLX_RESULT *res;
  MYSQLX_ERROR *warn;
  int warn_count = 0;

  authenticate();

  exec_sql("CREATE TABLE cc_api_test.warn_tab (a TINYINT NOT NULL, b CHAR(4))");
  exec_sql("SET sql_mode=''"); // We want warnings, not errors

  RESULT_CHECK(crud = mysqlx_table_insert_new(get_session(), "cc_api_test", "warn_tab"));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_columns(crud, "a", "b", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(crud, PARAM_SINT(100), PARAM_STRING("mysql"), PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(crud, PARAM_NULL(), PARAM_STRING("test"), PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(crud, PARAM_SINT(300), PARAM_STRING("xyz"), PARAM_END));

  CRUD_CHECK(res = mysqlx_crud_execute(crud), crud);

  EXPECT_EQ(3, mysqlx_result_warning_count(res));

  while((warn = mysqlx_result_next_warning(res)) != NULL)
  {
    printf("\nWarning: %d %s", mysqlx_error_num(warn), mysqlx_error_message(warn));
    ++warn_count;
  }
  EXPECT_EQ(3, warn_count);
}

TEST_F(xapi, auto_increment_test)
{
  SKIP_IF_NO_XPLUGIN

  MYSQLX_CRUD *crud;
  MYSQLX_RESULT *res;
  int i = 0;

  authenticate();

  exec_sql("CREATE TABLE cc_api_test.autoinc_tab" \
           "(id int auto_increment primary key, vchar varchar(32))");

  RESULT_CHECK(crud = mysqlx_table_insert_new(get_session(), "cc_api_test", "autoinc_tab"));

  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_columns(crud, "id", "vchar", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(crud, PARAM_SINT(100), PARAM_STRING("mysql"), PARAM_END));
  CRUD_CHECK(res = mysqlx_crud_execute(crud), crud);
  EXPECT_EQ(100, mysqlx_get_auto_increment_value(res));

  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_columns(crud, "vchar", PARAM_END));
  // Prepare 15 rows to insert
  for (i = 0; i < 15; ++i)
    EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(crud, PARAM_STRING("mysql"), PARAM_END));

  CRUD_CHECK(res = mysqlx_crud_execute(crud), crud);

  EXPECT_EQ(101, mysqlx_get_auto_increment_value(res));

  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_columns(crud, "vchar", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(crud, PARAM_STRING("mysql"), PARAM_END));
  CRUD_CHECK(res = mysqlx_crud_execute(crud), crud);

  EXPECT_EQ(116, mysqlx_get_auto_increment_value(res));
}

TEST_F(xapi, conn_string_test)
{
  SKIP_IF_NO_XPLUGIN

  unsigned short port = 0;
  char conn_error[MYSQLX_MAX_ERROR_LEN] = { 0 };
  char conn_str[1024];
  int conn_err_code = 0;
  const char *xplugin_port = getenv("XPLUGIN_PORT");
  const char *xplugin_usr = getenv("XPLUGIN_USER");
  const char *xplugin_pwd = getenv("XPLUGIN_PASSWORD");
  const char *xplugin_host = getenv("XPLUGIN_HOST");

  MYSQLX_SESSION *local_sess;
  MYSQLX_CRUD *crud;
  MYSQLX_RESULT *res;
  MYSQLX_ROW *row;

  if (xplugin_port)
    port= atoi(xplugin_port);


  xplugin_usr = (xplugin_usr && strlen(xplugin_usr) ? xplugin_usr : "root");
  xplugin_pwd = (xplugin_pwd && strlen(xplugin_pwd) ? xplugin_pwd : NULL);
  xplugin_host = (xplugin_host && strlen(xplugin_host) ? xplugin_host : "127.0.0.1");

  if (xplugin_pwd)
    sprintf(conn_str, "%s:%s@%s:%d", xplugin_usr, xplugin_pwd, xplugin_host, port);
  else
    sprintf(conn_str, "%s@%s:%d", xplugin_usr, xplugin_host, port);

  local_sess = mysqlx_get_node_session(conn_str, conn_error, &conn_err_code);

  if (!local_sess)
  {
    FAIL() << "Could not connect to xplugin. " << port << std::endl << conn_error <<
            " ERROR CODE: " << conn_err_code;
  }
  cout << "Connected to xplugin..." << endl;

  RESULT_CHECK(crud = mysqlx_sql_query(local_sess, "SELECT 'foo'", MYSQLX_NULL_TERMINATED));
  CRUD_CHECK(res = mysqlx_crud_execute(crud), crud);

  while ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    char data[32];
    size_t data_len = sizeof(data);
    EXPECT_EQ(RESULT_OK, mysqlx_get_bytes(row, 0, 0, data, &data_len));
    EXPECT_STREQ("foo", data);
    cout << "ROW DATA: " << data << " " << endl;
  }
}

TEST_F(xapi, default_db_test)
{
  SKIP_IF_NO_XPLUGIN

  MYSQLX_CRUD *crud;
  MYSQLX_RESULT *res;
  MYSQLX_ROW *row;

  // Use default user name and pwd, but set the database
  authenticate(NULL, NULL, "cc_api_test");

  exec_sql("CREATE TABLE default_tab(a INT, b VARCHAR(32))");

  RESULT_CHECK(crud = mysqlx_table_insert_new(get_session(), NULL, "default_tab"));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_columns(crud, "a", "b", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(crud, PARAM_SINT(200), PARAM_STRING("mysql"), PARAM_END));
  CRUD_CHECK(res = mysqlx_crud_execute(crud), crud);

  RESULT_CHECK(crud = mysqlx_table_select_new(get_session(), NULL, "default_tab"));
  CRUD_CHECK(res = mysqlx_crud_execute(crud), crud);

  while ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    char buf[256];
    size_t buflen = sizeof(buf);
    int64_t a = 0;

    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &a));
    EXPECT_EQ(200, a);

    EXPECT_EQ(RESULT_OK, mysqlx_get_bytes(row, 1, 0, buf, &buflen));
    EXPECT_STREQ("mysql", buf);
  }
}

TEST_F(xapi, no_cursor_test)
{
  SKIP_IF_NO_XPLUGIN

  MYSQLX_RESULT *res;

  authenticate();

  SESS_CHECK(res = mysqlx_sql_exec(get_session(), "set @any_var=1", MYSQLX_NULL_TERMINATED));
  EXPECT_EQ(0, mysqlx_column_get_count(res));
  EXPECT_EQ(NULL, mysqlx_column_get_catalog(res, 0));
  EXPECT_EQ(0, mysqlx_column_get_type(res, 0));
}

TEST_F(xapi, transaction_test)
{
  SKIP_IF_NO_XPLUGIN

  MYSQLX_RESULT *res;
  MYSQLX_ROW *row;

  authenticate(NULL, NULL, "cc_api_test");
  exec_sql("CREATE TABLE transact_tab(a INT)");

  EXPECT_EQ(RESULT_OK, mysqlx_transaction_begin(get_session()));
  SESS_CHECK( res = mysqlx_table_insert_exec(get_session(), NULL, "transact_tab",
                                             "a", PARAM_SINT(200), PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_transaction_commit(get_session()));

  // Check how the row was inserted after committing the transaction
  SESS_CHECK( res = mysqlx_table_select_exec(get_session(), NULL, "transact_tab", "a > 0"));

  while ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    int64_t a = 0;
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &a));
    EXPECT_EQ(200, a);
  }

  EXPECT_EQ(RESULT_OK, mysqlx_transaction_begin(get_session()));
  SESS_CHECK(res = mysqlx_table_delete_exec(get_session(), NULL, "transact_tab", "a > 0"));
  EXPECT_EQ(RESULT_OK, mysqlx_transaction_rollback(get_session()));

  // Check how the row was not deleted after rolling back the transaction
  SESS_CHECK( res = mysqlx_table_select_exec(get_session(), NULL, "transact_tab", "a > 0"));

  while ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    int64_t a = 0;
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &a));
    EXPECT_EQ(200, a);
  }
}

TEST_F(xapi, doc_id_test)
{
  SKIP_IF_NO_XPLUGIN

  MYSQLX_RESULT *res;
  MYSQLX_CRUD *crud;
  const char *id;
  char id_buf[2][128];
  const char * json_string = NULL;
  int id_index = 0;
  int i = 0;
  size_t json_len = 0;

  authenticate(NULL, NULL, "cc_api_test");
  EXPECT_EQ(RESULT_OK, mysqlx_collection_create(get_session(), NULL, "doc_id_test"));

  SESS_CHECK(res = mysqlx_collection_add_exec(get_session(), NULL, "doc_id_test",
                   "{\"a\" : \"12345\"}", "{\"a\" : \"abcde\"}",
                   PARAM_END));

  while ((id = mysqlx_fetch_doc_id(res)) != NULL)
  {
    /*
      We need to copythe the returned string because it will become invalid
      when we start the FIND operation to read the actual JSON data
    */
    strcpy(id_buf[id_index], id);
    ++id_index;
  }

  RESULT_CHECK(crud = mysqlx_collection_find_new(get_session(), NULL, "doc_id_test"));
  EXPECT_EQ(RESULT_OK, mysqlx_set_find_order_by(crud, "a", SORT_ORDER_ASC, PARAM_END));
  CRUD_CHECK(res = mysqlx_crud_execute(crud), crud);

  while ((json_string = mysqlx_json_fetch_one(res, &json_len)) != NULL)
  {
    if (json_string)
      printf("\n[json: %s][doc_id: %s]", json_string, id_buf[i]);

    EXPECT_TRUE(strstr(json_string, id_buf[i]) != NULL);
    ++i;
  }
}