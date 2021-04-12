/*
 * Copyright (c) 2015, 2020, Oracle and/or its affiliates.
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

#include "test.h"
#include <iostream>
#include <future>
#include <chrono>
#include <thread>

using std::cout;
using std::endl;
using namespace mysqlx;


class Sess : public mysqlx::test::DevAPI
{
public:

  static
  std::string get_var(mysqlx::Session& sess, std::string var)
  {
    std::stringstream query;
    query << "SHOW STATUS LIKE '" << var << "'";

    SqlResult res = sess.sql(query.str()).execute();
    auto row = res.fetchOne();
    return (std::string)row[1];
  }

  std::string get_var(std::string var)
  {
    return get_var(get_sess(), var);
  }

  static
    std::string check_var(mysqlx::Session& sess, std::string var)
  {
    std::string val = get_var(sess, var);
    cout << var << ": " << val << endl;
    return val;
  }

  std::string check_var(std::string var)
  {
    return check_var(get_sess(), var);
  }

  SessionSettings get_opt() const
  {
    return {
        SessionOption::HOST, get_host(),
        SessionOption::PORT, get_port(),
        SessionOption::USER, get_user(),
        SessionOption::PWD, get_password() ? get_password() : nullptr,
    };
  }

  /*
    Returns OpenSSL name of the cipher if supported,
    otherwise an empty string.
  */

  std::string check_cipher(std::string name, const char *tls_ver = nullptr)
  {
    try {

      SessionSettings opt = get_opt();

      opt.set(
        SessionOption::SSL_MODE, SSLMode::REQUIRED,
        SessionOption::TLS_CIPHERSUITES, name
      );

      if (tls_ver)
        opt.set(SessionOption::TLS_VERSIONS, tls_ver);

      mysqlx::Session sess(opt);

      return get_var(sess, "Mysqlx_ssl_cipher");
    }
    catch (const Error&)
    {
      return {};
    }
  }

  bool check_tls_ver(const char* tls_ver)
  {
    try {

      SessionSettings opt = get_opt();

      opt.set(
        SessionOption::SSL_MODE, SSLMode::REQUIRED,
        SessionOption::TLS_VERSIONS, tls_ver
      );

      mysqlx::Session sess(opt);

      return true;
    }
    catch (const Error&)
    {
      return false;
    }
  }

};


void check_compress(mysqlx::Session &sess)
{
  {
    std::string query = "SELECT '";
    for (int i = 0; i < 5000; ++i)
      query.append("Test ");

    query.append("' as test_text UNION SELECT '");

    for (int i = 0; i < 5000; ++i)
      query.append("0123 ");

    query.append("'");

    SqlResult res = sess.sql(query).execute();

    auto test_row = [](const std::string &data, const std::string &row)
    {
      size_t len = data.length();
      for (int i = 0; i < 5000; ++i)
        if(row.substr(i*len,len) != data)
          FAIL() << "Data differs at position" << 5*len;
    };

    Row row;

    EXPECT_TRUE(row = res.fetchOne());
    test_row("Test ", row[0].get<std::string>());

    EXPECT_TRUE(row = res.fetchOne());
    test_row("0123 ", row[0].get<std::string>());

    cout << "Data is correct" << endl;
  }

  {
    SqlResult res = sess.sql("SHOW STATUS LIKE 'Mysqlx_bytes%compress%'").execute();
    Row row;
    int actual_row_count = 0;
    while((row = res.fetchOne()))
    {
      cout << row[0] << " : " << row[1] << endl;
      ++actual_row_count;
      EXPECT_TRUE(std::stol((std::string)row[1], nullptr, 0) > 0);
    }
    cout << "Status rows fetched: " << actual_row_count << endl;
    EXPECT_TRUE(actual_row_count > 0);
  }
};


TEST_F(Sess, compression)
{
  SKIP_IF_NO_XPLUGIN

    SessionSettings mysqldefault_set(
        SessionOption::USER, get_user(),
        SessionOption::PWD, get_password(),
        SessionOption::HOST, get_host(),
        SessionOption::PORT, get_port(),
        SessionOption::COMPRESSION, CompressionMode::PREFERRED
    );

    std::stringstream uri;
    uri << "mysqlx://" << get_user();
    if (get_password() && *get_password())
      uri << ":" << get_password();
    uri << "@" << get_host() << ":" << get_port();
    uri << "/?compression=PREFERRED";

  try {
    mysqlx::Session s(mysqldefault_set);
    check_compress(s);


    mysqlx::Session s2(uri.str());
    check_compress(s2);

  }

  catch (Error e) {
    FAIL() << "Unexpected throw!" << e;
  }
}


TEST_F(Sess, tls_ciphers_prio)
{
  SKIP_IF_NO_XPLUGIN;
  SKIP_IF_SERVER_VERSION_LESS(8, 0, 14);

  const char* a1 = "TLS_DHE_RSA_WITH_AES_128_CBC_SHA256";
  const char* d1 = "TLS_RSA_WITH_AES_256_CBC_SHA";

  std::string name = check_cipher(a1,"TLSv1.2");

  if (name.empty())
  {
    cout << "Skipping because required cipher does not work: " << a1 << endl;
    return;
  }

  // Note: Lower priority cipher is first

  std::list<string> ciphers = { d1, a1 };

  SessionSettings opt = get_opt();
  opt.set(
    SessionOption::SSL_MODE, SSLMode::REQUIRED,
    SessionOption::TLS_VERSIONS, "TLSv1.2",
    SessionOption::TLS_CIPHERSUITES, ciphers
  );

  mysqlx::Session sess(opt);

  // We expect that higher priority cipher is selected.

  EXPECT_EQ(name, get_var(sess, "Mysqlx_ssl_cipher"));
}


TEST_F(Sess, tls_ver_ciphers)
{
  SKIP_IF_NO_XPLUGIN;
  //USE_NATIVE_PWD;
  SKIP_IF_SERVER_VERSION_LESS(8, 0, 14)

  std::set<std::string> versions = {"TLSv1.1" ,"TLSv1.2"};
  std::map<std::string, std::string> suites_map = {
    { "DHE-RSA-AES128-GCM-SHA256", "TLS_DHE_RSA_WITH_AES_128_GCM_SHA256"},
    { "DES-CBC3-SHA", "TLS_RSA_WITH_3DES_EDE_CBC_SHA" }
  };

  std::string versions_str;
  std::string suites_str;
  std::vector<std::string> suites;

  for (auto ver : versions)
  {
    if (!versions_str.empty())
      versions_str.append(",");
    versions_str.append(ver);
  }

  for (auto c : suites_map)
  {
    if (!suites_str.empty())
      suites_str.append(",");
    suites_str.append(c.second);
    suites.push_back(c.second);
  }


  {
    cout << "TLS VERSIONS AND CIPHERS URL TEST" << endl;

    std::stringstream str;
    str << get_uri() << "/?tls-versions=[" << versions_str << "]&"
      << "tls-ciphersuites=[" << suites_str << "]";

    mysqlx::Session sess(str.str());

    EXPECT_TRUE(0 < versions.count(check_var(sess, "Mysqlx_ssl_version")));
    EXPECT_NO_THROW((void)suites_map.at(check_var(sess, "Mysqlx_ssl_cipher")));


    // Negative: invalid or not accepted ciphers

    // No valid ciphers
    EXPECT_THROW(
      mysqlx::Session sess(
        get_uri() + "/?tls-ciphersuites=["
        "foo,TLS_DHE_RSA_WITH_DES_CBC_SHA"
        "]"
      ),
      Error
    );

    // Some ciphers invalid, but some are OK
    EXPECT_NO_THROW(
      mysqlx::Session sess(
        get_uri() + "/?tls-versions=[TLSv1.1,TLSv1.2]"
        "&tls-ciphersuites=["
          "foo,TLS_DHE_RSA_WITH_DES_CBC_SHA,"
          "TLS_DHE_RSA_WITH_AES_128_GCM_SHA256,TLS_RSA_WITH_3DES_EDE_CBC_SHA"
        "]"
      );
    );


    // Negative: wrong version name

    EXPECT_THROW(
      mysqlx::Session sess(get_uri() + "/?tls-versions=[SSLv1]"), Error
    );

    EXPECT_THROW(
      mysqlx::Session sess(get_uri() + "/?tls-versions=[foo]"), Error
    );

    // Negative: no valid version given

    EXPECT_THROW(
      mysqlx::Session sess(get_uri() + "/?tls-versions=[]"), Error
    );

    if (!check_tls_ver("TLSv1.3"))
    {
      EXPECT_THROW(
        mysqlx::Session sess(get_uri() + "/?tls-versions=[TLSv1.3]"), Error
      );
    }

    // Negative: option defined twice

    EXPECT_THROW(
      mysqlx::Session sess(get_uri() + "&tls-versions=[TLSv1.3]"), Error
    );

    EXPECT_THROW(
      mysqlx::Session sess(
        get_uri() + "&tls-ciphersuites=[TLS_RSA_WITH_IDEA_CBC_SHA]"
      ),
      Error
    );

    EXPECT_THROW(
      mysqlx::Session sess(
        get_uri() + "/?tls-versions=[]&tls-versions=[TLSv1.3]"
      ),
      Error
    );

    EXPECT_THROW(
      mysqlx::Session sess(
        get_uri() + "/?tls-ciphersuites=[]"
        + "&tls-ciphersuites=[TLS_RSA_WITH_IDEA_CBC_SHA]"
      ),
      Error
    );
  }


  {
    cout << "TLS VERSIONS AND CIPHERS SETTINGS TEST" << endl;

    SessionSettings opt0 = get_opt();
    opt0.set(SessionOption::SSL_MODE, SSLMode::REQUIRED);

    {
      SessionSettings opt = opt0;
      opt.set(
        SessionOption::TLS_VERSIONS, versions,
        SessionOption::TLS_CIPHERSUITES, suites
      );

      mysqlx::Session sess(opt);

      EXPECT_TRUE(0 < versions.count(check_var(sess, "Mysqlx_ssl_version")));
      EXPECT_NO_THROW((void)suites_map.at(check_var(sess, "Mysqlx_ssl_cipher")));
    }

    // Negative: invalid or not accepted ciphers

    {
      // No valid ciphers

      SessionSettings opt = opt0;
      opt.set(
        SessionOption::TLS_VERSIONS, "TLSv1.1,TLSv1.2",
        SessionOption::TLS_CIPHERSUITES,
        std::list<string>{ "foo", "TLS_DHE_RSA_WITH_DES_CBC_SHA"}
      );

      EXPECT_THROW(mysqlx::Session sess(opt), Error);

      opt.erase(SessionOption::TLS_CIPHERSUITES);
      opt.set(
        SessionOption::TLS_CIPHERSUITES, "foo, TLS_DHE_RSA_WITH_DES_CBC_SHA"
      );

      EXPECT_THROW(mysqlx::Session sess(opt), Error);
    }

    {
      // Some ciphers invalid, but some are OK

      SessionSettings opt = opt0;
      opt.set(
        SessionOption::TLS_VERSIONS, "TLSv1.1,TLSv1.2",
        SessionOption::TLS_CIPHERSUITES,
        std::list<string>{
        "foo", "TLS_DHE_RSA_WITH_DES_CBC_SHA",
          "TLS_DHE_RSA_WITH_AES_128_GCM_SHA256",
          "TLS_RSA_WITH_3DES_EDE_CBC_SHA"
        }
      );

      EXPECT_NO_THROW(mysqlx::Session sess(opt));

      opt.erase(SessionOption::TLS_CIPHERSUITES);
      opt.set(
        SessionOption::TLS_CIPHERSUITES,
        "foo, TLS_DHE_RSA_WITH_DES_CBC_SHA"
        ",TLS_DHE_RSA_WITH_AES_128_GCM_SHA256"
        ",TLS_RSA_WITH_3DES_EDE_CBC_SHA"
      );

      EXPECT_NO_THROW(mysqlx::Session sess(opt));
    }


    // Negative: wrong version name

    {
      SessionSettings opt = opt0;

      opt.erase(SessionOption::TLS_VERSIONS);
      opt.set(SessionOption::TLS_VERSIONS, "SSLv1");
      EXPECT_THROW(mysqlx::Session sess(opt), Error);

      opt.erase(SessionOption::TLS_VERSIONS);
      opt.set(SessionOption::TLS_VERSIONS, "foo");
      EXPECT_THROW(mysqlx::Session sess(opt), Error);
    }


    // Negative: no valid version given

    {
      SessionSettings opt = opt0;

      opt.set(SessionOption::TLS_VERSIONS, std::vector<string>());
      EXPECT_THROW(mysqlx::Session sess(opt), Error);

      opt.erase(SessionOption::TLS_VERSIONS);
      opt.set(SessionOption::TLS_VERSIONS, "");
      EXPECT_THROW(mysqlx::Session sess(opt), Error);

      opt.erase(SessionOption::TLS_VERSIONS);
      opt.set(SessionOption::TLS_VERSIONS, string());
      EXPECT_THROW(mysqlx::Session sess(opt), Error);
    }

    if (!check_tls_ver("TLSv1.3"))
    {
      SessionSettings opt = opt0;

      /*
        It is OK to select TLSv1.3 inside TLS_VERSIONS, but connection will
        fail because this version is not supported.
      */

      EXPECT_NO_THROW(opt.set(SessionOption::TLS_VERSIONS, "TLSv1.3"));
      EXPECT_THROW(mysqlx::Session sess(opt), Error);
    }

    // Negative: option defined twice

    {
      SessionSettings opt = opt0;

      EXPECT_THROW(
        opt.set(
          SessionOption::TLS_VERSIONS, "one",
          SessionOption::TLS_VERSIONS, "two"
        ),
        Error
      );

      EXPECT_THROW(
        opt.set(
          SessionOption::TLS_CIPHERSUITES, "one",
          SessionOption::TLS_CIPHERSUITES, "two"
        ),
        Error
      );
    }

  }

}


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

  if (get_user())
  {
    user = get_user();
    if (get_password())
      user = user + ":" + get_password();
    user = user + "@";
  }

  std::string authority = get_host();
  if (get_port())
    authority = authority + ":" + std::to_string(get_port());

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
    mysqlx::Session s(get_host(), get_port(), get_user(), get_password(), "test");

    EXPECT_EQ(string("test"), s.getDefaultSchema().getName());
    EXPECT_EQ(string("test"), s.getDefaultSchemaName());
    SqlResult res = s.sql("SELECT DATABASE()").execute();
    string db = res.fetchOne()[0];
    EXPECT_EQ(string("test"), db);
  }

  {
    std::string url = get_uri() + "/test";
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
  std::string uri = get_uri();

  EXPECT_NO_THROW(
    SessionSettings settings(uri + "?connect-timeout=10")
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
    SessionSettings settings(uri + "?connect-timeout=-10"),
    Error
  );

  EXPECT_THROW(
    SessionSettings settings(SessionOption::CONNECT_TIMEOUT,-10),
    Error
  );

  EXPECT_THROW(
    SessionSettings settings(uri + "?connect-timeout=10.5"),
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

  SessionSettings common_opts(
    SessionOption::HOST, get_host(),
    SessionOption::PORT, get_port(),
    SessionOption::USER, get_user(),
    SessionOption::PWD, get_password() ? get_password() : nullptr
  );


  {
    SessionSettings opts = common_opts;
    opts.set(
      SessionOption::SSL_MODE, SSLMode::DISABLED,
      SessionOption::AUTH, AuthMethod::MYSQL41
    );
    mysqlx::Session sess(opts);
    check_user(sess);
  }

  {
    SessionSettings opts = common_opts;
    opts.set(
      SessionOption::SSL_MODE, SSLMode::DISABLED,
      SessionOption::AUTH, AuthMethod::PLAIN
    );
    // This will throw because of plain auth without SSL
    EXPECT_THROW(mysqlx::Session sess(opts), Error);
  }

  {
    // BAD PASSWORD
    SessionSettings opts = common_opts;
    opts.set(
      SessionOption::PWD, "notworkingpassword",
      SessionOption::AUTH, AuthMethod::MYSQL41
    );
    EXPECT_THROW(mysqlx::Session sess(opts), Error);
  }

  {
    SessionSettings opts = common_opts;
    opts.set(
      SessionOption::SSL_MODE, SSLMode::REQUIRED,
      SessionOption::AUTH, AuthMethod::PLAIN
    );
    mysqlx::Session sess(opts);
    check_user(sess);
  }

  {
    SessionSettings opts = common_opts;
    opts.set(
      SessionOption::SSL_MODE, SSLMode::REQUIRED,
      SessionOption::AUTH, AuthMethod::MYSQL41
    );
    mysqlx::Session sess(opts);
    check_user(sess);
  }


  std::stringstream uri;
  uri << get_uri();

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
  EXPECT_THROW(mysqlx::Session sess(
    SessionOption::HOST, get_host(),
    SessionOption::PORT, get_port(),
    SessionOption::USER, get_user(),
    SessionOption::PWD, get_password() ? get_password() : nullptr,
    SessionOption::SSL_MODE, SSLMode::DISABLED,
    SessionOption::AUTH, AuthMethod::PLAIN
  ), Error);

  std::stringstream uri;
  uri << get_uri();
  uri << "/?ssl-mode=required&auth=external";

  EXPECT_THROW(mysqlx::Session sess(uri.str()), Error);
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

  SessionSettings common_opts(
    SessionOption::HOST, get_host(),
    SessionOption::PORT, get_port(),
    SessionOption::USER, get_user(),
    SessionOption::PWD, get_password() ? get_password() : nullptr
  );

  {
    SessionSettings opts = common_opts;
    mysqlx::Session sess(opts);
    check_ssl(sess, true);
  }

  {
    SessionSettings opts = common_opts;
    opts.set(
      SessionOption::SSL_MODE, SSLMode::DISABLED
    );
    mysqlx::Session sess(opts);
    check_ssl(sess, false);
  }

  //Using URI

  std::stringstream uri;
  uri << get_uri();

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
    SessionSettings opts = common_opts;
    opts.set(
      SessionOption::SSL_MODE, SSLMode::VERIFY_CA,
      SessionOption::SSL_CA, "unknown"
    );
    EXPECT_THROW(mysqlx::Session sess(opts), Error);
  }


  //with ssl-ca and SSLMode < VERI FY_CA
  {
    SessionSettings opts = common_opts;
    EXPECT_THROW(opts.set(
      SessionOption::SSL_MODE, SSLMode::REQUIRED,
      SessionOption::SSL_CA, "unknown"
    ), Error);
  }

  //using wrong ssl-ca and ssl-ca-path on URI
  {
    std::stringstream bad_uri;
    bad_uri << uri.str() << "/?Ssl-modE=VErify_Ca&sSl-Ca=" << "unknown.file";

    EXPECT_THROW(mysqlx::Session sess(bad_uri.str()), mysqlx::Error);
  }

  //without ssl-ca as SessionSettings
  {
    SessionSettings opts = common_opts;
    opts.set(
      SessionOption::SSL_MODE, SSLMode::VERIFY_CA
    );
    EXPECT_THROW(mysqlx::Session sess(opts), Error);
  }

  string ssl_ca = get_ca();
  std::cout << "ssl-ca:" << ssl_ca << std::endl;

  uri << "/?ssl-ca=" << ssl_ca;

  // using ssl-mode and ssl-ca as SessionSettings

  {
    SessionSettings opts = common_opts;
    opts.set(
      SessionOption::SSL_MODE, SSLMode::VERIFY_CA,
      SessionOption::SSL_CA, ssl_ca
    );
    mysqlx::Session sess(opts);
    check_ssl(sess, true);
  }

  //using ssl-ca as SessionSettings

  {
    string bad_uri;

    bad_uri = uri.str() + "&ssl-mode=DISABLED";
    EXPECT_THROW(mysqlx::Session sess(bad_uri) , mysqlx::Error);

    bad_uri = uri.str() + "&ssl-mode=REQUIRED";
    EXPECT_THROW(mysqlx::Session sess(bad_uri) , mysqlx::Error);

    SessionSettings opts = common_opts;

    EXPECT_THROW(opts.set(
      SessionOption::SSL_MODE, SSLMode::DISABLED,
      SessionOption::SSL_CA, ssl_ca
    ), Error);

    EXPECT_THROW(opts.set(
      SessionOption::SSL_MODE, SSLMode::REQUIRED,
      SessionOption::SSL_CA, ssl_ca
    ), Error);
  }

  //using ssl-ca but with the wrong CA

  {
    /*
      Becaue we do not give valid CA setting, session creation should fail
      when verifying server certificate.
    */

    SessionSettings opts = common_opts;

    opts.set(SessionOption::SSL_CA, "wrong_ca.pem");

    EXPECT_THROW(mysqlx::Session sess(opts), Error);
  }

  //using ssl-mode=VERIFY_IDENTITY and ssl-ca as SessionSettings

  {

    try {
      mysqlx::Session sess(SessionOption::HOST, "127.0.0.1",
                           SessionOption::PORT, get_port(),
                           SessionOption::PRIORITY, 1,
                           SessionOption::HOST, get_host(),
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
    sess.set(SessionOption::HOST, get_host());

    EXPECT_THROW(sess.set(SessionOption::PORT, 13000), Error);
    EXPECT_THROW(sess.set(SessionOption::PRIORITY, 1), Error);
    EXPECT_THROW(sess.set(SessionOption::HOST, get_host(),
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
    std::string uri = get_uri();

    try {
      mysqlx::Session(uri + "?ssl-mode=disabled&ssl-mode=verify_ca");
      FAIL() << "No error thrown";
    }
    catch (Error &e)
    {
      cout << "Expected error: " << e << endl;
      EXPECT_EQ(string("Option SSL_MODE defined twice"),string(e.what()));
    }

    try {
      mysqlx::Session(uri + "?ssl-ca=unknown&ssl-ca=hereItIs");
      FAIL() << "No error thrown";
    }
    catch (Error &e)
    {
      cout << "Expected error: " << e << endl;
      EXPECT_EQ(string("Option SSL_CA defined twice"),string(e.what()));
    }

    try {
      mysqlx::Session(uri + "?ssl-mode=Whatever");
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

  std::string host = get_host();

  if (host != "localhost" && host != "127.0.0.1")
  {
    std::cerr << "SKIPPED: server not on localhost" << std::endl;
    return;
  }

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

    uri << "[(address=" << get_host() << ":" << get_port() <<",priority=0)";
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
           "localhost6:65535,"
           "wont_work,"
           "[::1]:65535,";
    uri << get_host();
    if (get_port() != 0)
      uri << ":" <<get_port();

    uri << "]/test";

    cout << "Connecting using URI: " << uri.str() << endl;

    mysqlx::Session s(uri.str());

    cout << "Connected." << endl;

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
    uri << "(address=" << get_host();
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
                      SessionOption::HOST, get_host(),
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
                 SessionOption::HOST, get_host(),
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
                           SessionOption::HOST, get_host(),
                           SessionOption::PORT, get_port(),
                           SessionOption::PRIORITY, 101),
          Error);


    std::stringstream uri;

    uri << "[(address=" << get_host() << ":" << get_port() <<",priority=101)]";

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
                           SessionOption::HOST, get_host(),
                           SessionOption::PRIORITY, 1);

  EXPECT_EQ(settings.find(SessionOption::SOCKET).get<string>(), string(get_socket()));

  EXPECT_EQ(settings.find(SessionOption::HOST).get<string>(), get_host());

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
                  SessionOption::HOST, get_host(),
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
        SessionOption::HOST, get_host(),
        SessionOption::PORT, get_port());

  SessionSettings mysqldefault_ssl(
        SessionOption::USER, "doomuserdefault",
        SessionOption::PWD, "!doomuserdefault_pass",
        SessionOption::HOST, get_host(),
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
    mysqlx::Session(
      SessionOption::SSL_MODE, SSLMode::DISABLED,
      SessionOption::USER, "noone",
      SessionOption::PWD, "!no_pass",
      SessionOption::HOST, get_host(),
      SessionOption::PORT, get_port()
    );
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
          SessionOption::HOST, get_host(),
          SessionOption::PORT, get_port());

    SessionSettings default_cleartext(
          SessionOption::SSL_MODE, SSLMode::DISABLED,
          SessionOption::USER, "doomuser",
          SessionOption::PWD, "!sha2user_pass",
          SessionOption::HOST, get_host(),
          SessionOption::PORT, get_port());

    //First authentication... should fail!
    EXPECT_THROW( mysqlx::Session s_sha256(sha_256_cleartext), Error);

    //Auth using normal logic
    SessionSettings default_opt(
          SessionOption::USER, "doomuser",
          SessionOption::PWD, "!sha2user_pass",
          SessionOption::HOST, get_host(),
          SessionOption::PORT, get_port());
    mysqlx::Session s_plain(default_opt);

    //Second authentication... should work!
    mysqlx::Session s_sha256_works(sha_256_cleartext);

    mysqlx::Session default_works(default_cleartext);

    SessionSettings default_cleartext_fail(
                            SessionOption::SSL_MODE, SSLMode::DISABLED,
                            SessionOption::USER, "doomuser",
                            SessionOption::PWD, "!sha2user_pass_fail",
                            SessionOption::HOST, get_host(),
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
    SessionSettings settings(
      SessionOption::HOST, get_host(),
      SessionOption::PORT, get_port(),
      SessionOption::USER,get_user(),
      SessionOption::PWD, get_password() ? get_password() : nullptr
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

  EXPECT_THROW(mysqlx::Session(
    SessionOption::SSL_CA, "Bad",
    SessionOption::SSL_MODE, SSLMode::VERIFY_CA,
    SessionOption::HOST, get_host(),
    SessionOption::PORT, get_port(),
    SessionOption::USER,get_user(),
    SessionOption::PWD, get_password() ? get_password() : nullptr
  ), Error);

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
  uri << get_uri() <<"/test";

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

  EXPECT_THROW(ClientSettings(get_uri(),
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
    mysqlx::Client client(
      SessionOption::HOST, get_host(),
      SessionOption::PORT, get_port(),
      SessionOption::USER, get_user(),
      SessionOption::PWD, get_password() ? get_password() : nullptr,
      ClientOption::POOLING, true,
      ClientOption::POOL_MAX_SIZE, 10,
      ClientOption::POOL_QUEUE_TIMEOUT, 1000,
      ClientOption::POOL_MAX_IDLE_TIME, 10000
    );
    mysqlx::Session s = client.getSession();
  }

  //Using just options (ClientOptions and SessionOptions) but starting with
  //ClientOption
  {
    mysqlx::Client client(
      ClientOption::POOLING, true,
      SessionOption::HOST, get_host(),
      SessionOption::PORT, get_port(),
      SessionOption::USER, get_user(),
      SessionOption::PWD, get_password() ? get_password() : nullptr,
      ClientOption::POOL_MAX_SIZE, 10,
      ClientOption::POOL_QUEUE_TIMEOUT, 1000,
      ClientOption::POOL_MAX_IDLE_TIME, 10000
    );
    mysqlx::Session s = client.getSession();
  }

  //Using URI + ClientOptions
  {
    mysqlx::Client client(
      uri.str(),
      ClientOption::POOL_MAX_SIZE, 10,
      ClientOption::POOL_QUEUE_TIMEOUT, 1000,
      ClientOption::POOL_MAX_IDLE_TIME, 10000
    );
    mysqlx::Session s = client.getSession();
  }

  /*
     Global Functions: getSession() and getCLient
  */

  mysqlx::getClient(
    uri.str(),
    ClientOption::POOL_MAX_SIZE, 10,
    ClientOption::POOL_QUEUE_TIMEOUT, 1000,
    ClientOption::POOL_MAX_IDLE_TIME, 10000
  ).getSession();

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
                          SessionOption::HOST, get_host(),
                          SessionOption::PORT, get_port(),
                          SessionOption::PRIORITY, 100,
                          SessionOption::HOST, get_host(),
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
                  SessionOption::HOST, get_host(),
                  SessionOption::PORT, get_port(),
                  SessionOption::PRIORITY, 100,
                  SessionOption::HOST, get_host(),
                  SessionOption::PORT, get_port(),
                  SessionOption::PRIORITY, 1,
                  SessionOption::USER, get_user(),
                  SessionOption::PWD, get_password(),
                  SessionOption::DB, "test");
    auto res = sess.sql("Select 1").execute();
    EXPECT_EQ(1, res.fetchOne()[0].get<int>());
  }

  // Corner ccase of 1 slot in the pool

  {
    settings.set(ClientOption::POOL_MAX_SIZE, 1);

    mysqlx::Client cli(settings);
    mysqlx::Session s1 = cli.getSession();
    s1.close();
    mysqlx::Session s2 = cli.getSession();
  }

  // Using many clients

  {
    settings.set(ClientOption::POOL_MAX_SIZE, 10);
    std::vector<mysqlx::Session> session_list;
    for (int i = 0; i < 5; ++i)
    {
      mysqlx::Client cli(settings);
      for (int j = 0; j < 10; ++j)
      {
        session_list.emplace_back(cli.getSession());
      }
    }
  }

}


TEST_F(Sess, pool_ttl)
{
  SKIP_IF_NO_XPLUGIN;

  sql("set global mysqlx_wait_timeout=28800");

  const int max_connections = 10;

  std::chrono::seconds queue_timeout(50);
  std::chrono::milliseconds pool_ttl(500);

  get_sess().createSchema("pool_ttl", true);

  std::mutex print_mtx;

  auto mk_session = [&print_mtx](mysqlx::Client &cli) -> mysqlx::Session* {
    try {
      return new mysqlx::Session(cli);
    }
    catch (const Error &e)
    {
      std::lock_guard<std::mutex> g(print_mtx);
      std::cout << "-- error while creating session: " << e << std::endl;
      std::flush(std::cout);
      return nullptr;
    }
    catch (...)
    {
      return nullptr;
    }
  };

  std::list<std::future<mysqlx::Session*>> session_list;

  // Wait for sessions in session_list and test if they work.
  // If expect_errors is true, then expect that all sessions will be
  // invalid.

  auto test_sessions = [&session_list, max_connections, &print_mtx](bool expect_errors)
  {
    int errors_found = 0;
    while (session_list.size() > 0)
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

            if (s)
            {
              SqlResult res = s->sql("SELECT 1").execute();
              EXPECT_EQ(1, res.fetchOne()[0].get<int>());
            }
            else
              errors_found++;

          } catch (Error &e) {
            std::lock_guard<std::mutex> g(print_mtx);
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
      EXPECT_EQ(4 * max_connections, errors_found);
    }
    else
    {
      EXPECT_EQ(0, errors_found);
    }
  };


  ClientSettings settings(ClientOption::POOLING, true,
                          SessionOption::AUTH, AuthMethod::SHA256_MEMORY,
                          SessionOption::SSL_MODE, SSLMode::DISABLED,
                          ClientOption::POOL_MAX_SIZE, max_connections,
                          ClientOption::POOL_QUEUE_TIMEOUT, queue_timeout,
                          ClientOption::POOL_MAX_IDLE_TIME, pool_ttl,
                          SessionOption::HOST, get_host(),
                          SessionOption::PORT, get_port(),
                          SessionOption::PRIORITY, 100,
                          SessionOption::HOST, get_host(),
                          SessionOption::PORT, get_port(),
                          SessionOption::PRIORITY, 1,
                          SessionOption::USER, get_user(),
                          SessionOption::PWD, get_password(),
                          SessionOption::DB, "pool_ttl");

  // threaded example
  {
    std::cout << "Threaded (short max idle time)" << std::endl;

    //short POOL_MAX_IDLE_TIME so that sessions expire.
    settings.set(ClientOption::POOL_QUEUE_TIMEOUT, 100000,
                 ClientOption::POOL_MAX_IDLE_TIME, 1);

    settings.set(ClientOption::POOL_QUEUE_TIMEOUT, std::chrono::seconds(100),
                 ClientOption::POOL_MAX_IDLE_TIME, std::chrono::microseconds(1));

    mysqlx::Client client(settings);

    std::cout << "- creating sessions... " << std::endl;

    for (int i=0; i < max_connections*4; ++i)
    {
      // note: compiler chokes on async(..., mk_session, client);
      session_list.emplace_back(
        std::async(std::launch::async, [&client, mk_session]() {
          return mk_session(client);
        })
      );
    }

    std::cout << "- test sessions... " << std::endl;

    test_sessions(false);

    // Now closing pool so that waiting threads get session without timeout
    std::cout << "- close client... " << std::endl;

    client.close();

    std::cout << "- done." << std::endl;
  }

  {
    std::cout << "Threaded (long max idle time)" << std::endl;

    ClientSettings settings1 = settings;

    settings1.set(ClientOption::POOL_QUEUE_TIMEOUT, std::chrono::hours(1),
                 ClientOption::POOL_MAX_IDLE_TIME, std::chrono::hours(1));

    mysqlx::Client client = mysqlx::getClient(settings1);

    std::cout << "- creating sessions... " << std::endl;

    session_list.clear();

    for (int i=0; i < max_connections*4; ++i)
    {
      session_list.emplace_back(
        std::async(std::launch::async, [&client,mk_session](){
          return mk_session(client);
        })
      );
    }

    std::cout << "- close client... " << std::endl;

    client.close();

    std::cout << "- test sessions... " << std::endl;

    test_sessions(true);

    std::cout << "- done." << std::endl;
  }

  {
    std::cout << "Not threaded" << std::endl;

    settings.set(ClientOption::POOL_QUEUE_TIMEOUT, std::chrono::seconds(100),
      ClientOption::POOL_MAX_IDLE_TIME, std::chrono::seconds(10));

    mysqlx::Client client(settings);

    /*
      Open as many sessions as there are slots in the session pool, and then
      close them so that they return to the pool. Returns ids of the created
      sessions.
    */

    auto get_sessions = [&client, max_connections]() -> std::set<unsigned>
    {
      std::list<mysqlx::Session> sessions;
      std::set<unsigned>      sess_ids;

      for (int i = 0; i < max_connections; ++i)
      {
        sessions.emplace_back(client);
        auto row = sessions.back().sql("SELECT CONNECTION_ID()").execute().fetchOne();
        sess_ids.insert(row[0]);
      }

      return std::move(sess_ids);
    };

    auto ids = get_sessions();
    EXPECT_EQ(max_connections, ids.size());

    /*
      Now we have pool full of sessions, and none of them has expired yet.
      When we request sessions again, we should get sessions from the pool,
      no new connections.
    */

    std::cout << "Get sessions" << std::endl;

    auto ids1 = get_sessions();
    EXPECT_EQ(max_connections, ids1.size());

    // Check that all connection ids are from the original set

    for (unsigned id : ids1)
    {
      EXPECT_TRUE(ids.count(id));
    }

    std::cout << "Wait 15s to timeout sessions" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(12));

    /*
      Now the idle timeout has expired, so sessions in the pool shoul
      not be used but new sessions should be created.
    */

    std::cout << "Get sessions" << std::endl;

    auto ids2 = get_sessions();
    EXPECT_EQ(max_connections, ids2.size());

    for (unsigned id : ids2)
    {
      EXPECT_FALSE(ids1.count(id));
    }
  }

}

/*
  Check the block-list logic of the pool.
  For this test to run the following env variables have to be set:
    EP_HOST=<IP address of the host with multiple MySQL instances>
    EP_PORT_1,2,3=<Port number>

  All MySQL instances must run on the same host.
*/
TEST_F(Sess, pool_block_list)
{
  SKIP_IF_NO_XPLUGIN;

  const int EP_NUMBER = 3;
  int   ep_port[EP_NUMBER];
  std::vector<mysqlx::Session> ctrl_session;

  const char* ep_addr = getenv("EP_HOST");
  if (ep_addr == nullptr)
  {
    cout << "Error! EP_HOST is not set. " << endl <<
            "This test expects an environment variable EP_HOST" <<
            " to be set to the host address where MySQL Server" <<
            " instances are running.";
    return;
  }

  for (int i = 0; i < EP_NUMBER; ++i)
  {
    std::string port_name = "EP_PORT_";
    const char *ep_cport = std::getenv(
        port_name.append(std::to_string(i + 1)).c_str());

    if (ep_cport == nullptr)
    {
      return;
    }
    ep_port[i] = std::atoi(ep_cport);

    /*
      Create control non-pooled sessions for each endpoint.
      They will be used to kill pooled sessions.
    */
    ctrl_session.push_back(mysqlx::Session(
                           SessionOption::HOST, ep_addr,
                           SessionOption::PORT, ep_port[i],
                           SessionOption::USER, get_user(),
                           SessionOption::PWD, get_password(),
                           SessionOption::DB, "test"));
  }


  struct MySession : public mysqlx::Session
  {
    int m_id = 0;
    int m_port = 0;

    MySession(mysqlx::Client &client) :
      mysqlx::Session(client)
    {
      auto res = sql("SELECT @@mysqlx_port, CONNECTION_ID()").execute();
      auto row = res.fetchOne();
      m_port = row[0].get<int>();
      m_id = row[1].get<int>();
      cout << "[Port:" << m_port << ",ID:" << m_id << "]";
    }
  };

  typedef std::list<MySession> SessionList;

  struct test_helper
  {
    static void create_sessions(mysqlx::Client &client,
                                SessionList &sessions,
                                int con_number)
    {
      cout << endl << "Creating " << con_number << " sessions:";
      for (int i = 0; i < con_number; ++i)
      {
        if (i % 5 == 0)
          cout << endl;

        sessions.emplace_back(client);
      }
      cout << endl;
    }

    static int count_sessions(SessionList& sessions, int port)
    {
      int cnt = 0;
      for (MySession &s : sessions)
      {
        if (s.m_port == port)
          ++cnt;
      }
      cout << "Port " << port << " : " << cnt << " sessions" << endl;
      return cnt;
    }

    static std::set<int>
    pool_close_all_on_port(SessionList& sessions, int port)
    {
      std::set<int> res;
      cout << endl << "Returning sessions to the pool:" << endl;

      auto checker = [&res, port](const MySession & s) {
        if (s.m_port == port)
        {
          res.emplace(s.m_id);
          cout << "[Port:" << port << ",ID:" << s.m_id << "]";
          if (res.size() % 5 == 0)
            cout << endl;
          return true;
        }
        return false;

      };

      sessions.remove_if(checker);
      cout << endl;

      return res;
    }

    static int close_and_kill(SessionList &sessions,
                                int kill_port,
                                mysqlx::Session &ctrl_sess,
                                bool do_close = true)
    {
      for (auto it = sessions.begin(); it != sessions.end(); ++it)
      {
        auto &s = *it;
        if (s.m_port == kill_port)
        {
          int id = s.m_id;
          if (do_close)
            s.close(); // Return connection to the pool

          cout << "Kill connection " << id << " on port " <<
            kill_port << endl;

          ctrl_sess.sql("KILL CONNECTION ?").bind(id).execute();

          if (do_close)
            sessions.erase(it);

          return id;
        }
      }
      return -1;
    }
  };

  using namespace std::chrono;
  time_point<system_clock> deadline;

  #define time_error_message "This part of the test is time critical " \
                             "and it cannot continue because it is " \
                             "taking too long (over 50 seconds)"

  {
    ClientSettings settings(ClientOption::POOLING, true,
      ClientOption::POOL_MAX_SIZE, 100,
      ClientOption::POOL_QUEUE_TIMEOUT, 1000000,
      ClientOption::POOL_MAX_IDLE_TIME, 1000000,
      SessionOption::HOST, ep_addr,
      SessionOption::PORT, ep_port[0],
      SessionOption::HOST, ep_addr,
      SessionOption::PORT, ep_port[1],
      SessionOption::HOST, ep_addr,
      SessionOption::PORT, ep_port[2],
      SessionOption::USER, get_user(),
      SessionOption::PWD, get_password(),
      SessionOption::DB, "test");

    const int con_number = 30;
    mysqlx::Client client(settings);
    SessionList m_sessions;

    /*
      Test #1
      Uniform session distribution

      Client is configured with 3 endpoints.
      We create 30 sessions and expect them to be
      distributed among the 3 endpoints.
    */

    cout << endl << "Test #1" << endl;

    /*
      Random distribution of end-points
    */
    test_helper::create_sessions(client, m_sessions, con_number);

    {
      int ep_count[3] = { // Number of connections to each end-point
        test_helper::count_sessions(m_sessions, ep_port[0]),
        test_helper::count_sessions(m_sessions, ep_port[1]),
        test_helper::count_sessions(m_sessions, ep_port[2]) };

      /*
        Check the random distribution of the end-points.
        The number of connections to each end-point should be 2 or more
      */
      EXPECT_TRUE(ep_count[0] >= 2);
      EXPECT_TRUE(ep_count[1] >= 2);
      EXPECT_TRUE(ep_count[2] >= 2);
      EXPECT_EQ(con_number, ep_count[0] + ep_count[1] + ep_count[2]);
    }

    /*
      Test #2
      Block-list connection endpoint
    */

    cout << endl << "Test #2" << endl;
    deadline = system_clock::now() + seconds(static_cast<int64_t>(50));
    /*
      Kill one connection to EP1, which is in the pool, so
      when it is requested the pool would get an error and
      add the end-point to a block list.
    */
    int kill_index = 0;
    test_helper::close_and_kill(m_sessions,
                                ep_port[kill_index],
                                ctrl_session[kill_index]);

    /*
      The pool only has one connection to be re-used, but it is made
      invalid. So, when it is requested the pool should
      add the end-point to a block list.
    */
    cout << "Try session: ";
    m_sessions.emplace_back(client);
    cout << endl;

    // A new connection must be made to another endpoint (EP2 or EP3)
    EXPECT_TRUE(m_sessions.back().m_port != ep_port[kill_index]);

    /*
      Test #3
      Block-list and session re-use
    */
    cout << endl << "Test #3" << endl;

    int ep_count[3] = { // Number of connections to each end-point
      test_helper::count_sessions(m_sessions, ep_port[0]),
      test_helper::count_sessions(m_sessions, ep_port[1]),
      test_helper::count_sessions(m_sessions, ep_port[2]) };
    // Return to the pool remaining connections to EP3
    auto pooled_id_list =
      test_helper::pool_close_all_on_port(m_sessions, ep_port[0]);

    auto pooled_id_list_2 =
      test_helper::pool_close_all_on_port(m_sessions, ep_port[1]);

    auto pooled_id_list_3 =
      test_helper::pool_close_all_on_port(m_sessions, ep_port[2]);

    EXPECT_EQ(0, m_sessions.size());

    int N2_N3_count = ep_count[1] + ep_count[2];

    if (deadline < system_clock::now())
      FAIL() << time_error_message;

    SessionList m_sessions3;
    test_helper::create_sessions(client, m_sessions3, N2_N3_count);
    {
      int ep_count3[3] = { // Number of connections to each end-point
        test_helper::count_sessions(m_sessions3, ep_port[0]),
        test_helper::count_sessions(m_sessions3, ep_port[1]),
        test_helper::count_sessions(m_sessions3, ep_port[2]) };

      /*
        Since EP corresponding to kill_index is on the block-list (see Test#2),
        none of the opened sessions should connect to that endpoint.
      */
      EXPECT_EQ(0, ep_count3[kill_index]);
      EXPECT_TRUE(ep_count3[1] >= 2);
      EXPECT_TRUE(ep_count3[2] >= 2);
      EXPECT_EQ(N2_N3_count, ep_count3[1] + ep_count3[2]);
    }

    /*
      Check that all new sessions are not from the block list and that
      they are re-used (IDs are from the lists of previously used sessions).
      We opened as many sessions (N2_N3_count) as were currently available
      in the pool, so no new connection should be made.
    */
    EXPECT_TRUE(std::all_of(m_sessions3.begin(), m_sessions3.end(),
      [&ep_port, kill_index, &pooled_id_list_2, &pooled_id_list_3]
      (MySession& elem)
      {
        return elem.m_port != ep_port[kill_index] &&
          (pooled_id_list_2.find(elem.m_id) != pooled_id_list_2.end() ||
            pooled_id_list_3.find(elem.m_id) != pooled_id_list_3.end());
      }));

    /*
      Test #4
      Block list and new sessions
    */
    cout << endl << "Test #4" << endl;

    if (deadline < system_clock::now())
      FAIL() << time_error_message;

    SessionList m_sessions4;
    test_helper::create_sessions(client, m_sessions4, ep_count[0]);
    {
      int ep_count4[3] = { // Number of connections to each end-point
        test_helper::count_sessions(m_sessions4, ep_port[0]),
        test_helper::count_sessions(m_sessions4, ep_port[1]),
        test_helper::count_sessions(m_sessions4, ep_port[2]) };

      /*
        Endpoint EP1 is still on the block - list, so none of
        the new sessions should connect to it.
      */
      EXPECT_EQ(0, ep_count4[0]);
      EXPECT_TRUE(ep_count4[1] >= 2);
      EXPECT_TRUE(ep_count4[2] >= 2);
      EXPECT_EQ(ep_count[0], ep_count4[1] + ep_count4[2]);

      /*
        Make sure that all created sessions are new ones and
        none of them re-use any existing combinations of ID/Port#
      */
      for (MySession& s : m_sessions4)
      {
        EXPECT_FALSE(std::any_of(m_sessions3.begin(), m_sessions3.end(),
          [&s](MySession& elem)
          {
            return elem.m_id == s.m_id && elem.m_port == s.m_port;
          })
        );
      }
    }

    /*
      Test #5
      Block list expiration (re-use)
    */
    cout << endl << "Test #5" << endl;
    cout << endl << "Wait 60 sec ..." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(61));

    SessionList m_sessions5;
    // At this point the block-listed pooled sessions should become available
    test_helper::create_sessions(client, m_sessions5, ep_count[kill_index]);
    {
      int ep_count5[3] = { // Number of connections to each end-point
        test_helper::count_sessions(m_sessions5, ep_port[0]),
        test_helper::count_sessions(m_sessions5, ep_port[1]),
        test_helper::count_sessions(m_sessions5, ep_port[2]) };

      /*
        All new sessions should be taken from the pool and all available
        sessions in the pool connect to EP1, thus none of the new sessions
        should connect to EP2 or EP3.
      */
      EXPECT_EQ(ep_count[0], ep_count5[0]);
      EXPECT_EQ(0, ep_count5[1]);
      EXPECT_EQ(0, ep_count5[2]);
    }

    // Check that all EP1 sessions are reused
    EXPECT_TRUE(std::all_of(m_sessions5.begin(), m_sessions5.end(),
      [&ep_port, kill_index, &pooled_id_list]
    (MySession& elem)
      {
        return elem.m_port == ep_port[kill_index] &&
          pooled_id_list.find(elem.m_id) != pooled_id_list.end();
      }));

    m_sessions4.clear();
    m_sessions5.clear();


    /*
      Test #6
      Block list expiration (new sessions)
    */
    cout << endl << "Test #6" << endl;
    EXPECT_EQ(0, m_sessions.size());

    test_helper::create_sessions(client, m_sessions, con_number);

    int ep_count6[3] = { // Number of connections to each end-point
      test_helper::count_sessions(m_sessions, ep_port[0]),
      test_helper::count_sessions(m_sessions, ep_port[1]),
      test_helper::count_sessions(m_sessions, ep_port[2]) };

    /*
      Check the random distribution of the end-points after
      block-list timeout has expired.
      The number of connections to each end-point should be 2 or more
    */
    EXPECT_TRUE(ep_count6[0] >= 2);
    EXPECT_TRUE(ep_count6[1] >= 2);
    EXPECT_TRUE(ep_count6[2] >= 2);
    EXPECT_EQ(con_number, ep_count6[0] + ep_count6[1] + ep_count6[2]);

    /*
      Test #7
      Block list with 2 endpoints
    */
    cout << endl << "Test #7" << endl;
    deadline = system_clock::now() + seconds(static_cast<int64_t>(50));

    kill_index = 0;
    test_helper::close_and_kill(m_sessions, ep_port[kill_index],
                                ctrl_session[kill_index]);

    /*
      The pool only has one connection to be re-used, but it is made
      invalid. So, when it is requested the pool should
      add the end-point to a block list.
    */
    cout << "Try session: ";
    m_sessions.emplace_back(client);
    cout << endl;

    // A new connection must be made to another endpoint (EP2 or EP3)
    EXPECT_TRUE(m_sessions.back().m_port != ep_port[kill_index]);

    cout << endl << "Wait 30 sec ..." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(30));

    kill_index = 1;
    test_helper::close_and_kill(m_sessions, ep_port[kill_index],
      ctrl_session[kill_index]);

    if (deadline < system_clock::now())
      FAIL() << time_error_message;

    // The deadline has to be set again because now EP2 gets into block list
    deadline = system_clock::now() + seconds(static_cast<int64_t>(50));

    cout << "Try session: ";
    m_sessions.emplace_back(client);
    cout << endl;

    /*
      A new connection must be made to EP3 only because now
      both EP1 and EP2 are on the block-list
    */
    EXPECT_TRUE(m_sessions.back().m_port == ep_port[2]);

    // Re-count number of connections to each end-point
    ep_count[0] = test_helper::count_sessions(m_sessions, ep_port[0]);
    ep_count[1] = test_helper::count_sessions(m_sessions, ep_port[1]);
    ep_count[2] = test_helper::count_sessions(m_sessions, ep_port[2]);

    std::set<int> pooled_list[3];

    // Close all sessions so that they are returned to the pool.
    for (int i = 0; i < 3; ++i)
      pooled_list[i] =
        test_helper::pool_close_all_on_port(m_sessions, ep_port[i]);

    EXPECT_EQ(0, m_sessions.size());

    cout << endl << "Wait 30 sec ..." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(30));

    /*
      Since we waited for 30s two times, the blocklist timeout for EP1
      expired and sessions connected to EP1 can be used again.
      The pool has num(EP1) + num(EP3) available sessions now - we get
      all of them.
    */
    test_helper::create_sessions(client, m_sessions,
                                 ep_count[0] + ep_count[2]);

    if (deadline < system_clock::now())
      FAIL() << time_error_message;

    /*
      EP2 is still on the block-list and no sessions given by the pool should
      connect to it.
    */
    EXPECT_EQ(ep_count[0], test_helper::count_sessions(m_sessions, ep_port[0]));
    EXPECT_EQ(0,           test_helper::count_sessions(m_sessions, ep_port[1]));
    EXPECT_EQ(ep_count[2], test_helper::count_sessions(m_sessions, ep_port[2]));

    // Make sure sessions are re-used
    EXPECT_TRUE(std::all_of(m_sessions.begin(), m_sessions.end(),
      [&ep_port, &pooled_list]
    (MySession& elem)
      {
        return elem.m_port != ep_port[1] &&
          (pooled_list[0].find(elem.m_id) != pooled_list[0].end() ||
           pooled_list[2].find(elem.m_id) != pooled_list[2].end());
      }));

    /*
      Wait another 30s so that blocklist timeout expires also for EP2.
      After that the block-list is empty and new session requests should
      be handled as usual.
    */
    cout << endl << "Wait 30 sec ..." << endl;
    std::this_thread::sleep_for(std::chrono::seconds(30));

    m_sessions.clear();

    test_helper::create_sessions(client, m_sessions,
      ep_count[0] + ep_count[1] + ep_count[2]);

    // All endpoints are present
    EXPECT_EQ(ep_count[0], test_helper::count_sessions(m_sessions, ep_port[0]));
    EXPECT_EQ(ep_count[1], test_helper::count_sessions(m_sessions, ep_port[1]));
    EXPECT_EQ(ep_count[2], test_helper::count_sessions(m_sessions, ep_port[2]));

    // Make sure all sessions are re-used
    EXPECT_TRUE(std::all_of(m_sessions.begin(), m_sessions.end(),
      [&ep_port, &pooled_list]
    (MySession& elem)
      {
        return pooled_list[0].find(elem.m_id) != pooled_list[0].end() ||
               pooled_list[1].find(elem.m_id) != pooled_list[1].end() ||
               pooled_list[2].find(elem.m_id) != pooled_list[2].end();
      }));

  }

  {
    ClientSettings settings(ClientOption::POOLING, true,
      ClientOption::POOL_MAX_SIZE, 3,
      ClientOption::POOL_QUEUE_TIMEOUT, 100000,
      ClientOption::POOL_MAX_IDLE_TIME, 100000,
      SessionOption::HOST, ep_addr,
      SessionOption::PORT, ep_port[0],
      SessionOption::USER, get_user(),
      SessionOption::PWD, get_password(),
      SessionOption::DB, "test");

    const int con_number = 3;
    mysqlx::Client client(settings);
    SessionList m_sessions;

    /*
      Test #8
      try block-listed endpoint on session request
    */

    cout << endl << "Test #8" << endl;

    /*
      Open 3 sessions to fill up session pool. Then close and kill one
      session and request a new session so that the end-point will be
      put on the block-list.
    */
    test_helper::create_sessions(client, m_sessions, con_number);
    int killed_id = test_helper::close_and_kill(m_sessions, ep_port[0],
      ctrl_session[0]);

    cout << "Try session: ";
    m_sessions.emplace_back(client);
    cout << endl;

    // A new connection must be made
    EXPECT_TRUE(m_sessions.back().m_id != killed_id);

    auto pooled_list =
      test_helper::pool_close_all_on_port(m_sessions, ep_port[0]);

    EXPECT_EQ(0, m_sessions.size());

    /*
      Since there is no room in the pool for new connections, the
      pool logic should try to re-use existing sessions even though EP1
      is on the block-list.
    */
    test_helper::create_sessions(client, m_sessions, con_number);

    EXPECT_EQ(3, m_sessions.size());

    EXPECT_TRUE(std::all_of(m_sessions.begin(), m_sessions.end(),
                [&pooled_list](MySession &elem)
                {
                  return pooled_list.find(elem.m_id) != pooled_list.end();
                }
                ));

  }

  try
  {
    try
    {
      ctrl_session[0].sql("DROP USER `trypooluser`@`%`").execute();
    }
    catch(...) {}

    ctrl_session[0].sql("CREATE USER `trypooluser`@`%` IDENTIFIED BY "
                        "'trypoolpass456'").execute();

    ClientSettings settings(ClientOption::POOLING, true,
      ClientOption::POOL_MAX_SIZE, 3,
      ClientOption::POOL_QUEUE_TIMEOUT, 100000,
      ClientOption::POOL_MAX_IDLE_TIME, 100000,
      SessionOption::HOST, ep_addr,
      SessionOption::PORT, ep_port[0],
      SessionOption::USER, "trypooluser",
      SessionOption::PWD, "trypoolpass456");

    const int con_number = 3;
    mysqlx::Client client(settings);
    SessionList m_sessions;

    /*
      Test #9
      No timeouts because of block listed endpoints
    */

    cout << endl << "Test #9" << endl;

    /*
      Open 3 sessions to fill up session pool. Then close and kill one
      session and request a new session so that the end-point will be
      put on the block-list.
    */
    test_helper::create_sessions(client, m_sessions, con_number);
    int killed_id = test_helper::close_and_kill(m_sessions, ep_port[0],
      ctrl_session[0]);

    /*
      EP1 will be put into the block list.
      A new session is created.
    */
    cout << "Try session: ";
    m_sessions.emplace_back(client);
    cout << endl;

    // A new connection must be made
    EXPECT_TRUE(m_sessions.back().m_id != killed_id);

    /*
      At this point EP1 should be on block-list and session pool should
      contain 3 session (its max capacity), all in use.

      Return 2 sessions to the pool and kill all of them.
    */

    test_helper::close_and_kill(m_sessions, ep_port[0], ctrl_session[0]);
    test_helper::close_and_kill(m_sessions, ep_port[0], ctrl_session[0]);

    // This session will be killed, but remains in use
    test_helper::close_and_kill(m_sessions, ep_port[0], ctrl_session[0],
                                false);

    EXPECT_EQ(1, m_sessions.size());

    // Drop the user used for the sessions to prevent successful connects
    ctrl_session[0].sql("DROP USER `trypooluser`@`%`").execute();
    ctrl_session[0].sql("FLUSH PRIVILEGES").execute();

    /*
      At this point it is not possible to either reset a session in the pool
      or create a new session for `trypooluser`@`%`.
    */

    /*
      POOL_QUEUE_TIMEOUT is 100 seconds, we should expect the error
      should not be the timeout and therefore it should be thrown faster
    */
    deadline = system_clock::now() + seconds(static_cast<int64_t>(100));
    try
    {
      cout << "Try session: ";
      m_sessions.emplace_back(client);
    }
    catch(Error &err)
    {
      cout << endl << "Expected error: " << err.what() << endl;
    }

    /*
      Check how long it took to get to this line. We must not hit
      POOL_QUEUE_TIMEOUT.
    */
    if(deadline <= system_clock::now())
      FAIL() << "POOL_QUEUE_TIMEOUT is hit";

    /*
      There should be still one session in use. No sessions should be
      re-used and no new sessions should be established.
    */
    EXPECT_EQ(1, m_sessions.size());

  }
  catch(...) {};

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

TEST_F(Sess, dns_srv)
{

  //ERRORS MODE

  //Specifying a port number with DNS SRV lookup is not allowed.

  try
  {
    auto cli = getClient("mysqlx+srv://root@_mysqlx._tcp.localhost:33060");
    FAIL() << "Error expected if port is specified for mysqlx+srv settings";
  }
  catch(Error &e)
  {
    std::cout << "Expected: " << e.what() << std::endl;
    SUCCEED() << "Expected: " << e.what() << std::endl;
  }

  try
  {
    auto cli = getClient(SessionOption::HOST, "_mysqlx._tcp.localhost",
                         SessionOption::PORT, 33060,
                         SessionOption::DNS_SRV, true,
                         SessionOption::USER, "root");
    FAIL() << "Error expected if PORT specified together with DNS_SRV";
  }
  catch(Error &e)
  {
    std::cout << "Expected: " << e.what() << std::endl;
    SUCCEED() << "Expected: " << e.what() << std::endl;
  }

  //Using Unix domain sockets with DNS SRV lookup is not allowed.

#ifndef _WIN32
  try {
    auto cli = getClient("mysqlx+srv://root@(/_mysqlx/_tcp/localhost)");
    FAIL();
  }
  catch(Error &e)
  {
    std::cout << "Expected: " << e.what() << std::endl;
    SUCCEED() << "Expected: " << e.what() << std::endl;
  }

  try {
    auto cli = getClient(SessionOption::SOCKET, "/_mysqlx/_tcp/localhost",
                         SessionOption::DNS_SRV, true,
                         SessionOption::USER, "root");
    FAIL();
  }
  catch(Error &e)
  {
    std::cout << "Expected: " << e.what() << std::endl;
    SUCCEED() << "Expected: " << e.what() << std::endl;
  }
#endif

  //Specifying multiple hostnames with DNS SRV look up is not allowed.

  try
  {
    auto cli = getClient("mysqlx+srv://root@[_mysqlx._tcp.localhost,_mysqlx._tcp.host2]");
    FAIL();
  }
  catch(Error &e)
  {
    std::cout << "Expected: " << e.what() << std::endl;
    SUCCEED() << "Expected: " << e.what() << std::endl;
  }


  try
  {
    auto cli = getClient(SessionOption::HOST, "_mysqlx._tcp._notfound.localhost",
                         SessionOption::HOST, "_mysqlx._tcp._notfound.localhost",
                         SessionOption::DNS_SRV, true,
                         SessionOption::USER, "root");
    FAIL();
  }
  catch(Error &e)
  {
    std::cout << "Expected: " << e.what() << std::endl;
    SUCCEED() << "Expected: " << e.what() << std::endl;
  }

  //Scheme {scheme} is not valid.

  try
  {
    auto cli = getClient("mysqlx+foo://root@_mysqlx._tcp.localhost");
    FAIL();
  }
  catch(Error &e)
  {
    std::cout << "Expected: " << e.what() << std::endl;
    SUCCEED() << "Expected: " << e.what() << std::endl;
  }

  //Unable to locate any hosts for {hostname}

  try
  {
    auto cli = getClient("mysqlx+srv://root@_mysqlx._tcp._notfound.localhost");
    cli.getSession();
    FAIL();
  }
  catch(Error &e)
  {
    std::cout << "Expected: " << e.what() << std::endl;
    SUCCEED() << "Expected: " << e.what() << std::endl;
  }

  try
  {
    auto cli = getClient(SessionOption::HOST, "_mysqlx._tcp._notfound.localhost",
                         SessionOption::DNS_SRV, true,
                         SessionOption::USER, "root");
    cli.getSession();
    FAIL();
  }
  catch(Error &e)
  {
    std::cout << "Expected: " << e.what() << std::endl;
    SUCCEED() << "Expected: " << e.what() << std::endl;
  }


  //WORKING MODE

  SKIP_IF_NO_XPLUGIN;
  SKIP_IF_NO_SRV_SERVICE;


  try {

    std::stringstream uri;

    uri << "mysqlx+srv://" << get_user();
    if(get_password())
      uri << ":" << get_password();
    uri << "@" << get_srv();

    auto client = getClient(uri.str());

    std::list<mysqlx::Session> session_list;
    for(int i=0; i < 10; ++i)
    {
      session_list.emplace_back(client);
    }

    session_list.emplace_back(mysqlx::Session(uri.str()));

    SessionSettings ss(SessionOption::HOST, get_srv(),
                       SessionOption::DNS_SRV, true,
                       SessionOption::USER, get_user(),
                       SessionOption::PWD, get_password());

    session_list.emplace_back(mysqlx::Session(ss));

  } catch (Error &e)
  {
    std::cout << e << std::endl;
    FAIL() << e.what();
  }
}

TEST_F(Sess, compression_algorithms)
{
  SKIP_IF_NO_XPLUGIN;

  /*
    Note: this captured to use Sess::get_var(). Even though it is static
    member, compiler (msvc) generated warning without the capture.
  */

  auto check_compress_alg = [this](mysqlx::Session s,string expected, int line)
  {
    std::string result = Sess::get_var(s, "Mysqlx_compression_algorithm");
    if (expected != result)
    {
      std::cout << line << ": " << std::endl;
      std::cout << "\tExpected: " << expected << std::endl;
      std::cout << "\tResult: " << result << std::endl;
    }
    EXPECT_EQ(expected, result);

    if (!result.empty())
    {
      check_compress(s);
    }

  };


  std::string uri(get_uri());

  struct test_data
  {
    std::string expected;
    std::string alias;
    std::string name;
    std::string second;
    std::string third;
  };

  std::vector<test_data> algs = {
    {"DEFLATE_STREAM", "deFlate","DeFlaTe_StreAM", "ZsTd", "Lz4"},
    {"LZ4_MESSAGE", "lZ4","Lz4_MeSSaGe","DeFlAte","zstd"},
    {"ZSTD_STREAM", "ZsTd","zStD_sTReaM","lz4","DEFLATE"},
  };

  //Reading the value of mysqlx_compression_algorithms at the beginning
  SqlResult res = sql("SHOW GLOBAL VARIABLES LIKE 'mysqlx_compression_algorithms';");
  Row row = res.fetchOne();
  std::string Val = row[1].get<std::string>();

  //By Default, it should use ZTSD compression

  check_compress_alg(mysqlx::Session(uri),"ZSTD_STREAM", __LINE__);

  check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                     SessionOption::PORT, get_port(),
                                     SessionOption::USER, get_user(),
                                     SessionOption::PWD, get_password())
                                     ,"ZSTD_STREAM", __LINE__);

  //Add parameters
  uri += "/?";

  for(const test_data& d : algs)
  {
    // Used algorithm should be d.expected on all of this cases
    //First round, pool clean
    std::chrono::time_point<std::chrono::system_clock> start_time
        = std::chrono::system_clock::now();

    check_compress_alg(
          mysqlx::Session(
            uri + "compression-algorithms=" + d.name
            ), d.expected, __LINE__);

    check_compress_alg(
          mysqlx::Session(
            uri + "compression-algorithms=" + d.alias
            ), d.expected, __LINE__);

    check_compress_alg(
          mysqlx::Session(
            uri + "compression-algorithms=[" + d.name + "]"
            ), d.expected, __LINE__);


    check_compress_alg(
          mysqlx::Session(
            uri + "compression-algorithms=[" + d.name + "," + d.second + "," + d.name + "]"
                         ), d.expected, __LINE__);


    check_compress_alg(
          mysqlx::Session(
            uri + "compression-algorithms=[" + d.name + "," + d.second + "," + d.third + "]"
          ), d.expected, __LINE__);

    check_compress_alg(
          mysqlx::Session(
            uri + "compression-algorithms=[" + d.alias + "]"
            ), d.expected, __LINE__);

    check_compress_alg(
          mysqlx::Session(
            uri + "compression-algorithms=[" + d.alias + "," + d.second + "," + d.name + "]"
            ), d.expected, __LINE__);


    check_compress_alg(
          mysqlx::Session(
            uri + "compression-algorithms=[MyALGORITHM," + d.name + "]"
            ), d.expected, __LINE__);

    //Even if a valid algorithm is selected, if compression is disabled,
    //no compression should be used
    check_compress_alg(
          mysqlx::Session(
            uri + "Compression=Disabled&Compression-Algorithms=" + d.name
          ), std::string(), __LINE__);

    check_compress_alg(
          mysqlx::Session(
            uri + "Compression=Disabled&Compression-Algorithms=" + d.alias
            ), std::string(), __LINE__);

    check_compress_alg(
          mysqlx::Session(
            uri + "Compression=Disabled&Compression-Algorithms=[" + d.name + "]"
            ), std::string(), __LINE__);

    check_compress_alg(
          mysqlx::Session(
            uri + "Compression=Disabled&Compression-Algorithms=[" + d.name + "," + d.name + "]"
            ), std::string(), __LINE__);

    check_compress_alg(
          mysqlx::Session(
            uri + "Compression=Disabled&Compression-Algorithms=[" + d.alias + "]"
            ), std::string(), __LINE__);

    check_compress_alg(
          mysqlx::Session(
            uri + "Compression=Disabled&Compression-Algorithms=[" + d.alias + "," + d.name + "]"
            ), std::string(), __LINE__);

    check_compress_alg(
          mysqlx::Session(
            uri + "Compression=Disabled&Compression-Algorithms=[MyALGORITHM," + d.name + "]"
          ), std::string(), __LINE__);

    // In this cases, no algorithm should be used, since algorithm value
    // is empty or not valid
    check_compress_alg(mysqlx::Session(uri + "compression-algorithms="), "", __LINE__);
    check_compress_alg(mysqlx::Session(uri + "compression-algorithms=[]"), "", __LINE__);
    check_compress_alg(mysqlx::Session(uri + "compression-algorithms=BAD_Algorithm"), "", __LINE__);
    check_compress_alg(mysqlx::Session(uri + "compression-algorithms=[BAD_Algorithm]"), "", __LINE__);
    check_compress_alg(mysqlx::Session(uri + "compression-algorithms=[BAD_Algorithm,BAD_Algorithm]"), "", __LINE__);

    //If compression=REQUIRED and no valid algorithm selected, an error should be thrown
    EXPECT_THROW(mysqlx::Session(uri + "compression=required&compression-algorithms="), Error);
    EXPECT_THROW(mysqlx::Session(uri + "compression=required&compression-algorithms=[]"), Error);
    EXPECT_THROW(mysqlx::Session(uri + "compression=required&compression-algorithms=BAD_Algorithm"), Error);
    EXPECT_THROW(mysqlx::Session(uri + "compression=required&compression-algorithms=[BAD_Algorithm]"), Error);
    EXPECT_THROW(mysqlx::Session(uri + "compression=required&compression-algorithms=[BAD_Algorithm,BAD_Algorithm]"), Error);


    //Using text only, used algorithm should be LZ4_MESSAGE
    check_compress_alg(
          mysqlx::Session(
            SessionOption::HOST, get_host(),
            SessionOption::PORT, get_port(),
            SessionOption::USER, get_user(),
            SessionOption::PWD, get_password(),
            SessionOption::COMPRESSION_ALGORITHMS, d.name),d.expected, __LINE__);

    check_compress_alg(
          mysqlx::Session(
            SessionOption::HOST, get_host(),
            SessionOption::PORT, get_port(),
            SessionOption::USER, get_user(),
            SessionOption::PWD, get_password(),
            SessionOption::COMPRESSION_ALGORITHMS, d.alias),d.expected, __LINE__);


    check_compress_alg(
          mysqlx::Session(
            SessionOption::HOST, get_host(),
            SessionOption::PORT, get_port(),
            SessionOption::USER, get_user(),
            SessionOption::PWD, get_password(),
            SessionOption::COMPRESSION_ALGORITHMS,
            std::string(d.name)+"," + d.second + "," + d.third
            ),d.expected, __LINE__);


    check_compress_alg(mysqlx::Session(
                         SessionOption::HOST, get_host(),
                                       SessionOption::PORT, get_port(),
                                       SessionOption::USER, get_user(),
                                       SessionOption::PWD, get_password(),
                                       SessionOption::COMPRESSION_ALGORITHMS,
                                       std::string(d.alias) + "," + d.second + "," + d.third + "," + d.name
                                       ),d.expected, __LINE__);

    //Even if valid algorithms, since compression is disabled
    //it will not be used
    check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                       SessionOption::PORT, get_port(),
                                       SessionOption::USER, get_user(),
                                       SessionOption::PWD, get_password(),
                                       SessionOption::COMPRESSION, CompressionMode::DISABLED,
                                       SessionOption::COMPRESSION_ALGORITHMS, d.name),
                       std::string(), __LINE__);

    check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                       SessionOption::PORT, get_port(),
                                       SessionOption::USER, get_user(),
                                       SessionOption::PWD, get_password(),
                                       SessionOption::COMPRESSION, CompressionMode::DISABLED,
                                       SessionOption::COMPRESSION_ALGORITHMS, d.alias),
                       std::string(), __LINE__);

    check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                       SessionOption::PORT, get_port(),
                                       SessionOption::USER, get_user(),
                                       SessionOption::PWD, get_password(),
                                       SessionOption::COMPRESSION, CompressionMode::DISABLED,
                                       SessionOption::COMPRESSION_ALGORITHMS, std::string(d.name) + "," + d.name),
                       std::string(), __LINE__);

    check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                       SessionOption::PORT, get_port(),
                                       SessionOption::USER, get_user(),
                                       SessionOption::PWD, get_password(),
                                       SessionOption::COMPRESSION, CompressionMode::DISABLED,
                                       SessionOption::COMPRESSION_ALGORITHMS, std::string(d.alias) + "," + d.name),
                       std::string(), __LINE__);

    {
      //Using container
      std::vector<string> algorithms = {d.alias,d.second, d.third, d.name};
      check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                         SessionOption::PORT, get_port(),
                                         SessionOption::USER, get_user(),
                                         SessionOption::PWD, get_password(),
                                         SessionOption::COMPRESSION_ALGORITHMS, algorithms),
                         d.expected, __LINE__);
    }

    {
      //Other container
      std::list<string> algorithms = {"MyALGORITHM",d.name, d.second, d.third, d.alias};
      check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                         SessionOption::PORT, get_port(),
                                         SessionOption::USER, get_user(),
                                         SessionOption::PWD, get_password(),
                                         SessionOption::COMPRESSION_ALGORITHMS, algorithms),
                         d.expected, __LINE__);
    }

    {
      //Other container
      std::list<string> algorithms = {"MyALGORITHM",d.name, d.alias, d.third, d.second};
      check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                         SessionOption::PORT, get_port(),
                                         SessionOption::USER, get_user(),
                                         SessionOption::PWD, get_password(),
                                         SessionOption::COMPRESSION_ALGORITHMS, algorithms),
                         d.expected, __LINE__);
    }


    {
      //Same as above, if compressions disabled, no compression will be used
      std::list<string> algorithms = {"MyALGORITHM",d.name, d.alias, d.third, d.second};
      check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                         SessionOption::PORT, get_port(),
                                         SessionOption::USER, get_user(),
                                         SessionOption::PWD, get_password(),
                                         SessionOption::COMPRESSION, CompressionMode::DISABLED,
                                         SessionOption::COMPRESSION_ALGORITHMS, algorithms),
                         std::string(), __LINE__);
    }



    //No algorithm will be used on these cases

      check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                         SessionOption::PORT, get_port(),
                                         SessionOption::USER, get_user(),
                                         SessionOption::PWD, get_password(),
                                         SessionOption::COMPRESSION_ALGORITHMS, nullptr),
                         std::string(), __LINE__);


    check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                       SessionOption::PORT, get_port(),
                                       SessionOption::USER, get_user(),
                                       SessionOption::PWD, get_password(),
                                       SessionOption::COMPRESSION_ALGORITHMS, ""),
                       std::string(), __LINE__);

    {
      std::vector<string> algorithms;
      check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                         SessionOption::PORT, get_port(),
                                         SessionOption::USER, get_user(),
                                         SessionOption::PWD, get_password(),
                                         SessionOption::COMPRESSION_ALGORITHMS, algorithms),
                         std::string(), __LINE__);
    }

    check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                       SessionOption::PORT, get_port(),
                                       SessionOption::USER, get_user(),
                                       SessionOption::PWD, get_password(),
                                       SessionOption::COMPRESSION_ALGORITHMS, "BAD_Algorithm"),
                       std::string(), __LINE__);

    {
      std::list<string> algorithms = {"Unknown","BAD_Algorithm"};
      check_compress_alg(mysqlx::Session(SessionOption::HOST, get_host(),
                                         SessionOption::PORT, get_port(),
                                         SessionOption::USER, get_user(),
                                         SessionOption::PWD, get_password(),
                                         SessionOption::COMPRESSION_ALGORITHMS, algorithms),
                         std::string(), __LINE__);
    }


    //In these cases, since algorithms are not valid and compression is
    //REQUIRED, an error should be thrown
    EXPECT_THROW(
          mysqlx::Session(SessionOption::HOST, get_host(),
                          SessionOption::PORT, get_port(),
                          SessionOption::USER, get_user(),
                          SessionOption::PWD, get_password(),
                          SessionOption::COMPRESSION, CompressionMode::REQUIRED,
                          SessionOption::COMPRESSION_ALGORITHMS, "BAD_Algorithm"),
          Error);

    {
      std::list<string> algorithms = {"Unknown","BAD_Algorithm"};
      EXPECT_THROW(
            mysqlx::Session(SessionOption::HOST, get_host(),
                            SessionOption::PORT, get_port(),
                            SessionOption::USER, get_user(),
                            SessionOption::PWD, get_password(),
                            SessionOption::COMPRESSION, CompressionMode::REQUIRED,
                            SessionOption::COMPRESSION_ALGORITHMS, algorithms),
            Error);
    }

    //In these cases, algorithm set on the server side is different from the algorithms in connection
    //and compression is REQUIRED, an error should be thrown

    {
      std::string query ="Set global mysqlx_compression_algorithms="+d.expected+";";
      sql(query);
      std::list<string> algorithms = {d.second,d.third};
      EXPECT_THROW(
            mysqlx::Session(SessionOption::HOST, get_host(),
                            SessionOption::PORT, get_port(),
                            SessionOption::USER, get_user(),
                            SessionOption::PWD, get_password(),
                            SessionOption::COMPRESSION, CompressionMode::REQUIRED,
                            SessionOption::COMPRESSION_ALGORITHMS, algorithms),
            Error);
    }

    EXPECT_THROW(mysqlx::Session(uri + "compression=required&compression-algorithms=["+ d.second + "," + d.third +"]"), Error);

    //Restoring to the original value of mysqlx_compression_algorithms
    std::string query ="Set global mysqlx_compression_algorithms='"+Val+"';";
    sql(query);

    std::cout << d.expected << ": " <<
                 std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::system_clock::now() - start_time).count() << "ms" <<std::endl;

  }
}
