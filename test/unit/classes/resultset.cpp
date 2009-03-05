/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include <cppconn/warning.h>

#include "resultset.h"
#include <sstream>
#include <stdlib.h>
#include <cppconn/resultset.h>
#include <cppconn/datatype.h>
#include <cppconn/connection.h>

namespace testsuite
{
namespace classes
{

void resultset::getInt()
{
  // Message for --verbose output
  logMsg("resultset::getInt - MySQL_ResultSet::getInt*");
  try
  {

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(i integer, i_uns integer unsigned, b bigint, b_uns bigint unsigned)");

    int64_t r1_c1=L64(2147483646),
            r1_c2=L64(2147483650),
            r1_c3=L64(9223372036854775806),
            r2_c1=L64(2147483647),
            r2_c2=L64(2147483649),
            r2_c3=L64(9223372036854775807);

    uint64_t r1_c4=UL64(9223372036854775810),
            r2_c4=UL64(18446744073709551615);

    pstmt.reset(con->prepareStatement("INSERT INTO test(i, i_uns, b, b_uns) VALUES(?,?,?,?)"));

    ASSERT(pstmt.get() != NULL);
    pstmt->clearParameters();
    pstmt->setInt(1, static_cast<int>(r1_c1));
    pstmt->setInt64(2, r1_c2);
    pstmt->setInt64(3, r1_c3);
    pstmt->setUInt64(4, r1_c4);
    ASSERT_EQUALS(false, pstmt->execute());

    pstmt->clearParameters();
    pstmt->setInt(1, static_cast<int>(r2_c1));
    pstmt->setInt64(2, r2_c2);
    pstmt->setInt64(3, r2_c3);
    pstmt->setUInt64(4, r2_c4);
    ASSERT_EQUALS(false, pstmt->execute());

    pstmt.reset(con->prepareStatement("SELECT i, i_uns, b, b_uns FROM test"));
    ASSERT(pstmt.get() != NULL);
    ASSERT(pstmt->execute());

    res.reset(pstmt->getResultSet());
    checkResultSetScrolling(res);

    ASSERT(res->next());

    ASSERT_EQUALS((int64_t) res->getInt("i"), r1_c1);
    ASSERT_EQUALS((int64_t) res->getInt(1), r1_c1);

    ASSERT_EQUALS(res->getInt64("i_uns"), r1_c2);
    ASSERT_EQUALS(res->getInt64(2), r1_c2);

    ASSERT_EQUALS(res->getInt64("b"), r1_c3);
    ASSERT_EQUALS(res->getInt64(3), r1_c3);

    ASSERT_EQUALS(res->getUInt64("b_uns"), r1_c4);
    ASSERT_EQUALS(res->getUInt64(4), r1_c4);

    ASSERT(res->next());

    ASSERT_EQUALS((int64_t) res->getInt("i"), r2_c1);
    ASSERT_EQUALS((int64_t) res->getInt(1), r2_c1);

    ASSERT_EQUALS(res->getInt64("i_uns"), r2_c2);
    ASSERT_EQUALS(res->getInt64(2), r2_c2);

    ASSERT_EQUALS(res->getInt64("b"), r2_c3);
    ASSERT_EQUALS(res->getInt64(3), r2_c3);

    ASSERT_EQUALS(res->getUInt64("b_uns"), r2_c4);
    ASSERT_EQUALS(res->getUInt64(4), r2_c4);

    ASSERT_EQUALS(res->next(), false);


    res.reset(stmt->executeQuery("SELECT i, i_uns, b, b_uns FROM test"));
    checkResultSetScrolling(res);

    ASSERT(res->next());

    ASSERT_EQUALS((int64_t) res->getInt("i"), r1_c1);
    ASSERT_EQUALS((int64_t) res->getInt(1), r1_c1);

    ASSERT_EQUALS(res->getInt64("i_uns"), r1_c2);
    ASSERT_EQUALS(res->getInt64(2), r1_c2);

    ASSERT_EQUALS(res->getInt64("b"), r1_c3);
    ASSERT_EQUALS(res->getInt64(3), r1_c3);

    ASSERT_EQUALS(res->getUInt64("b_uns"), r1_c4);
    ASSERT_EQUALS(res->getUInt64(4), r1_c4);

    ASSERT(res->next());

    ASSERT_EQUALS((int64_t) res->getInt("i"), r2_c1);
    ASSERT_EQUALS((int64_t) res->getInt(1), r2_c1);

    ASSERT_EQUALS(res->getInt64("i_uns"), r2_c2);
    ASSERT_EQUALS(res->getInt64(2), r2_c2);

    ASSERT_EQUALS(res->getInt64("b"), r2_c3);
    ASSERT_EQUALS(res->getInt64(3), r2_c3);

    ASSERT_EQUALS(res->getUInt64("b_uns"), r2_c4);
    ASSERT_EQUALS(res->getUInt64(4), r2_c4);

    ASSERT_EQUALS(res->next(), false);

    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultset::getTypes()
{
  logMsg("resultset::getTypes - MySQL_ResultSet::get*");
  std::vector<columndefinition>::iterator it;
  std::stringstream msg;
  TODO("Under development...");
  bool got_warning=false;
  ResultSet pres;

  try
  {
    stmt.reset(con->createStatement());
    logMsg("... looping over all kinds of column types");
    for (it=columns.begin(); it != columns.end(); it++)
    {
      stmt->execute("DROP TABLE IF EXISTS test");
      msg.str("");
      msg << "CREATE TABLE test(dummy TIMESTAMP, id " << it->sqldef << ")";
      try
      {
        stmt->execute(msg.str());

        msg.str("");
        msg << "... testing " << it->sqldef;
        logMsg(msg.str());
      }
      catch (sql::SQLException &)
      {
        msg.str("");
        msg << "... skipping " << it->sqldef;
        logMsg(msg.str());
        continue;
      }

      msg.str("");
      msg << "INSERT INTO test(id) VALUES ('" << it->value << "')";
      stmt->execute(msg.str());

      msg.str("");
      msg << "... testing '" << it->sqldef << "'";
      logMsg(msg.str());

      res.reset(stmt->executeQuery("SELECT id FROM test"));
      checkResultSetScrolling(res);
      ASSERT(res->next());
      
      pstmt.reset(con->prepareStatement("SELECT id FROM test"));
      pstmt->clearParameters();
      pres.reset(pstmt->executeQuery());
      checkResultSetScrolling(pres);
      ASSERT(pres->next());
      
      
      if (it->check_as_string)
      {
        logMsg("... checking string value");
        if (it->as_string != res->getString("id"))
        {
          msg.str("");
          msg << "... expecting '" << it->as_string << "', got '" << res->getString("id") << "'";
          logMsg(msg.str());
          got_warning=true;
        }
      }
      ASSERT_EQUALS(res->getString("id"), res->getString(1));
      ASSERT_EQUALS(res->getDouble("id"), res->getDouble(1));
      ASSERT_EQUALS(res->getInt64("id"), res->getInt64(1));
      ASSERT_EQUALS(res->getUInt64("id"), res->getUInt64(1));
      ASSERT_EQUALS(res->getBoolean("id"), res->getBoolean(1));
      
      ASSERT_EQUALS(pres->getString("id"), pres->getString(1));
      ASSERT_EQUALS(pres->getDouble("id"), pres->getDouble(1));
      ASSERT_EQUALS(pres->getInt64("id"), pres->getInt64(1));
      ASSERT_EQUALS(pres->getUInt64("id"), pres->getUInt64(1));
      ASSERT_EQUALS(pres->getBoolean("id"), pres->getBoolean(1));

     // Comparing prepared statement resultset and statement resultset
       ASSERT_EQUALS(pres->getString("id"), res->getString(1));
      ASSERT_EQUALS(pres->getDouble("id"), res->getDouble(1));
      ASSERT_EQUALS(pres->getInt64("id"), res->getInt64(1));
      ASSERT_EQUALS(pres->getUInt64("id"), res->getUInt64(1));
      ASSERT_EQUALS(pres->getBoolean("id"), res->getBoolean(1));
      
    }
    if (got_warning)
      FAIL("See warnings!");
    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

} /* namespace resultset */
} /* namespace testsuite */
