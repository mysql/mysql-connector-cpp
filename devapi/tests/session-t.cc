/*
 * Copyright (c) 2015, 2019, Oracle and/or its affiliates. All rights reserved.
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
#include <future>
#include <chrono>

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
    mysqlx::string wurl = u"Rafa\u0142";
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
    SqlResult res = s.sql("SELECT DATABASE()").execute();
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
    SqlResult res = s.sql("SELECT DATABASE()").execute();
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


TEST_F(Sess, timeout_opts)
{

  EXPECT_NO_THROW(
    SessionSettings settings("root@localhost?connect-timeout=10")
  );

  EXPECT_NO_THROW(
    SessionSettings settings(SessionOption::CONNECT_TIMEOUT,10)
  );

  EXPECT_NO_THROW(
    SessionSettings settings(
      SessionOption::CONNECT_TIMEOUT,std::chrono::seconds(10)
    )
  );

  // Negative tests

  EXPECT_THROW(
    SessionSettings settings("root@localhost?connect-timeout=-10"),
    Error
  );

  EXPECT_THROW(
    SessionSettings settings(SessionOption::CONNECT_TIMEOUT,-10),
    Error
  );

  EXPECT_THROW(
    SessionSettings settings("root@localhost?connect-timeout=10.5"),
    Error
  );

  EXPECT_THROW(
    SessionSettings settings(SessionOption::CONNECT_TIMEOUT,10.5),
    Error
  );

}


TEST_F(Sess, connect_timeout)
{
// Set MANUAL_TESTING to 1 and define NON_BOUNCE_SERVER
#define MANUAL_TESTING 0
#if(MANUAL_TESTING == 1)

#define NON_BOUNCE_SERVER "define.your.server"
#define NON_BOUNCE_PORT1 81
#define NON_BOUNCE_PORT2 82


  SKIP_IF_NO_XPLUGIN;
  {
    auto start = std::chrono::high_resolution_clock::now();

    // Timeout was not specified, assume 10s
    EXPECT_THROW(mysqlx::Session sess(SessionOption::HOST, NON_BOUNCE_SERVER,
                                      SessionOption::PORT, NON_BOUNCE_PORT1,
                                      SessionOption::USER, get_user(),
                                      SessionOption::PWD, get_password() ? get_password() : nullptr),
                 Error);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds nsec = end - start;
    cout << "Timeout default test passed " << nsec.count()/1000000 << " ms" << endl;
  }

  {
    auto start = std::chrono::high_resolution_clock::now();

    EXPECT_THROW(mysqlx::Session sess(SessionOption::HOST, NON_BOUNCE_SERVER,
                                      SessionOption::PORT, NON_BOUNCE_PORT1,
                                      SessionOption::USER, get_user(),
                                      SessionOption::PWD, get_password() ? get_password() : nullptr,
                                      SessionOption::CONNECT_TIMEOUT,
                                      std::chrono::seconds(5)),
                 Error);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds nsec = end - start;
    cout << "Timeout std::chrono::seconds(5) passed " << nsec.count() / 1000000 << " ms" << endl;
  }


  {

    SessionSettings settings(SessionOption::HOST, NON_BOUNCE_SERVER,
                             SessionOption::PORT, NON_BOUNCE_PORT1,
                             SessionOption::USER, get_user(),
                             SessionOption::PWD, get_password() ? get_password() : nullptr,
                             SessionOption::CONNECT_TIMEOUT, 1000);

    settings.erase(SessionOption::CONNECT_TIMEOUT);
    settings.set(SessionOption::CONNECT_TIMEOUT, 5000);
    auto start = std::chrono::high_resolution_clock::now();
    EXPECT_THROW(mysqlx::Session sess(settings),
                  Error);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds nsec = end - start;
    cout << "Timeout plain integer 5000 ms test passed " <<
      nsec.count() / 1000000 << " ms" << endl;
  }

  {
    std::stringstream uri;
    uri << "mysqlx://" << get_user();
    if (get_password() && *get_password())
      uri << ":" << get_password();
    uri << "@" << NON_BOUNCE_SERVER << ":" << NON_BOUNCE_PORT1;
    std::stringstream str;
    str << uri.str() << "/?connect-timeout=5000";

    // Record start time
    auto start = std::chrono::high_resolution_clock::now();

    EXPECT_THROW(
      mysqlx::Session sess(str.str()),
      Error);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds nsec = end - start;
    cout << "Timeout URI (connect-timeout=5000) test passed " <<
      nsec.count() / 1000000 << " ms" << endl;
  }

  {
    // Record start time
    auto start = std::chrono::high_resolution_clock::now();
    EXPECT_THROW(
    mysqlx::Session sess(SessionOption::HOST, NON_BOUNCE_SERVER,
                         SessionOption::PORT, NON_BOUNCE_PORT1,
                         SessionOption::PRIORITY, 1,
                         SessionOption::HOST, NON_BOUNCE_SERVER,
                         SessionOption::PORT, NON_BOUNCE_PORT2,
                         SessionOption::PRIORITY, 2,
                         SessionOption::CONNECT_TIMEOUT, std::chrono::seconds(3),
                         SessionOption::USER, get_user(),
                         SessionOption::PWD, get_password() ? get_password() : NULL
                      ),
                      Error);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::nanoseconds nsec = end - start;
    cout << "Timeout multihost 2x3 sec test passed " <<
      nsec.count() / 1000000 << " ms" << endl;
  }

#ifndef _WIN32
  {
    // Record start time
    auto start = std::chrono::high_resolution_clock::now();
    EXPECT_THROW(
      // but ignore then when not having host
      mysqlx::Session sess(SessionOption::SOCKET, "/tmp/socket_wrong.sock",
                      SessionOption::USER, get_user(),
                      SessionOption::PWD, get_password(),
                      SessionOption::CONNECT_TIMEOUT, 3000
                    ),
                    Error);

    auto end = std::chrono::high_resolution_clock::now();
    cout << "Timeout socket test passed " <<
      nsec.count() / 1000000 << " ms" << endl;
  }
#endif

#endif
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
    // BAD PASSWORD
    EXPECT_THROW(
          mysqlx::Session(SessionOption::PORT, get_port(),
                          SessionOption::USER, get_user(),
                          SessionOption::PWD, "notworkingpassword",
                          SessionOption::AUTH, AuthMethod::MYSQL41
                          ), Error);
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

  //with ssl-ca and SSLMode < VERIFY_CA
  {
    EXPECT_THROW(
    mysqlx::Session sess(SessionOption::PORT, get_port(),
                         SessionOption::USER,get_user(),
                         SessionOption::PWD, get_password() ? get_password() : nullptr ,
                         SessionOption::SSL_MODE, SSLMode::REQUIRED,
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

  //without ssl-ca as SessionSettings
  {
    EXPECT_THROW(
    mysqlx::Session sess(SessionOption::PORT, get_port(),
                         SessionOption::USER,get_user(),
                         SessionOption::PWD, get_password() ? get_password() : nullptr ,
                         SessionOption::SSL_MODE, SSLMode::VERIFY_CA);
          , mysqlx::Error);


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

  //using ssl-ca but with the wrong CA

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

  //using ssl-mode=VERIFY_IDENTITY and ssl-ca as SessionSettings

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
    catch (Error &)
    {
      // Auto-generated server certificates cannot be verified
      // against localhost.
      // TODO: Generate certificates with localhost in
      // Subject Alternative Name
      // std::cout << e.what() << std::endl;
      // EXPECT_NE(std::string::npos,
      //  std::string(e.what()).find("SSL certificate validation failure")
      // );
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
    uri << ",not_good:65535" << get_port();
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
           "wont_work:65535,"
           "[::1]:65535,";
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
           "(address=localhost6:65535,priority=99),"
           "(address=127.0.0.1:65535,priority=99),"
           "(address=[::1]:65535,priority=1),";
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
                      SessionOption::HOST, "rubish",
                      SessionOption::PORT, 65535,
                      SessionOption::PRIORITY, 99,
                      SessionOption::HOST, "[::1]",
                      SessionOption::PORT, 65535,
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
                              SessionOption::PORT, 65535,
                              SessionOption::PRIORITY, 1), Error);

    settings.erase(SessionOption::HOST);
    settings.erase(SessionOption::PORT);

    settings.set(SessionOption::HOST, "server.example.com",
                 SessionOption::PRIORITY, 1,
                 SessionOption::HOST, "away",
                 SessionOption::PORT, 65535,
                 SessionOption::PRIORITY, 98,
                 SessionOption::HOST, "to_far_away",
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

    settings.erase(SessionOption::HOST);
    settings.erase(SessionOption::PORT);
    settings.erase(SessionOption::PRIORITY);
    settings.erase(SessionOption::SOCKET);
    settings.erase(SessionOption::SSL_CA);
    settings.erase(SessionOption::SSL_MODE);
    settings.erase(SessionOption::AUTH);

    EXPECT_THROW(mysqlx::Session(std::move(settings)), Error);
  }

  cout << "SessionOption::set() tests without Port and Priority" << endl;

  {
    SessionSettings settings(SessionOption::USER, get_user(),
                             SessionOption::PWD, get_password() ?
                               get_password() :
                               nullptr);

    settings.set(SessionOption::HOST, "looking_for_it",
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
      settings.set(SessionOption::HOST, "not_found",
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

  EXPECT_TRUE(settings.find(SessionOption::SSL_MODE).isNull());

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


TEST_F(Sess, sha256_memory)
{
  SKIP_IF_NO_XPLUGIN

  try{
    sql("DROP USER 'doomuser'@'%';");
  }
  catch(...) {}

  try{
    sql("DROP USER 'doomuserdefault'@'%';");
  }
  catch(...) {}


  // DEFAULT AUTH: MYSQL41 on 5.7 and SHA256_MEMORY on 8.0
  sql("CREATE USER 'doomuserdefault'@'%' IDENTIFIED  WITH sha256_password BY '!doomuserdefault_pass';");

  SessionSettings mysqldefault_cleartext(
        SessionOption::SSL_MODE, SSLMode::DISABLED,
        SessionOption::USER, "doomuserdefault",
        SessionOption::PWD, "!doomuserdefault_pass",
        SessionOption::HOST, "localhost",
        SessionOption::PORT, get_port());

  SessionSettings mysqldefault_ssl(
        SessionOption::USER, "doomuserdefault",
        SessionOption::PWD, "!doomuserdefault_pass",
        SessionOption::HOST, "localhost",
        SessionOption::PORT, get_port());

  try {
    mysqlx::Session s(mysqldefault_cleartext);
    FAIL() << "Should have thrown!";
  } catch (Error) {
    SUCCEED() << "Expected throw!";
  }

  mysqlx::Session s_mysqldefault_ssl(mysqldefault_ssl);

  mysqlx::Session s_mysqldefault_cleartext(mysqldefault_cleartext);

  try{
    mysqlx::Session(          SessionOption::SSL_MODE, SSLMode::DISABLED,
                              SessionOption::USER, "noone",
                              SessionOption::PWD, "!no_pass",
                              SessionOption::HOST, "localhost",
                              SessionOption::PORT, get_port());
    FAIL() << "No exception sent on bad login";
  }
  catch(Error &e)
  {
    std::cout << "Expected error: " << e << std::endl;
  }

  try{
    sql("CREATE USER 'doomuser'@'%' IDENTIFIED WITH caching_sha2_password BY '!sha2user_pass';");
  }catch(...)
  {
    SKIP_TEST("No caching_sha2_password support");
    return;
  }

  {
    SessionSettings sha_256_cleartext(
          SessionOption::AUTH, AuthMethod::SHA256_MEMORY,
          SessionOption::SSL_MODE, SSLMode::DISABLED,
          SessionOption::USER, "doomuser",
          SessionOption::PWD, "!sha2user_pass",
          SessionOption::HOST, "localhost",
          SessionOption::PORT, get_port());

    SessionSettings default_cleartext(
          SessionOption::SSL_MODE, SSLMode::DISABLED,
          SessionOption::USER, "doomuser",
          SessionOption::PWD, "!sha2user_pass",
          SessionOption::HOST, "localhost",
          SessionOption::PORT, get_port());

    //First authentication... should fail!
    EXPECT_THROW( mysqlx::Session s_sha256(sha_256_cleartext), Error);

    //Auth using normal logic
    SessionSettings default_opt(
          SessionOption::USER, "doomuser",
          SessionOption::PWD, "!sha2user_pass",
          SessionOption::HOST, "localhost",
          SessionOption::PORT, get_port());
    mysqlx::Session s_plain(default_opt);

    //Second authentication... should work!
    mysqlx::Session s_sha256_works(sha_256_cleartext);

    mysqlx::Session default_works(default_cleartext);

    SessionSettings default_cleartext_fail(
                            SessionOption::SSL_MODE, SSLMode::DISABLED,
                            SessionOption::USER, "doomuser",
                            SessionOption::PWD, "!sha2user_pass_fail",
                            SessionOption::HOST, "localhost",
                            SessionOption::PORT, get_port());
    EXPECT_THROW( mysqlx::Session s_sha256_fail(default_cleartext_fail),Error);
  }

}


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

  cout << "empty string as password" << endl;

  {
    SessionSettings sess_settings("localhost_not_found", 13009, "rafal", "");
  }


  cout << "Using same Result on different sessions" << endl;

  {
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

  cout << "memory leak when using bad ssl_ca" << endl;

  EXPECT_THROW(mysqlx::Session(SessionOption::SSL_CA, "Bad",
                               SessionOption::SSL_MODE, SSLMode::VERIFY_CA,
                               SessionOption::PORT, get_port(),
                               SessionOption::USER,get_user(),
                               SessionOption::PWD, get_password() ?
                                 get_password() :
                                 nullptr
                                 ),
               Error);

  cout << "Session shut-down with pending multi-result set." << endl;

  {
    Session sess(this);

    sess.sql("drop procedure if exists test.test").execute();
    sess.sql("CREATE PROCEDURE test.test() BEGIN select 1; select 2; END")
        .execute();

    SqlResult res = sess.sql("call test.test()").execute();
  }
}


TEST_F(Sess, pool_opts)
{
  ClientSettings dummy("mysqlx://root@localhost:1/test",
                       ClientOption::POOLING            , 1,
                       ClientOption::POOL_MAX_SIZE      , 2,
                       ClientOption::POOL_QUEUE_TIMEOUT , 3,
                       ClientOption::POOL_MAX_IDLE_TIME  , 4);

  EXPECT_TRUE(dummy.has_option(SessionOption::USER             ));
  EXPECT_TRUE(dummy.has_option(SessionOption::HOST             ));
  EXPECT_TRUE(dummy.has_option(SessionOption::PORT             ));
  EXPECT_TRUE(dummy.has_option(SessionOption::DB               ));
  EXPECT_TRUE(dummy.has_option(ClientOption::POOLING           ));
  EXPECT_TRUE(dummy.has_option(ClientOption::POOL_MAX_SIZE     ));
  EXPECT_TRUE(dummy.has_option(ClientOption::POOL_QUEUE_TIMEOUT));
  EXPECT_TRUE(dummy.has_option(ClientOption::POOL_MAX_IDLE_TIME ));

  EXPECT_EQ(string("root")     ,dummy.find(SessionOption::USER             ).get<string>());
  EXPECT_EQ(string("localhost"),dummy.find(SessionOption::HOST             ).get<string>());
  EXPECT_EQ(1                  ,dummy.find(SessionOption::PORT             ).get<int>());
  EXPECT_EQ(string("test")     ,dummy.find(SessionOption::DB               ).get<string>());
  EXPECT_EQ(1                  ,dummy.find(ClientOption::POOLING           ).get<int>());
  EXPECT_EQ(2                  ,dummy.find(ClientOption::POOL_MAX_SIZE     ).get<int>());
  EXPECT_EQ(3                  ,dummy.find(ClientOption::POOL_QUEUE_TIMEOUT).get<int>());
  EXPECT_EQ(4                  ,dummy.find(ClientOption::POOL_MAX_IDLE_TIME).get<int>());

  dummy.erase(SessionOption::USER             );
  dummy.erase(ClientOption::POOLING           );
  EXPECT_FALSE(dummy.has_option(SessionOption::USER            ));
  EXPECT_FALSE(dummy.has_option(ClientOption::POOLING          ));
  EXPECT_TRUE(dummy.has_option(SessionOption::HOST             ));
  EXPECT_TRUE(dummy.has_option(SessionOption::PORT             ));
  EXPECT_TRUE(dummy.has_option(SessionOption::DB               ));
  EXPECT_TRUE(dummy.has_option(ClientOption::POOL_MAX_SIZE     ));
  EXPECT_TRUE(dummy.has_option(ClientOption::POOL_QUEUE_TIMEOUT));
  EXPECT_TRUE(dummy.has_option(ClientOption::POOL_MAX_IDLE_TIME));

  EXPECT_TRUE(dummy.find(SessionOption::USER             ).isNull());
  EXPECT_TRUE(dummy.find(ClientOption::POOLING           ).isNull());

  dummy.clear();

  EXPECT_FALSE(dummy.has_option(SessionOption::USER             ));
  EXPECT_FALSE(dummy.has_option(SessionOption::HOST             ));
  EXPECT_FALSE(dummy.has_option(SessionOption::PORT             ));
  EXPECT_FALSE(dummy.has_option(SessionOption::DB               ));
  EXPECT_FALSE(dummy.has_option(ClientOption::POOLING           ));
  EXPECT_FALSE(dummy.has_option(ClientOption::POOL_MAX_SIZE     ));
  EXPECT_FALSE(dummy.has_option(ClientOption::POOL_QUEUE_TIMEOUT));
  EXPECT_FALSE(dummy.has_option(ClientOption::POOL_MAX_IDLE_TIME));


  std::stringstream uri;
  uri << "mysqlx://" << (get_user() ? get_user() : "user")
      << "@localhost:" << get_port() <<"/test";

  ClientSettings working_settings(uri.str(),
                        R"( { "pooling": {
                        "enabled": true,
                        "maxSize": 25,
                        "queueTimeout": 1000,
                        "maxIdleTime": 5000}
                        })");

  ClientSettings working_settings_2(uri.str(),
                        DbDoc(R"( { "pooling": {
                        "enabled": true,
                        "maxSize": 25,
                        "queueTimeout": 1000,
                        "maxIdleTime": 5000}
                        })"));

  EXPECT_THROW(
        ClientSettings(uri.str(),
                       DbDoc(R"( { "enabled": true, "maxSize": 25, "queueTimeout": 1000, "maxIdleTime": 5000 })")),
        Error);

  EXPECT_THROW(
    ClientSettings(uri.str(),
      R"( { "enabled": true, "queueTimeout": 10.5, "maxIdleTime": 5000 })"
    ),
    Error
  );

  EXPECT_THROW(
    ClientSettings(uri.str(),
      R"( { "enabled": true, "queueTimeout": 10, "maxIdleTime": 50.5 })"
    ),
    Error
  );

  ClientSettings(ClientOption::POOL_QUEUE_TIMEOUT, std::chrono::milliseconds::max());

  EXPECT_THROW(
    ClientSettings(
      ClientOption::POOL_QUEUE_TIMEOUT,
      std::numeric_limits<uint64_t>::max()
    ),
    Error
  );

  EXPECT_THROW(ClientSettings(ClientOption::POOL_MAX_SIZE, 0),Error);

  EXPECT_THROW(ClientSettings("mysqlx://root@localhost",
                              R"( { "pooling": {"enabled": true, "maxSize": 0, "queueTimeout": 1000, "maxIdleTime": 5000}})"),
                       Error);

  EXPECT_THROW(ClientSettings(ClientOption::POOL_QUEUE_TIMEOUT, 10.5), Error);
  EXPECT_THROW(ClientSettings(ClientOption::POOL_MAX_IDLE_TIME, 10.5), Error);


  //Client constructors
  SKIP_IF_NO_XPLUGIN;

  //Using ClientSettings;
  {
    mysqlx::Client client(working_settings);
    mysqlx::Session s = client.getSession();
  }

  //Using connection string and ClientSettings
  {
    mysqlx::Client client(uri.str(), working_settings);
    mysqlx::Session s = client.getSession();
  }

  // Using connection string plus JSON string
  {
    mysqlx::Client client(uri.str(),
                          R"( { "pooling": {"enabled": true, "maxSize": 25, "queueTimeout": 1000, "maxIdleTime": 5000}})");
    mysqlx::Session s = client.getSession();
  }

  //Using just options (ClientOptions and SessionOptions
  {
    mysqlx::Client client(SessionOption::PORT, get_port(),
                          SessionOption::USER, get_user(),
                          SessionOption::PWD, get_password() ? get_password() : nullptr,
                          ClientOption::POOLING, true,
                          ClientOption::POOL_MAX_SIZE, 10,
                          ClientOption::POOL_QUEUE_TIMEOUT, 1000,
                          ClientOption::POOL_MAX_IDLE_TIME, 10000);
    mysqlx::Session s = client.getSession();
  }

  //Using just options (ClientOptions and SessionOptions) but starting with
  //ClientOption
  {
    mysqlx::Client client(ClientOption::POOLING, true,
                          SessionOption::PORT, get_port(),
                          SessionOption::USER, get_user(),
                          SessionOption::PWD, get_password() ? get_password() : nullptr,
                          ClientOption::POOL_MAX_SIZE, 10,
                          ClientOption::POOL_QUEUE_TIMEOUT, 1000,
                          ClientOption::POOL_MAX_IDLE_TIME, 10000);
    mysqlx::Session s = client.getSession();
  }

  //Using URI + ClientOptions
  {
    mysqlx::Client client(uri.str(),
                          ClientOption::POOL_MAX_SIZE, 10,
                          ClientOption::POOL_QUEUE_TIMEOUT, 1000,
                          ClientOption::POOL_MAX_IDLE_TIME, 10000);
    mysqlx::Session s = client.getSession();
  }

  /*
     Global Functions: getSession() and getCLient
  */

  mysqlx::getClient(uri.str(),
                        ClientOption::POOL_MAX_SIZE, 10,
                        ClientOption::POOL_QUEUE_TIMEOUT, 1000,
                        ClientOption::POOL_MAX_IDLE_TIME, 10000).getSession();

  mysqlx::getSession(uri.str());

}


TEST_F(Sess, pool_use)
{
  SKIP_IF_NO_XPLUGIN;


  const int max_connections = 80;

  ClientSettings settings(SessionOption::AUTH, AuthMethod::SHA256_MEMORY,
                          ClientOption::POOLING, true,
                          SessionOption::SSL_MODE, SSLMode::DISABLED,
                          ClientOption::POOL_MAX_SIZE, max_connections,
                          ClientOption::POOL_QUEUE_TIMEOUT, 1000,
                          ClientOption::POOL_MAX_IDLE_TIME, 10000,
                          SessionOption::HOST, "localhost",
                          SessionOption::PORT, get_port(),
                          SessionOption::PRIORITY, 100,
                          SessionOption::HOST, "localhost",
                          SessionOption::PORT, get_port(),
                          SessionOption::PRIORITY, 1,
                          SessionOption::USER, get_user(),
                          SessionOption::PWD, get_password(),
                          SessionOption::DB, "test");

  {

    {
      mysqlx::Client client(settings);
      mysqlx::Session s1 = client.getSession();
      {
        mysqlx::Session s2(client);
        {
          std::list<mysqlx::Session> m_sessions;
          for(int i = 3; i <= max_connections+10; ++i)
          {
            if (i <= max_connections)
              m_sessions.emplace_back(client);
            else
              EXPECT_THROW(m_sessions.emplace_back(client),Error);
          }
        }
        EXPECT_EQ(string("test"), s2.getSchema("test",true).getName());


        //Closing Client!
        client.close();
        {
        //Closes all opened sessions and creating new ones will throw error!
          EXPECT_THROW( mysqlx::Session s3(client), Error);
        }

        // Closing a client invalidates all sessions created from it

        EXPECT_THROW(s2.getSchema("test",true).getName(), Error);
      }
      EXPECT_THROW(s1.getSchema("test",true).getName(), Error);
    }

    {

      // getting sessions from pool takes less time than getting new ones...
      // so will wait more than time to live and then get sessions and compare time

      ClientSettings settings_local(settings);
      //Garantee pool will stay full!
      settings_local.set(ClientOption::POOL_QUEUE_TIMEOUT, std::chrono::seconds(100));
      settings_local.set(ClientOption::POOL_MAX_IDLE_TIME, std::chrono::seconds(100));

      mysqlx::Client client(settings_local);

      auto get_sessions = [&client, &max_connections] ()
      {
        std::list<mysqlx::Session> m_sessions;
        for(int i = 0; i < max_connections; ++i)
        {
          m_sessions.emplace_back(client);
        }
      };


      //First round, pool clean
      std::chrono::time_point<std::chrono::system_clock> start_time
          = std::chrono::system_clock::now();

      get_sessions();

      auto clean_pool_duration = std::chrono::system_clock::now() - start_time;



      //Second round, pool full
      start_time = std::chrono::system_clock::now();

      get_sessions();

      auto full_pool_duration = std::chrono::system_clock::now() - start_time;

      std::cout << "Clean Pool: " <<
                   std::chrono::duration_cast<std::chrono::milliseconds>(
                     clean_pool_duration).count() << "ms" << std::endl;
      std::cout << "Populated Pool: " <<
                   std::chrono::duration_cast<std::chrono::milliseconds>(
                     full_pool_duration).count() << "ms" <<std::endl;

    }

  }

  //Global getSession function
  {
    auto sess = mysqlx::getSession(
                  SessionOption::HOST, "localhost",
                  SessionOption::PORT, get_port(),
                  SessionOption::PRIORITY, 100,
                  SessionOption::HOST, "localhost",
                  SessionOption::PORT, get_port(),
                  SessionOption::PRIORITY, 1,
                  SessionOption::USER, get_user(),
                  SessionOption::PWD, get_password(),
                  SessionOption::DB, "test");
    auto res = sess.sql("Select 1").execute();
    EXPECT_EQ(1, res.fetchOne()[0].get<int>());
  }


}


TEST_F(Sess, pool_ttl)
{
  SKIP_IF_NO_XPLUGIN;

  sql("set global mysqlx_wait_timeout=28800");

  const int max_connections = 10;

  std::chrono::seconds queue_timeout(50);
  std::chrono::milliseconds pool_ttl(500);

  ClientSettings settings(ClientOption::POOLING, true,
                          SessionOption::AUTH, AuthMethod::SHA256_MEMORY,
                          SessionOption::SSL_MODE, SSLMode::DISABLED,
                          ClientOption::POOL_MAX_SIZE, max_connections,
                          ClientOption::POOL_QUEUE_TIMEOUT, queue_timeout,
                          ClientOption::POOL_MAX_IDLE_TIME, pool_ttl,
                          SessionOption::HOST, "localhost",
                          SessionOption::PORT, get_port(),
                          SessionOption::PRIORITY, 100,
                          SessionOption::HOST, "localhost",
                          SessionOption::PORT, get_port(),
                          SessionOption::PRIORITY, 1,
                          SessionOption::USER, get_user(),
                          SessionOption::PWD, get_password(),
                          SessionOption::DB, "test");

#if 1

  // threaded example
  {
    std::cout << "Threaded" << std::endl;

    //short POOL_MAX_IDLE_TIME so that sessions expire.
    settings.set(ClientOption::POOL_QUEUE_TIMEOUT, 100000,
                 ClientOption::POOL_MAX_IDLE_TIME, 1);

    settings.set(ClientOption::POOL_QUEUE_TIMEOUT, std::chrono::seconds(100),
                 ClientOption::POOL_MAX_IDLE_TIME, std::chrono::microseconds(1));

    mysqlx::Client client(settings);

    std::list<std::future<mysqlx::Session*>> session_list;
    for (int i=0; i < max_connections*4; ++i)
    {
      session_list.emplace_back(std::async(std::launch::async,
                                           [&client] () -> mysqlx::Session*
                                           {
                                             return new mysqlx::Session(client.getSession());
                                           }
                                           ));
    }

    auto test_sessions = [&session_list,max_connections] (bool expect_errors)
    {
      int errors_found = 0;
      while(session_list.size() > 0)
      {
        auto el = session_list.begin();
        for (; el != session_list.end();)
        {
          mysqlx::Session *s = nullptr;
          if (el->wait_for(std::chrono::milliseconds(100)) ==
              std::future_status::ready)
          {
            try {
              s = el->get();
              SqlResult res = s->sql("SELECT 1").execute();

              EXPECT_EQ(1, res.fetchOne()[0].get<int>());

            } catch (Error &e) {
              std::cout << "EXPECTED: " << e << std::endl;
              errors_found++;
            }
            session_list.erase(el++);
            delete s;
          }
          else
          {
            ++el;
          }
        }
      }

      if (expect_errors)
      {
        EXPECT_EQ(4*max_connections, errors_found);
      }
      else
      {
        EXPECT_EQ(0, errors_found);
      }
    };

    test_sessions(false);

    // Now closing pool so that waiting threads get session without timeout
    client.close();

    ClientSettings settings1 = settings;

    settings1.set(ClientOption::POOL_QUEUE_TIMEOUT, std::chrono::hours(1),
                 ClientOption::POOL_MAX_IDLE_TIME, std::chrono::hours(1));

    mysqlx::Client client1 = mysqlx::getClient(settings1);

    for (int i=0; i < max_connections*4; ++i)
    {
      session_list.emplace_back(std::async(std::launch::async,
                                           [&client1] () -> mysqlx::Session*
                                           {
                                             return new mysqlx::Session(client1.getSession());
                                           }
                                           ));
    }

    client1.close();

    test_sessions(true);

  }

#endif

  // Temporary disabled, bug#30532629

#if 0
  {
    std::cout << "Not threaded" << std::endl;

    settings.set(ClientOption::POOL_QUEUE_TIMEOUT, std::chrono::seconds(100),
      ClientOption::POOL_MAX_IDLE_TIME, std::chrono::seconds(10));

    mysqlx::Client client(settings);


    auto get_sessions = [&client, &max_connections]()
    {
      std::list<mysqlx::Session> sessions;
      for (int i = 0; i < max_connections; ++i)
      {
        sessions.emplace_back(client);
        EXPECT_EQ(1, sessions.back().sql("select 1").execute()
      }
    };

    get_sessions();

    std::cout << "Kill connections" << std::endl;

    std::vector<int> proccess_ids;

    int this_thread_id = sql("SELECT CONNECTION_ID()").fetchOne()[0].get<int>();

    std::list<Row> rows = sql("show processlist").fetchAll();

    for (auto row : rows)
    {
      auto val = row.get(7);
      int thread_id = row.get(0).get<int>();
      if (val.isNull() ||
        continue;

      proccess_ids.push_back(thread_id);
    }


    for (auto id : proccess_ids)
    {
      std::stringstream query;
      query << "KILL CONNECTION " << id;
      sql(query.str());
    }


    std::cout << "set global mysqlx_wait_timeout=20" << std::endl;

    sql("set global mysqlx_wait_timeout=20");

    get_sessions();

    std::this_thread::sleep_for(std::chrono::seconds(20));

    get_sessions();

    std::cout << "set global mysqlx_wait_timeout=28800" << std::endl;
    client.getSession().sql("set global mysqlx_wait_timeout=28800").execute();
  }

#endif // 0


  {
    settings.set(ClientOption::POOL_MAX_SIZE, 1);

    mysqlx::Client cli(settings);
    mysqlx::Session s1 = cli.getSession();
    s1.close();
    mysqlx::Session s2 = cli.getSession();

  }

  {

    std::stringstream uri;
    uri << "mysqlx://" << get_user();
    if (get_password() && *get_password())
      uri << ":" << get_password();
    uri << "@" << "localhost:" << get_port();

    mysqlx::Session s = mysqlx::getSession(uri.str());
    s.close();
  }

  {
    settings.set(ClientOption::POOL_MAX_SIZE, 10);
    std::vector<mysqlx::Session> session_list;
    for (int i=0; i < 5; ++i)
    {
      mysqlx::Client cli(settings);
      for (int j=0; j < 10; ++j)
      {
        session_list.emplace_back(cli.getSession());
      }
    }
  }

}


TEST_F(Sess, settings_iterator)
{
  {
    mysqlx::ClientSettings client_settings(
          ClientOption::POOLING, true,
          ClientOption::POOL_MAX_IDLE_TIME, 3,
          SessionOption::AUTH, AuthMethod::PLAIN,
          ClientOption::POOL_MAX_SIZE, 1,
          ClientOption::POOL_QUEUE_TIMEOUT, 2,
          SessionOption::SSL_MODE, SSLMode::DISABLED,
          SessionOption::HOST, "localhost1",
          SessionOption::PORT, 1,
          SessionOption::PRIORITY, 1,
          SessionOption::HOST, "localhost2",
          SessionOption::PORT, 2,
          SessionOption::PRIORITY, 2,
          SessionOption::USER, get_user(),
          SessionOption::PWD, get_password(),
          SessionOption::DB, "test");

    uint64_t host_count = 1;
    uint64_t port_count = 1;
    uint64_t prio_count = 1;

    for (auto set : client_settings)
    {
      ClientOption c_opt(set.first);
      std::cout << ClientOptionName(c_opt) << std::endl;

      switch(c_opt)
      {
        case ClientOption::POOLING:
          EXPECT_TRUE(set.second.get<bool>());
          break;
        case SessionOption::URI:
          break;
        case SessionOption::HOST:
          {
            std::stringstream hostname;
            hostname << "localhost" << host_count++;
            EXPECT_EQ(hostname.str(), set.second.get<std::string>());
          }
          break;
        case SessionOption::PORT:
          {
            EXPECT_EQ(port_count++, set.second.get<unsigned int>());
          }
          break;
      }

      if(set.first == ClientOption::POOL_MAX_SIZE)
        EXPECT_EQ(1, set.second.get<unsigned int>());
      else if(set.first == ClientOption::POOL_QUEUE_TIMEOUT)
        EXPECT_EQ(2, set.second.get<unsigned int>());
      else if(set.first == ClientOption::POOL_MAX_IDLE_TIME)
        EXPECT_EQ(3, set.second.get<unsigned int>());
      else if(set.first == SessionOption::PRIORITY)
        EXPECT_EQ(prio_count++, set.second.get<unsigned int>());
      else if(set.first == SessionOption::AUTH)
        EXPECT_EQ(static_cast<int>(AuthMethod::PLAIN), set.second.get<int>());
      else if(set.first == SessionOption::SSL_MODE)
        EXPECT_EQ(static_cast<int>(SSLMode::DISABLED), set.second.get<int>());
      else if(set.first == SessionOption::USER)
        EXPECT_EQ(std::string(get_user() ? get_user() : ""), set.second.get<std::string>());
      else if(set.first == SessionOption::PWD)
      {
        if (get_password())
          EXPECT_EQ(std::string(get_password()), set.second.get<std::string>());
        else
          EXPECT_TRUE(set.second.get<std::string>().empty());
      }
      else if(set.first == SessionOption::DB)
        EXPECT_EQ(std::string("test"), set.second.get<std::string>());

    }
  }

  {
    mysqlx::SessionSettings session_settings(
          SessionOption::AUTH, AuthMethod::PLAIN,
          SessionOption::SSL_MODE, SSLMode::DISABLED,
          SessionOption::HOST, "localhost1",
          SessionOption::PORT, 1,
          SessionOption::PRIORITY, 1,
          SessionOption::HOST, "localhost2",
          SessionOption::PORT, 2,
          SessionOption::PRIORITY, 2,
          SessionOption::USER, get_user(),
          SessionOption::PWD, get_password(),
          SessionOption::DB, "test");

    uint64_t host_count = 1;
    uint64_t port_count = 1;
    uint64_t prio_count = 1;

    for (auto set : session_settings)
    {
      SessionOption s_opt(set.first);
      ClientOption c_opt(set.first);
      std::cout << SessionOptionName(s_opt) << std::endl;
      std::cout << ClientOptionName(c_opt) << std::endl;
      if (set.first == SessionOption::HOST)
      {
        std::stringstream hostname;
        hostname << "localhost" << host_count++;
        EXPECT_EQ(hostname.str(), set.second.get<std::string>());
      }
      else if(set.first == SessionOption::PORT)
      {
        EXPECT_EQ(port_count++, set.second.get<unsigned int>());
      }
      else if(set.first == SessionOption::PRIORITY)
        EXPECT_EQ(prio_count++, set.second.get<unsigned int>());
      else if(set.first == SessionOption::AUTH)
        EXPECT_EQ(static_cast<int>(AuthMethod::PLAIN), set.second.get<int>());
      else if(set.first == SessionOption::SSL_MODE)
        EXPECT_EQ(static_cast<int>(SSLMode::DISABLED), set.second.get<int>());
      else if(set.first == SessionOption::USER)
        EXPECT_EQ(std::string(get_user() ? get_user() : ""), set.second.get<std::string>());
      else if(set.first == SessionOption::PWD)
      {
        if (get_password())
          EXPECT_EQ(std::string(get_password()), set.second.get<std::string>());
        else
          EXPECT_TRUE(set.second.get<string>().empty());
      }
      else if(set.first == SessionOption::DB)
        EXPECT_EQ(std::string("test"), set.second.get<std::string>());

    }
  }
}

TEST_F(Sess, connection_attributes)
{
  SKIP_IF_NO_XPLUGIN;

  SKIP_IF_SERVER_VERSION_LESS(8, 0, 15)

  const char* sql_attrs = "select ATTR_NAME, ATTR_VALUE, PROCESSLIST_ID from "
                          "performance_schema.session_connect_attrs where PROCESSLIST_ID=";

  auto check_attr = [] (std::list<Row> &attr_res)
  {
    for (auto &row : attr_res)
    {
      std::cout << "(" <<row[2] << ")" << row[0] << ": " << row[1] << std::endl;

      string varname = row[0];

      if (varname == "_client_name")
      {
        EXPECT_EQ(string("mysql-connector-cpp"), row[1].get<string>());
      } else if (varname == "foo")
      {
        EXPECT_EQ(string("bar"), row[1].get<string>());
      } else if(varname == "qux")
      {
        EXPECT_TRUE(row[1].isNull());
      } else if(varname == "baz")
      {
        EXPECT_TRUE(row[1].isNull());
      }

    }
  };

  auto get_pid = []( mysqlx::Session& sess) -> uint64_t
  {
    return sess.sql("SELECT CONNECTION_ID()").execute().fetchOne()[0]
        .get<uint64_t>();
  };

  auto get_attr = [sql_attrs,get_pid] ( mysqlx::Session& sess) -> std::list<Row>
  {
    std::stringstream query;
    query << sql_attrs << get_pid(sess) << ";";
    return sess.sql(query.str()).execute().fetchAll();
  };


  std::stringstream uri_base;
  uri_base << "mysqlx://" << get_user() << "@" << get_host() << ":" << get_port() << "/";

  {
    auto sess = getSession(std::string(uri_base.str())+"?connection-attributes=[foo=bar,qux,baz=]");

    std::list<Row> attr_res = get_attr(sess);

    EXPECT_EQ(10, attr_res.size());

    check_attr(attr_res);
  }

  {
    auto sess = getSession(std::string(uri_base.str())+"?connection-attributes=[]");

    std::list<Row> attr_res = get_attr(sess);

    EXPECT_EQ(7, attr_res.size());

    check_attr(attr_res);
  }

  {
    auto sess = getSession(std::string(uri_base.str())+"?connection-attributes=true");

    std::list<Row> attr_res = get_attr(sess);

    EXPECT_EQ(7, attr_res.size());

    check_attr(attr_res);
  }

  {
    auto sess = getSession(std::string(uri_base.str())+"?connection-attributes=false");

    std::list<Row> attr_res = get_attr(sess);

    EXPECT_EQ(0, attr_res.size());

  }

  {
    auto sess = getSession(std::string(uri_base.str())+"?connection-attributes");

    std::list<Row> attr_res = get_attr(sess);

    EXPECT_EQ(7, attr_res.size());

  }


  {
    try{
      getSession(std::string(uri_base.str())+"?connection-attributes=[foo=bar,_qux,baz=]");
    }
    catch(mysqlx::Error &e)
    {
      EXPECT_EQ(string("Connection attribute names cannot start with \"_\"."),
                e.what());
    }

    try {
      getSession(std::string(uri_base.str())+"?connection-attributes=fail");
      FAIL() << "Error not thrown!";
    } catch (mysqlx::Error &e)
    {
      std::cout << "Expected: " << e << std::endl;
    }

  }

  {
    SessionSettings opt(SessionOption::HOST, get_host(),
                        SessionOption::PORT, get_port(),
                        SessionOption::USER, get_user(),
                        SessionOption::CONNECTION_ATTRIBUTES,
                        DbDoc(R"({ "foo":"bar","qux" : null, "baz":"" })"));

    auto sess = getSession(opt);

    std::list<Row> attr_res = get_attr(sess);

    EXPECT_EQ(10, attr_res.size());

    check_attr(attr_res);
  }

  {
    SessionSettings opt(SessionOption::HOST, get_host(),
                        SessionOption::PORT, get_port(),
                        SessionOption::USER, get_user(),
                        SessionOption::CONNECTION_ATTRIBUTES,
                        R"({ "foo":"bar","qux" : null, "baz":"" })");

    auto sess = getSession(opt);

    std::list<Row> attr_res = get_attr(sess);

    EXPECT_EQ(10, attr_res.size());

    check_attr(attr_res);
  }


  {
    SessionSettings opt(SessionOption::HOST, get_host(),
                        SessionOption::PORT, get_port(),
                        SessionOption::USER, get_user(),
                        SessionOption::CONNECTION_ATTRIBUTES,
                        false);

    auto sess = getSession(opt);

    std::list<Row> attr_res = get_attr(sess);

    EXPECT_EQ(0, attr_res.size());

    opt.set(SessionOption::CONNECTION_ATTRIBUTES, true);

    auto sess2 = getSession(opt);

    std::list<Row> attr_res2 = get_attr(sess2);

    EXPECT_EQ(7, attr_res2.size());

  }

  {
    EXPECT_THROW(SessionSettings(SessionOption::HOST, get_host(),
                                 SessionOption::PORT, get_port(),
                                 SessionOption::USER, get_user(),
                                 SessionOption::CONNECTION_ATTRIBUTES,
                                 R"({ "foo":"bar", "qux": 1, baz:"" })"),
                 mysqlx::Error);
  }

  {
    try{
      SessionSettings(SessionOption::HOST, get_host(),
                      SessionOption::PORT, get_port(),
                      SessionOption::USER, get_user(),
                      SessionOption::CONNECTION_ATTRIBUTES,
                      R"({ "foo":"bar", "_qux":null, baz:"" })");
    }
    catch(mysqlx::Error &e)
    {
      EXPECT_EQ(string("Connection attribute names cannot start with \"_\"."),
                e.what());
    }
  }

  // Pool with reset
  {
    SessionSettings opt(SessionOption::HOST, get_host(),
                        SessionOption::PORT, get_port(),
                        SessionOption::USER, get_user(),
                        SessionOption::CONNECTION_ATTRIBUTES,
                        false);

    auto client = getClient(opt);


    {
      auto sess = client.getSession();

      std::list<Row> attr_res = get_attr(sess);

      EXPECT_EQ(0, attr_res.size());

      opt.set(SessionOption::CONNECTION_ATTRIBUTES, true);

      auto sess2 = getSession(opt);

      std::list<Row> attr_res2 = get_attr(sess2);

      EXPECT_EQ(7, attr_res2.size());
    }

    {
      auto sess = client.getSession();

      std::list<Row> attr_res = get_attr(sess);

      EXPECT_EQ(0, attr_res.size());

      opt.set(SessionOption::CONNECTION_ATTRIBUTES, true);

      auto sess2 = getSession(opt);

      std::list<Row> attr_res2 = get_attr(sess2);

      EXPECT_EQ(7, attr_res2.size());
    }

  }


}

