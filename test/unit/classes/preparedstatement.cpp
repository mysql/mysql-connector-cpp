
/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

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
  bool got_warning=false;

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

      pstmt.reset(con->prepareStatement("SELECT id, NULL FROM test"));
      res.reset(pstmt->executeQuery());
      checkResultSetScrolling(res);
      ASSERT(res->next());

      if (it->check_as_string && (res->getString(1) != it->as_string))
      {
        sql.str("");
        sql << "... \t\tWARNING - SQL: '" << it->sqldef << "' - expecting '" << it->as_string << "'";
        sql << " got '" << res->getString(1) << "'";
        logMsg(sql.str());
        got_warning=true;
      }
      ASSERT_EQUALS(res->getString("id"), res->getString(1));
      try
      {
        res->getString(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      try
      {
        res->getString(3);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      res->beforeFirst();
      try
      {
        res->getString(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->afterLast();
      try
      {
        res->getString(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->first();

      ASSERT_EQUALS(res->getDouble("id"), res->getDouble(1));
      try
      {
        res->getDouble(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      try
      {
        res->getDouble(3);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      res->beforeFirst();
      try
      {
        res->getDouble(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->afterLast();
      try
      {
        res->getDouble(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->first();

      ASSERT_EQUALS(res->getInt(1), res->getInt("id"));
      try
      {
        res->getInt(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      try
      {
        res->getInt(3);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      res->beforeFirst();
      try
      {
        res->getInt(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->afterLast();
      try
      {
        res->getInt(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->first();

      ASSERT_EQUALS(res->getUInt(1), res->getUInt("id"));
      try
      {
        res->getUInt(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      try
      {
        res->getUInt(3);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      res->beforeFirst();
      try
      {
        res->getUInt(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->afterLast();
      try
      {
        res->getUInt(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->first();

      ASSERT_EQUALS(res->getInt64("id"), res->getInt64(1));
      try
      {
        res->getInt64(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      try
      {
        res->getInt64(3);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      res->beforeFirst();
      try
      {
        res->getInt64(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->afterLast();
      try
      {
        res->getInt64(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->first();

      ASSERT_EQUALS(res->getUInt64("id"), res->getUInt64(1));
      try
      {
        res->getUInt64(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      try
      {
        res->getUInt64(3);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      res->beforeFirst();
      try
      {
        res->getUInt64(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->afterLast();
      try
      {
        res->getUInt64(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->first();

      ASSERT_EQUALS(res->getBoolean("id"), res->getBoolean(1));
      try
      {
        res->getBoolean(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      try
      {
        res->getBoolean(3);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }

      res->beforeFirst();
      try
      {
        res->getBoolean(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->afterLast();
      try
      {
        res->getBoolean(1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException &)
      {
      }
      res->first();
    }
    stmt->execute("DROP TABLE IF EXISTS test");
    if (got_warning)
      FAIL("See warnings");

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void preparedstatement::assortedSetType()
{
  logMsg("preparedstatement::assortedSetType() - MySQL_PreparedStatement::set*");

  std::stringstream sql;
  std::vector<columndefinition>::iterator it;
  stmt.reset(con->createStatement());
  bool got_warning=false;

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

      pstmt->clearParameters();
      try
      {
        pstmt->setString(0, "overflow");
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      try
      {
        pstmt->setString(2, "invalid");
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      pstmt->setBigInt(1, it->value);
      ASSERT_EQUALS(1, pstmt->executeUpdate());

      pstmt->clearParameters();
      try
      {
        pstmt->setBigInt(0, it->value);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      try
      {
        pstmt->setBigInt(2, it->value);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      pstmt->setBoolean(1, false);
      ASSERT_EQUALS(1, pstmt->executeUpdate());

      pstmt->clearParameters();
      try
      {
        pstmt->setBoolean(0, false);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      try
      {
        pstmt->setBoolean(2, false);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      pstmt->setDateTime(1, it->value);
      ASSERT_EQUALS(1, pstmt->executeUpdate());

      pstmt->clearParameters();
      try
      {
        pstmt->setDateTime(0, it->value);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      try
      {
        pstmt->setDateTime(2, it->value);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      pstmt->setDouble(1, (double) 1.23);
      ASSERT_EQUALS(1, pstmt->executeUpdate());

      pstmt->clearParameters();
      try
      {
        pstmt->setDouble(0, (double) 1.23);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      try
      {
        pstmt->setDouble(2, (double) 1.23);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      pstmt->setInt(1, (int32_t) - 1);
      ASSERT_EQUALS(1, pstmt->executeUpdate());

      pstmt->clearParameters();
      try
      {
        pstmt->setInt(0, (int32_t) - 1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      try
      {
        pstmt->setInt(2, (int32_t) - 1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      pstmt->setUInt(1, (uint32_t) 1);
      ASSERT_EQUALS(1, pstmt->executeUpdate());

      pstmt->clearParameters();
      try
      {
        pstmt->setUInt(0, (uint32_t) 1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      try
      {
        pstmt->setUInt(2, (uint32_t) 1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      pstmt->setInt64(1, (int64_t) - 123);
      ASSERT_EQUALS(1, pstmt->executeUpdate());

      if (it->is_nullable)
      {
        pstmt->clearParameters();
        pstmt->setNull(1, it->ctype);
        ASSERT_EQUALS(1, pstmt->executeUpdate());

        pstmt->clearParameters();
        try
        {
          pstmt->setNull(0, it->ctype);
          FAIL("Invalid argument not detected");
        }
        catch (sql::InvalidArgumentException)
        {
        }

        pstmt->clearParameters();
        try
        {
          pstmt->setNull(2, it->ctype);
          FAIL("Invalid argument not detected");
        }
        catch (sql::InvalidArgumentException)
        {
        }
      }

      pstmt->clearParameters();
      pstmt->setUInt(1, (uint32_t) 1);
      ASSERT_EQUALS(1, pstmt->executeUpdate());

      pstmt->clearParameters();
      try
      {
        pstmt->setUInt(0, (uint32_t) - 1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      try
      {
        pstmt->setUInt(2, (uint32_t) - 1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      pstmt->setUInt64(1, (uint64_t) 123);
      ASSERT_EQUALS(1, pstmt->executeUpdate());

      pstmt->clearParameters();
      try
      {
        pstmt->setUInt64(0, (uint64_t) - 1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt->clearParameters();
      try
      {
        pstmt->setUInt64(2, (uint64_t) - 1);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      pstmt.reset(con->prepareStatement("SELECT COUNT(IFNULL(id, 1)) AS _num FROM test"));
      res.reset(pstmt->executeQuery());
      checkResultSetScrolling(res);
      ASSERT(res->next());
      if (res->getInt("_num") != (10 + (int) it->is_nullable))
      {
        sql.str("");
        sql << "....\t\tWARNING, SQL: " << it->sqldef << ", nullable " << std::boolalpha;
        sql << it->is_nullable << ", found " << res->getInt(1) << "columns but";
        sql << " expecting " << (10 + (int) it->is_nullable);
        logMsg(sql.str());
        got_warning=true;
      }

    }
    stmt->execute("DROP TABLE IF EXISTS test");
    if (got_warning)
      FAIL("See warnings");

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void preparedstatement::setNull()
{
  logMsg("preparedstatement::setNull() - MySQL_PreparedStatement::*");

  std::stringstream sql;
  stmt.reset(con->createStatement());

  try
  {
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT)");
    pstmt.reset(con->prepareStatement("INSERT INTO test(id) VALUES (?)"));
    pstmt->setNull(1, sql::DataType::INTEGER);
    ASSERT_EQUALS(1, pstmt->executeUpdate());

    pstmt.reset(con->prepareStatement("SELECT id FROM test"));
    res.reset(pstmt->executeQuery());
    checkResultSetScrolling(res);
    ASSERT(res->next());
    ASSERT(res->isNull(1));
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  try
  {
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT NOT NULL)");
    pstmt.reset(con->prepareStatement("INSERT INTO test(id) VALUES (?)"));
    pstmt->setNull(1, sql::DataType::INTEGER);
    pstmt->executeUpdate();
    FAIL("Should fail");
  }
  catch (sql::SQLException &)
  {
  }

}

void preparedstatement::checkClosed()
{
  logMsg("preparedstatement::checkClosed() - MySQL_PreparedStatement::close()");

  try
  {
    pstmt.reset(con->prepareStatement("SELECT 1"));
    pstmt->close();
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

}

void preparedstatement::getMetaData()
{
  logMsg("preparedstatement::getMetaData() - MySQL_PreparedStatement::getMetaData()");

  std::stringstream sql;
  std::vector<columndefinition>::iterator it;
  stmt.reset(con->createStatement());
  ResultSetMetaData * meta_ps;
  ResultSetMetaData * meta_st;
  ResultSet res_st;
  bool got_warning=false;
  unsigned int i;

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

      pstmt.reset(con->prepareStatement("SELECT id, dummy, NULL, -1.1234, 'Warum nicht...' FROM test"));
      res.reset(pstmt->executeQuery());
      meta_ps=res->getMetaData();

      res_st.reset(stmt->executeQuery("SELECT id, dummy, NULL, -1.1234, 'Warum nicht...' FROM test"));
      meta_st=res->getMetaData();

      ASSERT_EQUALS(meta_ps->getColumnCount(), meta_st->getColumnCount());

      for (i=1; i <= meta_ps->getColumnCount(); i++)
      {
        ASSERT_EQUALS(meta_ps->getCatalogName(i), meta_st->getCatalogName(i));
        ASSERT_EQUALS(meta_ps->getColumnDisplaySize(i), meta_st->getColumnDisplaySize(i));
        ASSERT_EQUALS(meta_ps->getColumnLabel(i), meta_st->getColumnLabel(i));
        ASSERT_EQUALS(meta_ps->getColumnName(i), meta_st->getColumnName(i));
        ASSERT_EQUALS(meta_ps->getColumnType(i), meta_st->getColumnType(i));
        ASSERT_EQUALS(meta_ps->getColumnTypeName(i), meta_st->getColumnTypeName(i));
        ASSERT_EQUALS(meta_ps->getPrecision(i), meta_st->getPrecision(i));
        ASSERT_EQUALS(meta_ps->getScale(i), meta_st->getScale(i));
        ASSERT_EQUALS(meta_ps->getSchemaName(i), meta_st->getSchemaName(i));
        ASSERT_EQUALS(meta_ps->getTableName(i), meta_st->getTableName(i));
        ASSERT_EQUALS(meta_ps->isAutoIncrement(i), meta_st->isAutoIncrement(i));
        ASSERT_EQUALS(meta_ps->isCaseSensitive(i), meta_st->isCaseSensitive(i));
        ASSERT_EQUALS(meta_ps->isCurrency(i), meta_st->isCurrency(i));
        ASSERT_EQUALS(meta_ps->isDefinitelyWritable(i), meta_st->isDefinitelyWritable(i));
        ASSERT_EQUALS(meta_ps->isNullable(i), meta_st->isNullable(i));
        ASSERT_EQUALS(meta_ps->isReadOnly(i), meta_st->isReadOnly(i));
        ASSERT_EQUALS(meta_ps->isSearchable(i), meta_st->isSearchable(i));
        ASSERT_EQUALS(meta_ps->isSigned(i), meta_st->isSigned(i));
        ASSERT_EQUALS(meta_ps->isWritable(i), meta_st->isWritable(i));
      }

      try
      {
        meta_ps->getCatalogName(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

    }
    stmt->execute("DROP TABLE IF EXISTS test");
    if (got_warning)
      FAIL("See warnings");

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void preparedstatement::callSP()
{
  logMsg("preparedstatement::callSP() - MySQL_PreparedStatement::*()");

  try
  {
    try
    {
      pstmt.reset(con->prepareStatement("DROP PROCEDURE IF EXISTS p"));
      pstmt->execute();
    }
    catch (sql::SQLException &e)
    {
      logMsg("... skipping:");
      logMsg(e.what());
      return;
    }

    DatabaseMetaData * dbmeta=con->getMetaData();

    pstmt.reset(con->prepareStatement("CREATE PROCEDURE p(OUT ver_param VARCHAR(25)) BEGIN SELECT VERSION() INTO ver_param; END;"));
    ASSERT(!pstmt->execute());
    pstmt.reset(con->prepareStatement("CALL p(@version)"));
    ASSERT(!pstmt->execute());
    pstmt.reset(con->prepareStatement("SELECT @version AS _version"));
    res.reset(pstmt->executeQuery());
    ASSERT(res->next());
    ASSERT_EQUALS(dbmeta->getDatabaseProductVersion(), res->getString("_version"));
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

}

void preparedstatement::anonymousSelect()
{
  logMsg("preparedstatement::anonymousSelect() - MySQL_PreparedStatement::*, MYSQL_PS_Resultset::*");

  try
  {
    pstmt.reset(con->prepareStatement("SELECT ' ', NULL"));
    res.reset(pstmt->executeQuery());
    ASSERT(res->next());
    ASSERT_EQUALS(" ", res->getString(1));

    std::string mynull(res->getString(2));
    ASSERT(res->isNull(2));
    ASSERT(res->wasNull());

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void preparedstatement::crash()
{
  bool trace_on=true;
  // Can hit server bug http://bugs.mysql.com/bug.php?id=43833

  logMsg("preparedstatement::crash() - MySQL_PreparedStatement::*");
  try
  {
    trace_on=true;
    con->setClientOption("clientTrace", &trace_on);

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(dummy TIMESTAMP, id VARCHAR(1))");
    pstmt.reset(con->prepareStatement("INSERT INTO test(id) VALUES (?)"));

    pstmt->clearParameters();
    pstmt->setDouble(1, (double) 1.23);
    ASSERT_EQUALS(1, pstmt->executeUpdate());

    trace_on=false;
    con->setClientOption("clientTrace", &trace_on);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

}

void preparedstatement::getWarnings()
{
  logMsg("preparedstatement::getWarnings() - MySQL_PreparedStatement::get|clearWarnings()");
  std::stringstream msg;

  stmt.reset(con->createStatement());
  try
  {
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT UNSIGNED)");

    // Lets hope that this will always cause a 1264 or similar warning
    pstmt.reset(con->prepareStatement("INSERT INTO test(id) VALUES (-1)"));
    pstmt->executeUpdate();

    for (const sql::SQLWarning* warn=pstmt->getWarnings(); warn; warn=warn->getNextWarning())
    {
      msg.str("");
      msg << "... ErrorCode = '" << warn->getErrorCode() << "', ";
      msg << "SQLState = '" << warn->getSQLState() << "', ";
      msg << "ErrorMessage = '" << warn->getMessage() << "'";
      logMsg(msg.str());

      ASSERT((0 != warn->getErrorCode()));
      if (1264 == warn->getErrorCode())
      {
        ASSERT_EQUALS("22003", warn->getSQLState());
      }
      else
      {
        ASSERT(("" != warn->getSQLState()));
      }
      ASSERT(("" != warn->getMessage()));
    }

    for (const sql::SQLWarning* warn=pstmt->getWarnings(); warn; warn=warn->getNextWarning())
    {
      msg.str("");
      msg << "... ErrorCode = '" << warn->getErrorCode() << "', ";
      msg << "SQLState = '" << warn->getSQLState() << "', ";
      msg << "ErrorMessage = '" << warn->getMessage() << "'";
      logMsg(msg.str());

      ASSERT((0 != warn->getErrorCode()));
      if (1264 == warn->getErrorCode())
      {
        ASSERT_EQUALS("22003", warn->getSQLState());
      }
      else
      {
        ASSERT(("" != warn->getSQLState()));
      }
      ASSERT(("" != warn->getMessage()));
    }

    pstmt->clearWarnings();
    for (const sql::SQLWarning* warn=pstmt->getWarnings(); warn; warn=warn->getNextWarning())
    {
      FAIL("There should be no more warnings!");
    }

    // TODO - how to use getNextWarning() ?
    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

} /* namespace preparedstatement */
} /* namespace testsuite */
