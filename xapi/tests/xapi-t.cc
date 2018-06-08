/*
 * Copyright (c) 2016, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#include <stdio.h>
#include <string.h>
#include <climits>
#include "test.h"


TEST_F(xapi, store_result_select)
{
  SKIP_IF_NO_XPLUGIN

  mysqlx_stmt_t *stmt;
  mysqlx_result_t *res;
  mysqlx_row_t *row;
  size_t row_num = 0, col_num = 0;
  /*
    Query produces 3 rows in one result-set
  */
  const char * query = "SELECT 100 as col_1, 'abc' as col_2, 9.8765E+2 "\
                       "UNION SELECT 200, 'def', 4.321E+1 " \
                       "UNION SELECT 300, 'ghi', 2.468765E+3";

  AUTHENTICATE();

  RESULT_CHECK(stmt = mysqlx_sql_new(get_session(), query, strlen(query)));
  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

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

  mysqlx_stmt_t *stmt;
  mysqlx_result_t *res;
  mysqlx_schema_t *schema;
  mysqlx_collection_t *collection;

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

  EXPECT_TRUE((schema = mysqlx_get_schema(get_session(), "cc_ddl_test", 1)) != NULL);
  EXPECT_EQ(RESULT_OK, mysqlx_collection_create(schema, "store_result_test"));
  EXPECT_TRUE((collection = mysqlx_get_collection(schema, "store_result_test", 1)) != NULL);

  RESULT_CHECK(stmt = mysqlx_collection_add_new(collection));
  for (i = 0; i < 3; ++i)
  {
    EXPECT_EQ(RESULT_OK, mysqlx_set_add_document(stmt, json_add[i]));
    printf("\nJSON FOR ADD %d [ %s ]", i + 1, json_add[i]);
  }
  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

  // Expect error for an operation without the resulting data
  EXPECT_EQ(RESULT_ERROR, mysqlx_store_result(res, &row_num));
  printf("\n Expected error: %s", mysqlx_error_message(res));

  RESULT_CHECK(stmt = mysqlx_collection_find_new(collection));

  EXPECT_EQ(RESULT_OK, mysqlx_set_find_order_by(stmt, "key_1", SORT_ORDER_ASC, PARAM_END));
  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

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

  mysqlx_stmt_t *stmt;
  mysqlx_result_t *res;
  mysqlx_row_t *row;
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

  RESULT_CHECK(stmt = mysqlx_sql_new(get_session(), query, strlen(query)));
  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

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

  mysqlx_stmt_t *stmt;
  mysqlx_result_t *res;
  mysqlx_error_t *warn;
  mysqlx_schema_t *schema;
  mysqlx_table_t *table, *table2;

  int warn_count = 0;

  authenticate();

  mysqlx_schema_drop(get_session(), "cc_api_test");
  mysqlx_schema_create(get_session(), "cc_api_test");

  exec_sql("CREATE TABLE cc_api_test.warn_tab (a TINYINT NOT NULL, b CHAR(4))");
  exec_sql("CREATE TABLE cc_api_test.warn_tab2 (a bigint,b int unsigned not NULL,c char(4),d decimal(2,1))");
  exec_sql("SET sql_mode=''"); // We want warnings, not errors

  EXPECT_TRUE((schema = mysqlx_get_schema(get_session(), "cc_api_test", 1)) != NULL);
  EXPECT_TRUE((table = mysqlx_get_table(schema, "warn_tab", 1)) != NULL);

  RESULT_CHECK(stmt = mysqlx_table_insert_new(table));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_columns(stmt, "a", "b", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_SINT(100), PARAM_STRING("mysql"), PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_NULL(), PARAM_STRING("test"), PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_SINT(300), PARAM_STRING("xyz"), PARAM_END));

  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

  EXPECT_EQ(3, mysqlx_result_warning_count(res));

  while((warn = mysqlx_result_next_warning(res)) != NULL)
  {
    printf("\nWarning: %d %s", mysqlx_error_num(warn), mysqlx_error_message(warn));
    ++warn_count;
  }
  EXPECT_EQ(3, warn_count);

  authenticate();

  EXPECT_TRUE((schema = mysqlx_get_schema(get_session(), "cc_api_test", 1)) != NULL);
  EXPECT_TRUE((table2 = mysqlx_get_table(schema, "warn_tab2", 1)) != NULL);
  EXPECT_TRUE((res = mysqlx_table_insert(table2, "a", PARAM_SINT(1), "b", PARAM_UINT(10),
                            "c", PARAM_STRING("a"), "d", PARAM_NULL(), PARAM_END)) != NULL);
  EXPECT_TRUE((res = mysqlx_sql_param(get_session(),
               "SELECT (`c` / ?),(`a` / 0),(1 / `b`),(`a` / ?) FROM " \
               "`cc_api_test`.`warn_tab2` ORDER BY (`c` / ?)",
               MYSQLX_NULL_TERMINATED, PARAM_SINT(0), PARAM_STRING("x"),
               PARAM_SINT(0), PARAM_END)) != NULL);

  /* All rows have to be read before getting warnings */
  mysqlx_store_result(res, NULL);

  // Bug #28047970
  SKIP_TEST("Bug #28047970");

  EXPECT_EQ(7, mysqlx_result_warning_count(res));
  warn_count = 0;
  while ((warn = mysqlx_result_next_warning(res)) != NULL)
  {
    printf("\nWarning: %d %s", mysqlx_error_num(warn), mysqlx_error_message(warn));
    ++warn_count;
  }
  EXPECT_EQ(7, warn_count);

}


TEST_F(xapi, auto_increment_test)
{
  SKIP_IF_NO_XPLUGIN

  mysqlx_stmt_t *stmt;
  mysqlx_result_t *res;
  mysqlx_schema_t *schema;
  mysqlx_table_t *table;

  int i = 0;

  authenticate();

  EXPECT_EQ(RESULT_OK, mysqlx_schema_drop(get_session(), "cc_api_test"));
  EXPECT_EQ(RESULT_OK, mysqlx_schema_create(get_session(), "cc_api_test"));

  exec_sql("CREATE TABLE cc_api_test.autoinc_tab" \
           "(id int auto_increment primary key, vchar varchar(32))");

  EXPECT_TRUE((schema = mysqlx_get_schema(get_session(), "cc_api_test", 1)) != NULL);
  EXPECT_TRUE((table = mysqlx_get_table(schema, "autoinc_tab", 1)) != NULL);
  RESULT_CHECK(stmt = mysqlx_table_insert_new(table));

  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_columns(stmt, "id", "vchar", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_SINT(100), PARAM_STRING("mysql"), PARAM_END));
  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);
  EXPECT_EQ(100, mysqlx_get_auto_increment_value(res));

  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_columns(stmt, "vchar", PARAM_END));
  // Prepare 15 rows to insert
  for (i = 0; i < 15; ++i)
    EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_STRING("mysql"), PARAM_END));

  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

  EXPECT_EQ(101, mysqlx_get_auto_increment_value(res));

  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_columns(stmt, "vchar", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_STRING("mysql"), PARAM_END));
  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

  EXPECT_EQ(116, mysqlx_get_auto_increment_value(res));
}


TEST_F(xapi, conn_string_test)
{
  SKIP_IF_NO_XPLUGIN

  USE_NATIVE_PWD;

  unsigned short port = 0;
  char conn_error[MYSQLX_MAX_ERROR_LEN] = { 0 };
  std::string  conn_str_basic;

  int conn_err_code = 0;
  bool ssl_enable = false;

  mysqlx_session_t *local_sess;

  conn_str_basic = m_xplugin_usr;

  if (m_xplugin_pwd)
  {
    conn_str_basic += ":";
    conn_str_basic += m_xplugin_pwd;
  }

  conn_str_basic += "@";
  conn_str_basic += m_xplugin_host;

  if (m_xplugin_port)
  {
    conn_str_basic += ":";
    conn_str_basic += m_xplugin_port;
  }

DO_CONNECT:


  {
    std::string conn_str = conn_str_basic;

    conn_str += "/?SsL-MoDe=";
    conn_str += (ssl_enable ? "rEQuiREd" : "diSAblEd");

    local_sess = mysqlx_get_session_from_url(
      conn_str.c_str(),
      conn_error, &conn_err_code);

    if (!local_sess)
    {
      FAIL() << "Could not connect to xplugin. " << port << std::endl << conn_error <<
              " ERROR CODE: " << conn_err_code;
    }
    cout << "Connected to xplugin..." << endl;

    std::string ssl = get_ssl_cipher(local_sess);

    if (ssl_enable)
    {
      EXPECT_FALSE(ssl.empty());
      cout << "SSL Cipher: " << ssl << endl;
    }
    else
      EXPECT_TRUE(ssl.empty());

    mysqlx_session_close(local_sess);
  }

  if (!ssl_enable)
  {
    ssl_enable = true;
    goto DO_CONNECT;
  }

  authenticate();

  {
    std::string conn_str = conn_str_basic;
    conn_str += "?ssl-nonexistent=true";

    local_sess = mysqlx_get_session_from_url(
      conn_str.c_str(),
      conn_error, &conn_err_code);

    if (local_sess)
    {
      mysqlx_session_close(local_sess);
      FAIL() << "Connection should not be established" << endl;
    }
    cout << "Expected error: " << conn_error << endl;
  }

  // Get the location of server's CA file

  std::string conn_str_ca = get_ca_file();

  if (conn_str_ca.empty())
    FAIL() << "Could not get CA path from server";

  cout << "CA file: " << conn_str_ca << endl;

  /* If ssl-ca set ssl-mode can not be DISABLED or REQUIRED*/

  for (unsigned i = 0; i < 2; ++i)
  {
    std::string conn_str = conn_str_basic;
    conn_str += (i > 0 ? "/?ssl-mode=REQUIRED" : "/?ssl-mode=DISABLED");
    conn_str += "&Ssl-cA=" + conn_str_ca;

    local_sess = mysqlx_get_session_from_url(
      conn_str.c_str(),
      conn_error, &conn_err_code);

    if (local_sess)
    {
      mysqlx_session_close(local_sess);
      FAIL() << "Connection should not be established" << endl;
    }
    cout << "Expected error: " << conn_error << endl;
  }

  /* Same thing with VERIFY_CA should work */

  {
    std::string conn_str = conn_str_basic
                         + "/?ssl-mode=VERIFY_CA&ssl-ca="
                         + conn_str_ca;

    local_sess = mysqlx_get_session_from_url(
      conn_str.c_str(),
      conn_error, &conn_err_code);

    if (!local_sess)
      FAIL() << "Connection could not be established: " << conn_error << endl;

    mysqlx_session_close(local_sess);
  }

  /*
    If ssl-ca is set and ssl-mode is not specified, it should default to
    VERIFY_CA. Thus, if ssl-ca points to invalid path, we should get error when
    creating session.
  */

  {
    std::string conn_str = conn_str_basic + "?ssl-mode=VERIFY_CA&ssl-ca=wrong_ca.pem";

    local_sess = mysqlx_get_session_from_url(
      conn_str.c_str(),
      conn_error, &conn_err_code);

    if (local_sess)
    {
      mysqlx_session_close(local_sess);
      FAIL() << "Connection should not be established" << endl;
    }
    cout << "Expected error: " << conn_error << endl;
  }
}


TEST_F(xapi, failover_test)
{
  SKIP_IF_NO_XPLUGIN

  char conn_error[MYSQLX_MAX_ERROR_LEN] = { 0 };
  int conn_err_code = 0;
  unsigned int max_prio = 100;
  unsigned int prio = 0;
  const char *db_name = "failover_db";

  char buf[1024];

  mysqlx_session_t *local_sess = NULL;
  mysqlx_stmt_t *stmt;
  mysqlx_result_t *res;
  mysqlx_row_t *row;

  authenticate();
  mysqlx_schema_create(get_session(), db_name);

  /* Checking when errors should be returned */

  {
    mysqlx_session_options_t *opt = mysqlx_session_options_new();

    // No priority, should be ok

    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
      OPT_HOST(m_xplugin_host),
      OPT_PORT(m_port + 2),
      PARAM_END));

    EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_get(opt, MYSQLX_OPT_PRIORITY, &prio));
    cout << "Expected error: " << mysqlx_error_message(opt) << endl;

    // Error expected: trying to add a priority to unprioritized list

    EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_set(opt,
      OPT_HOST(m_xplugin_host),
      OPT_PRIORITY(max_prio - 1),
      OPT_PORT(m_port + 2),
      PARAM_END));
    cout << "Expected error: " << mysqlx_error_message(opt) << endl;

    // Start again, this time building list with priorities

    mysqlx_free_options(opt);
    opt = mysqlx_session_options_new();

    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                         OPT_HOST(m_xplugin_host),
                         OPT_PORT(m_port),
                         OPT_PRIORITY(max_prio),
                         PARAM_END));

    // Port is given before host, should fail

    EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_set(opt,
      OPT_PORT(m_port),
      OPT_HOST(m_xplugin_host),
      OPT_PRIORITY(max_prio - 1),
      PARAM_END));
    cout << "Expected error: " << mysqlx_error_message(opt) << endl;

    // Port is given, but no host, should fail

    EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_set(opt,
      OPT_PRIORITY(max_prio - 2),
      OPT_PORT(m_port + 2),
      PARAM_END));
    cout << "Expected error: " << mysqlx_error_message(opt) << endl;

    // No priority, should fail

    EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_set(opt,
      OPT_HOST(m_xplugin_host),
      OPT_PORT(m_port + 2),
      PARAM_END));
    cout << "Expected error: " << mysqlx_error_message(opt) << endl;

    // Priority > 100, should fail

    EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_set(opt,
      OPT_HOST(m_xplugin_host),
      OPT_PORT(m_port + 2),
      OPT_PRIORITY(101),
      PARAM_END));
    cout << "Expected error: " << mysqlx_error_message(opt) << endl;

    mysqlx_free_options(opt);
  }

  /* Positive sceanario */

  {
    mysqlx_session_options_t *opt = mysqlx_session_options_new();

    /* Set user/pass/db before setting list */
    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
      OPT_USER(m_xplugin_usr),
      OPT_PWD(m_xplugin_pwd),
      OPT_DB(db_name),
      PARAM_END));

    /* Starting to build the prioritized list */

    // The one which connects
    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
      OPT_HOST(m_xplugin_host),
      OPT_PORT(m_port),
      OPT_PRIORITY(max_prio),
      PARAM_END));

    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
      OPT_HOST("wrong_port+1"),
      OPT_PORT(m_port + 1), // Wrong port
      OPT_PRIORITY(max_prio - 1),
      OPT_HOST("wrong_port+2"),
      OPT_PORT(m_port + 2), // Wrong port
      OPT_PRIORITY(max_prio - 2),
      OPT_HOST(m_xplugin_host),
      OPT_PORT(m_port),     // Correct port
      OPT_PRIORITY(max_prio - 3),
      PARAM_END));

#ifndef _WIN32
    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
      OPT_SOCKET("/no/socket.sock"),     // invalid socket
      OPT_PRIORITY(max_prio - 3),
      PARAM_END));
#endif


    EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_HOST, buf));
    EXPECT_STRCASEEQ(m_xplugin_host, buf);

    local_sess = mysqlx_get_session_from_options(opt, conn_error, &conn_err_code);
    if (!local_sess)
    {
      mysqlx_free_options(opt);
      FAIL() << "Could not connect to xplugin. " << m_port << std::endl << conn_error <<
        " ERROR CODE: " << conn_err_code;
    }
    cout << "Connected to xplugin..." << endl;

    RESULT_CHECK(stmt = mysqlx_sql_new(local_sess, "SELECT DATABASE()", MYSQLX_NULL_TERMINATED));
    CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

    if ((row = mysqlx_row_fetch_one(res)) != NULL)
    {
      char data[128] = { 0 };
      size_t data_len = sizeof(data);
      EXPECT_EQ(RESULT_OK, mysqlx_get_bytes(row, 0, 0, data, &data_len));
      EXPECT_STRCASEEQ(db_name, data);
    }

    mysqlx_session_close(local_sess);
    mysqlx_free_options(opt);
  }

}

TEST_F(xapi, failover_test_url)
{
  SKIP_IF_NO_XPLUGIN

  char conn_error[MYSQLX_MAX_ERROR_LEN] = { 0 };
  int conn_err_code = 0;
  const char *db_name = "failover_db";
  char conn_str[4096];
  char conn_str2[4096];

  if (m_xplugin_pwd)
  {
    sprintf(conn_str, "%s:%s@[(address=%s:%d,priority=100),"\
                      "(address=%s:%d,priority=90)," \
                      "(address=%s:%d,priority=80)]/%s",
                      m_xplugin_usr, m_xplugin_pwd,
                      m_xplugin_host, m_port + 1,
                      m_xplugin_host, m_port + 2,
                      m_xplugin_host, m_port, db_name );

    sprintf(conn_str2, "%s:%s@[(address=%s:%d,priority=100),"\
                       "address=%s:%d," \
                       "(address=%s:%d,priority=80)]/%s",
                       m_xplugin_usr, m_xplugin_pwd,
                       m_xplugin_host, m_port + 1,
                       m_xplugin_host, m_port + 2,
                       m_xplugin_host, m_port, db_name);
  }
  else
  {
    sprintf(conn_str, "%s@[(address=%s:%d,priority=100),"\
                      "(address=%s:%d,priority=90)," \
                      "(address=%s:%d,priority=80)]/%s",
                      m_xplugin_usr,
                      m_xplugin_host, m_port + 1,
                      m_xplugin_host, m_port + 2,
                      m_xplugin_host, m_port, db_name );

    sprintf(conn_str2, "%s@[(address=%s:%d,priority=100),"\
                       "address=%s:%d," \
                       "(address=%s:%d,priority=80)]/%s",
                       m_xplugin_usr,
                       m_xplugin_host, m_port + 1,
                       m_xplugin_host, m_port + 2,
                       m_xplugin_host, m_port, db_name);
  }

  mysqlx_session_t *local_sess;
  mysqlx_stmt_t *stmt;
  mysqlx_result_t *res;
  mysqlx_row_t *row;

  authenticate();
  mysqlx_schema_create(get_session(), db_name);

  local_sess = mysqlx_get_session_from_url(conn_str2, conn_error, &conn_err_code);
  if (local_sess)
  {
    mysqlx_session_close(local_sess);
    FAIL() << "Wrong option was not handled correctly";
  }
  else
  {
    cout << "Expected connection error: " << conn_error << endl;
  }


  local_sess = mysqlx_get_session_from_url(conn_str, conn_error, &conn_err_code);
  if (!local_sess)
  {
    FAIL() << "Could not connect to xplugin. " << m_port << std::endl << conn_error <<
      " ERROR CODE: " << conn_err_code;
  }
  cout << "Connected to xplugin..." << endl;

  RESULT_CHECK(stmt = mysqlx_sql_new(local_sess, "SELECT DATABASE()", MYSQLX_NULL_TERMINATED));
  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

  if ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    char data[128] = { 0 };
    size_t data_len = sizeof(data);
    EXPECT_EQ(RESULT_OK, mysqlx_get_bytes(row, 0, 0, data, &data_len));
    EXPECT_STRCASEEQ(db_name, data);
  }

  mysqlx_session_close(local_sess);

  // Error when priority > 100.

  std::stringstream conn;
    conn << m_xplugin_usr;
    if (m_xplugin_pwd)
      conn << ":" <<m_xplugin_pwd;
    conn << "@[(address=" << m_xplugin_host<< ":" << m_xplugin_port << ",priority=101)]";

  local_sess = mysqlx_get_session_from_url(conn.str().c_str(),
                                           conn_error, &conn_err_code);

  if (local_sess)
  {
    FAIL() << "Should give error priority>100";
  }

  cout << "Expected error: " << conn_error << endl;

}


TEST_F(xapi, auth_method)
{
  SKIP_IF_NO_XPLUGIN

  USE_NATIVE_PWD;

  char conn_error[MYSQLX_MAX_ERROR_LEN] = { 0 };
  int conn_err_code = 0;
  mysqlx_session_t *local_sess = NULL;
  mysqlx_session_options_t *opt = mysqlx_session_options_new();
  unsigned int auth_method = 0;

  authenticate();

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                       OPT_HOST(m_xplugin_host), OPT_PORT(m_port),
                       OPT_USER(m_xplugin_usr), OPT_PWD(m_xplugin_pwd),
                       PARAM_END));

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                       OPT_SSL_MODE(SSL_MODE_DISABLED),
                       OPT_AUTH(MYSQLX_AUTH_PLAIN),
                       PARAM_END));

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_AUTH,
                                                 &auth_method));
  EXPECT_EQ(MYSQLX_AUTH_PLAIN, auth_method);

  local_sess = mysqlx_get_session_from_options(opt, conn_error, &conn_err_code);
  if (local_sess)
  {
    mysqlx_session_close(local_sess);
    FAIL() << "Session should not be established";
  }
  else
    cout << "Expected error: " << conn_error << endl;

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                       OPT_SSL_MODE(SSL_MODE_REQUIRED),
                       OPT_AUTH(MYSQLX_AUTH_PLAIN),
                       PARAM_END));

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_AUTH,
                                                 &auth_method));
  EXPECT_EQ(MYSQLX_AUTH_PLAIN, auth_method);
  local_sess = mysqlx_get_session_from_options(opt, conn_error, &conn_err_code);
  if (!local_sess)
    FAIL() << "Failed to establish session";

  mysqlx_session_close(local_sess);
  local_sess = NULL;

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                       OPT_SSL_MODE(SSL_MODE_DISABLED),
                       OPT_AUTH(MYSQLX_AUTH_MYSQL41),
                       PARAM_END));

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_AUTH,
                                                 &auth_method));
  EXPECT_EQ(MYSQLX_AUTH_MYSQL41, auth_method);
  local_sess = mysqlx_get_session_from_options(opt, conn_error, &conn_err_code);
  if (!local_sess)
    FAIL() << "Failed to establish session";

  mysqlx_session_close(local_sess);
  local_sess = NULL;

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                       OPT_SSL_MODE(SSL_MODE_REQUIRED),
                       OPT_AUTH(MYSQLX_AUTH_MYSQL41),
                       PARAM_END));

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_AUTH,
                                                 &auth_method));
  EXPECT_EQ(MYSQLX_AUTH_MYSQL41, auth_method);
  local_sess = mysqlx_get_session_from_options(opt, conn_error, &conn_err_code);
  if (!local_sess)
    FAIL() << "Failed to establish session";

  mysqlx_session_close(local_sess);
  local_sess = NULL;

  mysqlx_free_options(opt);

  std::stringstream conn;
  conn << m_xplugin_usr;
  if (m_xplugin_pwd)
    conn << ":" << m_xplugin_pwd;
  conn << "@" << m_xplugin_host << ":" << m_xplugin_port;

  std::string connstr = conn.str().data();
  local_sess = mysqlx_get_session_from_url(connstr.append("?ssl-mode=disabled&auth=plain").data(),
                                           conn_error, &conn_err_code);
  if (local_sess)
  {
    mysqlx_session_close(local_sess);
    FAIL() << "Session should not be established";
  }
  else
    cout << "Expected error: " << conn_error << endl;

  connstr = conn.str().data();
  local_sess = mysqlx_get_session_from_url(connstr.append("?ssl-mode=disabled&auth=mysql41").data(),
                                           conn_error, &conn_err_code);
  if (!local_sess)
    FAIL() << "Session could not be established";

  mysqlx_session_close(local_sess);
  local_sess = NULL;

  connstr = conn.str().data();
  local_sess = mysqlx_get_session_from_url(connstr.append("?ssl-mode=required&auth=plain").data(),
                                           conn_error, &conn_err_code);
  if (!local_sess)
    FAIL() << "Session could not be established";

  mysqlx_session_close(local_sess);
  local_sess = NULL;

  connstr = conn.str().data();
  local_sess = mysqlx_get_session_from_url(connstr.append("?ssl-mode=required&auth=mysql41").data(),
                                           conn_error, &conn_err_code);
  if (!local_sess)
    FAIL() << "Session could not be established";

  mysqlx_session_close(local_sess);
  local_sess = NULL;
}


TEST_F(xapi, auth_method_external)
{
  SKIP_IF_NO_XPLUGIN

  char conn_error[MYSQLX_MAX_ERROR_LEN] = { 0 };
  int conn_err_code = 0;
  mysqlx_session_t *local_sess = NULL;
  mysqlx_session_options_t *opt = mysqlx_session_options_new();
  unsigned int auth_method = 0;

  authenticate();

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                       OPT_HOST(m_xplugin_host), OPT_PORT(m_port),
                       OPT_USER(m_xplugin_usr), OPT_PWD(m_xplugin_pwd),
                       PARAM_END));

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                       OPT_SSL_MODE(SSL_MODE_REQUIRED),
                       OPT_AUTH(MYSQLX_AUTH_EXTERNAL),
                       PARAM_END));

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_AUTH,
                                                 &auth_method));
  EXPECT_EQ(MYSQLX_AUTH_EXTERNAL, auth_method);

  local_sess = mysqlx_get_session_from_options(opt, conn_error, &conn_err_code);
  if (local_sess)
  {
    mysqlx_session_close(local_sess);
    FAIL() << "Session should not be established";
  }
  else
    cout << "Expected error: " << conn_error << endl;

  mysqlx_session_close(local_sess);
  local_sess = NULL;

  mysqlx_free_options(opt);

  std::stringstream conn;
  conn << m_xplugin_usr;
  if (m_xplugin_pwd)
    conn << ":" << m_xplugin_pwd;
  conn << "@" << m_xplugin_host << ":" << m_xplugin_port;

  std::string connstr = conn.str().data();
  local_sess = mysqlx_get_session_from_url(connstr.append("?ssl-mode=required&auth=external").data(),
                                           conn_error, &conn_err_code);
  if (local_sess)
  {
    mysqlx_session_close(local_sess);
    FAIL() << "Session should not be established";
  }
  else
    cout << "Expected error: " << conn_error << endl;
}


TEST_F(xapi, conn_options_test)
{
  SKIP_IF_NO_XPLUGIN

  USE_NATIVE_PWD;

  unsigned int port2 = 0;
  unsigned int ssl_enable = 0;
  char conn_error[MYSQLX_MAX_ERROR_LEN] = { 0 };
  int conn_err_code = 0;

  char buf[1024];
  char buf_check[2048];

  mysqlx_session_t *local_sess = NULL;
  mysqlx_session_options_t *opt = mysqlx_session_options_new();


  ASSERT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                      OPT_HOST(m_xplugin_host), OPT_PORT(m_port),
                      OPT_USER(m_xplugin_usr), OPT_PWD(""),
                      PARAM_END));

  ASSERT_EQ(RESULT_OK,
    mysqlx_session_option_set(opt, OPT_PWD(m_xplugin_pwd),PARAM_END)
  );

  EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_set(opt, (mysqlx_opt_type_t)127, port2, PARAM_END));
  cout << "Expected error: " << mysqlx_error_message(mysqlx_error(opt)) << std::endl;

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_HOST, buf));
  EXPECT_STREQ(m_xplugin_host, buf);
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_USER, buf));
  EXPECT_STREQ(m_xplugin_usr, buf);
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_PORT, &port2));
  EXPECT_EQ(true, m_port == port2);

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
    OPT_SSL_MODE(SSL_MODE_DISABLED), PARAM_END
  ));


DO_CONNECT:

  if (local_sess)
    mysqlx_session_close(local_sess);

  local_sess = mysqlx_get_session_from_options(opt, conn_error, &conn_err_code);

  if (!local_sess)
  {
    mysqlx_free_options(opt);
    FAIL() << "Could not connect to xplugin. " << m_xplugin_port << std::endl << conn_error <<
            " ERROR CODE: " << conn_err_code;
  }
  cout << "Connected to xplugin (" << (ssl_enable ? "SSL" : "no SSL") <<")..." << endl;

  std::string ssl = get_ssl_cipher(local_sess);

  if (ssl_enable)
  {
    EXPECT_FALSE(ssl.empty());
    cout << "SSL Cipher: " << ssl << endl;
  }
  else
    EXPECT_TRUE(ssl.empty());

  if (!ssl_enable)
  {
    ssl_enable = true;

    authenticate();

    std::string  ca = get_ca_file();

    EXPECT_FALSE(ca.empty());

    cout << "CA file: " << ca << endl;

    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
      OPT_SSL_MODE(SSL_MODE_VERIFY_CA),
      OPT_SSL_CA(ca.c_str()),
      PARAM_END
    ));

    EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_SSL_CA, buf_check));
    EXPECT_STREQ(ca.c_str(), buf_check);

    /*
      Check invalid ssl options combinations.
    */

    {
      mysqlx_session_options_t *opt1 = mysqlx_session_options_new();

      for (unsigned i = 0; i < 2; ++i)
      {

        EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt1,
          OPT_SSL_MODE(i > 0 ? SSL_MODE_REQUIRED : SSL_MODE_DISABLED),
          PARAM_END
          ));

        EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_set(opt1,
          OPT_SSL_CA(ca.c_str()), PARAM_END
          ));
        cout << "Expected error: "
          << mysqlx_error_message(mysqlx_error(opt1)) << std::endl;
      }

      mysqlx_free_options(opt1);
    }

    goto DO_CONNECT;
  }

  mysqlx_session_close(local_sess);
  mysqlx_free_options(opt);

  {
    /*
      Since we set SSL_CA to wrong file, the connection should fail.
    */

    mysqlx_session_options_t *opt1 = mysqlx_session_options_new();

    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt1,
      OPT_HOST(m_xplugin_host),
      OPT_PORT(m_port),
      OPT_USER(m_xplugin_usr),
      OPT_PWD(m_xplugin_pwd),
      OPT_SSL_MODE(SSL_MODE_VERIFY_CA),
      OPT_SSL_CA("wrong_ca.pem"),
      PARAM_END
    ));

    local_sess
      = mysqlx_get_session_from_options(opt1, conn_error, &conn_err_code);

    if (local_sess)
    {
      mysqlx_session_close(local_sess);
      mysqlx_free_options(opt1);
      FAIL() << "Should not connect to xplugin. ";
    }
    cout << "Expected error: " << conn_error << endl;
    mysqlx_free_options(opt1);
  }
}

TEST_F(xapi, conn_options_atomic)
{
  SKIP_IF_NO_XPLUGIN

  unsigned int port = 0, ssl_mode = SSL_MODE_DISABLED;

  char buf[1024];

  const char *test_host1 = "host1";
  const char *test_db1 =   "db1";
  const char *test_user1 = "user1";
  const char *test_pwd1 =  "pwd1";
  unsigned short test_port1 = 1;

  const char *test_host2 = "host2";
  const char *test_db2 = "db2";
  const char *test_user2 = "user2";
  const char *test_pwd2 = "pwd2";
  unsigned short test_port2 = 1;

  mysqlx_session_options_t *opt = mysqlx_session_options_new();

  // Setting options Call 1
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                       OPT_HOST(test_host1),
                       OPT_PORT(test_port1),
                       OPT_USER(test_user1),
                       OPT_PWD(test_pwd1),
                       OPT_DB(test_db1),
                       OPT_SSL_MODE(SSL_MODE_REQUIRED),
                       PARAM_END));

  // Setting options Call 2
  EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_set(opt,
                       OPT_HOST(test_host2),
                       OPT_PORT(test_port2),
                       OPT_USER(test_user2),
                       OPT_PWD(test_pwd2),
                       OPT_DB(test_db2),
                       OPT_SSL_MODE(SSL_MODE_DISABLED),
                       OPT_SSL_CA("ca.pem"),
                       PARAM_END));

  //  Call 2 failed, but opt settings are supposed to be from Call 1
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_HOST, buf));
  EXPECT_STREQ(test_host1, buf);
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_PORT, &port));
  EXPECT_EQ(true, test_port1 == port);
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_USER, buf));
  EXPECT_STREQ(test_user1, buf);
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_PWD, buf));
  EXPECT_STREQ(test_pwd1, buf);
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_DB, buf));
  EXPECT_STREQ(test_db1, buf);
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_SSL_MODE, &ssl_mode));
  EXPECT_EQ(true, SSL_MODE_REQUIRED == ssl_mode);
  // the SSL_CA option was not set in the end, so can't read it
  EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_get(opt, MYSQLX_OPT_SSL_CA, buf));

  mysqlx_free_options(opt);
}


TEST_F(xapi, default_db_test)
{
  SKIP_IF_NO_XPLUGIN

  mysqlx_stmt_t *stmt;
  mysqlx_result_t *res;
  mysqlx_row_t *row;
  mysqlx_schema_t *schema;
  mysqlx_table_t *table;

  // Use default user name and pwd, but set the database
  authenticate(NULL, NULL, "cc_api_test");

  exec_sql("CREATE TABLE default_tab(a INT, b VARCHAR(32))");

  EXPECT_TRUE((schema = mysqlx_get_schema(get_session(), "cc_api_test", 1)) != NULL);
  EXPECT_TRUE((table = mysqlx_get_table(schema, "default_tab", 1)) != NULL);

  RESULT_CHECK(stmt = mysqlx_table_insert_new(table));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_columns(stmt, "a", "b", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_SINT(200), PARAM_STRING("mysql"), PARAM_END));
  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

  RESULT_CHECK(stmt = mysqlx_table_select_new(table));
  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

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

  mysqlx_result_t *res;

  authenticate();

  SESS_CHECK(res = mysqlx_sql(get_session(), "set @any_var=1", MYSQLX_NULL_TERMINATED));
  EXPECT_EQ(0, mysqlx_column_get_count(res));
  EXPECT_EQ(NULL, mysqlx_column_get_catalog(res, 0));
  EXPECT_EQ(0, mysqlx_column_get_type(res, 0));
}


TEST_F(xapi, transaction_test)
{
  SKIP_IF_NO_XPLUGIN

  mysqlx_result_t *res;
  mysqlx_row_t *row;
  mysqlx_schema_t *schema;
  mysqlx_table_t *table;

  authenticate(NULL, NULL, "cc_api_test");
  exec_sql("CREATE TABLE transact_tab(a INT)");

  EXPECT_TRUE((schema = mysqlx_get_schema(get_session(), "cc_api_test", 1)) != NULL);
  EXPECT_TRUE((table = mysqlx_get_table(schema, "transact_tab", 1)) != NULL);

  EXPECT_EQ(RESULT_OK, mysqlx_transaction_begin(get_session()));
  SESS_CHECK(res = mysqlx_table_insert(table, "a", PARAM_SINT(200), PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_transaction_commit(get_session()));

  // Check how the row was inserted after committing the transaction
  SESS_CHECK(res = mysqlx_table_select(table, "a > 0"));

  while ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    int64_t a = 0;
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &a));
    EXPECT_EQ(200, a);
  }

  EXPECT_EQ(RESULT_OK, mysqlx_transaction_begin(get_session()));
  SESS_CHECK(res = mysqlx_table_delete(table, "a > 0"));
  EXPECT_EQ(RESULT_OK, mysqlx_transaction_rollback(get_session()));

  // Check how the row was not deleted after rolling back the transaction
  SESS_CHECK(res = mysqlx_table_select(table, "a > 0"));

  while ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    int64_t a = 0;
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &a));
    EXPECT_EQ(200, a);
  }

  // Savepoints

  std::vector<string> savepoints;

  EXPECT_EQ(RESULT_OK, mysqlx_transaction_begin(get_session()));
  SESS_CHECK(res = mysqlx_table_insert(table, "a", PARAM_SINT(500), PARAM_END));
  savepoints.emplace_back(mysqlx_savepoint_set(get_session(), NULL)); //savepoints[0]
  SESS_CHECK(res = mysqlx_table_insert(table, "a", PARAM_SINT(600), PARAM_END));
  savepoints.emplace_back(mysqlx_savepoint_set(get_session(), NULL)); //savepoints[1]
  SESS_CHECK(res = mysqlx_table_insert(table, "a", PARAM_SINT(700), PARAM_END));
  savepoints.emplace_back(mysqlx_savepoint_set(get_session(), NULL)); //savepoints[2]
  SESS_CHECK(res = mysqlx_table_insert(table, "a", PARAM_SINT(800), PARAM_END));
  savepoints.emplace_back(mysqlx_savepoint_set(get_session(), "MySave")); //savepoints[3]


  EXPECT_EQ(RESULT_OK, mysqlx_savepoint_release(get_session(), "MySave"));
  EXPECT_EQ(RESULT_ERROR, mysqlx_savepoint_release(get_session(),
                                                   savepoints.back().c_str()));
  savepoints.pop_back();
  // rollback to a=600
  EXPECT_EQ(RESULT_OK, mysqlx_rollback_to(get_session(),
                                          savepoints[1].c_str()));
  //savepoint of a=700 was removed because of the rollback to a=600
  EXPECT_EQ(RESULT_ERROR, mysqlx_rollback_to(get_session(),
                                             savepoints[2].c_str()));
  EXPECT_EQ(RESULT_ERROR, mysqlx_rollback_to(get_session(),""));

  EXPECT_EQ(RESULT_OK, mysqlx_rollback_to(get_session(),
                                             savepoints.front().c_str()));

  EXPECT_EQ(RESULT_OK, mysqlx_transaction_commit(get_session()));

  SESS_CHECK(res = mysqlx_table_select(table, "a > 0"));

  int i = 0;
  for(auto row = mysqlx_row_fetch_one(res);
      row;
      row = mysqlx_row_fetch_one(res),++i)
  {
    int64_t a = 0;
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &a));
    EXPECT_EQ(i == 0 ? 200 : 500, a);
  }

}


TEST_F(xapi, doc_id_test)
{
  SKIP_IF_NO_XPLUGIN

  mysqlx_result_t *res;
  mysqlx_stmt_t *stmt;
  mysqlx_schema_t *schema;
  mysqlx_collection_t *collection;

  const char *id;
  char id_buf[2][128];
  const char * json_string = NULL;
  int id_index = 0;
  int i = 0;
  size_t json_len = 0;

  authenticate(NULL, NULL, "cc_api_test");

  EXPECT_EQ(RESULT_OK, mysqlx_schema_drop(get_session(), "cc_api_test"));
  EXPECT_EQ(RESULT_OK, mysqlx_schema_create(get_session(), "cc_api_test"));

  EXPECT_TRUE((schema = mysqlx_get_schema(get_session(), "cc_api_test", 1)) != NULL);
  EXPECT_EQ(RESULT_OK, mysqlx_collection_create(schema, "doc_id_test"));

  EXPECT_TRUE((collection = mysqlx_get_collection(schema, "doc_id_test", 1)) != NULL);

  SESS_CHECK(res = mysqlx_collection_add(collection,
                                         "{\"a\" : \"12345\"}", "{\"a\" : \"abcde\"}",
                                         PARAM_END));

  while ((id = mysqlx_fetch_generated_id(res)) != NULL)
  {
    /*
      We need to copythe the returned string because it will become invalid
      when we start the FIND operation to read the actual JSON data
    */
    strcpy(id_buf[id_index], id);
    ++id_index;
  }

  RESULT_CHECK(stmt = mysqlx_collection_find_new(collection));
  EXPECT_EQ(RESULT_OK, mysqlx_set_find_order_by(stmt, "a", SORT_ORDER_ASC, PARAM_END));
  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

  while ((json_string = mysqlx_json_fetch_one(res, &json_len)) != NULL)
  {
    if (json_string)
      printf("\n[json: %s][doc_id: %s]", json_string, id_buf[i]);

    EXPECT_TRUE(strstr(json_string, id_buf[i]) != NULL);
    ++i;
  }

  /*
    Test that non-string document id triggers expected error.
  */

  CRUD_CHECK(mysqlx_collection_add(collection, "{\"_id\": 127}", NULL), collection)

  CRUD_CHECK(mysqlx_collection_add(collection, "{\"_id\": 12.7}", NULL), collection)

  res = mysqlx_collection_add(collection, "{\"_id\": \"127\"}", NULL);
  EXPECT_EQ(NULL, res);
  printf("\nExpected error: %s", mysqlx_error_message(collection));
}


TEST_F(xapi, myc_344_sql_error_test)
{
  SKIP_IF_NO_XPLUGIN

  mysqlx_result_t *res;
  mysqlx_schema_t *schema;
  mysqlx_table_t *table;
  mysqlx_row_t *row;
  const char *err_msg;
  int64_t v1 = LLONG_MIN;
  int64_t v2 = LLONG_MAX;
  int64_t v = 0;
  int num = 0;

  authenticate();

  mysqlx_schema_create(get_session(), "cc_api_test");
  schema = mysqlx_get_schema(get_session(), "cc_api_test", 1);
  exec_sql("DROP TABLE IF EXISTS cc_api_test.myc_344");
  exec_sql("CREATE TABLE cc_api_test.myc_344(b bigint)");

  table = mysqlx_get_table(schema, "myc_344", 1);

  res = mysqlx_table_insert(table, "b", PARAM_SINT(v1), PARAM_END);
  EXPECT_TRUE(res != NULL);
  res = mysqlx_table_insert(table, "b", PARAM_SINT(v2), PARAM_END);
  EXPECT_TRUE(res != NULL);

  res = mysqlx_sql(get_session(), "SELECT b+1000 from cc_api_test.myc_344", MYSQLX_NULL_TERMINATED);
  EXPECT_TRUE(mysqlx_error_message(res) == NULL);

  while ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    switch (num)
    {
      case 0:
        EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &v));
        EXPECT_EQ(v1 + 1000, v);
        break;
      default:
        FAIL(); // No more rows expected
    }
    ++num;
  }
  EXPECT_TRUE((err_msg = mysqlx_error_message(res)) != NULL);
  printf("\nExpected error: %s\n", err_msg);
}


#ifndef _WIN32
TEST_F(xapi, unix_socket)
{
  SKIP_IF_NO_UNIX_SOCKET;

  char conn_error[MYSQLX_MAX_ERROR_LEN] = { 0 };
  int conn_err_code = 0;

  std::stringstream uri;
  uri << "mysqlx://" << m_xplugin_usr;
  if (m_xplugin_pwd)
    uri << ":" << m_xplugin_pwd;

  uri << "@(" << m_xplugin_socket << ")";

  auto local_sess = mysqlx_get_session_from_url(uri.str().c_str(),
                                                conn_error,
                                                &conn_err_code);

  if (!local_sess)
  {
    FAIL() << "Cant connect to socket: " << m_xplugin_socket;
  }

  mysqlx_session_close(local_sess);

  auto opt = mysqlx_session_options_new();

  EXPECT_EQ(RESULT_OK,
            mysqlx_session_option_set(opt,
                                      OPT_SOCKET(m_xplugin_socket),
                                      OPT_USER(m_xplugin_usr),
                                      OPT_PWD(m_xplugin_pwd),
                                      PARAM_END
                                      )
            );

  local_sess = mysqlx_get_session_from_options(opt, conn_error, &conn_err_code);

  if (!local_sess)
  {
    FAIL() << "Error connecting to "<< m_xplugin_socket << " : " << conn_error;
  }

  //Mixing UNIX Socket with port should give error
  {
    auto opt2 = mysqlx_session_options_new();

    EXPECT_EQ(RESULT_ERROR,
              mysqlx_session_option_set(opt2,
                                        OPT_SOCKET(m_xplugin_socket),
                                        OPT_PORT(13000),
                                        PARAM_END
                                        )
              );

    mysqlx_free_options(opt2);
  }

  EXPECT_EQ(RESULT_OK,
            mysqlx_session_option_set(opt,
                                      OPT_HOST(m_xplugin_host),

                                      PARAM_END
                                      )
            );

  char buf[1024];

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_HOST, buf));
  EXPECT_STRCASEEQ(m_xplugin_host, buf);

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_SOCKET, buf));
  EXPECT_STRCASEEQ(m_xplugin_socket, buf);

  mysqlx_free_options(opt);


  uri << "?ssl-mode=REQUIRED";

  local_sess = mysqlx_get_session_from_url(uri.str().c_str(),
                                           conn_error,
                                           &conn_err_code);
  if (local_sess)
  {
    mysqlx_session_close(local_sess);
    FAIL() << "ssl-mode used on unix domain socket";
  }

  std::cout << "Expected connection error: " << conn_error << std::endl;

  opt = mysqlx_session_options_new();

  EXPECT_EQ(RESULT_OK,
            mysqlx_session_option_set(opt,
                                      OPT_SOCKET(m_xplugin_socket),
                                      OPT_USER(m_xplugin_usr),
                                      OPT_PWD(m_xplugin_pwd),
                                      OPT_SSL_MODE(SSL_MODE_REQUIRED),
                                      PARAM_END
                                      )
            );

  local_sess = mysqlx_get_session_from_options(opt, conn_error, &conn_err_code);

  {
    // Bug 26742948
    EXPECT_EQ(RESULT_OK,
      mysqlx_session_option_set(opt,
        OPT_SOCKET("../../../../../../../tmp/mysqlx_11.sock"),
        PARAM_END
      )
    );

    mysqlx_session_option_set(opt, MYSQLX_OPT_USER, "mysqld_user", PARAM_END);
  }

  mysqlx_free_options(opt);

  if (local_sess)
  {
    mysqlx_session_close(local_sess);
    FAIL() << "ssl-mode used on unix domain socket";
  }

  std::cout << "Expected connection error: " << conn_error << std::endl;


  std::cout << "Done" << std::endl;
}
#endif //_WIN32

TEST_F(xapi, sha256_memory)
{
  SKIP_IF_NO_XPLUGIN

  authenticate();

  exec_sql("DROP USER 'doomuser'@'%';");

  if(!exec_sql(
       "CREATE USER 'doomuser'@'%' IDENTIFIED WITH caching_sha2_password "
       "BY '!sha2user_pass';"
       ))
  {
    SKIP_TEST("No caching_sha2_password support");
    return;
  }

  char conn_error[MYSQLX_MAX_ERROR_LEN] = { 0 };
  int conn_err_code = 0;


  {

    auto sha_256_cleartext = mysqlx_session_options_new();

    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(sha_256_cleartext,
                         OPT_HOST(m_xplugin_host),
                         OPT_PORT(m_port),
                         OPT_AUTH(MYSQLX_AUTH_SHA256_MEMORY),
                         OPT_SSL_MODE(SSL_MODE_DISABLED),
                         OPT_USER("doomuser"),
                         OPT_PWD("!sha2user_pass"),
                         PARAM_END));

    auto default_cleartext = mysqlx_session_options_new();

    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(default_cleartext,
                         OPT_HOST(m_xplugin_host),
                         OPT_PORT(m_port),
                         OPT_SSL_MODE(SSL_MODE_DISABLED),
                         OPT_USER("doomuser"),
                         OPT_PWD("!sha2user_pass"),
                         PARAM_END));

    //First authentication... should fail!
    auto local_sess = mysqlx_get_session_from_options(sha_256_cleartext,
                                                      conn_error, &conn_err_code);
    if (local_sess)
      FAIL() << "First authentication... should fail!";

    //Auth using normal logic
    auto default_opt = mysqlx_session_options_new();
    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(default_opt,
                         OPT_HOST(m_xplugin_host),
                         OPT_PORT(m_port),
                         OPT_USER("doomuser"),
                         OPT_PWD("!sha2user_pass"),
                         PARAM_END));
    local_sess = mysqlx_get_session_from_options(default_opt,
                                                 conn_error, &conn_err_code);
    if (!local_sess)
      FAIL() << "Fail auth against caching_sha2_password";

    mysqlx_session_close(local_sess);

    //Second authentication... should work!
    local_sess = mysqlx_get_session_from_options(sha_256_cleartext,
                                                 conn_error, &conn_err_code);
    if (!local_sess)
      FAIL() << "Fail auth against cached user using cleartext connection";

    mysqlx_session_close(local_sess);

    //Connect using default (will use MYSQL41 and SHA256_MEMORY)
    local_sess = mysqlx_get_session_from_options(default_cleartext,
                                                 conn_error, &conn_err_code);
    if (!local_sess)
      FAIL() << "Fail auth against cached user using cleartext connection";

    mysqlx_session_close(local_sess);

    //FAIL auth (bad password)

    auto default_cleartext_fail = mysqlx_session_options_new();

    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(default_cleartext_fail,
                         OPT_HOST(m_xplugin_host),
                         OPT_PORT(m_port),
                         OPT_SSL_MODE(SSL_MODE_DISABLED),
                         OPT_USER("doomuser"),
                         OPT_PWD("!sha2user_pass_fail"),
                         PARAM_END));


    local_sess = mysqlx_get_session_from_options(default_cleartext_fail,
                                                      conn_error,
                                                      &conn_err_code);
    if (local_sess)
      FAIL() << "First authentication... should fail!";


    mysqlx_free_options(sha_256_cleartext);
    mysqlx_free_options(default_cleartext);
    mysqlx_free_options(default_cleartext_fail);

  }
}
