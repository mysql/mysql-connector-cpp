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
#include <climits>
#include "test.h"

TEST_F(xapi, view_ddl_test)
{
  SKIP_IF_NO_XPLUGIN

  mysqlx_result_t *res;
  mysqlx_row_t *row;
  mysqlx_schema_t *schema;
  mysqlx_table_t *table, *view, *view2;
  mysqlx_stmt_t *stmt, *vstmt;
  bool view2_read = false;

  uint32_t col_num = 0, row_num = 0, i = 0;

  authenticate();
  mysqlx_schema_drop(get_session(), "view_ddl");
  mysqlx_schema_create(get_session(), "view_ddl");

  schema = mysqlx_get_schema(get_session(), "view_ddl", 1);
  exec_sql("CREATE TABLE view_ddl.tab1(a INT, b VARCHAR(32))");
  EXPECT_TRUE((table = mysqlx_get_table(schema, "tab1", 1)) != NULL);

  RESULT_CHECK(stmt = mysqlx_table_insert_new(table));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_columns(stmt, "a", "b", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_SINT(100), PARAM_STRING("mysql"), PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_SINT(200), PARAM_STRING("test"), PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_SINT(300), PARAM_STRING("view"), PARAM_END));
  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

  RESULT_CHECK(stmt = mysqlx_table_select_new(table));
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_items(stmt, "a", "a*5 AS a5", "800 as a800", "CONCAT(b, ' tab') as conview", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_where(stmt, "a > 100"));
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_order_by(stmt, PARAM_SORT_DESC("a"), PARAM_END));

  // We do not have to execute the SELECT statement
  RESULT_CHECK(vstmt = mysqlx_view_create_new(schema, "view1", stmt));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_algorithm(vstmt, VIEW_ALGORITHM_MERGE));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_security(vstmt, VIEW_SECURITY_INVOKER));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_check_option(vstmt, VIEW_CHECK_OPTION_CASCADED));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_definer(vstmt, NULL));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_definer(vstmt, "root"));

  // Change parent SELECT STMT, it is not supposed to affect the VIEW STMT in any way
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_items(stmt, "b", PARAM_END));

  EXPECT_EQ(RESULT_OK, mysqlx_set_view_columns(vstmt, "viewcol1", "viewcol2", "viewcol3", "viewcol4", PARAM_END));
  CRUD_CHECK(res = mysqlx_execute(vstmt), vstmt);

  EXPECT_TRUE((view = mysqlx_get_table(schema, "view1", 0)) != NULL);
  RESULT_CHECK(res = mysqlx_table_select(view, NULL));

  col_num = mysqlx_column_get_count(res);
  EXPECT_EQ(col_num, 4);

  for (i = 0; i < col_num; ++i)
  {
    char name_val[32] = { 0 };
    const char *cname = mysqlx_column_get_name(res, i);
    sprintf(name_val, "viewcol%d", i+1);
    EXPECT_STREQ(name_val, cname);
  }

  while ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    int64_t a, a5, a800;
    char buf[128] = { 0 };

    size_t buflen = sizeof(buf);
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &a));
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 1, &a5));
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 2, &a800));
    EXPECT_EQ(RESULT_OK, mysqlx_get_bytes(row, 3, 0, buf, &buflen));

    cout << "Row #" << row_num << ": [" << a << " " << a5 << " " << a800
      << " '" << buf << "']" << endl;

    switch (row_num)
    {
    case 0:
      EXPECT_EQ(300, a);
      EXPECT_EQ(1500, a5);
      EXPECT_EQ(800, a800);
      EXPECT_STREQ(buf, "view tab");
      break;
    case 1:
      EXPECT_EQ(200, a);
      EXPECT_EQ(1000, a5);
      EXPECT_EQ(800, a800);
      EXPECT_STREQ(buf, "test tab");
      break;
    default:
      FAIL() << "Wrong number of rows is fetched";
    }

    ++row_num;
  }

  RESULT_CHECK(res = mysqlx_sql(get_session(), "SHOW CREATE VIEW `view_ddl`.`view1`",
                                MYSQLX_NULL_TERMINATED));
  EXPECT_TRUE((row = mysqlx_row_fetch_one(res)) != NULL);
  {
    char vbuf[4096] = { 0 };
    size_t vbuflen = sizeof(vbuf);
    EXPECT_EQ(RESULT_OK, mysqlx_get_bytes(row, 1, 0, vbuf, &vbuflen));
    EXPECT_TRUE(strstr(vbuf, "ALGORITHM=MERGE") != NULL );
    EXPECT_TRUE(strstr(vbuf, "SQL SECURITY INVOKER") != NULL);
    EXPECT_TRUE(strstr(vbuf, "WITH CASCADED CHECK OPTION") != NULL);
    EXPECT_TRUE(strstr(vbuf, "`root`@") != NULL);
  }

  /* Check how CREATE VIEW works on existing view, error expected */
  RESULT_CHECK(vstmt = mysqlx_view_create_new(schema, "view1", stmt));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_columns(vstmt, "rviewcol1", "rviewcol2", "rviewcol3", "rviewcol4", PARAM_END));
  EXPECT_TRUE(mysqlx_execute(vstmt) == NULL);
  printf("Expected error: %s\n", mysqlx_error_message(vstmt));

  RESULT_CHECK(stmt = mysqlx_table_select_new(table));
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_items(stmt, "a", "a*10 AS a10", "8000 as a8000", "CONCAT(b, ' mod') as conview", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_where(stmt, "a > 200"));
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_order_by(stmt, PARAM_SORT_DESC("a"), PARAM_END));

  /* Check how REPLACE VIEW works on existing view */
  RESULT_CHECK(vstmt = mysqlx_view_replace_new(schema, "view1", stmt));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_algorithm(vstmt, VIEW_ALGORITHM_TEMPTABLE));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_security(vstmt, VIEW_SECURITY_DEFINER));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_columns(vstmt, "rviewcol1", "rviewcol2", "rviewcol3", "rviewcol4", PARAM_END));
  CRUD_CHECK(res = mysqlx_execute(vstmt), vstmt);

  /* Check how REPLACE VIEW works on non-existing view */
  RESULT_CHECK(vstmt = mysqlx_view_replace_new(schema, "view2", stmt));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_algorithm(vstmt, VIEW_ALGORITHM_TEMPTABLE));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_security(vstmt, VIEW_SECURITY_DEFINER));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_columns(vstmt, "rviewcol1", "rviewcol2", "rviewcol3", "rviewcol4", PARAM_END));
  CRUD_CHECK(res = mysqlx_execute(vstmt), vstmt);

  RESULT_CHECK(res = mysqlx_table_select(view, NULL));

REPEAT_VIEW_CHECK:
  row_num = 0;
  col_num = mysqlx_column_get_count(res);
  EXPECT_EQ(col_num, 4);

  for (i = 0; i < col_num; ++i)
  {
    char name_val[32] = { 0 };
    const char *cname = mysqlx_column_get_name(res, i);
    sprintf(name_val, "rviewcol%d", i + 1);
    EXPECT_STREQ(name_val, cname);
  }

  while ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    int64_t a, a10, a8000;
    char buf[128] = { 0 };

    size_t buflen = sizeof(buf);
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &a));
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 1, &a10));
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 2, &a8000));
    EXPECT_EQ(RESULT_OK, mysqlx_get_bytes(row, 3, 0, buf, &buflen));

    cout << endl << "Row #" << row_num << ": [" << a << " " << a10 << " " << a8000
      << " '" << buf << "']";

    switch (row_num)
    {
      case 0:
        EXPECT_EQ(300, a);
        EXPECT_EQ(3000, a10);
        EXPECT_EQ(8000, a8000);
        EXPECT_STREQ(buf, "view mod");
      break;
      default:
        FAIL() << "Wrong number of rows is fetched";
    }
    ++row_num;
  }

  if (!view2_read)
  {
    view2_read = true;
    EXPECT_TRUE((view2 = mysqlx_get_table(schema, "view2", 0)) != NULL);
    RESULT_CHECK(res = mysqlx_table_select(view2, NULL));
    goto REPEAT_VIEW_CHECK; // Same results for a new view
  }

  /* Check how MODIFY VIEW works on non-existing view, error expected */
  RESULT_CHECK(vstmt = mysqlx_view_modify_new(schema, "view3", stmt));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_algorithm(vstmt, VIEW_ALGORITHM_TEMPTABLE));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_security(vstmt, VIEW_SECURITY_DEFINER));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_columns(vstmt, "rviewcol1", "rviewcol2", "rviewcol3", "rviewcol4", PARAM_END));
  EXPECT_TRUE(mysqlx_execute(vstmt) == NULL);
  printf("\nExpected error: %s\n", mysqlx_error_message(vstmt));

  /* Check how MODIFY VIEW works on existing view */
  RESULT_CHECK(vstmt = mysqlx_view_modify_new(schema, "view2", stmt));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_algorithm(vstmt, VIEW_ALGORITHM_TEMPTABLE));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_security(vstmt, VIEW_SECURITY_DEFINER));
  EXPECT_EQ(RESULT_OK, mysqlx_set_view_columns(vstmt, "modcol1", "modcol2", "modcol3", "modcol4", PARAM_END));
  CRUD_CHECK(res = mysqlx_execute(vstmt), vstmt);

  RESULT_CHECK(res = mysqlx_table_select(view2, NULL));
  col_num = mysqlx_column_get_count(res);
  EXPECT_EQ(col_num, 4);

  for (i = 0; i < col_num; ++i)
  {
    char name_val[32] = { 0 };
    const char *cname = mysqlx_column_get_name(res, i);
    sprintf(name_val, "modcol%d", i + 1);
    EXPECT_STREQ(name_val, cname);
  }

}

TEST_F(xapi, view_ddl_one_call_test)
{
  SKIP_IF_NO_XPLUGIN

  mysqlx_result_t *res;
  mysqlx_row_t *row;
  mysqlx_schema_t *schema;
  mysqlx_table_t *table, *view, *view2;
  mysqlx_stmt_t *stmt;
  bool view2_read = false;

  uint32_t col_num = 0, row_num = 0, i = 0;

  authenticate();
  mysqlx_schema_drop(get_session(), "view_ddl");
  mysqlx_schema_create(get_session(), "view_ddl");

  schema = mysqlx_get_schema(get_session(), "view_ddl", 1);
  exec_sql("CREATE TABLE view_ddl.tab1(a INT, b VARCHAR(32))");
  EXPECT_TRUE((table = mysqlx_get_table(schema, "tab1", 1)) != NULL);

  RESULT_CHECK(stmt = mysqlx_table_insert_new(table));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_columns(stmt, "a", "b", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_SINT(100), PARAM_STRING("mysql"), PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_SINT(200), PARAM_STRING("test"), PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_insert_row(stmt, PARAM_SINT(300), PARAM_STRING("view"), PARAM_END));
  CRUD_CHECK(res = mysqlx_execute(stmt), stmt);

  RESULT_CHECK(stmt = mysqlx_table_select_new(table));
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_items(stmt, "a", "a*5 AS a5", "800 as a800", "CONCAT(b, ' tab') as conview", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_where(stmt, "a > 100"));
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_order_by(stmt, PARAM_SORT_DESC("a"), PARAM_END));

  // We do not have to execute the SELECT statement
  CRUD_CHECK(res = mysqlx_view_create(schema, "view1", stmt,
                                      VIEW_ALGORITHM(VIEW_ALGORITHM_MERGE),
                                      VIEW_SECURITY(VIEW_SECURITY_INVOKER),
                                      VIEW_CHECK_OPTION(VIEW_CHECK_OPTION_CASCADED),
                                      VIEW_DEFINER("root"),
                                      VIEW_COLUMNS("viewcol1", "viewcol2", "viewcol3", "viewcol4"),
                                      PARAM_END), schema);

  EXPECT_TRUE((view = mysqlx_get_table(schema, "view1", 0)) != NULL);
  RESULT_CHECK(res = mysqlx_table_select(view, NULL));

  col_num = mysqlx_column_get_count(res);
  EXPECT_EQ(col_num, 4);

  for (i = 0; i < col_num; ++i)
  {
    char name_val[32] = { 0 };
    const char *cname = mysqlx_column_get_name(res, i);
    sprintf(name_val, "viewcol%d", i + 1);
    EXPECT_STREQ(name_val, cname);
  }

  while ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    int64_t a, a5, a800;
    char buf[128] = { 0 };

    size_t buflen = sizeof(buf);
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &a));
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 1, &a5));
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 2, &a800));
    EXPECT_EQ(RESULT_OK, mysqlx_get_bytes(row, 3, 0, buf, &buflen));

    cout << "Row #" << row_num << ": [" << a << " " << a5 << " " << a800
      << " '" << buf << "']" << endl;

    switch (row_num)
    {
    case 0:
      EXPECT_EQ(300, a);
      EXPECT_EQ(1500, a5);
      EXPECT_EQ(800, a800);
      EXPECT_STREQ(buf, "view tab");
      break;
    case 1:
      EXPECT_EQ(200, a);
      EXPECT_EQ(1000, a5);
      EXPECT_EQ(800, a800);
      EXPECT_STREQ(buf, "test tab");
      break;
    default:
      FAIL() << "Wrong number of rows is fetched";
    }

    ++row_num;
  }

  RESULT_CHECK(res = mysqlx_sql(get_session(), "SHOW CREATE VIEW `view_ddl`.`view1`",
    MYSQLX_NULL_TERMINATED));
  EXPECT_TRUE((row = mysqlx_row_fetch_one(res)) != NULL);
  {
    char vbuf[4096] = { 0 };
    size_t vbuflen = sizeof(vbuf);
    EXPECT_EQ(RESULT_OK, mysqlx_get_bytes(row, 1, 0, vbuf, &vbuflen));
    EXPECT_TRUE(strstr(vbuf, "ALGORITHM=MERGE") != NULL);
    EXPECT_TRUE(strstr(vbuf, "SQL SECURITY INVOKER") != NULL);
    EXPECT_TRUE(strstr(vbuf, "WITH CASCADED CHECK OPTION") != NULL);
    EXPECT_TRUE(strstr(vbuf, "`root`@") != NULL);
  }

  /* Check how CREATE VIEW works on existing view, error expected */
  EXPECT_TRUE(mysqlx_view_create(schema, "view1", stmt,
                                 VIEW_COLUMNS("viewcol1", "viewcol2", "viewcol3", "viewcol4"),
                                 PARAM_END) == NULL);
  printf("Expected error: %s\n", mysqlx_error_message(schema));

  RESULT_CHECK(stmt = mysqlx_table_select_new(table));
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_items(stmt, "a", "a*10 AS a10", "8000 as a8000", "CONCAT(b, ' mod') as conview", PARAM_END));
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_where(stmt, "a > 200"));
  EXPECT_EQ(RESULT_OK, mysqlx_set_select_order_by(stmt, PARAM_SORT_DESC("a"), PARAM_END));

  /* Check how REPLACE VIEW works on existing view */
  CRUD_CHECK(res = mysqlx_view_replace(schema, "view1", stmt,
                                       VIEW_ALGORITHM(VIEW_ALGORITHM_TEMPTABLE),
                                       VIEW_SECURITY(VIEW_SECURITY_DEFINER),
                                       VIEW_COLUMNS("rviewcol1", "rviewcol2", "rviewcol3", "rviewcol4"),
                                       PARAM_END), schema);

  /* Check how REPLACE VIEW works on non-existing view */
  CRUD_CHECK(res = mysqlx_view_replace(schema, "view2", stmt,
                                       VIEW_ALGORITHM(VIEW_ALGORITHM_TEMPTABLE),
                                       VIEW_SECURITY(VIEW_SECURITY_DEFINER),
                                       VIEW_COLUMNS("rviewcol1", "rviewcol2", "rviewcol3", "rviewcol4"),
                                       PARAM_END), schema);

  RESULT_CHECK(res = mysqlx_table_select(view, NULL));

REPEAT_VIEW_CHECK:
  row_num = 0;
  col_num = mysqlx_column_get_count(res);
  EXPECT_EQ(col_num, 4);

  for (i = 0; i < col_num; ++i)
  {
    char name_val[32] = { 0 };
    const char *cname = mysqlx_column_get_name(res, i);
    sprintf(name_val, "rviewcol%d", i + 1);
    EXPECT_STREQ(name_val, cname);
  }

  while ((row = mysqlx_row_fetch_one(res)) != NULL)
  {
    int64_t a, a10, a8000;
    char buf[128] = { 0 };

    size_t buflen = sizeof(buf);
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 0, &a));
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 1, &a10));
    EXPECT_EQ(RESULT_OK, mysqlx_get_sint(row, 2, &a8000));
    EXPECT_EQ(RESULT_OK, mysqlx_get_bytes(row, 3, 0, buf, &buflen));

    cout << endl << "Row #" << row_num << ": [" << a << " " << a10 << " " << a8000
      << " '" << buf << "']";

    switch (row_num)
    {
    case 0:
      EXPECT_EQ(300, a);
      EXPECT_EQ(3000, a10);
      EXPECT_EQ(8000, a8000);
      EXPECT_STREQ(buf, "view mod");
      break;
    default:
      FAIL() << "Wrong number of rows is fetched";
    }
    ++row_num;
  }

  if (!view2_read)
  {
    view2_read = true;
    EXPECT_TRUE((view2 = mysqlx_get_table(schema, "view2", 0)) != NULL);
    RESULT_CHECK(res = mysqlx_table_select(view2, NULL));
    goto REPEAT_VIEW_CHECK; // Same results for a new view
  }

  /* Check how MODIFY VIEW works on non-existing view, error expected */
  EXPECT_TRUE((res = mysqlx_view_modify(schema, "view3", stmt,
                                      VIEW_ALGORITHM(VIEW_ALGORITHM_TEMPTABLE),
                                      VIEW_SECURITY(VIEW_SECURITY_DEFINER),
                                      VIEW_COLUMNS("rviewcol1", "rviewcol2", "rviewcol3", "rviewcol4"),
                                      PARAM_END)) == NULL);
  printf("\nExpected error: %s\n", mysqlx_error_message(schema));

  /* Check how MODIFY VIEW works on existing view */
  CRUD_CHECK(res = mysqlx_view_modify(schema, "view2", stmt,
                                      VIEW_ALGORITHM(VIEW_ALGORITHM_TEMPTABLE),
                                      VIEW_SECURITY(VIEW_SECURITY_DEFINER),
                                      VIEW_COLUMNS("modcol1", "modcol2", "modcol3", "modcol4"),
                                      PARAM_END), schema);

  RESULT_CHECK(res = mysqlx_table_select(view2, NULL));
  col_num = mysqlx_column_get_count(res);
  EXPECT_EQ(col_num, 4);

  for (i = 0; i < col_num; ++i)
  {
    char name_val[32] = { 0 };
    const char *cname = mysqlx_column_get_name(res, i);
    sprintf(name_val, "modcol%d", i + 1);
    EXPECT_STREQ(name_val, cname);
  }

}


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
  printf("\n Expected error: %s", mysqlx_error_message(stmt));

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

  mysqlx_session_close(get_session());

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

  conn_str_ca = conn_str_basic + "/?Ssl-cA=" + conn_str_ca;

  /* If ssl-ca set ssl-mode can not be DISABLED or REQUIRED*/

  for (unsigned i = 0; i < 2; ++i)
  {
    std::string conn_str = conn_str_ca;
    conn_str += (i > 0 ? "&ssl-mode=REQUIRED" : "&ssl-mode=DISABLED");

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
    std::string conn_str = conn_str_ca + "&ssl-mode=VERIFY_CA";

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
    std::string conn_str = conn_str_basic + "?ssl-ca=wrong_ca.pem";

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
  mysqlx_session_options_t *opt = mysqlx_session_options_new();
  mysqlx_session_options_t *opt2 = mysqlx_session_options_new();
  mysqlx_stmt_t *stmt;
  mysqlx_result_t *res;
  mysqlx_row_t *row;

  authenticate();
  mysqlx_schema_create(get_session(), db_name);

  /* Checking when errors should be returned */

  // No priority, should be ok
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt2,
                       OPT_HOST(m_xplugin_host),
                       OPT_PORT(m_port + 2),
                       PARAM_END));

  EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_get(opt2, MYSQLX_OPT_PRIORITY, &prio));
  cout << "Expected error: " << mysqlx_error_message(opt2) << endl;

  // Error expected: trying to add a priority to unprioritized list
  EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_set(opt2,
                          OPT_PRIORITY(max_prio - 1),
                          OPT_HOST(m_xplugin_host),
                          OPT_PORT(m_port + 2),
                          PARAM_END));
  cout << "Expected error: " << mysqlx_error_message(opt2) << endl;

  /* Starting to build the prioritized list */

  // The one which connects
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                       OPT_PRIORITY(max_prio),
                       OPT_HOST(m_xplugin_host),
                       OPT_PORT(m_port),
                       PARAM_END));

  // Wrong port, should not connect
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                       OPT_PRIORITY(max_prio - 1),
                       OPT_HOST(m_xplugin_host),
                       OPT_PORT(m_port + 1),
                       PARAM_END));

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt, MYSQLX_OPT_PRIORITY, &prio));
  EXPECT_EQ(max_prio - 1, prio);

  // Wrong port, should not connect
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                       OPT_PRIORITY(max_prio - 2),
                       OPT_HOST(m_xplugin_host),
                       OPT_PORT(m_port + 2),
                       PARAM_END));

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

  // All is correct, should connect
  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                       OPT_PRIORITY(max_prio - 3),
                       OPT_HOST(m_xplugin_host),
                       OPT_PORT(m_port),
                       OPT_USER(m_xplugin_usr),
                       OPT_PWD(m_xplugin_pwd),
                       OPT_DB(db_name),
                       PARAM_END));

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
    sprintf(conn_str, "%s:%s@[(address=%s:%d, priority=100),"\
                      "(address=%s:%d, priority=90)," \
                      "(address=%s:%d, priority=80)]/%s",
                      m_xplugin_usr, m_xplugin_pwd,
                      m_xplugin_host, m_port + 1,
                      m_xplugin_host, m_port + 2,
                      m_xplugin_host, m_port, db_name );

    sprintf(conn_str2, "%s:%s@[(address=%s:%d, priority=100),"\
                       "address=%s:%d," \
                       "(address=%s:%d, priority=80)]/%s",
                       m_xplugin_usr, m_xplugin_pwd,
                       m_xplugin_host, m_port + 1,
                       m_xplugin_host, m_port + 2,
                       m_xplugin_host, m_port, db_name);
  }
  else
  {
    sprintf(conn_str, "%s@[(address=%s:%d, priority=100),"\
                      "(address=%s:%d, priority=90)," \
                      "(address=%s:%d, priority=80)]/%s",
                      m_xplugin_usr,
                      m_xplugin_host, m_port + 1,
                      m_xplugin_host, m_port + 2,
                      m_xplugin_host, m_port, db_name );

    sprintf(conn_str2, "%s@[(address=%s:%d, priority=100),"\
                       "address=%s:%d," \
                       "(address=%s:%d, priority=80)]/%s",
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
}




TEST_F(xapi, conn_options_test)
{
  SKIP_IF_NO_XPLUGIN

  unsigned int port2 = 0;
  unsigned int ssl_enable = 0;
  char conn_error[MYSQLX_MAX_ERROR_LEN] = { 0 };
  int conn_err_code = 0;

  char buf[1024];
  char buf_check[2048];

  mysqlx_session_t *local_sess = NULL;
  mysqlx_session_options_t *opt = mysqlx_session_options_new();

  EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt,
                      OPT_HOST(m_xplugin_host), OPT_PORT(m_port),
                      OPT_USER(m_xplugin_usr), OPT_PWD(m_xplugin_pwd),
                      PARAM_END));

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

    {
      mysqlx_session_options_t *opt2 = mysqlx_session_options_new();

      EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt2,
        OPT_SSL_CA(ca.c_str()), PARAM_END
      ));

      /*
        If ssl-ca is set without setting ssl-mode, the latter defaults
        to VERIFY_CA.
      */

      unsigned int cur_ssl_mode = SSL_MODE_DISABLED;
      EXPECT_EQ(RESULT_OK, mysqlx_session_option_get(opt2,
        MYSQLX_OPT_SSL_MODE, &cur_ssl_mode
      ));
      EXPECT_EQ(SSL_MODE_VERIFY_CA, cur_ssl_mode);

      for (unsigned i = 0; i < 2; ++i)
      {
        EXPECT_EQ(RESULT_ERROR, mysqlx_session_option_set(opt2,
          OPT_SSL_MODE(i > 0 ? SSL_MODE_DISABLED : SSL_MODE_REQUIRED),
          PARAM_END
        ));
        cout << "Expected error: "
          << mysqlx_error_message(mysqlx_error(opt2)) << std::endl;
      }

      mysqlx_free_options(opt2);
    }

    goto DO_CONNECT;
  }

  mysqlx_session_close(local_sess);
  mysqlx_free_options(opt);

  {
    /*
      Check that setting SSL_CA also sets SSL_MODE to VERIFY_CA. Since we set
      SSL_CA to wrong file, the connection should fail.
    */

    mysqlx_session_options_t *opt1 = mysqlx_session_options_new();

    EXPECT_EQ(RESULT_OK, mysqlx_session_option_set(opt1,
      OPT_HOST(m_xplugin_host),
      OPT_USER(m_xplugin_usr),
      OPT_PWD(m_xplugin_pwd),
      OPT_PORT(m_port),
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

  while ((id = mysqlx_fetch_doc_id(res)) != NULL)
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

  res = mysqlx_collection_add(collection, "{\"_id\": 127}", NULL);
  EXPECT_EQ(NULL, res);
  printf("\nExpected error: %s", mysqlx_error_message(collection));

  res = mysqlx_collection_add(collection, "{\"_id\": 12.7}", NULL);
  EXPECT_EQ(NULL, res);
  printf("\nExpected error: %s", mysqlx_error_message(collection));

  CRUD_CHECK(mysqlx_collection_add(collection, "{\"_id\": \"127\"}", NULL), collection);
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
  mysqlx_table_drop(schema, "myc_344");
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
