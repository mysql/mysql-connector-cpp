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
#include <boost/format.hpp>

using std::cout;
using std::endl;
using namespace mysqlx;

class Sess : public mysqlx::test::Xplugin
{
};



TEST_F(Sess, databaseObj)
{
  SKIP_IF_NO_XPLUGIN;

  cout << "DatabaseObject implementation..." << endl;

  const string schema_name = "schemaObj";
  const string coll_name = "collObj";
  const string tbl_name = "tblObj";

  try {
    get_sess().dropSchema(schema_name);
  } catch (...) {}


  Schema schema = get_sess().createSchema(schema_name);

  //Test Schema Obj

  EXPECT_TRUE(schema.existsInDatabase());
  EXPECT_EQ(schema_name ,schema.getName());
  EXPECT_FALSE(schema.getSession().getSchema("NOT_FOUND").existsInDatabase());

  //Test Collection Obj

  Collection coll = schema.createCollection(coll_name);

  EXPECT_TRUE(coll.existsInDatabase());
  EXPECT_EQ(coll_name, coll.getName());
  EXPECT_FALSE(coll.getSession().getSchema(schema_name)
               .getCollection("NOT_FOUND").existsInDatabase());


  //Test Table Obj

  get_sess().sql((boost::format("CREATE TABLE `%s`.`%s` (name VARCHAR(20) ,age INT);")
                  %schema_name %tbl_name).str()).execute();

  Table tbl = schema.getTable(tbl_name);

  EXPECT_TRUE(tbl.existsInDatabase());
  EXPECT_EQ(tbl_name, tbl.getName());
  EXPECT_FALSE(tbl.getSession().getSchema(schema_name)
               .getTable("NOT_FOUND").existsInDatabase());


  cout << "Done!" << endl;
}


TEST_F(Sess, url)
{
  SKIP_IF_NO_XPLUGIN;

  using std::ostringstream;
  using std::ends;

  ostringstream buf;

  std::string user;

  if (m_user)
  {
    user = m_user;
    if (m_password)
      user = user + ":" + m_password;
    user = user + "@";
  }

  std::string authority = "localhost";
  if (m_port)
    authority = authority + ":" + std::to_string(m_port);

  std::string url = user + authority;

  {
    cout << "Creating session: " << url << endl;

    NodeSession sess(url);
    SqlResult res = sess.sql("SELECT @@version").execute();
    cout << "Talking to MySQL Server: " << res.fetchOne()[0] << endl;
  }

  {
    url = std::string("mysqlx://") + url;
    cout << "Creating session: " << url << endl;

    NodeSession sess(url);
    SqlResult res = sess.sql("SELECT @@version").execute();
    cout << "Talking to MySQL Server: " << res.fetchOne()[0] << endl;
  }

  // Test URL containing non-ascii characters.

  {
    mysqlx::string wurl = L"Rafa\u0142";
    wurl = wurl + string("@") + string(authority);

    cout << "Creating session: " << wurl << endl;

    try {
      NodeSession sess(wurl);
      SqlResult res = sess.sql("SELECT @@version").execute();
      cout << "Talking to MySQL Server: " << res.fetchOne()[0] << endl;
    }
    catch (const Error &e)
    {
      cout << "Expected error: " << e << endl;
    }
  }

  cout << "Done!" << endl;
}


TEST_F(Sess, default_schema)
{
  SKIP_IF_NO_XPLUGIN;

  EXPECT_THROW(get_sess().getDefaultSchema(),Error);

  {
    mysqlx::NodeSession s(m_port, m_user, m_password, "test");

    EXPECT_EQ(string("test"), s.getDefaultSchema().getName());
    SqlResult res = s.sql(L"SELECT DATABASE()").execute();
    string db = res.fetchOne()[0];
    EXPECT_EQ(string("test"), db);
  }

  {
    std::string url = m_user;
    url = url + "@localhost";
    if (m_port)
      url = url + ":" + std::to_string(m_port);
    url = url + "/test";

    mysqlx::NodeSession s(url);

    EXPECT_EQ(string("test"), s.getDefaultSchema().getName());
    SqlResult res = s.sql(L"SELECT DATABASE()").execute();
    string db = res.fetchOne()[0];
    EXPECT_EQ(string("test"), db);
  }

  cout << "Done!" << endl;
}


TEST_F(Sess, trx)
{
  SKIP_IF_NO_XPLUGIN;

  Collection coll = get_sess().getSchema("test").createCollection("c", true);
  coll.remove().execute();

  try {
    coll.getSession().startTransaction();
    coll.add("{\"foo\": 1}").execute();
    coll.add("{\"foo\": 2}").execute();
    coll.getSession().commit();

    coll.getSession().startTransaction();
    coll.add("{\"bar\": 3}").execute();
    coll.add("{\"bar\": 4}").execute();
    coll.getSession().rollback();
  }
  catch (...)
  {
    get_sess().rollback();
    throw;
  }

  cout << "After insertions collection has " << coll.count()
       << " documents." << endl;

  EXPECT_EQ(2U, coll.count());

  for (DbDoc doc : coll.find().execute())
  {
    cout << "- doc: " << doc << endl;
    EXPECT_FALSE(doc.hasField("bar"));
  }

  /*
    Check that deleted session rollbacks transaction that is not
    committed.
  */

  {
    XSession sess(this);
    Collection coll = sess.getSchema("test").getCollection("c");

    sess.startTransaction();
    coll.add("{\"bar\": 5}").execute();
    coll.add("{\"bar\": 6}").execute();
  }

  cout << "Collection has " << coll.count()
    << " documents." << endl;

  EXPECT_EQ(2U, coll.count());

  for (DbDoc doc : coll.find().execute())
  {
    cout << "- doc: " << doc << endl;
    EXPECT_FALSE(doc.hasField("bar"));
  }

  /*
    Check error thrown if starting new transaction while previous
    one is not closed.
  */

  get_sess().startTransaction();

  try {
    get_sess().startTransaction();
    FAIL() << "Expected an error";
  }
  catch (const Error &e)
  {
    cout << "Expected error: " << e << endl;
  }

  cout << "Done!" << endl;
}


TEST_F(Sess, bind_node_session)
{
  SKIP_IF_NO_XPLUGIN;

   XSession *sess = new XSession(this);

   sess->dropSchema("node_session");
   Schema sch = sess->createSchema("node_session");

   // Create Collection with data
   sch.createCollection("coll1")
       .add("{\"name\":\"foo\"}")
       .add("{\"name\":\"bar\"}")
       .add("{\"name\":\"baz\"}").execute();

   {
     // Get first NodeSession
     NodeSession node= sess->bindToDefaultShard();

     // Execute query to be checked later
     SqlResult res = node.sql("select * from node_session.coll1").execute();

     // Closing NodeSession
     // Only affects this session, no changes on other nodes or master XSession
     node.close();

     // Expect throw Error() because session is closed
     EXPECT_THROW(node.sql("select * from node_session.coll1")
                  .execute(),
                  mysqlx::Error);

     {
       // Get second NodeSession
       NodeSession node2= sess->bindToDefaultShard();

       // Execute query but doesn't retrieve results right away
       SqlResult res2 = node2.sql("select * from node_session.coll1").execute();

       //Close Session so other NodeSession obj are closed.
       sess->close();

       // Expect throw Error() because session is closed
       EXPECT_THROW(node2.sql("select * from node_session.coll1")
                                .execute(),
                    mysqlx::Error);


       // Delete Xsession, so other NodeSession obj are closed.
       delete sess;

       // Expect throw Error() because session is closed
       EXPECT_THROW(node2.sql("select * from node_session.coll1")
                                .execute(),
                    mysqlx::Error);

       // Results are available, because where cached when destructing XSession
       EXPECT_EQ(3U, res2.count());


       int i = 0;
       for (auto row : res2)
       {
         std::cout << "Row " << i << ": "
                    << static_cast<string>(row[1]) << std::endl;
         ++i;
       }

       EXPECT_EQ(3U, i);
     }

     //Same here
     EXPECT_EQ(3U, res.count());

     unsigned i = 0;
     for (auto row : res)
     {
       std::cout << "Row " << i << ": " << static_cast<string>(row[1]) << std::endl;
       ++i;
     }

     EXPECT_EQ(3U, i);
   }

  cout << "Done!" << endl;
}

TEST_F(Sess, ssl_session)
{

  SKIP_IF_NO_XPLUGIN;

  {
    mysqlx::XSession sess(SessionSettings::PORT, get_port(),
                          SessionSettings::USER,get_user(),
                          SessionSettings::PWD, get_password() ? get_password() : NULL ,
                          SessionSettings::SSL_ENABLE, true);

    SqlResult res =  sess.bindToDefaultShard().sql("SHOW STATUS LIKE 'mysqlx_ssl_cipher'").execute();

    auto row = res.fetchOne();
    cout << row[0] << ":" << row[1] << endl;

    string cipher = row[1];

    EXPECT_FALSE(cipher.empty());
  }

  {
    mysqlx::XSession sess(SessionSettings::PORT, get_port(),
                          SessionSettings::USER, get_user(),
                          SessionSettings::PWD, get_password() ? get_password() : nullptr ,
                          SessionSettings::SSL_ENABLE, false);

    SqlResult res =  sess.bindToDefaultShard().sql("SHOW STATUS LIKE 'mysqlx_ssl_cipher'").execute();

    auto row = res.fetchOne();
    cout << row[0] << ":" << row[1] << endl;

    string cipher = row[1];

    EXPECT_TRUE(cipher.empty());
  }

  //Using URI

  std::stringstream uri;

  uri << "mysqlx://" << get_user();

  if (get_password() && *get_password())
    uri << ":"<< get_password();

  uri << "@" << "localhost:" << get_port();

  //URI without ssl_enable
  {
    mysqlx::XSession sess(uri.str());

    SqlResult res =  sess.bindToDefaultShard().sql("SHOW STATUS LIKE 'mysqlx_ssl_cipher'").execute();

    auto row = res.fetchOne();
    cout << row[0] << ":" << row[1] << endl;

    string cipher = row[1];

    EXPECT_TRUE(cipher.empty());
  }


  //Enable SSL
  uri << "/?ssl-enable";
  {
    mysqlx::XSession sess(uri.str());

    SqlResult res =  sess.bindToDefaultShard().sql("SHOW STATUS LIKE 'mysqlx_ssl_cipher'").execute();

    auto row = res.fetchOne();
    cout << row[0] << ":" << row[1] << endl;

    string cipher = row[1];

    EXPECT_FALSE(cipher.empty());
  }

}

TEST_F(Sess, view)
{
  SKIP_IF_NO_XPLUGIN;

  XSession s(this);

  const string schema_name = "schemaView";
  const string tbl_name = "tblView";
  const string view_name = "view1";
  const string view_name2 = "view2";

  Schema sch = s.createSchema(schema_name,true);

  s.dropTable(schema_name, tbl_name);

  sch.dropView(view_name).ifExists().execute();

  std::stringstream qry;

  qry << "CREATE TABLE `"<< schema_name <<
         "`.`" << tbl_name <<"` (name VARCHAR(20) ,age INT);";

  get_sess().sql(qry.str()).execute();

  Table tbl = sch.getTable(tbl_name, true);

  tbl.insert("name", "age")
      .values("Foo", 20)
      .values("Bar", 30)
      .values("Baz", 40)
      .execute();

  std::cout << "Create View" << std::endl;

  sch.createView(view_name, false)
      .security(mysqlx::SQLSecurity::DEFINER)
      .definer("root")
      .definedAs(tbl.select("name as view_name", "2016-age as view_birth"))
      .withCheckOption(mysqlx::CheckOption::LOCAL)
      .execute();

  std::cout << "Check View" << std::endl;

  {
    Table view = sch.getTable(view_name);

    RowResult res = view.select().execute();

    EXPECT_EQ(string("view_name"), res.getColumn(0).getColumnName());
    EXPECT_EQ(string("view_birth"), res.getColumn(1).getColumnName());

    for(auto row : res)
    {
      if (row.get(0).get<string>() == string("Foo"))
        EXPECT_EQ(1996, row.get(1).get<int>());
      else if (row.get(0).get<string>() == string("Bar"))
        EXPECT_EQ(1986, row.get(1).get<int>());
      else if (row.get(0).get<string>() == string("Baz"))
        EXPECT_EQ(1976, row.get(1).get<int>());
    }
  }

  std::cout << "Expects error, since view is already created" << std::endl;

  EXPECT_THROW(
        sch.createView(view_name, false)
        .security(mysqlx::SQLSecurity::DEFINER)
        .definedAs(tbl.select("name as view_name", "2016-age as view_birth"))
        .withCheckOption(mysqlx::CheckOption::LOCAL)
        .execute()
        , mysqlx::Error
        );

  TableSelect tbl_select=
      tbl.select("name", "2*age", "1 as one", "2 as two");

  std::vector<string> columns_list = {"view_name", "view_double_age"};

  std::cout << "Different number of columns... Error expected" << std::endl;

  EXPECT_THROW(
  sch.alterView(view_name)
     .columns(columns_list, "one")
     .security(mysqlx::SQLSecurity::DEFINER)
     .definer("root")
     .definedAs(tbl_select)
     .withCheckOption(mysqlx::CheckOption::LOCAL)
     .execute()
        , mysqlx::Error);

  auto view_exec = sch.alterView(view_name)
                   .columns(columns_list, "one", string("two"))
                   .security(mysqlx::SQLSecurity::DEFINER)
                   .definer("root")
                   .definedAs(tbl_select)
                   .withCheckOption(mysqlx::CheckOption::LOCAL);

  TableSelect tbl_select_2 = tbl_select;

  std::cout << "Shouldn't update the alterView" << std::endl;

  tbl_select.limit(1);

  tbl_select_2.limit(2);

  //Execute copy of ViewAlter obj
  auto view_exec2 = view_exec;

  view_exec2.execute();

  std::cout << "Execute previously create TableSelect" << std::endl;

  EXPECT_EQ(1, tbl_select.execute().count());

  std::cout << "Execute previously create TableSelect" << std::endl;

  EXPECT_EQ(2, tbl_select_2.execute().count());

  std::cout << "Cannot alter View that is not created" << std::endl;

  EXPECT_THROW( sch.alterView(view_name2)
                .columns("view_name", "fake")
                .security(mysqlx::SQLSecurity::DEFINER)
                .definer("root")
                .definedAs(tbl_select)
                .withCheckOption(mysqlx::CheckOption::LOCAL)
                .execute()
                , mysqlx::Error);

  std::cout << "Check View after alterView" << std::endl;

  {
    Table view = sch.getTable(view_name);

    RowResult res = view.select().execute();

    std::vector<Column> columns_metadata = res.getColumns();

    EXPECT_EQ(string("view_name"), res.getColumn(0).getColumnName());
    EXPECT_EQ(string("view_double_age"), res.getColumn(1).getColumnName());
    EXPECT_EQ(string("one"), res.getColumn(2).getColumnName());
    EXPECT_EQ(string("two"), res.getColumn(3).getColumnName());

    //EXPECT 3 rows, since the changed tableSelect shouldn't affect the view
    EXPECT_EQ(3, res.count());

    for(auto row : res)
    {
      if (row.get(0).get<string>() == string("Foo"))
        EXPECT_EQ(40, row.get(1).get<int>());
      if (row.get(0).get<string>() == string("Bar"))
        EXPECT_EQ(60, row.get(1).get<int>());
      if (row.get(0).get<string>() == string("Baz"))
        EXPECT_EQ(80, row.get(1).get<int>());
    }

  }

  std::cout << "Create view with select bind" << std::endl;

  {
    TableSelect sel = tbl.select();
    sel.where("age = :age").bind("age", 40);

    sch.dropView(view_name).execute();

    sch.createView(view_name).definedAs(sel).execute();

    Table view = sch.getTable(view_name);

    RowResult res = view.select().execute();

    EXPECT_EQ(1, res.count());

    Row row = res.fetchOne();

    EXPECT_EQ(40, row.get(1).get<int>());

    sel.bind("age", 30);

    sch.alterView(view_name).definedAs(sel).execute();

    res = view.select().execute();

    EXPECT_EQ(1, res.count());

    row = res.fetchOne();

    EXPECT_EQ(30, row.get(1).get<int>());

  }

  std::cout << "Drop view" << std::endl;

  sch.dropView(view_name).execute();

  std::cout << "Drop view doesn't throw error because using ifExists" << std::endl;

  sch.dropView(view_name).ifExists().execute();

  std::cout << "Without ifExists, will throw Error if no such View." << std::endl;

  EXPECT_THROW(sch.dropView(view_name).execute(), mysqlx::Error);

}
