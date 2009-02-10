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

void resultsetmetadata::init()
{
  /* DO NOT test for unsinged/signed in the column names.
   Server and libmysql return whatever they want in the flags depending on the version.
   Its a madness. */
  columns.push_back(columndefinition("BIT", "BIT", sql::DataType::BIT, "0", true));
  columns.push_back(columndefinition("BIT", "BIT(8)", sql::DataType::BIT, "0", false));
  columns.push_back(columndefinition("TINYINT", "TINYINT", sql::DataType::TINYINT, "127", true));
  columns.push_back(columndefinition("TINYINT", "TINYINT(1)", sql::DataType::TINYINT, "0", false));
  columns.push_back(columndefinition("TINYINT", "TINYINT UNSIGNED", sql::DataType::TINYINT, "255", false));
  columns.push_back(columndefinition("TINYINT", "TINYINT ZEROFILL", sql::DataType::TINYINT, "-1", false));
  /* Alias of BOOLEAN */
  columns.push_back(columndefinition("TINYINT", "BOOLEAN", sql::DataType::TINYINT, "1", true));
  columns.push_back(columndefinition("SMALLINT", "SMALLINT", sql::DataType::SMALLINT, "-32768", true));
  columns.push_back(columndefinition("SMALLINT", "SMALLINT(5)", sql::DataType::SMALLINT, "-32768", false));
  columns.push_back(columndefinition("SMALLINT", "SMALLINT UNSIGNED", sql::DataType::SMALLINT, "65535", false));
  columns.push_back(columndefinition("SMALLINT", "SMALLINT ZEROFILL", sql::DataType::SMALLINT, "123", false));
  columns.push_back(columndefinition("MEDIUMINT", "MEDIUMINT", sql::DataType::MEDIUMINT, "-8388608", true));
  /* Alias of INTEGER */
  columns.push_back(columndefinition("INT", "INTEGER", sql::DataType::INTEGER, "2147483647", true));
  columns.push_back(columndefinition("INTEGER UNSIGNED", "INT UNSIGNED", sql::DataType::INTEGER, "4294967295", false));
  /* If you specify ZEROFILL for a numeric column, MySQL automatically adds the UNSIGNED  attribute to the column.  */
  columns.push_back(columndefinition("INTEGER UNSIGNED", "INT(4) SIGNED ZEROFILL", sql::DataType::INTEGER, "1", false));
  columns.push_back(columndefinition("BIGINT", "BIGINT", sql::DataType::BIGINT, "-9223372036854775808", true));
  columns.push_back(columndefinition("BIGINT UNSIGNED", "BIGINT UNSIGNED", sql::DataType::BIGINT, "18446744073709551615", false));
  columns.push_back(columndefinition("BIGINT UNSIGNED", "BIGINT(4) ZEROFILL", sql::DataType::BIGINT, "2", false));
  columns.push_back(columndefinition("FLOAT", "FLOAT", sql::DataType::REAL, "-1.01", true));
  columns.push_back(columndefinition("FLOAT", "FLOAT UNSIGNED", sql::DataType::REAL, "1.01", false));
  columns.push_back(columndefinition("FLOAT", "FLOAT(5,3) UNSIGNED ZEROFILL", sql::DataType::REAL, "1.01", false));
  columns.push_back(columndefinition("FLOAT", "FLOAT(6)", sql::DataType::REAL, "1.01", false));
  columns.push_back(columndefinition("DOUBLE", "DOUBLE", sql::DataType::DOUBLE, "-1.01", true));
  columns.push_back(columndefinition("DOUBLE", "DOUBLE UNSIGNED", sql::DataType::DOUBLE, "1.01", false));
  columns.push_back(columndefinition("DOUBLE", "DOUBLE(5,3) UNSIGNED ZEROFILL", sql::DataType::DOUBLE, "1.01", false));
  columns.push_back(columndefinition("DECIMAL", "DECIMAL", sql::DataType::DECIMAL, "-1.01", true));
  columns.push_back(columndefinition("DECIMAL", "DECIMAL UNSIGNED", sql::DataType::DECIMAL, "1.01", false));
  columns.push_back(columndefinition("DECIMAL", "DECIMAL(5,3) UNSIGNED ZEROFILL", sql::DataType::DECIMAL, "1.01", false));
  columns.push_back(columndefinition("DATE", "DATE", sql::DataType::DATE, "2009-02-09", true));
  columns.push_back(columndefinition("DATETIME", "DATETIME", sql::DataType::TIMESTAMP, "2009-02-09 20:05:43", true));
  columns.push_back(columndefinition("TIMESTAMP", "TIMESTAMP", sql::DataType::TIMESTAMP, "2038-01-09 03:14:07", true));
  columns.push_back(columndefinition("TIME", "TIME", sql::DataType::TIME, "-838:59:59", true));
  columns.push_back(columndefinition("YEAR", "YEAR", sql::DataType::YEAR, "1901", true));
  columns.push_back(columndefinition("YEAR", "YEAR(4)", sql::DataType::YEAR, "2009", false));
  columns.push_back(columndefinition("YEAR", "YEAR(2)", sql::DataType::YEAR, "1", false));

}

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
    } catch (sql::InvalidArgumentException &e)
    {
    }

    try
    {
      meta->getCatalogName(6);
      FAIL("Only five columns available but requesting number six, should bail");
    } catch (sql::InvalidArgumentException &e)
    {
    }

    res->close();
    try
    {
      meta->getCatalogName(1);
      FAIL("Can fetch meta from invalid resultset");
    } catch (sql::SQLException &e)
    {
    }

  } catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    FAIL(e.what());
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
    } catch (sql::SQLException &e)
    {
    }


  } catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    FAIL(e.what());
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
    } catch (sql::InvalidArgumentException &e)
    {
    }

    try
    {
      meta->getColumnDisplaySize(6);
      FAIL("Only five columns available but requesting number six, should bail");
    } catch (sql::InvalidArgumentException &e)
    {
    }

    res->close();
    try
    {
      meta->getColumnDisplaySize(1);
      FAIL("Can fetch meta from invalid resultset");
    } catch (sql::SQLException &e)
    {
    }


  } catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    FAIL(e.what());
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
    } catch (sql::InvalidArgumentException &e)
    {
    }

    try
    {
      meta->getColumnLabel(0);
      FAIL("Column number starts at 1, invalid offset 0 not detected");
    } catch (sql::InvalidArgumentException &e)
    {
    }

    try
    {
      meta->getColumnName(6);
      FAIL("Only five columns available but requesting number six, should bail");
    } catch (sql::InvalidArgumentException &e)
    {
    }

    try
    {
      meta->getColumnLabel(6);
      FAIL("Only five columns available but requesting number six, should bail");
    } catch (sql::InvalidArgumentException &e)
    {
    }

    res->close();
    try
    {
      meta->getColumnName(1);
      FAIL("Can fetch meta from invalid resultset");
    } catch (sql::SQLException &e)
    {
    }

    try
    {
      meta->getColumnLabel(1);
      FAIL("Can fetch meta from invalid resultset");
    } catch (sql::SQLException &e)
    {
    }

  } catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    FAIL(e.what());
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
        if (it->check_name)
          ASSERT_EQUALS(it->name, meta->getColumnTypeName(1));

        sql.str("");
        sql << "... OK, SQL:" << it->sqldef << " -> Type = " << it->name;
        sql << " (" << it->ctype << ")";
        logMsg(sql.str());

      } catch (sql::SQLException &e)
      {
        logMsg(sql.str());
        sql.str("");
        sql << "... skipping " << it->name << " " << it->sqldef << ": ";
        sql << e.what();
        logMsg(sql.str());
      }

    }

  } catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
    FAIL(e.what());
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
    } catch (sql::SQLException &e)
    {
    }

    res->close();
    try
    {
      meta->getPrecision(1);
      FAIL("Can fetch meta from invalid resultset");
    } catch (sql::SQLException &e)
    {
    }


  } catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    FAIL(e.what());
  }
}

void resultsetmetadata::runStandardQuery()
{
  stmt.reset(con->createStatement());
  res.reset(stmt->executeQuery("SELECT 'Hello' AS a, ' ', 'world', '!', 123 AS z"));
}

} /* namespace resultsetmetadata */
} /* namespace testsuite */
