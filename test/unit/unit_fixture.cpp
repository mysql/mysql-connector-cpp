/*
   Copyright 2008 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   There are special exceptions to the terms and conditions of the GPL
   as it is applied to this software. View the full text of the
   exception in file EXCEPTIONS-conECTOR-C++ in the directory of this
   software distribution.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "unit_fixture.h"

namespace testsuite
{

/**
 * Fallback connection URL if no URL is passed to the test on the command line
 */
static const String default_url="tcp://127.0.0.1";

/**
 * Fallback connection schema to use if no schema is passed to the test on the command line
 */
static const String default_db="test";

/**
 * Fallback user name if no schema is passed to the test on the command line
 */
static const String default_user="root";

/**
 * Fallback user password if no schema is passwrd to the test on the command line
 */
static const String default_passwd="root";

Driver * unit_fixture::driver=NULL;

unit_fixture::unit_fixture(const String & name)
: super(name),
con(NULL),
pstmt(NULL),
stmt(NULL),
res(NULL)
{
  init();
}

void unit_fixture::init()
{
  url=TestsRunner::theInstance().getStartOptions()->dbUrl;
  user=TestsRunner::theInstance().getStartOptions()->dbUser;
  passwd=TestsRunner::theInstance().getStartOptions()->dbPasswd;
  db=TestsRunner::theInstance().getStartOptions()->dbSchema;
}

void unit_fixture::setUp()
{
  created_objects.clear();

  try
  {
    con.reset(getConnection());
  } catch (sql::SQLException & sqle)
  {
    logErr(String("Couldn't get connection") + sqle.what());
    throw sqle;
  }

  /* TODO: conect message incl. version using logDebug() */
  logDebug("Driver: " + driver->getName());
  /*
           + " " + String(driver->getMajorVersion() + driver->getMajorVersion + String(".") + driver->getMinorVersion());*/

  con->setSchema((db.length() > 0 ? db : default_db));
}

void unit_fixture::tearDown()
{

  res.reset();
  for (int i=0; i < static_cast<int> (created_objects.size() - 1); i+=2)
  {
    try
    {
      dropSchemaObject(created_objects[ i ], created_objects[ i + 1 ]);
    } catch (sql::SQLException &)
    {
    }
  }

  stmt.reset();
  pstmt.reset();
  con.reset();
}

void unit_fixture::createSchemaObject(String object_type, String object_name,
                                      String columns_and_other_stuff)
{
  created_objects.push_back(object_type);
  created_objects.push_back(object_name);

  dropSchemaObject(object_type, object_name);

  String sql("CREATE  ");
  sql.resize(object_name.length()
             + object_type.length() + columns_and_other_stuff.length() + 10);
  sql.append(object_type);
  sql.append(" ");
  sql.append(object_name);
  sql.append(" ");
  sql.append(columns_and_other_stuff);

  stmt->executeUpdate(sql);
}

void unit_fixture::dropSchemaObject(String object_type, String object_name)
{
  stmt->executeUpdate(String("DROP ") + object_type + " IF EXISTS "
                      + object_name);
}

void unit_fixture::createTable(String table_name, String columns_and_other_stuff)
{
  createSchemaObject("TABLE", table_name, columns_and_other_stuff);
}

void unit_fixture::dropTable(String table_name)
{
  dropSchemaObject("TABLE", table_name);
}

sql::Connection * unit_fixture::getConnection()
{
  if (driver == NULL)
  {
    driver=get_driver_instance();
    logMsg(String("Done: loaded ") + driver->getName());
  }

  if (url.length() == 0)
    url=default_url;

  if (user.length() == 0)
    user=default_user;

  if (passwd.length() == 0)
    passwd=default_passwd;

  return driver->connect(url, user, passwd);
}

void unit_fixture::logMsg(const String message)
{
  TestsListener::theInstance().messagesLog() << message << std::endl;
}

void unit_fixture::logErr(const String message)
{
  TestsListener::theInstance().errorsLog() << message << std::endl;
}

void unit_fixture::logDebug(const String message)
{
  logMsg(message);
}


} /* namespace testsuite */
