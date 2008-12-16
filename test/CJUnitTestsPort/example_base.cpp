/*
   Copyright 2008 Sun Microsystems, Inc.

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

	static const String default_url = "tcp://127.0.0.1";
	static const String default_db = "test";
	static const String default_user =  "root";
	static const String default_passwd = "root";
	
Driver * example_fixture::driver=NULL;

/**
 * Creates a new example_fixture object.
 *
 * @param name The name of the unit test case
 */
example_fixture::example_fixture(const String & name)
: super(name),
conn(NULL),
pstmt(NULL),
stmt(NULL),
rs(NULL)
{
  init();
}

void example_fixture::init()
{
  url=TestsRunner::theInstance().getStartOptions()->dbUrl;
  user=TestsRunner::theInstance().getStartOptions()->dbUser;
  passwd=TestsRunner::theInstance().getStartOptions()->dbPasswd;
  db=TestsRunner::theInstance().getStartOptions()->dbSchema;
}

/**
 * Creates resources used by all tests.
 *
 * @throws Exception
 *             if an error occurs.
 */
void example_fixture::setUp()
{
  this->created_objects.clear();

  try
  {
    this->conn.reset(getConnection());
  } catch (sql::SQLException & sqle)
  {
    logErr(String("Couldn't get connection") + sqle.what());
    throw sqle;
  }

  this->stmt.reset(this->conn->createStatement());

	/* TODO: connect message incl. version using logDebug() */
  
	DatabaseMetaData dbmd(conn->getMetaData());
  stmt->execute(String("USE ") + (db.length() > 0 ? db : default_db));
}

/**
 * Destroys SQL schema objects created during the test case.
 *
 * @throws Exception
 */
void example_fixture::tearDown()
{
	
  rs.reset();
  for (int i=0; i < static_cast<int> (this->created_objects.size() - 1); i+=2)
  {
    try
    {
      dropSchemaObject(this->created_objects[ i ], this->created_objects[ i + 1 ]);
    } catch (sql::SQLException &) { }
  }

  stmt.reset();
  pstmt.reset();
  conn.reset();
}

/* throws SQLException & */

void example_fixture::createSchemaObject(String objectType, String objectName,
                                         String columnsAndOtherStuff)
{
  this->created_objects.push_back(objectType);
  this->created_objects.push_back(objectName);

  dropSchemaObject(objectType, objectName);
  String sql(_T("CREATE  "));

  sql.resize(objectName.length()
                   + objectType.length() + columnsAndOtherStuff.length() + 10);

  sql.append(objectType);
  sql.append(" ");
  sql.append(objectName);
  sql.append(" ");
  sql.append(columnsAndOtherStuff);
  this->stmt->executeUpdate(sql);
}

void example_fixture::dropSchemaObject(String objectType, String objectName)
{
  this->stmt->executeUpdate(String("DROP ") + objectType + " IF EXISTS "
                            + objectName);
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

/**
 * Returns a database connection
 *
 * @throws SQLException &
 */
sql::Connection * example_fixture::getConnection()
{
  if (driver == NULL)
  {
    driver=get_driver_instance();
    logMsg(String(_T("Done: loaded ")) + driver->getName());
  }

  if (url.length() == 0)  
    url=default_url;
	
	if (user.length() == 0)
		user = default_user;
	
	if (passwd.length() == 0)
		passwd = default_passwd;

  return driver->connect(url, user, passwd);
}




/**
 * Checks whether the database we're connected to meets the given version
 * minimum
 *
 * @param major the major version to meet
 * @param minor the minor version to meet 
 * @return bool if the major/minor is met
 * @throws SQLException& if an error occurs.
 */
bool example_fixture::versionMeetsMinimum(int major, int minor)
{
  return versionMeetsMinimum(major, minor, 0);
}

/**
 * Checks whether the database we're connected to meets the given version
 * minimum
 *
 * @param major the major version to meet
 * @param minor the minor version to meet
 * @return bool if the major/minor is met
 * @throws SQLException&
 */
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

void example_fixture::logMsg(String message)
{
  TestsListener::theInstance().messagesLog() << message << std::endl;
}

void example_fixture::logErr(String message)
{
  TestsListener::theInstance().errorsLog() << message << std::endl;
}

void example_fixture::logDebug(const String & message)
{
  logMsg(message);
}


} /* namespace testsuite */
