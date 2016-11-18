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

#include <test.h>
#include <iostream>
#include <list>

using std::cout;
using std::endl;
using namespace mysqlx;


class Ddl : public mysqlx::test::Xplugin
{
};

TEST_F(Ddl, create_drop)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "Preparing test.ddl..." << endl;

  // Cleanup

  const string schema_name_1 = "schema_to_drop_1";
  const string schema_name_2 = "schema_to_drop_2";

  try {
    get_sess().dropSchema(schema_name_1);
  } catch (...) {};
  try {
    get_sess().dropSchema(schema_name_2);
  } catch (...) {};


  // Create 2 schemas

  get_sess().createSchema(schema_name_1);
  get_sess().createSchema(schema_name_2);


  // Catch error because schema is already created

  EXPECT_THROW(get_sess().createSchema(schema_name_1),mysqlx::Error);

  // Reuse created schema

  Schema schema = get_sess().createSchema(schema_name_1, true);

  //Tables Test

  {
    //TODO substitute with createTable when available

    sql(L"CREATE TABLE schema_to_drop_1.tb1 (`name` varchar(20), `age` int)");
    sql(L"CREATE TABLE schema_to_drop_1.tb2 (`name` varchar(20), `age` int)");
    sql(L"CREATE VIEW schema_to_drop_1.view1 as select * from schema_to_drop_1.tb1");


    std::list<Table> tables_list = schema.getTables();

    EXPECT_EQ(3, tables_list.size());

    for (auto tb : tables_list)
    {
      if (tb.getName().find(L"view") != std::string::npos)
      {
        EXPECT_TRUE(tb.isView());

        //check using getTable() passing check_existence = true
        EXPECT_TRUE(schema.getTable(tb.getName(), true).isView());

        //check using getTable() on isView()
        EXPECT_TRUE(schema.getTable(tb.getName()).isView());

      }
    }

    //Drop Tables/Views

    std::list<string> names_list = schema.getTableNames();

    for (auto name : names_list)
    {
      //View is not dropped, but still no error thrown
      get_sess().dropTable(schema.getName(), name);
    }

    EXPECT_THROW(schema.getTable("tb1", true), mysqlx::Error);
    EXPECT_THROW(schema.getTable("tb2", true), mysqlx::Error);
  }

  //Collection tests

  {

    const string collection_name_1 = "collection_1";
    const string collection_name_2 = "collection_2";
    // Create Collections
    schema.createCollection(collection_name_1);

    schema.createCollection(collection_name_2);

    // Get Collections

    std::list<Collection> list_coll = schema.getCollections();

    EXPECT_EQ(2, list_coll.size());

    for (Collection col : list_coll)
    {
      col.add("{\"name\": \"New Guy!\"}").execute();
    }

    // Drop Collections

    std::list<string> list_coll_name = schema.getCollectionNames();
    for (auto name : list_coll_name)
    {
      get_sess().dropCollection(schema.getName(), name);
    }

    //Doesn't throw even if don't exist
    for (auto name : list_coll_name)
    {
      get_sess().dropCollection(schema.getName(), name);
    }

    //Test Drop Collection
    EXPECT_THROW(schema.getCollection(collection_name_1, true), mysqlx::Error);
    EXPECT_THROW(schema.getCollection(collection_name_2, true), mysqlx::Error);
  }



  // Get Schemas

  std::list<Schema> schemas = get_sess().getSchemas();

  // Drop Schemas

  for (auto schema_ : schemas)
  {
    if (schema_.getName() == schema_name_1 ||
        schema_.getName() == schema_name_2)
      get_sess().dropSchema(schema_.getName());
  }

  // Drop Schemas doesn't throw if it doesnt exist
  for (auto schema_ : schemas)
  {
    if (schema_.getName() == schema_name_1 ||
        schema_.getName() == schema_name_2)
      EXPECT_NO_THROW(get_sess().dropSchema(schema_.getName()));
  }

  EXPECT_THROW(get_sess().getSchema(schema_name_1, true), mysqlx::Error);
  EXPECT_THROW(get_sess().getSchema(schema_name_2, true), mysqlx::Error);


  cout << "Done!" << endl;
}
