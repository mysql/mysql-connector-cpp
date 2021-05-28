/*
 * Copyright (c) 2009, 2020, Oracle and/or its affiliates.
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


#include "bugs.h"
#include <locale>
#include <sstream>
#include <limits>


//Prevent windows min() macro because of std::numeric_limits<int>::min()
#undef min

namespace testsuite
{
namespace regression
{

void bugs::net_write_timeout39878()
{
  logMsg("Test for #39878 - not fixed. And looks like there is no way to fix it at the moment.");
  logMsg("If the test doesn't fail - that rather means that you should tweak parameters for your system");
  logMsg("You can do that with command line parameters:");
  logMsg(" --nwtTimeout=<val> for the value net_write timeout to set to (default 1)");
  logMsg(" --nwtPause=<val> for the length of pause test shoud take while fetching rows (default 2)");
  logMsg(" --nwtRows=<val> for the number of rows to insert into table (default 9230)");

  int timeout = 1;
  int pause = 2;
  int rows = 9230;

  int tmp = TestsRunner::getStartOptions()->getInt("nwtTimeout");
  if (tmp > 0)
    timeout = tmp;


  tmp = TestsRunner::getStartOptions()->getInt("nwtPause");
  if (tmp > 0)
    pause = tmp;

  tmp = TestsRunner::getStartOptions()->getInt("nwtRows");
  if (tmp > 0)
    rows = tmp;


  pstmt.reset(con->prepareStatement("set net_write_timeout=?"));
  pstmt->setInt(1, timeout);
  pstmt->execute();

  res.reset(stmt->executeQuery("show variables like \"net_write_timeout\""));

  res->next();

  TestsListener::messagesLog() << "We've set net_write_timeout to " << res->getString(2) << std::endl;


  stmt->executeUpdate("drop table if exists bug39878");
  stmt->executeUpdate("create table bug39878 (id int unsigned not null)");

  stmt->execute("lock table bug39878 write");

  pstmt.reset(con->prepareStatement("insert into bug39878 ( id ) values( ? )"));

  for (int i = 1; i <= rows; ++i) {
    pstmt->setInt(1, i);
    pstmt->execute();
  }

  stmt->execute("unlock tables");


  res.reset(stmt->executeQuery("select count(*) from bug39878"));

  res->next();

  uint32_t rowsCount = res->getUInt(1);

  TestsListener::messagesLog() << "Now we have " << rowsCount << " rows in the table" << std::endl;

  // Must set ResultSet Type  to TYPE_FORWARD_ONLY
  stmt->setResultSetType(sql::ResultSet::TYPE_FORWARD_ONLY);

  res.reset(stmt->executeQuery("select * from bug39878"));

  TestsListener::messagesLog() << "ResultSetType: " << stmt->getResultSetType() << std::endl;

  uint32_t rowsRead = 0;

  try {
    while (res->next()) {
      if (rowsRead == 0) {
        TestsListener::messagesLog() << "Pause " << pause << "s." << std::endl;
        SLEEP(pause);
      }

      ++rowsRead;
    }
  } catch (sql::SQLException& /*e*/) {
    //if ( e.errNo == 2006 )

    /* let's think that exception is a god result itself.
       also not sure which errno should it be */

    return;
  }

  TestsListener::messagesLog() << "We've fetched " << rowsRead << " rows." << std::endl;

  try {
    stmt->execute("drop table if exists bug39878");
  } catch (sql::SQLException& e) {
    // Expected, that Server has gone away
    logMsg(e.what());

    // Lazy way to reconnect
    setUp();

    stmt->execute("drop table if exists bug39878");
  }

  ASSERT_EQUALS(rowsCount, rowsRead);
}


void bugs::store_result_error_51562()
{
  std::stringstream msg;

  logMsg("Regression test for #51562");
  try {
    stmt.reset(con->createStatement());
    /* Running a SELECT and storing the returned result set in this->res */
    res.reset(stmt->executeQuery("select 1, (select 'def' union all select 'abc')"));

    fail("SQL error not detected or the server has changed its behavior", __FILE__, __LINE__);
  } catch (sql::SQLException &e) {
    /* If anything goes wrong, write some info to the log... */
    logMsg("Expecting error: ERROR 1242 (21000): Subquery returns more than 1 row, got:");
    logMsg(e.what());
    logMsg("SQLState: " + std::string(e.getSQLState()));
    if (e.getErrorCode() != 1242) {
      msg.str("");
      msg << "Expecting MySQL error code 1242, got " << e.getErrorCode() << ".";
      msg << "This may be a compatible and acceptable code - check manually and update test if nedded!";
      logMsg(msg.str());
      fail("Wrong error code, check verbose output for details", __FILE__, __LINE__);
    }
  }

  logMsg("Checking prepared statements");
  try {
    pstmt.reset(con->prepareStatement("select 1, (select 'def' union all select 'abc')"));
    /* Running a SELECT and storing the returned result set in this->res */
    res.reset(pstmt->executeQuery());

    fail("SQL error not detected or the server has changed its behavior", __FILE__, __LINE__);
  } catch (sql::SQLException &e) {
    /* If anything goes wrong, write some info to the log... */
    logMsg("Expecting error: ERROR 1242 (21000): Subquery returns more than 1 row, got:");
    logMsg(e.what());
    logMsg("SQLState: " + std::string(e.getSQLState()));
    if (e.getErrorCode() != 1242) {
      msg.str("");
      msg << "Expecting MySQL error code 1242, got " << e.getErrorCode() << ".";
      msg << "This may be a compatible and acceptable code - check manually and update test if nedded!";
      logMsg(msg.str());
      fail("Wrong error code, check verbose output for details", __FILE__, __LINE__);
    }
  }
}


void bugs::getResultSet_54840()
{
  stmt->executeUpdate("DROP function if exists _getActivePost");
  stmt->executeUpdate("CREATE Function _getActivePost(_author INT) "
                                                "RETURNS INT "
                                                "DETERMINISTIC "
                                                "BEGIN "
                                                "       RETURN 55;"
                                                "END");

  try
  {
    stmt->execute("select _getActivePost()");
    res.reset(stmt->getResultSet());
  }
  catch (::sql::SQLException & /*e*/)
  {
    stmt->executeUpdate("DROP function _getActivePost");
    return; /* Everything is fine */
  }

  stmt->executeUpdate("DROP function _getActivePost");
  FAIL("Exception wasn't thrown by getResultSet");
}

void bugs::supportIssue_52319()
{
  std::stringstream msg;

  unsigned int uiStartTime = 1289837776;
  unsigned int uiProductsID = 20;
  unsigned int uiParSetID = 2;

  logMsg("Test for MySQL support issue 52319");

  stmt->execute("DROP TABLE IF EXISTS products");
  createSchemaObject("TABLE",
                     "products",
                     "(uiProductsIdx int(10) unsigned NOT NULL AUTO_INCREMENT, startTime timestamp NULL DEFAULT NULL, stopTime timestamp NULL DEFAULT NULL, uiProductsID int(10) DEFAULT NULL, uiParameterSetID int(10) unsigned DEFAULT NULL, PRIMARY KEY (uiProductsIdx))");

  stmt->execute("DROP PROCEDURE IF EXISTS insertProduct");
  stmt->execute("CREATE PROCEDURE insertProduct(IN dwStartTimeIN INT UNSIGNED, IN uiProductsIDIN INT UNSIGNED, IN dwParSetIDIN INT UNSIGNED) BEGIN DECLARE stStartTime TIMESTAMP; SET stStartTime = FROM_UNIXTIME(dwStartTimeIN); INSERT INTO `products` (startTime, uiProductsID, uiParameterSetID) VALUES (stStartTime, uiProductsIDIN, dwParSetIDIN); END");

  pstmt.reset(con->prepareStatement("CALL insertProduct(?, ?, ?)"));
  pstmt->setInt(1, uiStartTime);
  pstmt->setInt(2, uiProductsID);
  pstmt->setInt(3, uiParSetID);

  pstmt->execute();
  logMsg("Procedure called, checking products table contents");

  res.reset(stmt->executeQuery("SELECT uiProductsIdx, startTime, stopTime, uiProductsID, uiParameterSetID FROM products"));
  ASSERT(res->next());


  msg.str("");
  msg << "uiProductsIdx     = " << res->getString("uiProductsIdx") << "\n";
  msg << "startTime         = " << res->getString("startTime") << "\n";
  msg << "stopTime          = " << res->getString("stopTime") << "\n";
  msg << "uiPrpductsID      = " << res->getString("uiProductsID") << "\n";
  msg << "uiParameterSetID  = " << res->getString("uiParameterSetID") << "\n";
  logMsg(msg.str());

  /* SKIP - timezone may trick us ASSERT_EQUALS("2010-11-15 17:16:16", res->getString("startTime")); */
  ASSERT_EQUALS("20", res->getString("uiProductsID"));
  ASSERT_EQUALS("2", res->getString("uiParameterSetID"));

  ASSERT(!res->next());
}


/* Bug#15936764/67325 */
void bugs::expired_pwd()
{
  logMsg("bugs::expired_pwd");

  //TODO: Enable it after fixing
  SKIP("Removed untill fixed");

  if (getMySQLVersion(con) < 56006)
  {
    SKIP("The server does not support tested functionality(expired password)");
  }

  stmt->executeUpdate("DROP TABLE IF EXISTS test.ccpp_expired_pwd");

  try
  {
    stmt->executeUpdate("DROP USER ccpp_expired_pwd");
  }
  catch (sql::SQLException &)
  {
    // Catching exception if user did not exist
  }

  stmt->executeUpdate("CREATE USER ccpp_expired_pwd IDENTIFIED BY 'foo'");
  stmt->executeUpdate("GRANT ALL ON test to ccpp_expired_pwd");
  stmt->executeUpdate("ALTER USER ccpp_expired_pwd PASSWORD EXPIRE");

  sql::ConnectOptionsMap opts;

  opts["userName"]=     sql::SQLString("ccpp_expired_pwd");
  opts["password"]=     sql::SQLString("");

  try
  {
    if (getConnection(&opts))
    {
      SKIP("The expired password does not work with anonymous-user accounts.");
    }
  }
  catch (sql::SQLException&)
  {
    /* If no anonymous user present then continue */
  }

  opts["password"]=     sql::SQLString("foo");

  testsuite::Connection c2;

  /* Seeing error first without OPT_CAN_HANDLE_EXPIRED_PASSWORDS ... */
  try
  {
    c2.reset(getConnection(&opts));
  }
  catch (sql::SQLException &e)
  {
    ASSERT_EQUALS(1862, e.getErrorCode()/*ER_MUST_CHANGE_PASSWORD_LOGIN*/);
  }

  /* ... Now with it */
  opts["OPT_CAN_HANDLE_EXPIRED_PASSWORDS"]= true;

  try
  {
    c2.reset(getConnection(&opts));
  }
  catch (sql::SQLException &e)
  {
    /*
      In case of sql::mysql::mydeCL_CANT_HANDLE_EXP_PWD tests fail - means that in
      the setup where test is run the driver does not support expired password
    */
    ASSERT_EQUALS(1820, e.getErrorCode()/*ER_MUST_CHANGE_PASSWORD*/);
  }

  // Now setting new password and getting fully functional connection
  opts["preInit"]= sql::SQLString("set password= password('bar')");

  // Connect should go thru fine
  try
  {
    c2.reset(getConnection(&opts));
  }
  catch(sql::SQLException &e)
  {
    // We can get here in case of old libmysql library
    ASSERT_EQUALS(sql::mysql::deCL_CANT_HANDLE_EXP_PWD, e.getErrorCode());
    // Wrong libmysql - we can't test anything else as we can't get connection
    return;
  }

  // Trying to connect with new pwd
  opts.erase("preInit");
  opts.erase("OPT_CAN_HANDLE_EXPIRED_PASSWORDS");
  opts["password"]= sql::SQLString("bar");
  opts["CLIENT_MULTI_STATEMENTS"]= true;
  opts["postInit"]= sql::SQLString("create table test.ccpp_expired_pwd(i int);"
                                   "insert into test.ccpp_expired_pwd(i) values(2)");

  c2.reset(getConnection(&opts));

  // postInit is introduced anong with preInit - testing it too
  Statement s2(c2->createStatement());

  /* Checking 2 things - that executeUpdate detects a resultset returning stmt, and that
     connection is usable after that */
  try
  {
    s2->executeUpdate("insert into test.ccpp_expired_pwd(i) values(7);select i from test.ccpp_expired_pwd");
    FAIL("Driver had to throw \"Query returning resultset\" exception!");
  }
  catch (sql::SQLException &)
  {
  }

  res.reset(s2->executeQuery("select i from test.ccpp_expired_pwd"));

  ASSERT(res->next());

  ASSERT_EQUALS(2, res->getInt(1));

  c2->close();
  stmt->executeUpdate("DROP TABLE test.ccpp_expired_pwd");
  stmt->executeUpdate("DROP USER ccpp_expired_pwd");
}


/* Bug#16970753/69492
 * Cannot Connect error when using legacy authentication
 */
void bugs::legacy_auth()
{
  logMsg("bugs::legacy_auth");
  try
  {
    stmt->executeUpdate("DROP USER ccpp_legacy_auth");
  }
  catch (sql::SQLException &)
  {
    // Catching exception if user did not exist
  }

  res.reset(stmt->executeQuery("SELECT @@secure_auth"));
  res->next();
  /* We need to know if we need to set it back*/
  int secure_auth= res->getInt(1);

  if (secure_auth)
  {
    try
    {
      stmt->executeUpdate("SET GLOBAL secure_auth= OFF");
    }
    catch(sql::SQLException &)
    {
      SKIP("For this test user should be able to set global variables.");
    }
  }

  try
  {
    stmt->executeUpdate("CREATE USER ccpp_legacy_auth@'%' IDENTIFIED BY 'foo'");
    stmt->executeUpdate("GRANT ALL ON test to ccpp_legacy_auth@'%'");
  }
  catch(sql::SQLException &)
  {
    SKIP("For this test user should have GRANT privelege");
  }

  /* Simplest way to create user identified with old password on all server versions */
  stmt->executeUpdate("UPDATE mysql.user SET Password=old_password('foo')\
                       WHERE User='ccpp_legacy_auth' AND Host='%'");

  try
  {
    stmt->executeUpdate("UPDATE mysql.user SET Plugin=''\
                        WHERE User='ccpp_legacy_auth' AND Host='%'");
  }
  catch(sql::SQLException &)
  {
    /* Doing nothing - catching for case of old server that does not have Plugin field */
  }

  stmt->executeUpdate("FLUSH PRIVILEGES");
  testsuite::Connection c2;

  sql::ConnectOptionsMap opts;

  opts["useLegacyAuth"]=  false;
  opts["userName"]=       sql::SQLString("ccpp_legacy_auth");
  opts["password"]=       sql::SQLString("foo");
  opts["schema"]=         sql::SQLString("test");

  /* First verifying that we can't connect without the option */
  try
  {
    c2.reset(getConnection(&opts));
  }
  catch (sql::SQLException &e)
  {
    ASSERT_EQUALS(2049, e.getErrorCode() /*CR_SECURE_AUTH*/);
  }

  opts["useLegacyAuth"]= true;
  c2.reset(getConnection(&opts));
  /* If this passed - we are fine */

  c2->close();

  /* Returning secure_auth if needed */
  if (secure_auth)
  {
    stmt->executeUpdate("SET GLOBAL secure_auth= ON");
  }

  stmt->executeUpdate("DROP USER ccpp_legacy_auth");
}


/* Bug #18193771/71605 - The driver does not recognize utf8mb4 charset */
void bugs::bug71606()
{
  logMsg("bugs::bug71606");

  if (getMySQLVersion(con) < 56000)
  {
    SKIP("The server does not support tested functionality(utf8mb4 charset)");
  }

  /* We only loop thru default collations - thus the testcase does not cover all
     possible cases */
  res.reset(stmt->executeQuery("SHOW CHARSET"));

  /* Connection is reset after the testcase finishes - thus we can safely "set names" here */
  Statement stmt2(con->createStatement());
  pstmt.reset(con->prepareStatement("SELECT 'a'"));
  ResultSet res2;
  ResultSetMetaData *rsmd2;

  while (res->next())
  {
    String csname(res->getString(1));
    String query("SET NAMES ");

    query.append(res->getString(1));

    try
    {
      stmt2->executeUpdate(query);
    }
    catch(sql::SQLException &)
    {
      /* Assuming this particular charset charset cannot be set as client charset,
         like atm we can't do that with utf16/32 */
      continue;
    }

    res2.reset(pstmt->executeQuery());
    rsmd2= res2->getMetaData();
    rsmd2->isCaseSensitive(1);
  }
}


void bugs::bug72700()
{
  logMsg("bugs::bug72700");
  ASSERT(stmt->execute("select ST_AsText(ST_GeomFromText('point(10 10)'))"));

  try
  {
    res.reset(stmt->getResultSet());
    checkResultSetScrolling(res);
    ResultSetMetaData * meta=res->getMetaData();
    ASSERT_EQUALS(meta->getColumnType(1), 15);
    ASSERT_EQUALS(meta->getColumnTypeName(1), "LONGTEXT");
  }
  catch (::sql::SQLException & /*e*/)
  {
    return; /* Everything is fine */
  }

  stmt->execute("DROP TABLE IF EXISTS bug72700");
  stmt->execute("CREATE TABLE bug72700(valtext longtext)");
  ASSERT(stmt->execute("SELECT valtext FROM bug72700"));
  try
  {
    res.reset(stmt->getResultSet());
    checkResultSetScrolling(res);
    ResultSetMetaData * meta=res->getMetaData();
    ASSERT_EQUALS(meta->getColumnType(1), 15);
    ASSERT_EQUALS(meta->getColumnTypeName(1), "TEXT");
  }
  catch (::sql::SQLException & /*e*/)
  {
    return; /* Everything is fine */
  }

  stmt->execute("DROP TABLE IF EXISTS bug72700");
}


void bugs::bug66871()
{
  sql::Connection *con = NULL;
  sql::Statement *stmt = NULL;
  sql::ResultSet *res = NULL;

  logMsg("bugs::bug66871");
  try
  {
    con = getConnection(NULL);
    stmt = con->createStatement();
    ASSERT(stmt->execute("select 1"));
    res= stmt->getResultSet();
    ASSERT(res->next());
    ASSERT_EQUALS(res->getInt(1), 1);

    con->close();
    delete con;

    ASSERT(stmt->execute("select 2"));
    res= stmt->getResultSet();
    ASSERT(res->next());
    ASSERT_EQUALS(res->getInt(1), 2);

  }
  catch (::sql::SQLException & /*e*/)
  {
    delete res;
    delete stmt;

    return; /* Everything is fine */
  }

  delete res;
  delete stmt;

  FAIL("Exception wasn't thrown by execute");
}

/*
  Segementation fault in row retrieval when defaultstatementresulttype=
  forward_only and row position is after last row
*/
void bugs::bug20085944()
{
  logMsg("bugs::bug20085944");
  try
  {
    sql::ConnectOptionsMap connection_properties;

    connection_properties["hostName"]=url;
    connection_properties["userName"]=user;
    connection_properties["password"]=passwd;
    connection_properties["defaultStatementResultType"]=sql::ResultSet::TYPE_FORWARD_ONLY;

    created_objects.clear();
    con.reset(driver->connect(connection_properties));
    con->setSchema(db);
    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS bug20085944");
    stmt->execute("CREATE TABLE bug20085944(id INT)");
    stmt->execute("INSERT INTO bug20085944 VALUES(1),(2),(3)");
    res.reset(stmt->executeQuery("SELECT * FROM bug20085944"));

    for (int i=1; i <= 3; ++i) {
      ASSERT(res->next());
      ASSERT_EQUALS(i, res->getInt(1));
    }

    ASSERT(!res->next());
    res->getInt(1);
  }
  catch (::sql::SQLException & /*e*/)
  {
    return; /* Everything is fine */
  }

  FAIL("Exception wasn't thrown by getInt() in bug20085944()");
}


void bugs::bug19938873_pstmt()
{
  logMsg("bugs::bug19938873_pstmt");
  try
  {
    pstmt.reset(con->prepareStatement("SELECT NULL"));
    res.reset(pstmt->executeQuery());
    ASSERT(res->next());
    res->wasNull();
  }
  catch (sql::SQLException & /*e*/)
  {
    return; /* Everything is fine */
  }

  FAIL("Exception wasn't thrown by wasNull()");
}


void bugs::bug19938873_stmt()
{
  logMsg("bugs::bug19938873_stmt");
  try
  {
    stmt.reset(con->createStatement());
    res.reset(stmt->executeQuery("SELECT NULL"));
    ASSERT(res->next());
    res->wasNull();
  }
  catch (sql::SQLException & /*e*/)
  {
    return; /* Everything is fine */
  }

  FAIL("Exception wasn't thrown by wasNull()");
}


void bugs::bug68523()
{
  try
  {
    stmt->execute("DROP TABLE IF EXISTS bug68523");
    stmt->execute("CREATE TABLE bug68523(ts TIMESTAMP(6))");
    stmt->execute("INSERT INTO bug68523(ts) values('2015-01-20 16:14:36.709649')");

    pstmt.reset(con->prepareStatement("SELECT ts, TIME(ts) from bug68523"));
    res.reset(pstmt->executeQuery());
    ASSERT(res->next());

    ASSERT_EQUALS(res->getString(1), "2015-01-20 16:14:36.709649");
    ASSERT_EQUALS(res->getString(2), "16:14:36.709649");

    stmt->execute("DROP TABLE IF EXISTS bug68523");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

}


void bugs::bug66235()
{
  logMsg("bug::bug66235");
  try
  {
    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id BIT(3))");
    stmt->execute("INSERT INTO test(id) VALUES(0b1), (0b10), (0b1), (0b1), (0b10), (0b111);");

    res.reset(stmt->executeQuery("SELECT MAX(id), MIN(id) FROM test"));
    while (res->next())
    {
      ASSERT_EQUALS(res->getString(1), "7");
      ASSERT_EQUALS(res->getInt(1), 7);

      ASSERT_EQUALS(res->getString(2), "1");
      ASSERT_EQUALS(res->getInt(2), 1);
    }

    res.reset(stmt->executeQuery("SELECT id FROM test limit 1"));
    while (res->next())
    {
      ASSERT_EQUALS(res->getString(1), "1");
      ASSERT_EQUALS(res->getInt(1), 1);
    }

    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void bugs::bug21066575()
{
  logMsg("bug::bug21066575");
  try
  {


    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS bug21066575");
    stmt->execute("CREATE TABLE bug21066575(txt text)");
    stmt->execute("INSERT INTO bug21066575(txt) VALUES('abc')");

    pstmt.reset(con->prepareStatement("select * from bug21066575"));

    res.reset();

    for (int i = 0; i < 10; ++i)
    {
      res.reset(pstmt->executeQuery());
      res->next();
      ASSERT_EQUALS("abc", res->getString(1));
    }

    stmt->execute("DROP TABLE IF EXISTS bug21066575_2");

    stmt->execute("CREATE TABLE `bug21066575_2` (\
                  `f1` longtext,\
                  `id` int(11) NOT NULL AUTO_INCREMENT,\
                  PRIMARY KEY (`id`)\
                  ) ENGINE=MyISAM AUTO_INCREMENT=3 DEFAULT CHARSET=latin1");

    stmt->execute("insert into bug21066575_2(f1) values(repeat('f',1024000)),"
                  "(repeat('f',1024000)),"
                  "(repeat('f',1024000)),"
                  "(repeat('f',1024000))");


    for(int i= 0; i < 100; i++)
    {
      pstmt.reset(con->prepareStatement("select id, f1 from bug21066575_2"));

      //pstmt->setInt(1, i);
      res.reset(pstmt->executeQuery());
      while (res->next()) {
        std::stringstream ss;
        ss << "id = " << res->getInt(1);
        ss << std::endl;
        std::string out = res->getString(2);
        ASSERT_EQUALS(1024000UL, static_cast<uint64_t>(out.length()));
        ss << "f1 = " << out;
        logMsg(ss.str().c_str());
      }
      //Detect if process frees ResultSet resources.
      res.reset();
    }

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void bugs::bug14520822()
{

  logMsg("bug::bug14520822");
  try
  {

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS bug14520822");
    stmt->execute("CREATE TABLE bug14520822(b BIT NOT NULL DEFAULT 0)");
    stmt->execute("INSERT INTO bug14520822(b) VALUES(0b0), (0b1)");

    res.reset(stmt->executeQuery("select min(b) ,max(b) from bug14520822"));
    res->next();
    ASSERT_EQUALS("0", res->getString(1));
    ASSERT_EQUALS("1", res->getString(2));
    ASSERT_EQUALS(false, res->getBoolean(1));
    ASSERT_EQUALS(true, res->getBoolean(2));
    ASSERT_EQUALS((int64_t)0, res->getInt64(1));
    ASSERT_EQUALS((int64_t)1, res->getInt64(2));

    pstmt.reset(con->prepareStatement("select min(b) ,max(b) from bug14520822"));
    res.reset(pstmt->executeQuery());
    res->next();
    ASSERT_EQUALS("0", res->getString(1));
    ASSERT_EQUALS("1", res->getString(2));
    ASSERT_EQUALS(false, res->getBoolean(1));
    ASSERT_EQUALS(true, res->getBoolean(2));
    ASSERT_EQUALS((int64_t)0, res->getInt64(1));
    ASSERT_EQUALS((int64_t)1, res->getInt64(2));

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void bugs::bug21053335()
{

  logMsg("bugs::bug21053335");
  try
  {

    stmt->execute("DROP TABLE IF EXISTS bug21053335");
    stmt->execute("CREATE TABLE bug21053335(c char(10))");
    stmt->execute("INSERT INTO bug21053335 values(NULL), (1)");
    res.reset(stmt->executeQuery("select c from bug21053335"));
    res->next();
    std::stringstream log;
    log << "Data :" <<res->getString(1);
    log<<"\nrs->wasNull(1) : "<<res->wasNull()<<std::endl;
    logMsg(log.str().c_str());

    ASSERT(res->wasNull());

    res->next();

    try{
      ASSERT(res->wasNull());
      FAIL("Exception was not thrown by wasNull()");
    }
    catch (sql::SQLException&)
    {
      // Everything is ok
    }

    log.flush();
    log << "Data :" <<res->getString(1);
    log<<"\nrs->wasNull(1) : "<<res->wasNull()<<std::endl;
    logMsg(log.str().c_str());

    ASSERT(!res->wasNull());


  }

  catch (sql::SQLException&)
  {
    FAIL("Exception thrown by wasNull()");
    throw;
  }

}


void bugs::bug17218692()
{
  logMsg("bugs::bug17218692");
  try
  {

    stmt->execute("DROP TABLE IF EXISTS bug17218692");
    stmt->execute("CREATE TABLE bug17218692(c1  time(6))");
    stmt->execute("INSERT INTO bug17218692 VALUES('-838:59:58.987657')");

    res.reset(stmt->executeQuery("select * from bug17218692"));
    res->next();

    std::stringstream log;
    log<<"["<<res->getString(1)<<"]";

    ASSERT_EQUALS(log.str(), "[-838:59:58.987657]");

    pstmt.reset(con->prepareStatement("select * from bug17218692 "));
    res.reset(pstmt->executeQuery());
    res->next();

    std::stringstream log2;
    log2 << "["<<res->getString(1)<<"]";
    ASSERT_EQUALS(log.str(), log2.str());

  }
  catch (sql::SQLException&)
  {
    FAIL("Exception thrown");
    throw;
  }


}

void bugs::bug21067193()
{
  logMsg("bugs::bug21067193");
  try
  {


    stmt->execute("DROP TABLE IF EXISTS bug21067193");
    stmt->execute("create table bug21067193(id int)");
    stmt->execute("insert into bug21067193 values(1),(2),(3)");

    res.reset((stmt->setResultSetType(sql::ResultSet::TYPE_SCROLL_SENSITIVE)->executeQuery("select * from bug21067193")));

    ASSERT_EQUALS(true, res->absolute(2));
    ASSERT_EQUALS(2, res->getInt(1));

    ASSERT_EQUALS(true, res->absolute(-1));
    ASSERT_EQUALS(3, res->getInt(1));

    ASSERT_EQUALS(false, res->absolute(std::numeric_limits<int>::min())); //Invalid position, Returns FALSE

  }
  catch (sql::SQLException&)
  {
//	Error....
      throw;
  }


}

void bugs::bug21152054()
{

  stmt->execute("DROP TABLE IF EXISTS bug21152054");
  stmt->execute("create table bug21152054(c1 int);" );
  stmt->execute("insert into  bug21152054 values(1), (2), (3), (4);" );
  pstmt.reset( con->prepareStatement("select c1 from bug21152054;") );
  res.reset( pstmt->executeQuery() );
  ASSERT_EQUALS(true, res->absolute(4));
  ASSERT_EQUALS(4, res->getInt(1));
  int line = 4;
  ASSERT_EQUALS(true, res->relative(-1));
  do
  {
    --line;
    ASSERT_EQUALS(line, res->getInt(1));
  }  while(res->relative(-1));


}

void bugs::bug22292073()
{

  stmt->execute("DROP TABLE IF EXISTS bug22292073");
  stmt->execute("create table bug22292073 (jdoc JSON);" );
  stmt->execute("insert into bug22292073 values('{ \"name\": \"abc\", \"age\": 1 , \"misc\":\
                1.2}'), ('{ \"name\": \"abcdef\", \"age\": 31 , \"misc\": 1.237843}');" );
  pstmt.reset( con->prepareStatement("select JSON_EXTRACT(jdoc, '$.age') from bug22292073;") );
  res.reset( pstmt->executeQuery() );

  res->next();

  ASSERT_EQUALS(true, res->getBoolean(1));
  ASSERT_EQUALS(1, res->getInt(1));
  ASSERT_EQUALS((int64_t)1L, res->getInt64(1));
  ASSERT_EQUALS(1, res->getUInt(1));
  ASSERT_EQUALS(1UL, res->getUInt64(1));
  ASSERT_EQUALS(1.0, res->getDouble(1));

  res->next();

  ASSERT_EQUALS(true, res->getBoolean(1));
  ASSERT_EQUALS(31, res->getInt(1));
  ASSERT_EQUALS((int64_t)31L, res->getInt64(1));
  ASSERT_EQUALS(31, res->getUInt(1));
  ASSERT_EQUALS(31UL, res->getUInt64(1));
  ASSERT_EQUALS(31.0, res->getDouble(1));

  stmt.reset(con->createStatement());
  res.reset(stmt->executeQuery("select JSON_EXTRACT(jdoc, '$.age') from bug22292073;"));

  res->next();

  ASSERT_EQUALS(true, res->getBoolean(1));
  ASSERT_EQUALS(1, res->getInt(1));
  ASSERT_EQUALS((int64_t)1L, res->getInt64(1));
  ASSERT_EQUALS(1, res->getUInt(1));
  ASSERT_EQUALS(1UL, res->getUInt64(1));
  ASSERT_EQUALS(1.0, res->getDouble(1));

  res->next();

  ASSERT_EQUALS(true, res->getBoolean(1));
  ASSERT_EQUALS(31, res->getInt(1));
  ASSERT_EQUALS((int64_t)31L, res->getInt64(1));
  ASSERT_EQUALS(31, res->getUInt(1));
  ASSERT_EQUALS(31UL, res->getUInt64(1));
  ASSERT_EQUALS(31.0, res->getDouble(1));
}

void bugs::bug23212333()
{
  stmt->executeUpdate("drop table if exists bug23212333");
  stmt->executeUpdate("create table bug23212333(id longtext)");

  pstmt.reset(con->prepareStatement("insert into test.bug23212333 VALUES(?)"));

  std::string buffer;
  buffer.append(256*1024+1,'A');

  pstmt->setString(1, buffer);
  pstmt->execute();
}

void bugs::bug17227390()
{
  try
  {
    std::locale::global(std::locale("fr_CA.UTF-8"));

    for (int i=0; i < 2; ++i)
    {
      if (i == 0)
      {
        pstmt.reset( con->prepareStatement("select 1.001 as number;") );
        res.reset( pstmt->executeQuery() );
      }
      else
      {
        res.reset(stmt->executeQuery("select 1.001 as number;"));
      }

      res->next();

      ASSERT_EQUALS(1.001L, res->getDouble(1));
      ASSERT_EQUALS(1.001L, res->getDouble("number"));

    }
  }
  catch (...) {
    // Some systems don't have this encoding, so could throw error here
  }
}

/* Bug #28204677 - The driver defaults to utf8mb4 */
void bugs::bug28204677()
{
  logMsg("bugs::bug71606");

  if (getMySQLVersion(con) < 57000)
  {
    SKIP("The server does not support tested functionality(default utf8mb4 charset)");
  }

  res.reset(stmt->executeQuery("SHOW VARIABLES like 'character_set_client'"));
  res->next();

  ASSERT_EQUALS(res->getString(2),"utf8mb4");
}

void bugs::bug31399362()
{
  logMsg("bugs::bug31399362");

  sql::ConnectOptionsMap opt;
  opt["CLIENT_MULTI_STATEMENTS"] = true;

  Connection con2(getConnection(&opt));

  con2->setSchema(db);

  stmt.reset(con2->createStatement());



  // getMoreResults() returned false for INSERT/UPDATE/DELETE getting then out of sync error

  stmt->execute("DROP TABLE IF EXISTS t");
  stmt->execute("CREATE TABLE t(id INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, val BIGINT)");
  stmt->execute("INSERT INTO t SELECT 1,1;"
                "SELECT * FROM t;"
                "UPDATE t SET val=2;"
                "INSERT INTO t SELECT 3,3;");

  int n = 0;
  do
  {
    TestsListener::messagesLog() << n++ << std::endl;
    res.reset(stmt->getResultSet());

    if (!res.get())
      continue;

    while (res->next())
    {
      TestsListener::messagesLog() << "\t... MySQL replies: "
                                   << res->getString(1) << std::endl;
    }

   }while(stmt->getMoreResults() == true || stmt->getUpdateCount() != UL64(~0));

  res.reset(stmt->executeQuery("SELECT * FROM t"));
  while (res->next()) {
    TestsListener::messagesLog() << "\t... MySQL replies: "
                                 << res->getString(1) << std::endl;
  }

  stmt.reset();

}

void bugs::bug30126457()
{
  logMsg("bugs::bug31399362");

  sql::ConnectOptionsMap opt;
  opt["CLIENT_MULTI_STATEMENTS"] = true;

  pstmt.reset( con->prepareStatement(" SELECT JSON_OBJECT('City03a', 'A', 'Population', 111111) AS 'Result'") );

  std::unique_ptr<sql::ResultSet> mysql_result( pstmt->executeQuery() );

  {
    int column_found = mysql_result->findColumn( "result" );
    ASSERT_EQUALS(column_found, 0);
  }
  {
    int column_found = mysql_result->findColumn( "Result" );
    ASSERT_EQUALS(column_found, 1);
  }
}

void bugs::bug32695580()
{
  logMsg("bugs::bug32695580");
  sql::ConnectOptionsMap opt;
  opt[OPT_HOSTNAME] = "127.0.0.1";
  opt[OPT_PORT] = 111;

  try {
    Connection con2(getConnection(&opt));
  }  catch (const sql::SQLException &e) {
    ASSERT_EQUALS(2003,e.getErrorCode());
  }
}

void bugs::bug23235968()
{
  logMsg("bugs::bug23235968");

  stmt->executeUpdate("drop table if exists bug23235968_1");
  stmt->executeUpdate("create table bug23235968_1 (id int unsigned not null)");

  stmt->executeUpdate("drop table if exists bug23235968_2");
  stmt->executeUpdate("create table bug23235968_2 (id int unsigned not null)");



  sql::ConnectOptionsMap opt;

  opt[OPT_SCHEMA] = "test";

  Connection con2(getConnection(&opt));

  Statement my_stmt(con2->createStatement());

  res.reset(my_stmt->executeQuery("select connection_id()"));

  res->next();

  uint64_t connection_id = res->getUInt64(1);

  con2->setAutoCommit(false);

  PreparedStatement my_pstmt(con2->prepareStatement("insert into bug23235968_1 ( id ) values( ? )"));
  PreparedStatement my_pstmt2(con2->prepareStatement("insert into bug23235968_2 ( id ) values( ? )"));

  for (int i = 1; i <= 10; ++i)
  {
    my_pstmt->setInt(1, i);
    my_pstmt->execute();

    my_pstmt2->setInt(1, i);
    my_pstmt2->execute();
  }

  con2->commit();

  for (int i = 11; i <= 20; ++i)
  {
    my_pstmt->setInt(1, i);
    my_pstmt->execute();

    my_pstmt2->setInt(1, i);
    my_pstmt2->execute();
  }

  //Now, lets kill the connection 2
  std::string sql ("KILL CONNECTION ");
  sql+= std::to_string(connection_id);

  stmt->execute(sql);

  try {
    con2->commit();
    FAIL("No Exception thrown");
  }  catch (const sql::SQLException &e) {
    std::cout << "Expected: " << e.what() << std::endl;
  }

  res.reset(stmt->executeQuery("select count(*) from bug23235968_1"));
  res->next();
  ASSERT_EQUALS(10,res->getUInt(1));

  res.reset(stmt->executeQuery("select count(*) from bug23235968_2"));
  res->next();
  ASSERT_EQUALS(10,res->getUInt(1));



  stmt->executeUpdate("drop table bug23235968_1");
  stmt->executeUpdate("drop table bug23235968_2");


}


} /* namespace regression */
} /* namespace testsuite */


