
/*
   Copyright 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include <cppconn/prepared_statement.h>


#include <cppconn/connection.h>


#include <cppconn/warning.h>

#include "preparedstatement.h"
#include <stdlib.h>

namespace testsuite
{
namespace classes
{

void preparedstatement::InsertSelectAllTypes()
{
  logMsg("preparedstatement::InsertSelectAllTypes() - MySQL_PreparedStatement::*");

  std::stringstream sql;
  std::vector<columndefinition>::iterator it;
  stmt.reset(con->createStatement());
  bool got_warning;

  try
  {  

    for (it=columns.end(), it--; it != columns.begin(); it--)
    {
      stmt->execute("DROP TABLE IF EXISTS test");
      sql.str("");
      sql << "CREATE TABLE test(dummy TIMESTAMP, id " << it->sqldef << ")";
      try
      {
        stmt->execute(sql.str());
        sql.str("");
        sql << "... testing '" << it->sqldef << "'";
        logMsg(sql.str());
      }
      catch (sql::SQLException &)
      {
        sql.str("");
        sql << "... skipping '" << it->sqldef << "'";
        logMsg(sql.str());
        continue;
      }

      pstmt.reset(con->prepareStatement("INSERT INTO test(id) VALUES (?)"));
      pstmt->setString(1, it->value);
      ASSERT_EQUALS(1, pstmt->executeUpdate());
      
      pstmt.reset(con->prepareStatement("SELECT id FROM test"));
      res.reset(pstmt->executeQuery());
      checkResultSetScrolling(res);
      ASSERT(res->next());

      if (it->check_as_string && (res->getString(1) != it->as_string)) {
        sql.str("");
        sql << "... WARNING - SQL: '" << it->sqldef << "' - expecting '" << it->as_string << "'";
        sql << " got '" << res->getString(1) << "'";
        logMsg(sql.str());
        got_warning = true;
      }
      ASSERT_EQUALS(res->getString(1), res->getString("id"));

      ASSERT_EQUALS(res->getBoolean(1), res->getBoolean("id"));
      ASSERT_EQUALS(res->getDouble(1), res->getDouble("id"));
      ASSERT_EQUALS(res->getInt(1), res->getInt("id"));
      ASSERT_EQUALS(res->getInt64(1), res->getInt64("id"));
      ASSERT_EQUALS(res->getUInt(1), res->getUInt("id"));
      ASSERT_EQUALS(res->getUInt64(1), res->getUInt64("id"));
    }

    if (got_warning)
      FAIL("See warnings");

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void preparedstatement::assortedSetType()
{
  logMsg("preparedstatement::assortedSetType() - MySQL_PreparedStatement::set*");

  std::stringstream sql;
  std::vector<columndefinition>::iterator it;
  stmt.reset(con->createStatement());
  bool got_warning;

  try
  {  

    for (it=columns.end(), it--; it != columns.begin(); it--)
    {
      stmt->execute("DROP TABLE IF EXISTS test");
      sql.str("");
      sql << "CREATE TABLE test(dummy TIMESTAMP, id " << it->sqldef << ")";
      try
      {
        stmt->execute(sql.str());
        sql.str("");
        sql << "... testing '" << it->sqldef << "'";
        logMsg(sql.str());
      }
      catch (sql::SQLException &)
      {
        sql.str("");
        sql << "... skipping '" << it->sqldef << "'";
        logMsg(sql.str());
        continue;
      }

      pstmt.reset(con->prepareStatement("INSERT INTO test(id) VALUES (?)"));
      pstmt->setString(1, it->value);
      ASSERT_EQUALS(1, pstmt->executeUpdate());

      pstmt->clearParameter();
      pstmt->setBigInt(1,it->value);
      ASSERT_EQUALS(1, pstmt->executeUpdate());

      /*
      
      pstmt.reset(con->prepareStatement("SELECT id FROM test"));
      res.reset(pstmt->executeQuery());
      checkResultSetScrolling(res);
      ASSERT(res->next());

      if (it->check_as_string && (res->getString(1) != it->as_string)) {
        sql.str("");
        sql << "... WARNING - SQL: '" << it->sqldef << "' - expecting '" << it->as_string << "'";
        sql << " got '" << res->getString(1) << "'";
        logMsg(sql.str());
        got_warning = true;
      }
      ASSERT_EQUALS(res->getString(1), res->getString("id"));

      ASSERT_EQUALS(res->getBoolean(1), res->getBoolean("id"));
      ASSERT_EQUALS(res->getDouble(1), res->getDouble("id"));
      ASSERT_EQUALS(res->getInt(1), res->getInt("id"));
      ASSERT_EQUALS(res->getInt64(1), res->getInt64("id"));
      ASSERT_EQUALS(res->getUInt(1), res->getUInt("id"));
      ASSERT_EQUALS(res->getUInt64(1), res->getUInt64("id"));
       */
    }

    if (got_warning)
      FAIL("See warnings");

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

} /* namespace preparedstatement */
} /* namespace testsuite */
