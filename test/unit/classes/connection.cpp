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

#include "connection.h"

namespace testsuite
{
namespace classes
{

void connection::getClientInfo()
{
  logMsg("connection::getClientInfo() - MySQL_Connection::getClientInfo()");
  try
  {
    std::string client_info;
    std::string ret;

    ret=con->getClientInfo(client_info);
    if (ret != "cppconn")
      FAIL("Expecting 'cppconn' got '" + ret + "'.");

    if (!client_info.empty())
      FAIL("Expecting parameter to be unchanged but it seems to have been modified.");

  } catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    FAIL(e.what());
  }
}

void connection::getNoWarningsOnNewLine()
{
  logMsg("connection::getNoWarningsOnNewLine() - MySQL_Connection::getWarnings()");
  try
  {
    const sql::SQLWarning* warning;

    warning=con->getWarnings();
    if (warning != NULL)
      FAIL("There should be no warnings on the default connection");

  } catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    FAIL(e.what());
  }
}

void connection::getNoWarningsAfterClear()
{
  logMsg("connection::getNoWarningsAfterClear() - MySQL_Connection::getWarnings()");
  try
  {
    const sql::SQLWarning* warning;

    /* TODO: pointless test as there is no warning before running clearWarnings() */
    con->clearWarnings();
    warning=con->getWarnings();
    if (warning != NULL)
      FAIL("There should be no warnings on the default connection");

  } catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    FAIL(e.what());
  }
}

void connection::checkClosed()
{
  logMsg("connection::checkClosed - MySQL_Connection::close, isClosed() and internal check_closed()");
  try
  {
    if (con->isClosed())
      FAIL("Connection should not be reported as closed");

    con->close();

    if (!con->isClosed())
      FAIL("Connection should be closed");

    try
    {
      con->rollback();
    } catch (sql::SQLException &e)
    {
      if (e.getErrorCode() != 1000)
        FAIL("Exception should report error code 1000 got '" + e.getErrorCode() + "'");
      if (e.getSQLState() != "HY000")
        FAIL("Exception should report SQL state 'HY1000' got '" + e.getSQLState() + "'");

      std::string what(e.what());
      if (what.empty())
        FAIL("Exception should have a reason");
    }


  } catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    FAIL(e.what());
  }
}


void connection::connectUsingMap()
{
  logMsg("connection::connectUsingMap - using map to pass connection parameters");
  try
  {

    std::map<std::string, sql::ConnectPropertyVal> connection_properties;

    {
      sql::ConnectPropertyVal tmp;
      /* url comes from the unit testing framework */
      tmp.str.val = url.c_str();
      tmp.str.len = url.length();
      connection_properties[std::string("hostName")] = tmp;
    }

    {
      sql::ConnectPropertyVal tmp;
      /* user comes from the unit testing framework */
      tmp.str.val = user.c_str();
      tmp.str.len = user.length();
      connection_properties[std::string("userName")] = tmp;
    }

    {
      sql::ConnectPropertyVal tmp;
      tmp.str.val = passwd.c_str();
      tmp.str.len = passwd.length();
      connection_properties[std::string("password")] = tmp;
    }

    created_objects.clear();
    con.reset(driver->connect(connection_properties));

    // Ok, these have been the minimum connection properties for the connect
    // We can now add optional ones.

    /* port */
    {
      sql::ConnectPropertyVal tmp;
      tmp.str.val = "wrong way to set port";
      tmp.str.len = (size_t)1000;
      connection_properties[std::string("port")] = tmp;
    }
    created_objects.clear();
    con.reset(driver->connect(connection_properties));

    connection_properties.erase(connection_properties.find("port"));
    {
      sql::ConnectPropertyVal tmp;
      tmp.lval = 1
      connection_properties[std::string("port")] = tmp;
    }
    created_objects.clear();
    con.reset(driver->connect(connection_properties));

  } catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    FAIL(e.what());
  }
}


} /* namespace connection */
} /* namespace testsuite */
