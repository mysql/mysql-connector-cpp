/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
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

#include <test.h>
#include <iostream>


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

  /*
    Mixed case in schema name does not work on MacOS due to server-side
    bug#25769683.

    TODO: Change it back to mixed case when the bug is fixed.
  */

  const string schema_name = "schemaobj"; // "schemaObj";
  const string coll_name = "collobj";     // "collObj";
  const string tbl_name = "tblobj";       // "tblObj";

  try {
    get_sess().dropSchema(schema_name);
  } catch (...) {}


  Schema schema = get_sess().createSchema(schema_name);

  //Test Schema Obj

  EXPECT_TRUE(schema.existsInDatabase());
  EXPECT_EQ(schema_name ,schema.getName());
  // EXPECT_FALSE(schema.getSession().getSchema("NOT_FOUND").existsInDatabase());

  //Test Collection Obj

  Collection coll = schema.createCollection(coll_name);

  EXPECT_TRUE(coll.existsInDatabase());
  EXPECT_EQ(coll_name, coll.getName());
  EXPECT_FALSE(coll.getSession().getSchema(schema_name)
               .getCollection("NOT_FOUND").existsInDatabase());


  //Test Table Obj

  {
    std::stringstream create_table;

    create_table << "CREATE TABLE"
      << "`" << schema_name << "`"
      << ".`" << tbl_name << "`"
      << "(name VARCHAR(20) ,age INT)" << std::ends;

    get_sess().sql(create_table.str()).execute();
  }

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

    mysqlx::Session sess(url);
    SqlResult res = sess.sql("SELECT @@version").execute();
    cout << "Talking to MySQL Server: " << res.fetchOne()[0] << endl;
  }

  {
    url = std::string("mysqlx://") + url;
    cout << "Creating session: " << url << endl;

    mysqlx::Session sess(url);
    SqlResult res = sess.sql("SELECT @@version").execute();
    cout << "Talking to MySQL Server: " << res.fetchOne()[0] << endl;
  }

  // Test URL containing non-ascii characters.

  {
    mysqlx::string wurl = L"Rafa\u0142";
    wurl = wurl + string("@") + string(authority);

    cout << "Creating session: " << wurl << endl;

    try {
      mysqlx::Session sess(wurl);
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
    mysqlx::Session s(m_port, m_user, m_password, "test");

    EXPECT_EQ(string("test"), s.getDefaultSchema().getName());
    EXPECT_EQ(string("test"), s.getDefaultSchemaName());
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

    mysqlx::Session s(url);

    EXPECT_EQ(string("test"), s.getDefaultSchema().getName());
    EXPECT_EQ(string("test"), s.getDefaultSchemaName());
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
  coll.remove("true").execute();

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
    Session sess(this);
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

  //With Savepoints!

  get_sess().startTransaction();

  std::vector<string> savepoints;

  coll.add("{\"bar\": 5}").execute();
  savepoints.emplace_back(get_sess().setSavepoint()); //savepoints[0]
  coll.add("{\"bar\": 6}").execute();
  savepoints.emplace_back(get_sess().setSavepoint()); //savepoints[1]
  coll.add("{\"bar\": 7}").execute();
  savepoints.emplace_back(get_sess().setSavepoint()); //savepoints[2]
  coll.add("{\"bar\": 8}").execute();
  savepoints.emplace_back(get_sess().setSavepoint("MySave")); //savepoints[3]

  get_sess().releaseSavepoint("MySave");
  EXPECT_THROW(get_sess().releaseSavepoint(savepoints.back()), Error);
  savepoints.pop_back();
  // rollback to bar:6
  get_sess().rollbackTo(savepoints[1]);
  //savepoint of bar:7 was removed because of the rollback to bar:6
  EXPECT_THROW(get_sess().rollbackTo(savepoints[2]), Error);
  EXPECT_THROW(get_sess().rollbackTo(""), Error);
  get_sess().rollbackTo(savepoints.front());
  get_sess().commit();

  cout << "Collection has " << coll.count()
    << " documents." << endl;

  EXPECT_EQ(3U, coll.count());

  cout << "Done!" << endl;
}

TEST_F(Sess, auth_method)
{
  SKIP_IF_NO_XPLUGIN;

  USE_NATIVE_PWD;

  auto check_user = [](mysqlx::Session &sess)
  {
    SqlResult res = sess.sql("SELECT CURRENT_USER()").execute();
    auto row = res.fetchOne();
    string str = row[0];
    cout << "User: " << str << endl;
  };

  {
    mysqlx::Session sess(SessionOption::PORT, get_port(),
                         SessionOption::USER, get_user(),
                         SessionOption::PWD, get_password() ? get_password() : nullptr,
                         SessionOption::SSL_MODE, SSLMode::DISABLED,
                         SessionOption::AUTH, AuthMethod::MYSQL41
    );
    check_user(sess);
  }

  {
    // This will throw because of plain auth without SSL
    EXPECT_THROW(mysqlx::Session sess(SessionOption::PORT, get_port(),
                         SessionOption::USER, get_user(),
                         SessionOption::PWD, get_password() ? get_password() : nullptr,
                         SessionOption::SSL_MODE, SSLMode::DISABLED,
                         SessionOption::AUTH, AuthMethod::PLAIN),
                 Error);
  }

  {
    mysqlx::Session sess(SessionOption::PORT, get_port(),
                         SessionOption::USER, get_user(),
                         SessionOption::PWD, get_password() ? get_password() : nullptr,
                         SessionOption::SSL_MODE, SSLMode::REQUIRED,
                         SessionOption::AUTH, AuthMethod::PLAIN
    );
    check_user(sess);
  }

  {
    mysqlx::Session sess(SessionOption::PORT, get_port(),
                         SessionOption::USER, get_user(),
                         SessionOption::PWD, get_password() ? get_password() : nullptr,
                         SessionOption::SSL_MODE, SSLMode::REQUIRED,
                         SessionOption::AUTH, AuthMethod::MYSQL41
    );
    check_user(sess);
  }

  std::stringstream uri;
  uri << "mysqlx://" << get_user();
  if (get_password() && *get_password())
    uri << ":" << get_password();
  uri << "@" << "localhost:" << get_port();

  {
    std::stringstream str;
    str << uri.str() << "/?ssl-mode=disabled&auth=mysql41";
    mysqlx::Session sess(str.str());
    check_user(sess);
  }

  {
    std::stringstream str;
    str << uri.str() << "/?ssl-mode=disabled&auth=plain";
    EXPECT_THROW(mysqlx::Session sess(str.str()), Error);
  }

  {
    std::stringstream str;
    str << uri.str() << "/?ssl-mode=required&auth=plain";
    mysqlx::Session sess(str.str());
    check_user(sess);
  }

  {
    std::stringstream str;
    str << uri.str() << "/?ssl-mode=required&auth=mysql41";
    mysqlx::Session sess(str.str());
    check_user(sess);
  }
}

TEST_F(Sess, auth_external)
{

  SKIP_IF_NO_XPLUGIN;

  // This will throw because of EXTERNAL is not supported
  EXPECT_THROW(mysqlx::Session sess(SessionOption::PORT, get_port(),
                                    SessionOption::USER, get_user(),
                                    SessionOption::PWD, get_password() ? get_password() : nullptr,
                                    SessionOption::SSL_MODE, SSLMode::DISABLED,
                                    SessionOption::AUTH, AuthMethod::PLAIN),
               Error);

  std::stringstream uri;
  uri << "mysqlx://" << get_user();
  if (get_password() && *get_password())
    uri << ":" << get_password();
  uri << "@" << "localhost:" << get_port();

    uri << "/?ssl-mode=required&auth=external";
    EXPECT_THROW(mysqlx::Session sess(uri.str()),
                 Error);
}


TEST_F(Sess, ssl_session)
{

  SKIP_IF_NO_XPLUGIN;

  USE_NATIVE_PWD;

  //Test if ssl is enabled using cipher
  auto check_ssl_impl = [](mysqlx::Session &sess, bool enable, int line)
  {
    SqlResult res =  sess.sql("SHOW STATUS LIKE 'mysqlx_ssl_cipher'").execute();

    auto row = res.fetchOne();
    cout << "Line "<< line << ": " << row[0] << ":" << row[1] << endl;

    string cipher = row[1];

    EXPECT_EQ(enable, !cipher.empty());
  };

#define check_ssl(x,y) check_ssl_impl(x, y, __LINE__)


  {
    mysqlx::Session sess(SessionOption::PORT, get_port(),
                         SessionOption::USER,get_user(),
                         SessionOption::PWD, get_password() ? get_password() : nullptr
                          );

    check_ssl(sess, true);
  }

  {
    mysqlx::Session sess(SessionOption::PORT, get_port(),
                         SessionOption::USER, get_user(),
                         SessionOption::PWD, get_password() ? get_password() : nullptr,
                         SessionOption::SSL_MODE, SSLMode::DISABLED
                          );

    check_ssl(sess, false);
  }

  //Using URI

  std::stringstream uri;

  uri << "mysqlx://" << get_user();

  if (get_password() && *get_password())
    uri << ":"<< get_password();

  uri << "@" << "localhost:" << get_port();

  //URI using ssl-mode=disabled
  {
    std::stringstream ssl_off;
    ssl_off << uri.str() << "/?sSl-mODe=DIsabled";
    mysqlx::Session sess(ssl_off.str());
    check_ssl(sess, false);
  }


  {
    std::stringstream uri_ssl;
    //Enable SSL
    uri_ssl << uri.str() << "/?SSl-Mode=RequireD";

    mysqlx::Session sess(uri_ssl.str());
    check_ssl(sess, true);
  }

  {
    std::stringstream uri_wrong;
    uri_wrong << uri.str() << "/?ssl-nonexisting=true";
    EXPECT_THROW(mysqlx::Session sess(uri_wrong.str()), mysqlx::Error);
  }

  //using wrong ssl-ca as SessionSettings
  {
    EXPECT_THROW(
    mysqlx::Session sess(SessionOption::PORT, get_port(),
                         SessionOption::USER,get_user(),
                         SessionOption::PWD, get_password() ? get_password() : nullptr ,
                         SessionOption::SSL_MODE, SSLMode::VERIFY_CA,
                         SessionOption::SSL_CA, "unknown")
          , mysqlx::Error);


  }

  //using wrong ssl-ca and ssl-ca-path on URI
  {
    std::stringstream bad_uri;
    bad_uri << uri.str() << "/?Ssl-modE=VErify_Ca&sSl-Ca=" << "unknown.file";

    EXPECT_THROW(mysqlx::Session sess(bad_uri.str()), mysqlx::Error);
  }

  string ssl_ca;
  string datadir;

  {
    mysqlx::Session sess(uri.str());

    SqlResult res = sess.sql(
                      "select if("
                      "@@ssl_ca REGEXP '^([^:]+:)?[/\\\\\\\\]'"
                      ", @@ssl_ca"
                      ", concat(ifnull(@@ssl_capath,@@datadir), @@ssl_ca))"
                      ).execute();

    ssl_ca = res.fetchOne().get(0);
  }

  std::cout << "ssl-ca:" << ssl_ca << std::endl;

  uri << "/?ssl-ca=" << ssl_ca;

  // using ssl-mode and ssl-ca as SessionSettings

  {
    mysqlx::Session sess(SessionOption::PORT, get_port(),
                         SessionOption::USER,get_user(),
                         SessionOption::PWD, get_password() ? get_password() : nullptr ,
                         SessionOption::SSL_MODE, SSLMode::VERIFY_CA,
                         SessionOption::SSL_CA, ssl_ca);

    check_ssl(sess, true);

  }

  //using ssl-ca as SessionSettings

  {
    string bad_uri;

    bad_uri = uri.str() + "&ssl-mode=DISABLED";
    EXPECT_THROW(mysqlx::Session sess(bad_uri) , mysqlx::Error);

    bad_uri = uri.str() + "&ssl-mode=REQUIRED";
    EXPECT_THROW(mysqlx::Session sess(bad_uri) , mysqlx::Error);

    EXPECT_THROW(
    mysqlx::Session sess(SessionOption::PORT, get_port(),
                         SessionOption::USER, get_user(),
                         SessionOption::PWD, get_password() ? get_password() : nullptr,
                         SessionOption::SSL_MODE, SSLMode::DISABLED,
                         SessionOption::SSL_CA, ssl_ca)
                    , mysqlx::Error);

    EXPECT_THROW(
    mysqlx::Session sess2(SessionOption::PORT, get_port(),
                          SessionOption::USER, get_user(),
                          SessionOption::PWD, get_password() ? get_password() : nullptr,
                          SessionOption::SSL_MODE, SSLMode::REQUIRED,
                          SessionOption::SSL_CA, ssl_ca)
                    , mysqlx::Error);
  }

  //using ssl-ca but ssl-enable = false on SessionSettings

  {
    /*
      Becaue we do not give valid CA setting, session creation should fail
      when verifying server certificate.
    */

    EXPECT_THROW(
      mysqlx::Session sess(
        SessionOption::PORT, get_port(),
        SessionOption::USER, get_user(),
        SessionOption::PWD, get_password() ? get_password() : nullptr,
        SessionOption::SSL_CA, "wrong_ca.pem"
      )
    , Error);
  }

  //using ssl-enable and ssl-ca as SessionSettings

  {

    try {
      mysqlx::Session sess(SessionOption::HOST, "127.0.0.1",
                           SessionOption::PORT, get_port(),
                           SessionOption::PRIORITY, 1,
                           SessionOption::HOST, "localhost",
                           SessionOption::PORT, get_port(),
                           SessionOption::PRIORITY, 100,
                           SessionOption::HOST, "localhost4",
                           SessionOption::PORT, get_port(),
                           SessionOption::PRIORITY, 1,
                           SessionOption::HOST, "::1",
                           SessionOption::PORT, get_port(),
                           SessionOption::PRIORITY, 1,
                           SessionOption::HOST, "localhost6",
                           SessionOption::PORT, get_port(),
                           SessionOption::PRIORITY, 1,
                           SessionOption::USER,get_user(),
                           SessionOption::PWD, get_password() ? get_password() : NULL ,
                           SessionOption::SSL_MODE, SSLMode::VERIFY_IDENTITY,
                           SessionOption::SSL_CA, ssl_ca);

      // If server cert CN=localhost, verification will succeed, and ssl is
      // enabled
      check_ssl(sess, true);
    }
    catch (Error &e)
    {
      // If server cert CN!=localhost, it will fail with this error
      std::cout << e.what() << std::endl;
      EXPECT_NE(std::string::npos,
        std::string(e.what()).find("SSL certificate validation failure")
      );
    }

  }

  //Errors
  {
    //Defined twice
    EXPECT_THROW(SessionSettings(SessionOption::SSL_MODE, SSLMode::DISABLED,
                                 SessionOption::SSL_MODE, SSLMode::DISABLED),
                 Error);

    EXPECT_THROW(SessionSettings(SessionOption::SSL_CA, "dummy",
                                 SessionOption::SSL_CA, "dummy"),
                 Error);

    EXPECT_THROW(SessionSettings(SessionOption::SSL_MODE, SSLMode::DISABLED,
                                 SessionOption::SSL_CA, "dummy"),
                Error);

    SessionSettings sess(SessionOption::SSL_CA, "dummy");
    sess.set(SessionOption::HOST, "localhost");

    EXPECT_THROW(sess.set(SessionOption::PORT, 13000), Error);
    EXPECT_THROW(sess.set(SessionOption::PRIORITY, 1), Error);
    EXPECT_THROW(sess.set(SessionOption::HOST, "localhost",
                          SessionOption::PORT, 13000,
                          SessionOption::PRIORITY, 1,
                          SessionOption::PORT, 13000,
                          SessionOption::PORT, 13000,
                          SessionOption::PRIORITY, 1), Error);


    sess.set(SessionOption::SSL_MODE, SSLMode::VERIFY_IDENTITY);

    EXPECT_THROW(sess.set(SessionOption::SSL_MODE, SSLMode::VERIFY_IDENTITY,
                          SessionOption::SSL_MODE, SSLMode::VERIFY_IDENTITY),
                 Error);
  }

  {
    //Defined twice
    try {
      mysqlx::Session("localhost?ssl-mode=disabled&ssl-mode=verify_ca");
      FAIL() << "No error thrown";
    }
    catch (Error &e)
    {
      cout << "Expected error: " << e << endl;
      EXPECT_EQ(string("Option SSL_MODE defined twice"),string(e.what()));
    }

    try {
      mysqlx::Session("localhost?ssl-ca=unknown&ssl-ca=hereItIs");
      FAIL() << "No error thrown";
    }
    catch (Error &e)
    {
      cout << "Expected error: " << e << endl;
      EXPECT_EQ(string("Option SSL_CA defined twice"),string(e.what()));
    }

    try {
      mysqlx::Session("localhost?ssl-mode=Whatever");
      FAIL() << "No error thrown";
    }
    catch (Error &e)
    {
      cout << "Expected error: " << e << endl;
      EXPECT_NE(std::string::npos,
        std::string(e.what()).find("Invalid ssl mode"));
    }
  }

}


TEST_F(Sess, ipv6)
{

  SKIP_IF_NO_XPLUGIN;

  USE_NATIVE_PWD;

  {
    mysqlx::Session sess(SessionOption::HOST, "::1",
                          SessionOption::PORT, get_port(),
                          SessionOption::USER, get_user(),
                          SessionOption::PWD, get_password() ? get_password() : nullptr,
                          SessionOption::SSL_MODE, SSLMode::DISABLED
                          );
  }

  //Using URI

  std::stringstream uri;

  uri << "mysqlx://" << get_user();

  if (get_password() && *get_password())
    uri << ":"<< get_password();

  uri << "@" << "[::1]:" << get_port();

  //URI without ssl_mode
  {
    mysqlx::Session sess(uri.str());

    SqlResult res =  sess.sql("SHOW STATUS LIKE 'mysqlx_ssl_cipher'").execute();

    auto row = res.fetchOne();
    cout << row[0] << ":" << row[1] << endl;

    string cipher = row[1];

    EXPECT_FALSE(cipher.empty());
  }

  //Disable SSL_MODE
  uri << "/?Ssl-Mode=DisabLED";
  {
    mysqlx::Session sess(uri.str());

    SqlResult res =  sess.sql("SHOW STATUS LIKE 'mysqlx_ssl_cipher'").execute();

    auto row = res.fetchOne();
    cout << row[0] << ":" << row[1] << endl;

    string cipher = row[1];

    EXPECT_TRUE(cipher.empty());
  }
}


TEST_F(Sess, failover)
{
  SKIP_IF_NO_XPLUGIN;

  {
    Session s(this);

    s.createSchema("test", true);
  }

  cout << "Host with and without priority" << endl;

  {
    std::stringstream uri;

    uri << "[(address=localhost:" << get_port() <<",priority=0)";
    uri << ",127.0.0.1:" << get_port();
    uri << "]";
    EXPECT_THROW(
      mysqlx::Session s(uri.str())
      , Error
    );
  }

  cout << "URI multiple hosts tests" << endl;
  {
    std::stringstream uri;

    uri << "mysqlx://" << get_user();

    if (get_password())
      uri << ":" << get_password();

    uri << "@["
           "localhost6,"
           "127.0.1.250:33060,"
           "[::2]:1,";
    uri << "127.0.0.1";
    if (get_port() != 0)
      uri << ":" <<get_port();

    uri << "]/test";

    mysqlx::Session s(uri.str());

    EXPECT_EQ(string("test"),s.getDefaultSchema().getName());
  }

  cout << "URI multiple hosts tests with priority" << endl;

  {
    std::stringstream uri;

    uri << "mysqlx://" << get_user();

    if (get_password())
      uri << ":" << get_password();

    uri << "@["
           "(address=localhost6,priority=99),"
           "(address=127.0.1.250:33060,priority=99),"
           "(address=[::2]:1,priority=1),";
    uri << "(address=127.0.0.1";
    if (get_port() != 0)
      uri << ":" <<get_port();
    uri << ",priority=100)";

    uri << "]/test";

    mysqlx::Session s(uri.str());

    EXPECT_EQ(string("test"),s.getDefaultSchema().getName());
  }

  cout << "Using session settings" << endl;

  {
    mysqlx::Session s(SessionOption::USER, get_user(),
                      SessionOption::PWD, get_password() ? get_password() : nullptr,
                      SessionOption::HOST, "server.example.com",
                      SessionOption::PRIORITY, 1,
                      SessionOption::HOST, "192.0.2.11",
                      SessionOption::PORT, 33060,
                      SessionOption::PRIORITY, 99,
                      SessionOption::HOST, "[2001:db8:85a3:8d3:1319:8a2e:370:7348]",
                      SessionOption::PORT, 1,
                      SessionOption::PRIORITY, 2,
                      SessionOption::HOST, "::1",
                      SessionOption::PORT, get_port(),
                      SessionOption::PRIORITY, 100,
                      SessionOption::HOST, "localhost",
                      SessionOption::PORT, get_port(),
                      SessionOption::PRIORITY, 100,
                      SessionOption::DB, "test");

    EXPECT_EQ(string("test"),s.getDefaultSchema().getName());
  }

  cout << "SessionOption::set() tests" << endl;

  {
    SessionSettings settings(SessionOption::USER, get_user(),
                             SessionOption::PWD, get_password() ?
                               get_password() :
                               nullptr);

    // TODO: why error here?
    //EXPECT_THROW(
    settings.set(SessionOption::DB, "test", SessionOption::PORT, get_port()); // ,
    //  Error);

    EXPECT_THROW(settings.set(SessionOption::PRIORITY, 1), Error);

    EXPECT_THROW(settings.set(SessionOption::HOST, "server.example.com",
                              SessionOption::USER, get_user(),
                              SessionOption::PORT, 1,
                              SessionOption::PRIORITY, 1), Error);

    settings.erase(SessionOption::HOST);
    settings.erase(SessionOption::PORT);

    settings.set(SessionOption::HOST, "server.example.com",
                 SessionOption::PRIORITY, 1,
                 SessionOption::HOST, "192.0.2.11",
                 SessionOption::PORT, 33060,
                 SessionOption::PRIORITY, 98,
                 SessionOption::HOST, "[2001:db8:85a3:8d3:1319:8a2e:370:7348]",
                 SessionOption::PRIORITY, 2,
                 SessionOption::HOST, "::1",
                 SessionOption::PORT, get_port(),
                 SessionOption::PRIORITY, 99,
                 SessionOption::HOST, "localhost",
                 SessionOption::PORT, get_port(),
                 SessionOption::PRIORITY, 99
                 );


    mysqlx::Session s(settings);

    EXPECT_EQ(string("test"),s.getDefaultSchema().getName());
  }

  cout << "SessionOption::set() tests without Port and Priority" << endl;

  {
    SessionSettings settings(SessionOption::USER, get_user(),
                             SessionOption::PWD, get_password() ?
                               get_password() :
                               nullptr);

    settings.set(SessionOption::HOST, "192.0.2.11",
                 SessionOption::DB, "test"
                 );


    EXPECT_THROW(mysqlx::Session s(settings), Error);
  }

  cout << "Multiple host with 1st host defined only by port" << endl;

  {
    SessionSettings settings(SessionOption::USER, get_user(),
                             SessionOption::PWD, get_password() ?
                               get_password() :
                               nullptr,
                             SessionOption::PORT, 1);

    // Error because first host was not explicit.

    EXPECT_THROW(
      settings.set(SessionOption::HOST, "192.0.2.11",
                   SessionOption::PORT, 33060),
      Error);
  }

  cout << "Priority > 100" << endl;

  {
    EXPECT_THROW(
          mysqlx::Session(SessionOption::USER, get_user(),
                           SessionOption::PWD, get_password() ?
                             get_password() :
                             nullptr,
                           SessionOption::HOST, "localhost",
                           SessionOption::PORT, get_port(),
                           SessionOption::PRIORITY, 101),
          Error);


    std::stringstream uri;

    uri << "[(address=localhost:" << get_port() <<",priority=101)]";

    EXPECT_THROW(mysqlx::Session s(uri.str()) , Error);
  }

}

#ifndef _WIN32
TEST_F(Sess, unix_socket)
{
  SKIP_IF_NO_SOCKET;

  mysqlx::Session(SessionOption::SOCKET, get_socket(),
                  SessionOption::USER, get_user(),
                  SessionOption::PWD, get_password());

  std::stringstream uri;

  uri << "mysqlx://" << get_user();

  if (get_password())
    uri << ":" << get_password();

  uri << "@["
         "(address=(" << get_socket() << "),priority=99),";
  uri << "(address=127.0.0.1";
  if (get_port() != 0)
    uri << ":" <<get_port();
  uri << ",priority=100)";

  uri << "]/test";


  for (int i = 0; i < 10; ++i)
  {
    mysqlx::Session(uri.str());
  }

  SessionSettings settings(SessionOption::SOCKET, get_socket(),
                           SessionOption::PRIORITY, 100,
                           SessionOption::USER, get_user(),
                           SessionOption::PWD, get_password(),
                           SessionOption::HOST, "localhost",
                           SessionOption::PRIORITY, 1);

  EXPECT_EQ(settings.find(SessionOption::SOCKET).get<string>(), string(get_socket()));

  EXPECT_EQ(settings.find(SessionOption::HOST).get<string>(), string("localhost"));

  EXPECT_THROW(mysqlx::Session(SessionOption::SOCKET, "c:\\mtsqlx.socket")
               ,Error);

  // SSL is not supported and should throw error if forced
  EXPECT_THROW(mysqlx::Session(SessionOption::SOCKET, get_socket(),
                               SessionOption::USER, get_user(),
                               SessionOption::PWD, get_password(),
                               SessionOption::SSL_MODE, SSLMode::REQUIRED)
               ,Error);

  // but ignore then when not having host
  mysqlx::Session(SessionOption::SOCKET, get_socket(),
                  SessionOption::USER, get_user(),
                  SessionOption::PWD, get_password(),
                  SessionOption::SSL_MODE, SSLMode::REQUIRED,
                  SessionOption::HOST, "localhost",
                  SessionOption::PORT, get_port());


  uri << "?ssl-mode=REQUIRED";

  EXPECT_NO_THROW(mysqlx::Session(uri.str()));

  std::stringstream bad_uri;

  bad_uri << "mysqlx://" << get_user();

  if (get_password())
    bad_uri << ":" << get_password();

  bad_uri << "@(" << get_socket() << ")/test?ssl-mode=REQUIRED";

  EXPECT_THROW(mysqlx::Session(bad_uri.str()), Error);

}
#endif //_WIN32


TEST_F(Sess, bugs)
{
  SKIP_IF_NO_XPLUGIN

  {
    SessionSettings sess_settings("localhost_not_found", 13009, "rafal", (char*)NULL);
    EXPECT_THROW(mysqlx::Session sess(sess_settings), mysqlx::Error);
  }

  {
    SessionSettings sess_settings("localhost_not_found", 13009, "rafal", NULL);
    EXPECT_THROW(mysqlx::Session sess(sess_settings), mysqlx::Error);
  }

  {
    // empty string as password
    SessionSettings sess_settings("localhost_not_found", 13009, "rafal", "");
  }


  {
    // Using same Result on different sessions

    SessionSettings settings(SessionOption::PORT, get_port(),
                             SessionOption::USER,get_user(),
                             SessionOption::PWD, get_password() ?
                               get_password() :
                               nullptr
                               );

    mysqlx::Session sess(settings);

    cout << "Connection 1 passed" << endl;
    RowResult res = sess.sql("show status like 'mysqlx_ssl_cipher'").execute();
    Row row = res.fetchOne();
    cout << row[0] << " : " << row[1] << endl;
    string Val = row[1];

    mysqlx::Session sess2(settings);
    cout << "Connection 2 passed" << endl;
    res = sess2.sql("show status like 'mysqlx_ssl_cipher'").execute();
    row = res.fetchOne();
    cout << row[0] << " : " << row[1] << endl;
    Val = row[1];

  }

  try
  {
    Session sess(this);
    Session xsess(this);
    sess.sql("drop database database_doesnt_exist").execute();

  }
  catch(...)
  {}

  //memory leak when using bad ssl_ca
  EXPECT_THROW(mysqlx::Session(SessionOption::SSL_CA, "Bad",
                               SessionOption::SSL_MODE, SSLMode::VERIFY_CA,
                               SessionOption::PORT, get_port(),
                               SessionOption::USER,get_user(),
                               SessionOption::PWD, get_password() ?
                                 get_password() :
                                 nullptr
                                 ),
               Error);

}
