/* Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.

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

#include <time.h>

#include "example_base.h"
#include "../common/stringutils.h"

namespace testsuite
{
static const String defaultHost="127.0.0.1";
static const String defaultPort=_T("3306");
static const String defaultDb=_T("test");
static const String defaultLogin=_T("root");
static const String defaultPasswd=_T("root");

Driver * example_fixture::driver=NULL;

void example_fixture::init()
{
  host=TestsRunner::theInstance().getStartOptions()->dbUrl;
  login=TestsRunner::theInstance().getStartOptions()->dbUser;
  passwd=TestsRunner::theInstance().getStartOptions()->dbPasswd;
  db=TestsRunner::theInstance().getStartOptions()->dbSchema;
}

void example_fixture::logMsg(String message)
{
  TestsListener::theInstance().messagesLog() << message << std::endl;
}

void example_fixture::logErr(String message)
{
  TestsListener::theInstance().errorsLog() << message << std::endl;
}

/**
 * Creates a new example_fixture object.
 *
 * @param name
 *            The name of the JUnit test case
 */


example_fixture::example_fixture(const String & name)
: super(name),
conn(NULL),
pstmt(NULL),
stmt(NULL),
rs(NULL),
hasSps(true)

{
  init();
}

/* throws SQLException & */

void example_fixture::createSchemaObject(String objectType, String objectName,
                                         String columnsAndOtherStuff)
{
  this->createdObjects.push_back(objectType);
  this->createdObjects.push_back(objectName);

  dropSchemaObject(objectType, objectName);
  String createSql(_T("CREATE  "));

  createSql.resize(objectName.length()
                   + objectType.length() + columnsAndOtherStuff.length() + 10);

  createSql.append(objectType);
  createSql.append(" ");
  createSql.append(objectName);
  createSql.append(" ");
  createSql.append(columnsAndOtherStuff);
  this->stmt->executeUpdate(createSql);
}

void example_fixture::createTable(String tableName, String columnsAndOtherStuff)
{
  createSchemaObject("TABLE", tableName, columnsAndOtherStuff);
}

/* throws SQLException & */

void example_fixture::dropTable(String tableName)
{
  dropSchemaObject("TABLE", tableName);
}

/* throws SQLException & */

void example_fixture::dropSchemaObject(String objectType, String objectName)
{
  this->stmt->executeUpdate(String("DROP ") + objectType + " IF EXISTS "
                            + objectName);
}

void example_fixture::logDebug(const String & message)
{
  logMsg(message);
}

/**
 * Creates resources used by all tests.
 *
 * @throws Exception
 *             if an error occurs.
 */

/* throws Exception */

void example_fixture::setUp()
{
  this->createdObjects.clear();

  try
  {
    this->conn.reset(getConnection());
  } catch (sql::SQLException & sqle)
  {
    logErr(String("Couldn't get connection") + sqle.what());
    throw sqle;
  }

  this->stmt.reset(this->conn->createStatement());

  try
  {
    if (host.find_first_of("mysql") != String::npos)
    {
      this->rs.reset(this->stmt->executeQuery("SELECT VERSION()"));
      this->rs->next();
      logDebug("Connected to " + this->rs->getString(1));
      //this->rs->close();
      this->rs.reset();
    } else
    {
      logDebug("Connected to "
               + this->conn->getMetaData()->getDatabaseProductName()
               + " / "
               + this->conn->getMetaData()->getDatabaseProductVersion());
    }
  } catch (sql::SQLException & sqle)
  {
    logErr(sqle.what());
  }

  if (this->rs.get() != NULL)
    this->rs->close();

  DatabaseMetaData dbmd(conn->getMetaData());

  hasSps=dbmd->supportsStoredProcedures();

  stmt->execute(String("USE ") + (db.length() > 0 ? db : defaultDb));
}

sql::Connection * example_fixture::getConnection()
{
  if (driver == NULL)
  {
    driver=get_driver_instance();
    logMsg(String(_T("Done: loaded ")) + driver->getName());
  }

  if (host.length() == 0)
  {
    host=defaultHost;

    if (login.length() == 0)
    {
      login=defaultLogin;
      passwd=defaultPasswd;
    }
  }

  return driver->connect(host, /*port,*/ login, passwd);
}


/**
 * Destroys resources created during the test case.
 *
 * @throws Exception
 *             DOCUMENT ME!
 */

/* throws Exception */
void example_fixture::tearDown()
{
  rs.reset();

  for (int i=0; i < static_cast<int> (this->createdObjects.size() - 1); i+=2)
  {
    try
    {
      dropSchemaObject(this->createdObjects[ i ], this->createdObjects[ i + 1 ]);
    } catch (sql::SQLException &)
    {
    }
  }


  stmt.reset();

  pstmt.reset();

  conn.reset();

}

/**
 * Checks whether the database we're connected to meets the given version
 * minimum
 *
 * @param major
 *            the major version to meet
 * @param minor
 *            the minor version to meet
 *
 * @return bool if the major/minor is met
 *
 * @throws SQLException &
 *             if an error occurs.
 */

/* throws SQLException & */

bool example_fixture::versionMeetsMinimum(int major, int minor)
{
  return versionMeetsMinimum(major, minor, 0);
}

/**
 * Checks whether the database we're connected to meets the given version
 * minimum
 *
 * @param major
 *            the major version to meet
 * @param minor
 *            the minor version to meet
 *
 * @return bool if the major/minor is met
 *
 * @throws SQLException &
 *             if an error occurs.
 */

/* throws SQLException & */

bool example_fixture::versionMeetsMinimum(int major, int minor, int subminor)
{
  return true;
  /*
  ((dynamic_cast<sql::mysql::MySQL_Connection*> (this->conn)->versionMeetsMinimum(
  major, minor, subminor));*/

}

void example_fixture::closeMemberJDBCResources()
{
  if (this->rs.get() != NULL)
  {
    ResultSet toClose=this->rs;
    this->rs.reset();

    try
    {
      toClose->close();
    } catch (sql::SQLException &)
    {
      // ignore
    }
  }
  if (this->pstmt.get() != NULL)
  {
    PreparedStatement toClose=this->pstmt;
    this->pstmt.reset();
    try
    {
      toClose->close();
    } catch (sql::SQLException &)
    {
      // ignore
    }
  }
}




}
