/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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



#include <cppconn/warning.h>

#include "resultsetmetadata.h"
#include <sstream>
#include <stdlib.h>
#include <cppconn/resultset.h>
#include <cppconn/datatype.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/statement.h>
#include <cppconn/connection.h>

namespace testsuite
{
namespace classes
{

void resultsetmetadata::getCatalogName()
{
  logMsg("resultsetmetadata::getCatalogName() - MySQL_ResultSetMetaData::getCatalogName");
  bool got_warning=false;

  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    runStandardQuery();
    doGetCatalogName(false, got_warning);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doGetCatalogName(true, got_warning);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  if (got_warning)
  {
    TODO("See --verbose warnings");
    FAIL("TODO - see --verbose warnings");
  }
}

void resultsetmetadata::doGetCatalogName(bool is_ps, bool &got_warning)
{
  std::stringstream msg;
  ResultSetMetaData * meta=res->getMetaData();
  if (con->getCatalog() != "" && meta->getCatalogName(1) != "" && (con->getCatalog() != meta->getCatalogName(1)))
  {
    got_warning=true;
    msg.str("");
    msg << "...\t\tWARNING expecting catalog = '" << con->getCatalog() << "'";
    msg << " got '" << meta->getCatalogName(1) << "'";
    logMsg(msg.str());
  }

  if (meta->getCatalogName(1) != "" && meta->getCatalogName(1) != "def") {
    got_warning=true;
    msg.str("");
    msg << "...\t\tWARNING expecting catalog = 'def'";
    msg << " got '" << meta->getCatalogName(1) << "'";
    logMsg(msg.str());
  }

  try
  {
    meta->getCatalogName(0);
    FAIL("Column number starts at 1, invalid offset 0 not detected");
  }
  catch (sql::InvalidArgumentException &)
  {
  }

  try
  {
    meta->getCatalogName(6);
    FAIL("Only five columns available but requesting number six, should bail");
  }
  catch (sql::InvalidArgumentException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->getCatalogName(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::getColumnCount()
{
  logMsg("resultsetmetadata::getColumnCount() - MySQL_ResultSetMetaData::getColumnCount");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    runStandardQuery();
    doGetColumnCount(false);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doGetColumnCount(true);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doGetColumnCount(bool is_ps)
{
  ResultSetMetaData * meta=res->getMetaData();
  ASSERT_EQUALS((unsigned int) 5, meta->getColumnCount());


  if (!is_ps)
  {
    res->close();
    try
    {
      meta->getCatalogName(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::getColumnDisplaySize()
{
  logMsg("resultsetmetadata::getColumnDisplaySize() - MySQL_ResultSetMetaData::getColumnDisplaySize()");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    runStandardQuery();
    doGetColumnDisplaySize(false);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doGetColumnDisplaySize(true);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doGetColumnDisplaySize(bool is_ps)
{
  ResultSetMetaData * meta=res->getMetaData();

  ASSERT_EQUALS((unsigned int) 5, meta->getColumnDisplaySize(1));
  ASSERT_EQUALS((unsigned int) 1, meta->getColumnDisplaySize(2));
  ASSERT_EQUALS((unsigned int) 5, meta->getColumnDisplaySize(3));
  ASSERT_EQUALS((unsigned int) 1, meta->getColumnDisplaySize(4));
  ASSERT_EQUALS((unsigned int) 3, meta->getColumnDisplaySize(5));

  try
  {
    meta->getColumnDisplaySize(0);
    FAIL("Column number starts at 1, invalid offset 0 not detected");
  }
  catch (sql::InvalidArgumentException &)
  {
  }

  try
  {
    meta->getColumnDisplaySize(6);
    FAIL("Only five columns available but requesting number six, should bail");
  }
  catch (sql::InvalidArgumentException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->getColumnDisplaySize(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::getColumnNameAndLabel()
{
  logMsg("resultsetmetadata::getColumnName() - MySQL_ResultSetMetaData::getColumnName(), MySQL_ResultSetMetaData::getColumnLabel()");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS getColumnNameAndLabel");
    stmt->execute("CREATE TABLE getColumnNameAndLabel(col1 INT, col2 VARCHAR(10))");
    res.reset(stmt->executeQuery("SELECT col1 AS intColumn, col2 AS charColumn FROM getColumnNameAndLabel"));
    doGetColumnNameAndLabel(false);

    logMsg("... PreparedStatement");
    pstmt.reset(con->prepareStatement("SELECT col1 AS intColumn, col2 AS charColumn FROM getColumnNameAndLabel"));
    res.reset(pstmt->executeQuery());
    doGetColumnNameAndLabel(true);

    stmt->execute("DROP TABLE IF EXISTS getColumnNameAndLabel");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doGetColumnNameAndLabel(bool is_ps)
{
  ResultSetMetaData * meta=res->getMetaData();
  ASSERT_EQUALS("col1", meta->getColumnName(1));
  ASSERT_EQUALS("intColumn", meta->getColumnLabel(1));

  ASSERT_EQUALS("col2", meta->getColumnName(2));
  ASSERT_EQUALS("charColumn", meta->getColumnLabel(2));

  try
  {
    meta->getColumnName(0);
    FAIL("Column number starts at 1, invalid offset 0 not detected");
  }
  catch (sql::InvalidArgumentException &)
  {
  }

  try
  {
    meta->getColumnLabel(0);
    FAIL("Column number starts at 1, invalid offset 0 not detected");
  }
  catch (sql::InvalidArgumentException &)
  {
  }

  try
  {
    meta->getColumnName(6);
    FAIL("Only five columns available but requesting number six, should bail");
  }
  catch (sql::InvalidArgumentException &)
  {
  }

  try
  {
    meta->getColumnLabel(6);
    FAIL("Only five columns available but requesting number six, should bail");
  }
  catch (sql::InvalidArgumentException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->getColumnName(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }

}

void resultsetmetadata::getColumnType()
{
  logMsg("resultsetmetadata::getColumnType() - MySQL_ResultSetMetaData::getColumnType()");

  //TODO: Enable it after fixing
  SKIP("Removed untill fixed");

  try
  {
    std::stringstream sql;
    std::vector<columndefinition>::iterator it;
    stmt.reset(con->createStatement());
    bool type_found=false;

    sql::DatabaseMetaData * dbmeta=con->getMetaData();
    ResultSet restypes(dbmeta->getTypeInfo());

    logMsg("... looping over all kinds of columns and testing type and type name");

    for (it=columns.begin(); it != columns.end(); it++)
    {
      stmt->execute("DROP TABLE IF EXISTS test");

      sql.str("");
      sql << "CREATE TABLE test(col1 " << it->sqldef << ")";
      try
      {
        stmt->execute(sql.str());
        sql.str("");
        sql << "INSERT INTO test(col1) VALUES ('" << it->value << "')";
        stmt->execute(sql.str());

        res.reset(stmt->executeQuery("SELECT * FROM test"));
        checkResultSetScrolling(res);
        ResultSetMetaData * meta=res->getMetaData();
        logMsg(it->sqldef);
        ASSERT_EQUALS(it->ctype, meta->getColumnType(1));

        ASSERT_EQUALS(it->name, meta->getColumnTypeName(1));

        sql.str("");
        sql << "... OK, SQL:" << it->sqldef << " -> Type = " << it->name;
        sql << " (Code = " << it->ctype << ")";
        logMsg(sql.str());

        restypes->beforeFirst();
        type_found=false;
        while (restypes->next())
        {
          if (restypes->getInt("DATA_TYPE") == it->ctype)
          {
            type_found=true;
            break;
          }
        }
        if (!type_found)
        {
          sql.str("");
          sql << "The type code " << it->ctype << " seems not to be in the type list ";
          sql << "returned by DatabaseMetaData::getTypeInfo().";
          logMsg(sql.str());
          FAIL("Wrong type code");
        }

        sql.str("");
        sql << "... OK type name is known by DatabaseMetaData::getTypeInfo()";

        restypes->beforeFirst();
        type_found=false;
        while (restypes->next())
        {
          if (restypes->getString("TYPE_NAME") == std::string(it->name))
          {
            type_found=true;
            break;
          }
        }
        if (!type_found)
        {
          sql.str("");
          sql << "The type name " << it->name << " seems not to be in the type list ";
          sql << "returned by DatabaseMetaData::getTypeInfo().";
          logMsg(sql.str());
          FAIL("Wrong type name");
        }

        sql.str("");
        sql << "... OK type name is known by DatabaseMetaData::getTypeInfo()";

      }
      catch (sql::SQLException &e)
      {
        logMsg(sql.str());
        sql.str("");
        sql << "... skipping " << it->name << " " << it->sqldef << ": ";
        sql << e.what();
        logMsg(sql.str());
      }

    }

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

}

void resultsetmetadata::getPrecision()
{
  logMsg("resultsetmetadata::getPrecision() - MySQL_ResultSetMetaData::getPrecision");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    runStandardQuery();
    doGetPrecision(false);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doGetPrecision(true);


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doGetPrecision(bool is_ps)
{
  ResultSetMetaData * meta=res->getMetaData();

  ASSERT_GT((unsigned int) 4, meta->getPrecision(1));
  ASSERT_GT((unsigned int) 0, meta->getPrecision(2));
  ASSERT_GT((unsigned int) 4, meta->getPrecision(3));
  ASSERT_GT((unsigned int) 0, meta->getPrecision(4));
  ASSERT_GT((unsigned int) 2, meta->getPrecision(5));

  try
  {
    meta->getPrecision(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->getPrecision(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::getScale()
{
  logMsg("resultsetmetadata::getScale() - MySQL_ResultSetMetaData::getScale");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    runStandardQuery();
    doGetScale(false);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doGetScale(true);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doGetScale(bool is_ps)
{
  ResultSetMetaData * meta=res->getMetaData();

  try
  {
    meta->getScale(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->getScale(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::getSchemaName()
{
  logMsg("resultsetmetadata::getSchemaName() - MySQL_ResultSetMetaData::getSchemaName");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT)");
    stmt->execute("INSERT INTO test(id) VALUES (1)");
    res.reset(stmt->executeQuery("SELECT * FROM test"));
    checkResultSetScrolling(res);
    doGetSchemaName(false);

    logMsg("... PreparedStatement");
    pstmt.reset(con->prepareStatement("SELECT * FROM test"));
    res.reset(pstmt->executeQuery());
    checkResultSetScrolling(res);
    doGetSchemaName(true);

    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doGetSchemaName(bool is_ps)
{
  int i;
  ResultSetMetaData * meta2=res->getMetaData();
  ASSERT_EQUALS(meta2->getSchemaName(1), con->getSchema());

  runStandardQuery();
  ResultSetMetaData * meta=res->getMetaData();
  for (i=1; i < 6; i++)
    ASSERT_EQUALS(meta->getSchemaName(i), "");

  try
  {
    meta->getSchemaName(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->getSchemaName(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::getTableName()
{
  logMsg("resultsetmetadata::getTableName() - MySQL_ResultSetMetaData::getTableName");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT)");
    stmt->execute("INSERT INTO test(id) VALUES (1)");
    res.reset(stmt->executeQuery("SELECT * FROM test"));
    checkResultSetScrolling(res);

    logMsg("... Statement");
    doGetTableName(false);

    logMsg("... PreparedStatement");
    pstmt.reset(con->prepareStatement("SELECT * FROM test"));
    res.reset(pstmt->executeQuery());
    checkResultSetScrolling(res);
    doGetTableName(true);

    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doGetTableName(bool is_ps)
{
  int i;
  ResultSetMetaData * meta2=res->getMetaData();
  ASSERT_EQUALS(meta2->getTableName(1), "test");

  runStandardQuery();
  ResultSetMetaData * meta=res->getMetaData();
  for (i=1; i < 6; i++)
    ASSERT_EQUALS(meta->getTableName(i), "");

  try
  {
    meta->getTableName(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->getTableName(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }

}

void resultsetmetadata::isAutoIncrement()
{
  logMsg("resultsetmetadata::isAutoIncrement() - MySQL_ResultSetMetaData::isAutoIncrement");

  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, col1 CHAR(1))");
    stmt->execute("INSERT INTO test(id, col1) VALUES (1, 'a')");

    logMsg("... Statement");
    res.reset(stmt->executeQuery("SELECT id, col1 FROM test"));
    checkResultSetScrolling(res);
    ResultSetMetaData * meta2=res->getMetaData();
    ASSERT_EQUALS(meta2->isAutoIncrement(1), true);
    ASSERT_EQUALS(meta2->isAutoIncrement(2), false);

    runStandardQuery();
    doIsAutoIncrement(false);

    logMsg("... PreparedStatement");
    pstmt.reset(con->prepareStatement("SELECT id, col1 FROM test"));
    res.reset(pstmt->executeQuery());
    checkResultSetScrolling(res);
    meta2=(res->getMetaData());
    ASSERT_EQUALS(meta2->isAutoIncrement(1), true);
    ASSERT_EQUALS(meta2->isAutoIncrement(2), false);

    runStandardPSQuery();
    doIsAutoIncrement(true);

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doIsAutoIncrement(bool is_ps)
{
  int i;

  ResultSetMetaData * meta=res->getMetaData();
  for (i=1; i < 6; i++)
    ASSERT_EQUALS(meta->isAutoIncrement(i), false);

  try
  {
    meta->isAutoIncrement(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->isAutoIncrement(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::isCaseSensitive()
{
  logMsg("resultsetmetadata::isCaseSensitive() - MySQL_ResultSetMetaData::isCaseSensitive");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, col1 CHAR(1), col2 CHAR(10) CHARACTER SET 'utf8' COLLATE 'utf8_bin')");
    stmt->execute("INSERT INTO test(id, col1, col2) VALUES (1, 'a', 'b')");

    logMsg("... Statement");
    res.reset(stmt->executeQuery("SELECT id, col1, col2 FROM test"));
    checkResultSetScrolling(res);
    ResultSetMetaData * meta2=res->getMetaData();
    ASSERT_EQUALS(meta2->isCaseSensitive(1), false);
    ASSERT_EQUALS(meta2->isCaseSensitive(2), false);
    /*
     NOTE: There is no reliable way to detect CI/CS
    if ( !TestsRunner::getStartOptions()->getBool( "dont-use-is" ) )
    {
      // connection_collation distorts the collation of the results (character_set_results) doesn't help
      // and thus we can't say for sure whether the original column was CI or CS. Only I_S.COLUMNS can tell us.
      ASSERT_EQUALS(meta2->isCaseSensitive(3), true);
    }
    else
    {
      logMsg("... skipping 'collate_bin' test because we don't use I_S");
    }
     */

    logMsg("... PreparedStatement");
    pstmt.reset(con->prepareStatement("SELECT id, col1, col2 FROM test"));
    res.reset(pstmt->executeQuery());
    checkResultSetScrolling(res);
    meta2=(res->getMetaData());
    ASSERT_EQUALS(meta2->isCaseSensitive(1), false);
    ASSERT_EQUALS(meta2->isCaseSensitive(2), false);
    /*
    if (!TestsRunner::getStartOptions()->getBool( "dont-use-is" ))
    {
      // connection_collation distorts the collation of the results (character_set_results) doesn't help
      // and thus we can't say for sure whether the original column was CI or CS. Only I_S.COLUMNS can tell us.
      ASSERT_EQUALS(meta2->isCaseSensitive(3), true);
    }
     */
    logMsg("... Statement");
    runStandardQuery();
    doIsCaseSensitive(false);

    stmt->execute("SET @old_charset_res=@@session.character_set_results");
    stmt->execute("SET character_set_results=NULL");
    res.reset(stmt->executeQuery("SELECT id, col1, col2 FROM test"));
    checkResultSetScrolling(res);
    meta2=(res->getMetaData());
    ASSERT_EQUALS(meta2->isCaseSensitive(1), false);
    ASSERT_EQUALS(meta2->isCaseSensitive(2), false);
    ASSERT_EQUALS(meta2->isCaseSensitive(3), true);
    stmt->execute("SET character_set_results=@old_charset_res");

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doIsCaseSensitive(true);

    stmt->execute("SET @old_charset_res=@@session.character_set_results");
    stmt->execute("SET character_set_results=NULL");
    pstmt.reset(con->prepareStatement("SELECT id, col1, col2 FROM test"));
    res.reset(pstmt->executeQuery());
    checkResultSetScrolling(res);
    meta2=(res->getMetaData());
    ASSERT_EQUALS(meta2->isCaseSensitive(1), false);
    ASSERT_EQUALS(meta2->isCaseSensitive(2), false);
    ASSERT_EQUALS(meta2->isCaseSensitive(3), true);
    stmt->execute("SET character_set_results=@old_charset_res");


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doIsCaseSensitive(bool is_ps)
{
  int i;
  ResultSetMetaData * meta=res->getMetaData();
  for (i=1; i < 5; i++)
    ASSERT_EQUALS(meta->isCaseSensitive(i), false);

  try
  {
    meta->isCaseSensitive(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->isCaseSensitive(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::isZerofill()
{
  logMsg("resultsetmetadata::isZerofill() - MySQL_ResultSetMetaData::isZerofill");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, col1 INT ZEROFILL)");
    stmt->execute("INSERT INTO test(id, col1) VALUES (1, 1)");

    logMsg("... Statement");
    res.reset(stmt->executeQuery("SELECT id, col1 FROM test"));
    checkResultSetScrolling(res);
    ResultSetMetaData * meta2=res->getMetaData();
    ASSERT_EQUALS(meta2->isZerofill(1), false);
    ASSERT_EQUALS(meta2->isZerofill(2), true);

    logMsg("... PreparedStatement");
    pstmt.reset(con->prepareStatement("SELECT id, col1 FROM test"));
    res.reset(pstmt->executeQuery());
    checkResultSetScrolling(res);
    meta2=(res->getMetaData());
    ASSERT_EQUALS(meta2->isZerofill(1), false);
    ASSERT_EQUALS(meta2->isZerofill(2), true);

    logMsg("... Statement");
    runStandardQuery();
    doIsZerofill(false);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doIsZerofill(true);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doIsZerofill(bool is_ps)
{
  int i;
  ResultSetMetaData * meta=res->getMetaData();
  for (i=1; i < 5; i++)
    ASSERT_EQUALS(meta->isZerofill(i), false);

  try
  {
    meta->isZerofill(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->isZerofill(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::isCurrency()
{
  logMsg("resultsetmetadata::isCurrency() - MySQL_ResultSetMetaData::isCurrency");

  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    runStandardQuery();
    doIsCurrency(false);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doIsCurrency(true);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doIsCurrency(bool is_ps)
{
  int i;
  ResultSetMetaData * meta=res->getMetaData();
  for (i=1; i < 6; i++)
    ASSERT_EQUALS(meta->isCurrency(i), false);

  try
  {
    meta->isCurrency(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->isCurrency(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::isDefinitelyWritable()
{
  logMsg("resultsetmetadata::isDefinitelyWritable() - MySQL_ResultSetMetaData::isDefinitelyWritable");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    runStandardQuery();
    doIsDefinitelyWritable(false);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doIsDefinitelyWritable(true);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doIsDefinitelyWritable(bool is_ps)
{
  int i;
  ResultSetMetaData * meta=res->getMetaData();
  for (i=1; i < 6; i++)
  {
    ASSERT_EQUALS(meta->isDefinitelyWritable(i), false);
    ASSERT_EQUALS(meta->isWritable(i), false);
    ASSERT_EQUALS(meta->isReadOnly(i), true);
  }

  try
  {
    meta->isDefinitelyWritable(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->isDefinitelyWritable(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::isNullable()
{
  logMsg("resultsetmetadata::isNullable() - MySQL_ResultSetMetaData::isNullable");

  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    runStandardQuery();
    doIsNullable(false);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doIsNullable(true);

    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT, col1 CHAR(1) DEFAULT NULL, col2 CHAR(10) NOT NULL)");
    stmt->execute("INSERT INTO test(id, col2) VALUES (1, 'b')");

    logMsg("... Statement");
    res.reset(stmt->executeQuery("SELECT id, col1, col2 FROM test"));
    checkResultSetScrolling(res);
    ResultSetMetaData * meta2=res->getMetaData();
    ASSERT_EQUALS(meta2->isNullable(1), sql::ResultSetMetaData::columnNullable);
    ASSERT_EQUALS(meta2->isNullable(2), sql::ResultSetMetaData::columnNullable);
    ASSERT_EQUALS(meta2->isNullable(3), sql::ResultSetMetaData::columnNoNulls);

    logMsg("... PreparedStatement");
    pstmt.reset(con->prepareStatement("SELECT id, col1, col2 FROM test"));
    res.reset(pstmt->executeQuery());
    checkResultSetScrolling(res);
    meta2=(res->getMetaData());
    ASSERT_EQUALS(meta2->isNullable(1), sql::ResultSetMetaData::columnNullable);
    ASSERT_EQUALS(meta2->isNullable(2), sql::ResultSetMetaData::columnNullable);
    ASSERT_EQUALS(meta2->isNullable(3), sql::ResultSetMetaData::columnNoNulls);

    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doIsNullable(bool is_ps)
{
  int i;
  ResultSetMetaData * meta=res->getMetaData();
  for (i=1; i < 6; i++)
    ASSERT_EQUALS(meta->isNullable(i), sql::ResultSetMetaData::columnNoNulls);

  try
  {
    meta->isNullable(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->isNullable(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::isReadOnly()
{
  logMsg("resultsetmetadata::isReadOnly() - MySQL_ResultSetMetaData::isReadOnly");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    runStandardQuery();
    doIsReadOnly(false);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doIsReadOnly(true);

    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT, col1 CHAR(1), col2 CHAR(10))");
    stmt->execute("INSERT INTO test(id, col1, col2) VALUES (1, 'a', 'b')");

    logMsg("... Statement");
    res.reset(stmt->executeQuery("SELECT id AS 'abc', col1, col2, 1 FROM test"));
    checkResultSetScrolling(res);
    ResultSetMetaData * meta2=res->getMetaData();
    ASSERT_EQUALS(meta2->isReadOnly(1), false);
    ASSERT_EQUALS(meta2->isReadOnly(2), false);
    ASSERT_EQUALS(meta2->isReadOnly(3), false);
    ASSERT_EQUALS(meta2->isReadOnly(4), true);

    try
    {
      stmt->execute("DROP VIEW IF EXISTS v_test");
      stmt->execute("CREATE VIEW v_test(col1, col2) AS SELECT id, id + 1 FROM test");
      res.reset(stmt->executeQuery("SELECT col1, col2 FROM v_test"));
      checkResultSetScrolling(res);
      ResultSetMetaData * meta3=res->getMetaData();
      ASSERT_EQUALS(meta3->isReadOnly(1), false);
      /* Expecting ERROR 1348 (HY000): Column 'col2' is not updatable */
      ASSERT_EQUALS(meta3->isReadOnly(2), true);
    }
    catch (sql::SQLException &)
    {
      logMsg("... skipping VIEW test");
    }

    logMsg("... PreparedStatement");
    pstmt.reset(con->prepareStatement("SELECT id AS 'abc', col1, col2, 1 FROM test"));
    res.reset(pstmt->executeQuery());
    checkResultSetScrolling(res);
    meta2=(res->getMetaData());
    ASSERT_EQUALS(meta2->isReadOnly(1), false);
    ASSERT_EQUALS(meta2->isReadOnly(2), false);
    ASSERT_EQUALS(meta2->isReadOnly(3), false);
    ASSERT_EQUALS(meta2->isReadOnly(4), true);

    try
    {
      stmt->execute("DROP VIEW IF EXISTS v_test");
      stmt->execute("CREATE VIEW v_test(col1, col2) AS SELECT id, id + 1 FROM test");
      pstmt.reset(con->prepareStatement("SELECT col1, col2 FROM v_test"));
      res.reset(pstmt->executeQuery());
      checkResultSetScrolling(res);
      ResultSetMetaData * meta3=res->getMetaData();
      ASSERT_EQUALS(meta3->isReadOnly(1), false);
      /* Expecting ERROR 1348 (HY000): Column 'col2' is not updatable */
      ASSERT_EQUALS(meta3->isReadOnly(2), true);
      stmt->execute("DROP VIEW IF EXISTS v_test");
    }
    catch (sql::SQLException &)
    {
      logMsg("... skipping VIEW test");
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

void resultsetmetadata::doIsReadOnly(bool is_ps)
{
  int i;
  ResultSetMetaData * meta=res->getMetaData();
  for (i=1; i < 6; i++)
    ASSERT_EQUALS(meta->isReadOnly(i), true);

  try
  {
    meta->isReadOnly(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->isReadOnly(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::isSearchable()
{
  logMsg("resultsetmetadata::isSearchable() - MySQL_ResultSetMetaData::isSearchable");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    runStandardQuery();
    doIsSearchable(false);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doIsSearchable(true);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doIsSearchable(bool is_ps)
{
  int i;
  ResultSetMetaData * meta=res->getMetaData();
  for (i=1; i < 6; i++)
    ASSERT_EQUALS(meta->isSearchable(i), true);

  try
  {
    meta->isSearchable(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->isSearchable(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}

void resultsetmetadata::isSigned()
{
  logMsg("resultsetmetadata::isSigned() - MySQL_ResultSetMetaData::isSigned");

  //TODO: Enable it after fixing
  SKIP("Removed untill fixed");

  std::stringstream sql;
  std::vector<columndefinition>::iterator it;
  ResultSetMetaData * meta_st;
  ResultSetMetaData * meta_ps;
  ResultSet res_ps;

  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    runStandardQuery();
    doIsSigned(false);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doIsSigned(true);

    stmt.reset(con->createStatement());
    for (it=columns.begin(); it != columns.end(); it++)
    {
      stmt->execute("DROP TABLE IF EXISTS test");
      sql.str("");
      sql << "CREATE TABLE test(col1 " << it->sqldef << ")";
      try
      {
        stmt->execute(sql.str());
        sql.str("");
        sql << "INSERT INTO test(col1) VALUES ('" << it->value << "')";
        stmt->execute(sql.str());
        sql.str("");
        sql << std::boolalpha << "... testing, SQL:" << it->sqldef << " -> Signed = " << it->is_signed;
        logMsg(sql.str());
      }
      catch (sql::SQLException &e)
      {
        logMsg(sql.str());
        sql.str("");
        sql << "... skipping " << it->name << " " << it->sqldef << ": ";
        sql << e.what();
        logMsg(sql.str());
        continue;
      }

      res.reset(stmt->executeQuery("SELECT col1 FROM test"));
      checkResultSetScrolling(res);
      meta_st=(res->getMetaData());

      pstmt.reset(con->prepareStatement("SELECT col1 FROM test"));
      res_ps.reset(pstmt->executeQuery());
      meta_ps=(res_ps->getMetaData());

      ASSERT_EQUALS(meta_st->isSigned(1), meta_ps->isSigned(1));
      ASSERT_EQUALS(it->is_signed, meta_st->isSigned(1));
    }
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doIsSigned(bool is_ps)
{
  int i;
  ResultSetMetaData * meta=res->getMetaData();

  for (i=1; i < 5; i++)
  {
    ASSERT_EQUALS(meta->isSigned(i), true);
  }

  try
  {
    meta->isSigned(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->isSigned(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }

}

void resultsetmetadata::isWritable()
{
  logMsg("resultsetmetadata::isWritable() - MySQL_ResultSetMetaData::isWritable");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    logMsg("... Statement");
    runStandardQuery();
    doIsWritable(false);

    logMsg("... PreparedStatement");
    runStandardPSQuery();
    doIsWritable(true);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::doIsWritable(bool is_ps)
{
  ResultSetMetaData * meta=res->getMetaData();
  /* NOTE: isReadable covers isWritable */
  try
  {
    meta->isWritable(6);
    FAIL("Invalid offset 6 not recognized");
  }
  catch (sql::SQLException &)
  {
  }

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->isWritable(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}


void resultsetmetadata::getColumnCharset()
{
  logMsg("resultsetmetadata::getColumnCharset() - MySQL_ResultSetMetaData::getColumnCharset()");
  try
  {
  stmt.reset(con->createStatement());
  stmt->execute("DROP TABLE IF EXISTS columnCharset");
  stmt->execute("CREATE TABLE columnCharset(col1 VARCHAR(10) CHARACTER SET latin1 COLLATE latin1_general_ci)");
  stmt->execute("INSERT INTO columnCharset VALUES ('cal1val')");
  stmt->execute("SET @@character_set_results=NULL");

  /* This is a dull test, its about code coverage not achieved with the JDBC tests */
  logMsg("... Statement");
  res.reset(stmt->executeQuery("SELECT col1 from columnCharset"));
  doGetColumnCharset(false);

  logMsg("... PreparedStatement");
  pstmt.reset(con->prepareStatement("SELECT col1 from columnCharset"));
  res.reset(pstmt->executeQuery());
  checkResultSetScrolling(res);
  doGetColumnCharset(true);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void resultsetmetadata::doGetColumnCharset(bool is_ps)
{
  ResultSetMetaData * meta=res->getMetaData();
  ASSERT_EQUALS("latin1", meta->getColumnCharset(1));

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->getCatalogName(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}


void resultsetmetadata::getColumnCollation()
{
  logMsg("resultsetmetadata::getColumnCollation() - MySQL_ResultSetMetaData::getColumnCollation()");
  try
  {
  stmt.reset(con->createStatement());
  stmt->execute("DROP TABLE IF EXISTS columnCollation");
  stmt->execute("CREATE TABLE columnCollation(col1 VARCHAR(10) COLLATE latin1_general_ci)");
  stmt->execute("INSERT INTO columnCollation VALUES ('cal1val')");
  stmt->execute("SET @@character_set_results=NULL");

  /* This is a dull test, its about code coverage not achieved with the JDBC tests */
  logMsg("... Statement");
  res.reset(stmt->executeQuery("SELECT col1 from columnCollation"));
  doGetColumnCollation(false);

  logMsg("... PreparedStatement");
  pstmt.reset(con->prepareStatement("SELECT col1 from columnCollation"));
  res.reset(pstmt->executeQuery());
  checkResultSetScrolling(res);
  doGetColumnCollation(true);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void resultsetmetadata::doGetColumnCollation(bool is_ps)
{
  ResultSetMetaData * meta=res->getMetaData();
  ASSERT_EQUALS("latin1_general_ci", meta->getColumnCollation(1));

  if (!is_ps)
  {
    res->close();
    try
    {
      meta->getColumnCollation(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &)
    {
    }
  }
}


void resultsetmetadata::runStandardQuery()
{
  stmt.reset(con->createStatement());
  res.reset(stmt->executeQuery("SELECT 'Hello' AS a, ' ', 'world', '!', 123 AS z"));
  checkResultSetScrolling(res);
}

void resultsetmetadata::runStandardPSQuery()
{
  pstmt.reset(con->prepareStatement("SELECT 'Hello' AS a, ' ', 'world', '!', 123 AS z"));
  res.reset(pstmt->executeQuery());
  checkResultSetScrolling(res);
}

} /* namespace resultsetmetadata */
} /* namespace testsuite */
