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
#include <mysqlx.h>
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
