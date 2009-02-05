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
    ASSERT_EQUALS(5, meta->getColumnCount());

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
    ASSERT_EQUALS(5, meta->getColumnDisplaySize(1));
    ASSERT_EQUALS(1, meta->getColumnDisplaySize(2));
    ASSERT_EQUALS(5, meta->getColumnDisplaySize(3));
    ASSERT_EQUALS(1, meta->getColumnDisplaySize(4));
    ASSERT_EQUALS(3, meta->getColumnDisplaySize(5));

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
    ASSERT_EQUALS(" ", meta->getColumnName(2));
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

void resultsetmetadata::runStandardQuery()
{
  stmt.reset(con->createStatement());
  res.reset(stmt->executeQuery("SELECT 'Hello' AS a, ' ', 'world', '!', 123 AS z"));
}

} /* namespace resultsetmetadata */
} /* namespace testsuite */
