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

#include <string.h>
#include <mysql_classic.h>
#include "test.h"

TEST_F(mysql_capi, basic)
{
#ifdef C_API_FIXED
/*
  Disable this test for windows
*/

  MYSQL *mysql = get_mysql();
  authenticate();

  const char *query = "SELECT 100 as col_1, 'abc' as col_2, 5.234E-03 as col_3";
  MYSQL_RES *res;
  MYSQL_ROW row = NULL;
  MYSQL_FIELD *fld;

  if (mysql_real_query(mysql, query, strlen(query)))
    FAIL();

  if (!(res = mysql_store_result(mysql)))
    FAIL();

  EXPECT_EQ(mysql_num_fields(res), 3);

  while (fld = mysql_fetch_field(res))
  {
    cout << "Field: " << fld->name << " Type: ";
    switch (fld->type)
    {
      case MYSQL_TYPE_LONGLONG: cout << "MYSQL_TYPE_LONGLONG"; break;
      case MYSQL_TYPE_FLOAT: cout << "MYSQL_TYPE_FLOAT"; break;
      case MYSQL_TYPE_DOUBLE: cout << "MYSQL_TYPE_DOUBLE"; break;
      case MYSQL_TYPE_STRING: cout << "MYSQL_TYPE_STRING"; break;
      default: cout << "UNKNOWN"; break;
    }
    cout << endl;
  }

  row = mysql_fetch_row(res);
  EXPECT_STREQ(row[0], "100");
  EXPECT_STREQ(row[1], "abc");
  EXPECT_STREQ(row[2], "0.005234");
  row = mysql_fetch_row(res);
  EXPECT_EQ(row, NULL);

  mysql_free_result(res);
  cout << "DONE";
  mysql_close(mysql);
#endif
}

