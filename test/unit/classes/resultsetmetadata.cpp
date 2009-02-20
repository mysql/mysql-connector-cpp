/*
Copyright 2008 - 2009 Sun Microsystems, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

There are special exceptions to the terms and conditions of the GPL
as it is applied to this software. View the full text of the
exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
software distribution.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <cppconn/warning.h>

#include "resultsetmetadata.h"
#include <sstream>
#include <stdlib.h>
#include <cppconn/resultset.h>
#include <cppconn/datatype.h>

namespace testsuite
{
namespace classes
{

void resultsetmetadata::getCatalogName()
{
  logMsg("resultsetmetadata::getCatalogName() - MySQL_ResultSetMetaData::getCatalogName");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    ASSERT_EQUALS(con->getCatalog(), meta->getCatalogName(1));

    try
    {
      meta->getCatalogName(0);
      FAIL("Column number starts at 1, invalid offset 0 not detected");

    } catch (sql::InvalidArgumentException &)
    {
    }

    try
    {
      meta->getCatalogName(6);
      FAIL("Only five columns available but requesting number six, should bail");

    } catch (sql::InvalidArgumentException &)
    {
    }

    res->close();
    try
    {
      meta->getCatalogName(1);
      FAIL("Can fetch meta from invalid resultset");

    } catch (sql::SQLException &)
    {
    }

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::getColumnCount()
{
  logMsg("resultsetmetadata::getColumnCount() - MySQL_ResultSetMetaData::getColumnCount");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    ASSERT_EQUALS((unsigned int) 5, meta->getColumnCount());

    res->close();
    try
    {
      meta->getCatalogName(1);
      FAIL("Can fetch meta from invalid resultset");

    } catch (sql::SQLException &)
    {
    }


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::getColumnDisplaySize()
{
  logMsg("resultsetmetadata::getColumnDisplaySize() - MySQL_ResultSetMetaData::getColumnDisplaySize()");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    ASSERT_EQUALS((unsigned int) 5, meta->getColumnDisplaySize(1));
    ASSERT_EQUALS((unsigned int) 1, meta->getColumnDisplaySize(2));
    ASSERT_EQUALS((unsigned int) 5, meta->getColumnDisplaySize(3));
    ASSERT_EQUALS((unsigned int) 1, meta->getColumnDisplaySize(4));
    ASSERT_EQUALS((unsigned int) 3, meta->getColumnDisplaySize(5));

    try
    {
      meta->getColumnDisplaySize(0);
      FAIL("Column number starts at 1, invalid offset 0 not detected");

    } catch (sql::InvalidArgumentException &)
    {
    }

    try
    {
      meta->getColumnDisplaySize(6);
      FAIL("Only five columns available but requesting number six, should bail");

    } catch (sql::InvalidArgumentException &)
    {
    }

    res->close();
    try
    {
      meta->getColumnDisplaySize(1);
      FAIL("Can fetch meta from invalid resultset");

    } catch (sql::SQLException &)
    {
    }


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::getColumnNameAndLabel()
{
  logMsg("resultsetmetadata::getColumnName() - MySQL_ResultSetMetaData::getColumnName(), MySQL_ResultSetMetaData::getColumnLabel()");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    ASSERT_EQUALS("a", meta->getColumnName(1));
    ASSERT_EQUALS(meta->getColumnLabel(1), meta->getColumnName(1));
    /* NOTE: " " -> "" */
    ASSERT_EQUALS("", meta->getColumnName(2));
    ASSERT_EQUALS(meta->getColumnLabel(2), meta->getColumnName(2));
    ASSERT_EQUALS("world", meta->getColumnName(3));
    ASSERT_EQUALS(meta->getColumnLabel(3), meta->getColumnName(3));
    ASSERT_EQUALS("!", meta->getColumnName(4));
    ASSERT_EQUALS(meta->getColumnLabel(4), meta->getColumnName(4));
    ASSERT_EQUALS("z", meta->getColumnName(5));
    ASSERT_EQUALS(meta->getColumnLabel(5), meta->getColumnName(5));

    try
    {
      meta->getColumnName(0);
      FAIL("Column number starts at 1, invalid offset 0 not detected");

    } catch (sql::InvalidArgumentException &)
    {
    }

    try
    {
      meta->getColumnLabel(0);
      FAIL("Column number starts at 1, invalid offset 0 not detected");

    } catch (sql::InvalidArgumentException &)
    {
    }

    try
    {
      meta->getColumnName(6);
      FAIL("Only five columns available but requesting number six, should bail");

    } catch (sql::InvalidArgumentException &)
    {
    }

    try
    {
      meta->getColumnLabel(6);
      FAIL("Only five columns available but requesting number six, should bail");

    } catch (sql::InvalidArgumentException &)
    {
    }

    res->close();
    try
    {
      meta->getColumnName(1);
      FAIL("Can fetch meta from invalid resultset");

    } catch (sql::SQLException &)
    {
    }

    try
    {
      meta->getColumnLabel(1);
      FAIL("Can fetch meta from invalid resultset");

    } catch (sql::SQLException &)
    {
    }

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::getColumnType()
{
  logMsg("resultsetmetadata::getColumnType() - MySQL_ResultSetMetaData::getColumnType()");
  try
  {
    std::stringstream sql;
    std::vector<columndefinition>::iterator it;
    stmt.reset(con->createStatement());
    bool type_found=false;

    DatabaseMetaData dbmeta(con->getMetaData());
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
        ResultSetMetaData meta(res->getMetaData());
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
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }

}

void resultsetmetadata::getPrecision()
{
  logMsg("resultsetmetadata::getPrecision() - MySQL_ResultSetMetaData::getPrecision");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());

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
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->getPrecision(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::getScale()
{
  logMsg("resultsetmetadata::getScale() - MySQL_ResultSetMetaData::getScale");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());

    try
    {
      meta->getScale(6);
      FAIL("Invalid offset 6 not recognized");
    }
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->getScale(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::getSchemaName()
{
  logMsg("resultsetmetadata::getSchemaName() - MySQL_ResultSetMetaData::getSchemaName");
  int i;

  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT)");
    stmt->execute("INSERT INTO test(id) VALUES (1)");
    res.reset(stmt->executeQuery("SELECT * FROM test"));
    ResultSetMetaData meta2(res->getMetaData());
    ASSERT_EQUALS(meta2->getSchemaName(1), con->getSchema());

    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    for (i=1; i < 6; i++)
      ASSERT_EQUALS(meta->getSchemaName(i), "");

    try
    {
      meta->getSchemaName(6);
      FAIL("Invalid offset 6 not recognized");
    }
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->getSchemaName(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::getTableName()
{
  logMsg("resultsetmetadata::getTableName() - MySQL_ResultSetMetaData::getTableName");
  int i;

  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT)");
    stmt->execute("INSERT INTO test(id) VALUES (1)");
    res.reset(stmt->executeQuery("SELECT * FROM test"));
    ResultSetMetaData meta2(res->getMetaData());
    ASSERT_EQUALS(meta2->getTableName(1), "test");

    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    for (i=1; i < 6; i++)
      ASSERT_EQUALS(meta->getTableName(i), "");

    try
    {
      meta->getTableName(6);
      FAIL("Invalid offset 6 not recognized");
    }
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->getTableName(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::isAutoIncrement()
{
  logMsg("resultsetmetadata::isAutoIncrement() - MySQL_ResultSetMetaData::isAutoIncrement");
  int i;
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, col1 CHAR(1))");
    stmt->execute("INSERT INTO test(id, col1) VALUES (1, 'a')");
    res.reset(stmt->executeQuery("SELECT id, col1 FROM test"));
    ResultSetMetaData meta2(res->getMetaData());
    ASSERT_EQUALS(meta2->isAutoIncrement(1), true);
    ASSERT_EQUALS(meta2->isAutoIncrement(2), false);

    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    for (i=1; i < 6; i++)
      ASSERT_EQUALS(meta->isAutoIncrement(i), false);

    try
    {
      meta->isAutoIncrement(6);
      FAIL("Invalid offset 6 not recognized");
    }
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->isAutoIncrement(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::isCaseSensitive()
{
  logMsg("resultsetmetadata::isCaseSensitive() - MySQL_ResultSetMetaData::isCaseSensitive");
  int i;
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT NOT NULL PRIMARY KEY AUTO_INCREMENT, col1 CHAR(1), col2 CHAR(10) CHARACTER SET 'utf8' COLLATE 'utf8_bin')");
    stmt->execute("INSERT INTO test(id, col1, col2) VALUES (1, 'a', 'b')");
    res.reset(stmt->executeQuery("SELECT id, col1, col2 FROM test"));
    ResultSetMetaData meta2(res->getMetaData());
    ASSERT_EQUALS(meta2->isCaseSensitive(1), false);
    ASSERT_EQUALS(meta2->isCaseSensitive(2), false);
#if A0
    // connection_collation distorts the collation of the results (character_set_results) doesn't help
    // and thus we can't say for sure whether the original column was CI or CS. Only I_S.COLUMNS can tell us.
    ASSERT_EQUALS(meta2->isCaseSensitive(3), true);
#endif
    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    for (i=1; i < 5; i++)
      ASSERT_EQUALS(meta->isCaseSensitive(i), false);

    try
    {
      meta->isCaseSensitive(6);
      FAIL("Invalid offset 6 not recognized");
    }
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->isCaseSensitive(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }

    /* TODO: is this correct? */
    stmt->execute("SET @old_charset_res=@@session.character_set_results");
    stmt->execute("SET character_set_results=NULL");
    res.reset(stmt->executeQuery("SELECT id, col1, col2 FROM test"));
    ResultSetMetaData meta3(res->getMetaData());
    ASSERT_EQUALS(meta3->isCaseSensitive(1), false);
    ASSERT_EQUALS(meta3->isCaseSensitive(2), false);
    ASSERT_EQUALS(meta3->isCaseSensitive(3), true);
    stmt->execute("SET character_set_results=@old_charset_res");


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    printf("%s", e.what());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::isCurrency()
{
  logMsg("resultsetmetadata::isCurrency() - MySQL_ResultSetMetaData::isCurrency");
  int i;
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */


    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    for (i=1; i < 6; i++)
      ASSERT_EQUALS(meta->isCurrency(i), false);

    try
    {
      meta->isCurrency(6);
      FAIL("Invalid offset 6 not recognized");
    }
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->isCurrency(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::isDefinitelyWritable()
{
  logMsg("resultsetmetadata::isDefinitelyWritable() - MySQL_ResultSetMetaData::isDefinitelyWritable");
  int i;
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */

    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
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
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->isDefinitelyWritable(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::isNullable()
{
  logMsg("resultsetmetadata::isNullable() - MySQL_ResultSetMetaData::isNullable");
  int i;
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    for (i=1; i < 6; i++)
      ASSERT_EQUALS(meta->isNullable(i), sql::ResultSetMetaData::columnNoNulls);

    try
    {
      meta->isNullable(6);
      FAIL("Invalid offset 6 not recognized");
    }
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->isNullable(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }

    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT, col1 CHAR(1) DEFAULT NULL, col2 CHAR(10) NOT NULL)");
    stmt->execute("INSERT INTO test(id, col2) VALUES (1, 'b')");
    res.reset(stmt->executeQuery("SELECT id, col1, col2 FROM test"));
    ResultSetMetaData meta2(res->getMetaData());
    ASSERT_EQUALS(meta2->isNullable(1), sql::ResultSetMetaData::columnNullable);
    ASSERT_EQUALS(meta2->isNullable(2), sql::ResultSetMetaData::columnNullable);
    ASSERT_EQUALS(meta2->isNullable(3), sql::ResultSetMetaData::columnNoNulls);

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::isReadOnly()
{
  logMsg("resultsetmetadata::isReadOnly() - MySQL_ResultSetMetaData::isReadOnly");
  int i;
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    for (i=1; i < 6; i++)
      ASSERT_EQUALS(meta->isReadOnly(i), true);

    try
    {
      meta->isReadOnly(6);
      FAIL("Invalid offset 6 not recognized");
    }
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->isReadOnly(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }

    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT, col1 CHAR(1), col2 CHAR(10))");
    stmt->execute("INSERT INTO test(id, col1, col2) VALUES (1, 'a', 'b')");
    res.reset(stmt->executeQuery("SELECT id AS 'abc', col1, col2, 1 FROM test"));
    ResultSetMetaData meta2(res->getMetaData());
    ASSERT_EQUALS(meta2->isReadOnly(1), false);
    ASSERT_EQUALS(meta2->isReadOnly(2), false);
    ASSERT_EQUALS(meta2->isReadOnly(3), false);
    ASSERT_EQUALS(meta2->isReadOnly(4), true);

    try
    {
      stmt->execute("DROP VIEW IF EXISTS v_test");
      stmt->execute("CREATE VIEW v_test(col1, col2) AS SELECT id, id + 1 FROM test");
      res.reset(stmt->executeQuery("SELECT col1, col2 FROM v_test"));
      ResultSetMetaData meta3(res->getMetaData());
      ASSERT_EQUALS(meta3->isReadOnly(1), false);
      /* Expecting ERROR 1348 (HY000): Column 'col2' is not updatable */
      ASSERT_EQUALS(meta3->isReadOnly(2), true);
    }
    catch (sql::SQLException &e)
    {
      logMsg("... skipping VIEW test");
    }

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::isSearchable()
{
  logMsg("resultsetmetadata::isSearchable() - MySQL_ResultSetMetaData::isSearchable");
  int i;
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    for (i=1; i < 6; i++)
      ASSERT_EQUALS(meta->isSearchable(i), true);

    try
    {
      meta->isSearchable(6);
      FAIL("Invalid offset 6 not recognized");
    }
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->isSearchable(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::isSigned()
{
  logMsg("resultsetmetadata::isSigned() - MySQL_ResultSetMetaData::isSigned");
  int i;  
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());

    for (i=1; i < 5; i++) {      
      ASSERT_EQUALS(meta->isSigned(i), true);
    }

    try
    {
      meta->isSigned(6);
      FAIL("Invalid offset 6 not recognized");
    }
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->isSigned(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }

    std::stringstream sql;    
    std::vector<columndefinition>::iterator it;
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

        res.reset(stmt->executeQuery("SELECT * FROM test"));
        ResultSetMetaData meta(res->getMetaData());
        sql.str("");
        sql << std::boolalpha << "... testing, SQL:" << it->sqldef << " -> Signed = " << it->is_signed;
        logMsg(sql.str());
        /* TODO: the test needs to be tweaked!!! */
        ASSERT_EQUALS(it->is_signed, meta->isSigned(1));    

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
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }


}

void resultsetmetadata::isWritable()
{
  logMsg("resultsetmetadata::isWritable() - MySQL_ResultSetMetaData::isWritable");
  try
  {
    /* This is a dull test, its about code coverage not achieved with the JDBC tests */
    runStandardQuery();
    ResultSetMetaData meta(res->getMetaData());
    /* NOTE: isReadable covers isWritable */

    try
    {
      meta->isWritable(6);
      FAIL("Invalid offset 6 not recognized");
    }
    catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->isWritable(1);
      FAIL("Can fetch meta from invalid resultset");
    }
    catch (sql::SQLException &e)
    {
    }

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void resultsetmetadata::runStandardQuery()
{
  stmt.reset(con->createStatement());
  res.reset(stmt->executeQuery("SELECT 'Hello' AS a, ' ', 'world', '!', 123 AS z"));
}

} /* namespace resultsetmetadata */
} /* namespace testsuite */
