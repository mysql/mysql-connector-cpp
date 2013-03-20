/*
Copyright (c) 2009, 2013, Oracle and/or its affiliates. All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPLv2
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPLv2 as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/



#include "bugs.h"
#include <sstream>
#include "driver/mysql_error.h"

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


  stmt->execute("drop table if exists bug39878");
  stmt->execute("create table bug39878 (id int unsigned not null)");

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
						"	RETURN 55;"
						"END");

  ASSERT(stmt->execute("select _getActivePost()"));

  try
  {
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
  stmt->execute("CREATE TABLE products (uiProductsIdx int(10) unsigned NOT NULL AUTO_INCREMENT, startTime timestamp NULL DEFAULT NULL, stopTime timestamp NULL DEFAULT NULL, uiProductsID int(10) DEFAULT NULL, uiParameterSetID int(10) unsigned DEFAULT NULL, PRIMARY KEY (uiProductsIdx))");

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

  stmt->executeUpdate("DROP TABLE IF EXISTS test.ccpp_expired_pwd");
  stmt->executeUpdate("CREATE USER ccpp_expired_pwd IDENTIFIED BY 'foo'");
  stmt->executeUpdate("GRANT ALL ON test to ccpp_expired_pwd");
  stmt->executeUpdate("ALTER USER ccpp_expired_pwd PASSWORD EXPIRE");

  sql::ConnectOptionsMap opts;

  opts["userName"]=							sql::SQLString("ccpp_expired_pwd");
  opts["password"]=							sql::SQLString("foo");

  testsuite::Connection c2;

  /* Seeing error first without OPT_CAN_HANDLE_EXPIRED_PASSWORDS ... */
  try
  {
    c2.reset(getConnection(&opts));
  }
  catch (sql::SQLException &e)
  {
    ASSERT_EQUALS(1820, e.getErrorCode()/*ER_MUST_CHANGE_PASSWORD_LOGIN*/);
  }

  /* ... Now with it */
  opts["OPT_CAN_HANDLE_EXPIRED_PASSWORDS"]= true;

  try
  {
    c2.reset(getConnection(&opts));
  }
  catch (sql::SQLException &e)
  {
	/* In case of sql::mysql::mydeCL_CANT_HANDLE_EXP_PWD tests fail - means that in
	   the setup where test is run the driver does not support expired password */
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
} /* namespace regression */
} /* namespace testsuite */
