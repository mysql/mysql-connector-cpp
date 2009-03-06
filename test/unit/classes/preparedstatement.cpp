
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

      pstmt.reset(con->prepareStatement("SELECT id FROM test"));
      res.reset(pstmt->executeQuery());
      checkResultSetScrolling(res);
      ASSERT(res->next());

      if (it->check_as_string && (res->getString(1) != it->as_string))
      {
        sql.str("");
        sql << "... WARNING - SQL: '" << it->sqldef << "' - expecting '" << it->as_string << "'";
        sql << " got '" << res->getString(1) << "'";
        logMsg(sql.str());
        got_warning=true;
      }
      ASSERT_EQUALS(res->getString(1), res->getString("id"));
      try
      {
        res->getString(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      try
      {
        res->getString(2);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      ASSERT_EQUALS(res->getBoolean(1), res->getBoolean("id"));
      try
      {
        res->getBoolean(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      try
      {
        res->getBoolean(2);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      ASSERT_EQUALS(res->getDouble(1), res->getDouble("id"));
      try
      {
        res->getDouble(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      try
      {
        res->getDouble(2);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      ASSERT_EQUALS(res->getInt(1), res->getInt("id"));
      try
      {
        res->getInt(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      try
      {
        res->getInt(2);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      ASSERT_EQUALS(res->getInt64(1), res->getInt64("id"));
      try
      {
        res->getInt64(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      try
      {
        res->getInt64(2);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      ASSERT_EQUALS(res->getUInt(1), res->getUInt("id"));
      try
      {
        res->getUInt(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      try
      {
        res->getUInt(2);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      ASSERT_EQUALS(res->getUInt64(1), res->getUInt64("id"));
      try
      {
        res->getUInt64(0);
        FAIL("Invalid argument not detected");
      }
      catch (sql::InvalidArgumentException)
      {
      }

      try
      {
        res->getUInt64(2);
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
      if (res->getUInt(1) != (9 + (unsigned int) it->is_nullable))
      {
        sql.str("");
        sql << "....WARNING, SQL: " << it->sqldef << ", nullable " << std::boolalpha;
        sql << it->is_nullable << ", found " << res->getInt(1) << "columns but";
        sql << " expecting " << (9 + (unsigned int) it->is_nullable);
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
    logErr("SQLState: " + e.getSQLState());
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
    logErr("SQLState: " + e.getSQLState());
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
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }

}

void preparedstatement::getMetaData()
{
  logMsg("preparedstatement::getMetaData() - MySQL_PreparedStatement::getMetaData()");

  std::stringstream sql;
  std::vector<columndefinition>::iterator it;
  stmt.reset(con->createStatement());
  ResultSetMetaData meta_ps;
  ResultSetMetaData meta_st;
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
      meta_ps.reset(res->getMetaData());

      res_st.reset(stmt->executeQuery("SELECT id, dummy, NULL, -1.1234, 'Warum nicht...' FROM test"));
      meta_st.reset(res->getMetaData());

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
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}


} /* namespace preparedstatement */
} /* namespace testsuite */
