/*
* Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
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

#include <test.h>
#include <mysqlx.h>
#include <iostream>

using std::cout;
using std::endl;
using namespace mysqlx;

class Types : public mysqlx::test::Xplugin
{
};


TEST_F(Types, basic)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Preparing test.types..." << endl;

  sql("DROP TABLE IF EXISTS test.types");
  sql(
    "CREATE TABLE test.types("
    "  c0 INT,"
    "  c1 DECIMAL,"
    "  c2 FLOAT,"
    "  c3 DOUBLE,"
    "  c4 VARCHAR(32)"
    ")");

  Table types = getSchema("test").getTable("types");

  Row row(7, 3.14, 3.1415, 3.141592, "First row");
  types.insert()
    .values(row)
    .values(-7, -2.71, (float)-2.7182, -2.718281, "Second row")
    .execute();

  cout << "Table prepared, querying it..." << endl;

  SqlResult res = sql(L"SELECT * FROM test.types");

  cout << "Query sent, reading rows..." << endl;
  cout << "There are " << res.getColumnCount() << " columns in the result" << endl;

  while ((row = res.fetchOne()))
  {
    cout << "== next row ==" << endl;
    for (unsigned i = 0; i < res.getColumnCount(); ++i)
    {
      cout << "col#" << i << ": " << row[i] << endl;
    }
  }

  cout << "Done!" << endl;
}
