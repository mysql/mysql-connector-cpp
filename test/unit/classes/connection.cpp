/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates.
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

#include "connection.h"
#include <stdlib.h>
#include <cstdio>
#include <fstream>
#include <cppconn/connection.h>
#include <mysql_connection.h>
#include <cppconn/exception.h>
#include <cppconn/version_info.h>

#include <boost/scoped_ptr.hpp>
#include <list>

#ifdef _WIN32
#pragma warning (disable : 4996)
// warning C4996: 'mkdir': The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name: _mkdir.
#include <direct.h>
#else
#include <sys/stat.h>
#endif

namespace testsuite
{
namespace classes
{

void connection::getClientInfo()
{
  logMsg("connection::getClientInfo() - MySQL_Connection::getClientInfo()");
  try
  {
    std::string ret;

    ret=con->getClientInfo();
    if (ret != "cppconn")
      FAIL("Expecting 'cppconn' got '" + ret + "'.");

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connection::getClientOption()
{
  logMsg("connection::getClientOption() - MySQL_Connection::get|setClientOption()");
  try
  {
    const std::string option("metadataUseInfoSchema");

    {
      bool input_value=true;
      bool output_value=false;
      void * input;
      void * output;

      input=(static_cast<bool *> (&input_value));
      output=(static_cast<bool *> (&output_value));

      con->setClientOption("metadataUseInfoSchema", input);
      con->getClientOption("metadataUseInfoSchema", output);
      ASSERT_EQUALS(input_value, output_value);

      con->setClientOption("metadataUseInfoSchema", input);
      con->getClientOption("metadataUseInfoSchema", output);
      ASSERT_EQUALS(input_value, output_value);

      input_value=false;
      output_value=true;
      con->setClientOption("metadataUseInfoSchema", input);
      con->getClientOption("metadataUseInfoSchema", output);
      ASSERT_EQUALS(input_value, output_value);
    }

    {
      int input_value=sql::ResultSet::TYPE_SCROLL_INSENSITIVE;
      int output_value=sql::ResultSet::TYPE_FORWARD_ONLY;
      void * input;
      void * output;

      input=(static_cast<int *> (&input_value));
      output=(static_cast<int *> (&output_value));

      con->setClientOption("defaultStatementResultType", input);
      con->getClientOption("defaultStatementResultType", output);
      ASSERT_EQUALS(input_value, output_value);

      input_value=sql::ResultSet::TYPE_FORWARD_ONLY;
      output_value=sql::ResultSet::TYPE_SCROLL_INSENSITIVE;
      con->setClientOption("defaultStatementResultType", input);
      con->getClientOption("defaultStatementResultType", output);
      ASSERT_EQUALS(input_value, output_value);

      try
      {
        input_value=sql::ResultSet::TYPE_SCROLL_SENSITIVE;
        con->setClientOption("defaultStatementResultType", input);
        FAIL("API Change or bug, please check");
      }
      catch (sql::InvalidArgumentException &)
      {
        /* expected */
      }

      try
      {
        input_value=sql::ResultSet::TYPE_SCROLL_SENSITIVE + sql::ResultSet::TYPE_SCROLL_INSENSITIVE + sql::ResultSet::TYPE_FORWARD_ONLY;
        con->setClientOption("defaultStatementResultType", input);
        FAIL("API Change or bug, please check");
      }
      catch (sql::InvalidArgumentException &)
      {
        /* expected */
      }
    }

    try
    {
      bool input_value=true;
      bool output_value=false;
      void * input;
      void * output;

      input=(static_cast<bool *> (&input_value));
      output=(static_cast<bool *> (&output_value));

      con->setClientOption("defaultPreparedStatementResultType", input);
      con->getClientOption("defaultPreparedStatementResultType", output);
      ASSERT_EQUALS(input_value, output_value);

      input_value=false;
      output_value=true;
      con->setClientOption("defaultPreparedStatementResultType", input);
      con->getClientOption("defaultPreparedStatementResultType", output);
      ASSERT_EQUALS(input_value, output_value);

    }
    catch (sql::MethodNotImplementedException &)
    {
      /* compiled without -DWE_SUPPORT_USE_RESULT_WITH_PS */
    }

    try
    {
      sql::SQLString input_value("latin1");
      sql::SQLString output_value;

      con->setClientOption("characterSetResults", input_value);

      output_value=con->getClientOption("characterSetResults");
      ASSERT_EQUALS(input_value, output_value);
    }
    catch (sql::SQLException &e)
    {
      logErr(e.what());
      logErr("SQLState: " + std::string(e.getSQLState()));
      fail(e.what(), __FILE__, __LINE__);
    }

    int serverVersion=getMySQLVersion(con);
    if ( serverVersion >= 57003)
    {
      try
      {
        sql::ConnectOptionsMap opts;
        int input_value=111;
        int output_value=2367;
        void * output;

        opts["hostName"]=url;
        opts["userName"]=user;
        opts["password"]=passwd;
        opts["OPT_READ_TIMEOUT"]=111;

        created_objects.clear();
        con.reset(driver->connect(opts));

        output=(static_cast<int *> (&output_value));
        con->getClientOption("OPT_READ_TIMEOUT", output);
        ASSERT_EQUALS(input_value, output_value);
      }
      catch (sql::SQLException &e)
      {
        logErr(e.what());
        logErr("SQLState: " + std::string(e.getSQLState()));
        fail(e.what(), __FILE__, __LINE__);
      }

      try
      {
        sql::ConnectOptionsMap opts;
        bool input_value=true;
        bool output_value=false;
        void * output;

        opts["hostName"]=url;
        opts["userName"]=user;
        opts["password"]=passwd;
        opts["OPT_RECONNECT"]=input_value;

        created_objects.clear();
        con.reset(driver->connect(opts));

        output=(static_cast<bool *> (&output_value));
        con->getClientOption("OPT_RECONNECT", output);
        ASSERT_EQUALS(input_value, output_value);
      }
      catch (sql::SQLException &e)
      {
        logErr(e.what());
        logErr("SQLState: " + std::string(e.getSQLState()));
        fail(e.what(), __FILE__, __LINE__);
      }

      try
      {
        sql::ConnectOptionsMap opts;
        sql::SQLString input_value("../lib/plugin/");
        const char *output_value="../lib/plugin/";
        void * output;

        opts["hostName"]=url;
        opts["userName"]=user;
        opts["password"]=passwd;
        opts["pluginDir"]=input_value;

        created_objects.clear();
        con.reset(driver->connect(opts));

        output=(static_cast<const char **> (&output_value));
        con->getClientOption("pluginDir", output);

        ASSERT_EQUALS(input_value, output_value);
      }
      catch (sql::SQLException &e)
      {
        logErr(e.what());
        logErr("SQLState: " + std::string(e.getSQLState()));
        fail(e.what(), __FILE__, __LINE__);
      }
    }

    try
    {
      sql::SQLString tmp=con->getClientOption("characterSetDirectory");
      tmp=con->getClientOption("readDefaultFile");
    }
    catch (sql::SQLException &e)
    {
      logErr(e.what());
      logErr("SQLState: " + std::string(e.getSQLState()));
      fail(e.what(), __FILE__, __LINE__);
    }

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connection::getSessionVariable()
{
  logMsg("connection::getSessionVariable() - MySQL_Connection::get|setSessionVariable()");
  try
  {
    std::string value("");
    boost::scoped_ptr< sql::mysql::MySQL_Connection > my_con(dynamic_cast<sql::mysql::MySQL_Connection*> (driver->connect(url, user, passwd)));
    value=my_con->getSessionVariable("sql_mode");

    my_con->setSessionVariable("sql_mode", "ANSI");
    // The server will translate ANSI into something that is version dependent -
    // ASSERT_EQUALS(my_con->getSessionVariable("sql_mode"), "");

    my_con->setSessionVariable("sql_mode", value);
    ASSERT_EQUALS(value, my_con->getSessionVariable("sql_mode"));

    value=my_con->getSessionVariable("sql_warnings");

    std::string on("1");
    std::string off("0");
    try
    {
      my_con->setSessionVariable("sql_warnings", 0);
      on="ON";
      off="OFF";
    }
    catch (sql::SQLException &)
    {
    }

    try
    {
      my_con->setSessionVariable("sql_warnings", on);
      ASSERT_EQUALS(1, atoi(my_con->getSessionVariable("sql_warnings").c_str()));
      my_con->setSessionVariable("sql_warnings", off);
      ASSERT_EQUALS(0, atoi(my_con->getSessionVariable("sql_warnings").c_str()));
    }
    catch (sql::SQLException &)
    {
    }

    my_con->setSessionVariable("sql_warnings", atoi(value.c_str()));
    ASSERT_EQUALS(value, my_con->getSessionVariable("sql_warnings"));
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void connection::checkDnsSrv()
{
  logMsg("connection::checkDnsSrv()");
  try
  {
    sql::ConnectOptionsMap opts;

    opts["hostName"]=url;
    opts["userName"]=user;
    opts["password"]=passwd;
    opts["OPT_DNS_SRV"]=1;

    std::string value("");
    std::string old_value("");
    bool diff_found = false;
    for(int i = 0; i < 10; ++i)
    {
      std::unique_ptr< sql::Connection > con(driver->connect(opts));
      boost::scoped_ptr< sql::mysql::MySQL_Connection > my_con(dynamic_cast<sql::mysql::MySQL_Connection*> (driver->connect(opts)));
      old_value = value;
      value=my_con->getSessionVariable("server_id");
      if (!old_value.empty() && old_value.compare(value))
        diff_found = true;

      std::cout << "server_id = " << value << std::endl;
    }

    if (!diff_found)
      fail("Connection is always made to one host only!", __FILE__, __LINE__);

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
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

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connection::invalidCredentials()
{
  logMsg("connection::invalidCredentials() - MySQL_Connection connect");
  std::string myurl("tcp://");
  std::string myuser("");
  std::string mypasswd("");

  try
  {
    try
    {
      con.reset(driver->connect(myurl, user, passwd));
      logMsg("... using invalid URL should have failed, but we can't be sure that is it an issue, because we do not know for sure what defaults a test system is using,");
      con.reset(driver->connect(url, user, passwd));
    }
    catch (sql::SQLException &/*e*/)
    {
      logMsg("... using wrong URL caused expected failure");
      con.reset(driver->connect(url, user, passwd));
    }

    if (!url.empty())
    {
      try
      {
        con.reset(driver->connect("", user, passwd));
        logMsg("... using empty URL should have failed, but we can't be sure that is it an issue, because we do not know for sure what defaults a test system is using,");
        con.reset(driver->connect(url, user, passwd));
      }
      catch (sql::SQLException &)
      {
        logMsg("... using empty URL caused expected failure");
        con.reset(driver->connect(url, user, passwd));
        try
        {
          con.reset(driver->connect("", user, passwd));
          FAIL("Should have caused exception");
        }
        catch (sql::SQLException &)
        {
        }
      }
    }

    if (user.empty())
    {
      myuser.append("H17ba76inosuchuser");
      try
      {
        con.reset(driver->connect(url, myuser, passwd));
        FAIL("... using invalid user should cause failure");
      }
      catch (sql::SQLException &)
      {
        logMsg("... using wrong URL caused expected failure");
        con.reset(driver->connect(url, user, passwd));
        try
        {
          con.reset(driver->connect(url, myuser, passwd));
          FAIL("Should have caused exception");
        }
        catch (sql::SQLException &)
        {
        }
      }
    }
    else
    {
      /* Its a guess, but usually such a user won't exist... */
      myuser.append(user);
      myuser.append(user);
      try
      {
        con.reset(driver->connect(url, myuser, myuser));
        FAIL("... using invalid user should have failed");
      }
      catch (sql::SQLException &)
      {
        logMsg("... using wrong user caused expected failure");
        con.reset(driver->connect(url, user, passwd));
        try
        {
          con.reset(driver->connect(url, myuser, myuser));
          FAIL("Should have caused exception");
        }
        catch (sql::SQLException &)
        {
        }
      }
    }

    if (passwd.empty())
    {
      mypasswd.append("27jahjk327ahime27879xas");
      try
      {
        con.reset(driver->connect(url, user, mypasswd));
        FAIL("... using invalid password should cause failure");
      }
      catch (sql::SQLException &)
      {
        logMsg("... using wrong password caused expected failure");
        con.reset(driver->connect(url, user, passwd));
        try
        {
          con.reset(driver->connect(url, user, mypasswd));
          FAIL("Should have caused exception");
        }
        catch (sql::SQLException &)
        {
        }
      }
    }
    else
    {
      mypasswd.append(passwd);
      mypasswd.append(passwd);
      try
      {
        con.reset(driver->connect(url, user, mypasswd));
        FAIL("... using invalid password should have failed");
      }
      catch (sql::SQLException &)
      {
        logMsg("... using wrong password caused expected failure");
        con.reset(driver->connect(url, user, passwd));
        try
        {
          con.reset(driver->connect(url, user, mypasswd));
          FAIL("Should have caused exception");
        }
        catch (sql::SQLException &)
        {
        }
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

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
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
    }
    catch (sql::SQLException &e)
    {
      std::string reason(exceptionIsOK(e, "HY000", 0));
      if (!reason.empty())
        fail(reason.c_str(), __FILE__, __LINE__);
    }
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connection::connectUsingMapWrongTypes()
{
  logMsg("connection::connectUsingMapWrongTypes - using map to pass connection parameters but parameter of wrong type");

  try
  {
    sql::ConnectOptionsMap connection_properties;
    bool boolval=true;
    std::string strval("");

    try
    {
      connection_properties["hostName"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception I");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("hostName");
    connection_properties["hostName"]=url;

    try
    {
      connection_properties["userName"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception II");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("userName");
    connection_properties["userName"]=user;

    try
    {
      connection_properties["password"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception III");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("password");
    connection_properties["password"]=passwd;

    try
    {
      connection_properties["port"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception IV");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("port");

    try
    {
      connection_properties["socket"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception V");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("socket");

    try
    {
      connection_properties["pipe"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception VI");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("pipe");

    try
    {
      connection_properties["schema"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception VII");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("schema");

    try
    {
      connection_properties["characterSetResults"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception VIII");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("characterSetResults");

    try
    {
      connection_properties["sslKey"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception IX");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("sslKey");

    try
    {
      connection_properties["sslCert"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception X");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("sslCert");

    try
    {
      connection_properties["sslCA"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XI");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("sslCA");

    try
    {
      connection_properties["sslCAPath"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XII");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("sslCAPath");

    try
    {
      connection_properties["sslCipher"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XIII");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("sslCipher");

    /*
    TODO -- will be moved into driver class.
    try
    {
      connection_properties["clientlib"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XIV");
    }
    catch (sql::InvalidArgumentException&)
    {
      expected
    }
    connection_properties.erase("clientlib");
    */

    try
    {
      connection_properties["defaultStatementResultType"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XV");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("defaultStatementResultType");

    try
    {
      connection_properties["metadataUseInfoSchema"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XVI");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("metadataUseInfoSchema");

    try
    {
      connection_properties["CLIENT_COMPRESS"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XVII");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("CLIENT_COMPRESS");

    try
    {
      connection_properties["CLIENT_FOUND_ROWS"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XVIII");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("CLIENT_FOUND_ROWS");

    try
    {
      connection_properties["CLIENT_IGNORE_SIGPIPE"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XIX");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("CLIENT_IGNORE_SIGPIPE");

    try
    {
      connection_properties["CLIENT_IGNORE_SPACE"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XX");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("CLIENT_IGNORE_SPACE");

    try
    {
      connection_properties["CLIENT_INTERACTIVE"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXI");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("CLIENT_INTERACTIVE");

    try
    {
      connection_properties["CLIENT_LOCAL_FILES"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXII");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("CLIENT_LOCAL_FILES");

    try
    {
      connection_properties["CLIENT_MULTI_STATEMENTS"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXIII");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("CLIENT_MULTI_STATEMENTS");

    try
    {
      connection_properties["CLIENT_NO_SCHEMA"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXIV");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("CLIENT_NO_SCHEMA");

    try
    {
      connection_properties["OPT_CONNECT_TIMEOUT"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXV");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("OPT_CONNECT_TIMEOUT");

    try
    {
      connection_properties["OPT_READ_TIMEOUT"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXVI");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("OPT_READ_TIMEOUT");

    try
    {
      connection_properties["OPT_WRITE_TIMEOUT"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXVII");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("OPT_WRITE_TIMEOUT");

    try
    {
      connection_properties["OPT_RECONNECT"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXVIII");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("OPT_RECONNECT");

    try
    {
      connection_properties["OPT_CHARSET_NAME"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXIX");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("OPT_CHARSET_NAME");

    try
    {
      connection_properties["OPT_REPORT_DATA_TRUNCATION"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXX");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("OPT_REPORT_DATA_TRUNCATION");

#if MYCPPCONN_STATIC_MYSQL_VERSION_ID < 80000
    try
    {
      connection_properties["sslVerify"]=(strval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXXI - sslVerify");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("sslVerify");
#endif

    try
    {
      connection_properties["sslCRL"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXXII - sslCRL");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("sslCRL");

    try
    {
      connection_properties["sslCRLPath"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXXIII - sslCRLPath");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("sslCRLPath");

    try
    {
      connection_properties["rsaKey"]=(boolval);
      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      FAIL("No exception XXXIII - rsaKey");
    }
    catch (sql::InvalidArgumentException&)
    {
      /* expected */
    }
    connection_properties.erase("rsaKey");

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connection::connectUsingMap()
{
  logMsg("connection::connectUsingMap - using map to pass connection parameters");

  try
  {
    sql::ConnectOptionsMap connection_properties;

    connection_properties["hostName"]=url;
    connection_properties["userName"]=user;
    connection_properties["password"]=passwd;

    bool bval= !TestsRunner::getStartOptions()->getBool("dont-use-is");
    connection_properties["metadataUseInfoSchema"]=(bval);

    created_objects.clear();
    con.reset(driver->connect(connection_properties));

    /*
    The property map now contains the minimum required entries and it works.
    Lets play with so-to-say optional entries.
     */

    /* 1) Port */
    connection_properties.erase("port");
    {
      int port= -1;
      if (url.compare(0, sizeof ("tcp://") - 1, "tcp://") == 0)
      {
        size_t port_pos;
        port_pos=url.find_last_of(":", std::string::npos);
        if (port_pos != std::string::npos)
          port=atoi(url.substr(port_pos + 1, std::string::npos).c_str());

        if (port == -1)
        {
          /* The user is using TCP/IP and default port 3306 */
          connection_properties["password"]=(port);
          try
          {
            created_objects.clear();
            con.reset(driver->connect(connection_properties));
            FAIL("Connect works with invalid port of -1");
          }
          catch (sql::SQLException &e)
          {
            std::string reason(exceptionIsOK(e, "HY000", 2003));
            logErr(reason);
          }
        }
        else
        {
          /* The user is using TCP/IP and has specified the port.
           A port setting shall NOT overrule the setting from the URL */
          port= -1;
          try
          {
            created_objects.clear();
            con.reset(driver->connect(connection_properties));
          }
          catch (sql::SQLException &)
          {
            FAIL("URL shall overrule port setting");
          }
        }

      }
      else
      {
        /* We must be using a socket connection - all port settings shall be ignored */
        connection_properties["port"]=(port);
        /* Must not throw an exception */
        try
        {
          created_objects.clear();
          con.reset(driver->connect(connection_properties));
        }
        catch (sql::SQLException &)
        {
          FAIL("Port setting should be ignored, using Unix socket!");
        }
      }
    }
    connection_properties.erase("port");

    /* 2) Socket */
    connection_properties.erase("socket");
    {
      std::string socket("");
#ifndef CPPWIN_WIN32

      if (url.compare(0, sizeof ("unix://") - 1, "unix://") == 0)
      {
        // Unix socket connection
        socket="I hope this is invalid";
        try
        {
          created_objects.clear();
          con.reset(driver->connect(connection_properties));
        }
        catch (sql::SQLException &)
        {
          FAIL("Socket setting should be ignored, socket is part of the URL");
        }

      }
      else
#endif
        if (url.compare(0, sizeof ("tcp://") - 1, "tcp://") == 0)
      {
        // TCP/IP connection, socket shall be ignored
        socket="I hope this is invalid";
        try
        {
          created_objects.clear();
          con.reset(driver->connect(connection_properties));
        }
        catch (sql::SQLException &)
        {
          FAIL("Socket setting should be ignored because its a TCP/IP connection");
        }
      }
    }
    connection_properties.erase("socket");

    /* 2) Schema */
    connection_properties.erase("schema");
    {
      std::string schema("");
      std::string myschema("mysql");
      std::string retschema("");

      if (url.compare(0, sizeof ("tcp://") - 1, "tcp://") == 0)
      {
        // TCP/IP connection - schema cannot be set when using unix socket syntax
        size_t schema_pos;
        std::string host(url.substr(sizeof ("tcp://") - 1, std::string::npos));
        schema_pos=host.find("/");
        if (schema_pos != std::string::npos)
        {
          schema_pos++;
          schema=host.substr(schema_pos, host.size() - schema_pos);
        }
      }

      if (schema.empty())
      {
        logMsg("... schema not set through the URL");

        connection_properties[std::string("schema")]=schema;

        try
        {
          created_objects.clear();
          con.reset(driver->connect(connection_properties));
          schema=con->getSchema();
          if (!schema.empty())
            FAIL("Empty schama specified but certain schema selected upon connect");
        }
        catch (sql::SQLException &)
        {
          FAIL("Connect should have worked although schema property set to empty string");
        }


        logMsg("... trying to connect to mysql schema, may or may not work");

        connection_properties.erase("schema");
        connection_properties["schema"]=(myschema);

        try
        {
          created_objects.clear();
          con.reset(driver->connect(connection_properties));
          Connection mycon(driver->connect(connection_properties));
          retschema=mycon->getSchema();
          if (retschema != myschema)
          {
            logErr(retschema);
            logErr(myschema);
            logErr(mycon->getCatalog());
            logErr(mycon->getSchema());
            FAIL("Connected to schema mysql but getSchema() reports different schema");
          }
        }
        catch (sql::SQLException &)
        {
          logMsg("... cannot connect to mysql schema but that is OK, might be insufficient grants");
        }

      }
      else
      {
        /* schema is set in the TCP/IP url */
        logMsg("... schema is set in the URL and property shall be ignored");

        /* no property set */
        try
        {
          created_objects.clear();
          con.reset(driver->connect(connection_properties));
          retschema=con->getSchema();
          if (retschema != schema)
          {
            logErr(retschema);
            logErr(schema);
            FAIL("Connected to a certain schema but getSchema() reports different schema");
          }
        }
        catch (sql::SQLException &)
        {
          FAIL("Connect should not fail");
        }

        /* property set */
        connection_properties["schema"]=(myschema);
        try
        {
          created_objects.clear();
          con.reset(driver->connect(connection_properties));
          retschema=con->getSchema();
          if (retschema != schema)
          {
            logErr(retschema);
            logErr(schema);
            FAIL("Connected to a certain schema but getSchema() reports different schema");
          }
        }
        catch (sql::SQLException &)
        {
          FAIL("Connect should not fail");
        }
      }
    }
    connection_properties.erase("schema");

    /* 3) ssl* */
    connection_properties.erase("sslKey");
    connection_properties.erase("sslCert");
    connection_properties.erase("sslCA");
    connection_properties.erase("sslCAPath");
    connection_properties.erase("sslCipher");
    {
      logMsg("... setting bogus SSL properties");
      std::string sql("ramdom bogus value");
      connection_properties["sslKey"]=(sql);
      connection_properties["sslCert"]=(sql);
      connection_properties["sslCA"]=(sql);
      connection_properties["sslCAPath"]=(sql);
      connection_properties["sslCipher"]=(sql);
      /*
       mysql_ssl_set is silly:
       This function always returns 0.
       If SSL setup is incorrect, mysql_real_connect()
       returns an error when you attempt to connect.
       */
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

    }
    connection_properties.erase("sslKey");
    connection_properties.erase("sslCert");
    connection_properties.erase("sslCA");
    connection_properties.erase("sslCAPath");
    connection_properties.erase("sslCipher");

    /* All the CLIENT* are pointless. There is no way (yet) to verify the settings */
    /* 4) CLIENT_COMPRESS */
    connection_properties.erase("CLIENT_COMPRESS");
    {
      logMsg("... testing CLIENT_COMPRESS");
      connection_properties["CLIENT_COMPRESS"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties["CLIENT_COMPRESS"]=(false);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }
    }
    connection_properties.erase("CLIENT_COMPRESS");

    /* 5) CLIENT_FOUND_ROWS */
    connection_properties.erase("CLIENT_FOUND_ROWS");
    {
      logMsg("... testing CLIENT_FOUND_ROWS");
      connection_properties["CLIENT_FOUND_ROWS"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties["CLIENT_FOUND_ROWS"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_FOUND_ROWS");
      connection_properties["CLIENT_FOUND_ROWS"]=(false);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }
    }
    connection_properties.erase("CLIENT_FOUND_ROWS");

    /* 6) CLIENT_IGNORE_SIGPIPE */
    connection_properties.erase("CLIENT_IGNORE_SIGPIPE");
    {
      logMsg("... testing CLIENT_IGNORE_SIGPIPE");
      connection_properties["CLIENT_IGNORE_SIGPIPE"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties["CLIENT_IGNORE_SIGPIPE"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties["CLIENT_IGNORE_SIGPIPE"]=(false);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }
    }
    connection_properties.erase("CLIENT_IGNORE_SIGPIPE");

    /* 7) CLIENT_IGNORE_SPACE */
    connection_properties.erase("CLIENT_IGNORE_SPACE");
    {
      logMsg("... testing CLIENT_IGNORE_SPACE");
      connection_properties["CLIENT_IGNORE_SPACE"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties["CLIENT_IGNORE_SPACE"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_IGNORE_SPACE");
      connection_properties["CLIENT_IGNORE_SPACE"]=(false);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }
    }
    connection_properties.erase("CLIENT_IGNORE_SPACE");

    /* 8) CLIENT_INTERACTIVE */
    connection_properties.erase("CLIENT_INTERACTIVE");
    {
      logMsg("... testing CLIENT_INTERACTIVE");
      connection_properties["CLIENT_INTERACTIVE"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties["CLIENT_INTERACTIVE"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_INTERACTIVE");
      connection_properties["CLIENT_INTERACTIVE"]=(false);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }
    }
    connection_properties.erase("CLIENT_INTERACTIVE");

    /* 9) CLIENT_LOCAL_FILES */
    /* TODO - add proper test */
    connection_properties.erase("CLIENT_LOCAL_FILES");
    {
      logMsg("... testing CLIENT_LOCAL_FILES");
      connection_properties["CLIENT_LOCAL_FILES"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties["CLIENT_LOCAL_FILES"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_LOCAL_FILES");
      connection_properties["CLIENT_LOCAL_FILES"]=(false);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }
    }
    connection_properties.erase("CLIENT_LOCAL_FILES");

    /* 10) CLIENT_MULTI_RESULTS */
    /* TODO - add proper test */
    connection_properties.erase("CLIENT_MULTI_RESULTS");
    {
      logMsg("... testing CLIENT_MULTI_RESULTS");
      connection_properties["CLIENT_MULTI_RESULTS"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties["CLIENT_MULTI_RESULTS"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_MULTI_RESULTS");
      connection_properties["CLIENT_MULTI_RESULTS"]=(false);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }
    }
    connection_properties.erase("CLIENT_MULTI_RESULTS");

    /* 11) CLIENT_MULTI_STATEMENTS */
    /* TODO: add proper test */
    connection_properties.erase("CLIENT_MULTI_STATEMENTS");
    {
      logMsg("... testing CLIENT_MULTI_STATEMENTS");
      connection_properties["CLIENT_MULTI_STATEMENTS"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties["CLIENT_MULTI_STATEMENTS"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_MULTI_STATEMENTS");
      connection_properties["CLIENT_MULTI_STATEMENTS"]=(false);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }
    }
    connection_properties.erase("CLIENT_MULTI_STATEMENTS");

    /* 12) CLIENT_NO_SCHEMA */
    connection_properties.erase("CLIENT_NO_SCHEMA");
    {
      logMsg("... testing CLIENT_NO_SCHEMA");
      connection_properties["CLIENT_NO_SCHEMA"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties["CLIENT_NO_SCHEMA"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_NO_SCHEMA");
      connection_properties["CLIENT_NO_SCHEMA"]=(false);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }
    }
    connection_properties.erase("CLIENT_NO_SCHEMA");

    /* 13) MYSQL_OPT_CONNECT_TIMEOUT */
    connection_properties.erase("OPT_CONNECT_TIMEOUT");
    {
      logMsg("... testing OPT_CONNECT_TIMEOUT");
      /*
       C-API does not care about the actual value, its passed down to the OS,
       The OS may or may not detect bogus values such as negative values.
       */
      connection_properties["OPT_CONNECT_TIMEOUT"]=1;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }
    }
    connection_properties.erase("OPT_CONNECT_TIMEOUT");

    /* 14) MYSQL_OPT_READ_TIMEOUT */
    connection_properties.erase("OPT_READ_TIMEOUT");
    {
      logMsg("... testing OPT_READ_TIMEOUT");
      /*
       C-API does not care about the actual value, its passed down to the OS,
       The OS may or may not detect bogus values such as negative values.
       */
      connection_properties["OPT_READ_TIMEOUT"]=1;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }
    }
    connection_properties.erase("OPT_READ_TIMEOUT");

    /* 15) MYSQL_OPT_WRITE_TIMEOUT */
    connection_properties.erase("OPT_WRITE_TIMEOUT");
    {
      logMsg("... testing OPT_WRITE_TIMEOUT");
      /* C-API does not care about the actual value */
      connection_properties["OPT_WRITE_TIMEOUT"]=1;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }
    }
    connection_properties.erase("OPT_WRITE_TIMEOUT");

    /* 16) MYSQL_OPT_RECONNECT */
    connection_properties.erase("OPT_RECONNECT");
    {
      logMsg("... testing OPT_RECONNECT");
      /* C-API does not care about the actual value */
      connection_properties["OPT_RECONNECT"]=true;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }
    }
    connection_properties.erase("OPT_RECONNECT");

    /* 17) MYSQL_OPT_SET_CHARSET_NAME */
    connection_properties.erase("OPT_SET_CHARSET_NAME");
    {
      logMsg("... testing OPT_SET_CHARSET_NAME");
      std::string charset("utf8");
      /* C-API does not care about the actual value */
      connection_properties["OPT_SET_CHARSET_NAME"]=(charset);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }
    }
    connection_properties.erase("OPT_SET_CHARSET_NAME");

    /* 18) MYSQL_REPORT_DATA_TRUNCATION */
    connection_properties.erase("REPORT_DATA_TRUNCATION");
    {
      logMsg("... testing REPORT_DATA_TRUNCATION");
      std::string charset("1");
      /* C-API does not care about the actual value */
      connection_properties["REPORT_DATA_TRUNCATION"]=(charset);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }
    }
    connection_properties.erase("REPORT_DATA_TRUNCATION");


    /* 19) metadataUseInfoSchema */
    connection_properties.erase("metadataUseInfoSchema");
    {
      logMsg("... testing metadataUseInfoSchema");
      connection_properties["metadataUseInfoSchema"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }

      connection_properties.erase("metadataUseInfoSchema");
      connection_properties["metadataUseInfoSchema"]=(false);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }
    }
    connection_properties.erase("metadataUseInfoSchema");

    /* 20) defaultStatementResultType */
    connection_properties.erase("defaultStatementResultType");
    {
      logMsg("... testing defaultStatementResultType");
      connection_properties["defaultStatementResultType"]=(sql::ResultSet::TYPE_FORWARD_ONLY);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }

      connection_properties.erase("defaultStatementResultType");
      connection_properties["defaultStatementResultType"]=(sql::ResultSet::TYPE_SCROLL_INSENSITIVE);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }

      connection_properties.erase("defaultStatementResultType");
      connection_properties["defaultStatementResultType"]=(sql::ResultSet::TYPE_SCROLL_SENSITIVE);
      try
      {
        created_objects.clear();
        try
        {
          con.reset(driver->connect(connection_properties));
          FAIL("Bug or API change - TYPE_SCROLL_SENSITIVE is unsupported");
        }
        catch (sql::SQLException &e)
        {
          logMsg("... expected exception because TYPE_SCROLL_SENSITIVE is unsupported!");
          logMsg(e.what());
        }
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }

    }
    connection_properties.erase("defaultStatementResultType");

#ifdef CPPWIN_WIN2
    /* 21) OPT_NAMED_PIPE - handled but ignored! */
    connection_properties.erase("OPT_NAMED_PIPE");
    {
      logMsg("... testing OPT_NAMED_PIPE");
      std::string pipe("IGNORED");
      connection_properties["OPT_NAMED_PIPE"]=(pipe);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }
    }
    connection_properties.erase("OPT_NAMED_PIPE");
#endif

    /* 22) OPT_CHARSET_NAME = MYSQL_SET_CHARSET_NAME */
    connection_properties.erase("OPT_CHARSET_NAME");
    {
      logMsg("... testing OPT_CHARSET_NAME");
      std::string charset("utf8");
      connection_properties["OPT_CHARSET_NAME"]=(charset);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }
    }
    connection_properties.erase("OPT_CHARSET_NAME");


    /* 23) OPT_REPORT_DATA_TRUNCATION */
    connection_properties.erase("OPT_REPORT_DATA_TRUNCATION");
    {
      logMsg("... testing OPT_REPORT_DATA_TRUNCATION");
      connection_properties["OPT_REPORT_DATA_TRUNCATION"]=(true);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }

      connection_properties.erase("OPT_REPORT_DATA_TRUNCATION");
      connection_properties["OPT_REPORT_DATA_TRUNCATION"]=(false);
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &e)
      {
        fail(e.what(), __FILE__, __LINE__);
      }
    }
    connection_properties.erase("OPT_REPORT_DATA_TRUNCATION");

    /* 24) defaultPreparedStatementResultType */
    connection_properties.erase("defaultPreparedStatementResultType");
    {
      logMsg("... testing defaultPreparedStatementResultType");
      connection_properties["defaultPreparedStatementResultType"]=sql::ResultSet::TYPE_FORWARD_ONLY;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &/*e*/)
      {
        /* may not be compiled in - ignore */
      }
    }
    connection_properties.erase("defaultPreparedStatementResultType");

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

}

void connection::connectOptReconnect()
{
  logMsg("connection::connectOptReconnect - OPT_RECONNECT");
  std::stringstream msg;

  try
  {
    sql::ConnectOptionsMap connection_properties;

    connection_properties["hostName"]=url;
    connection_properties["userName"]=user;
    connection_properties["password"]=passwd;

    bool bval= !TestsRunner::getStartOptions()->getBool("dont-use-is");
    connection_properties["metadataUseInfoSchema"]=(bval);

    logMsg("... OPT_RECONNECT disabled");

    connection_properties.erase("OPT_RECONNECT");
    connection_properties["OPT_RECONNECT"]=false;

    created_objects.clear();
    con.reset(driver->connect(connection_properties));
    con->setSchema(db);

    con->close();
    ASSERT(con->isClosed());
    try
    {
      stmt.reset(con->createStatement());
      stmt->execute("DROP TABLE IF EXISTS test");
      FAIL("Can create statement although connection has been closed");
    }
    catch (sql::SQLException &/*e*/)
    {
      /* expected */
    }

    connection_properties.erase("OPT_RECONNECT");
    connection_properties["OPT_RECONNECT"]=false;

    created_objects.clear();
    con.reset(driver->connect(connection_properties));
    con->setSchema(db);

    ASSERT_EQUALS(false, con->isClosed());
    try
    {
      stmt.reset(con->createStatement());
      stmt->execute("DROP TABLE IF EXISTS test");
    }
    catch (sql::SQLException &e)
    {
      logErr(e.what());
      logErr("SQLState: " + std::string(e.getSQLState()));
      fail(e.what(), __FILE__, __LINE__);
    }

    logMsg("... OPT_RECONNECT enabled");

    connection_properties.erase("OPT_RECONNECT");
    connection_properties["OPT_RECONNECT"]=true;

    created_objects.clear();
    con.reset(driver->connect(connection_properties));
    con->setSchema(db);

    con->close();
    ASSERT(con->isClosed());
    try
    {
      stmt.reset(con->createStatement());
      stmt->execute("DROP TABLE IF EXISTS test");
      FAIL("Can create statement although connection has been closed");
    }
    catch (sql::SQLException &/*e*/)
    {
      /* expected */
    }

    connection_properties.erase("OPT_RECONNECT");
    connection_properties["OPT_RECONNECT"]=true;

    created_objects.clear();
    con.reset(driver->connect(connection_properties));
    con->setSchema(db);

    ASSERT_EQUALS(false, con->isClosed());
    try
    {
      stmt.reset(con->createStatement());
      stmt->execute("DROP TABLE IF EXISTS test");
    }
    catch (sql::SQLException &e)
    {
      logErr(e.what());
      logErr("SQLState: " + std::string(e.getSQLState()));
      fail(e.what(), __FILE__, __LINE__);
    }

    logMsg("... OPT_RECONNECT disabled and KILL");


    connection_properties.erase("OPT_RECONNECT");
    connection_properties["OPT_RECONNECT"]=false;

    created_objects.clear();
    con.reset(driver->connect(connection_properties));
    con->setSchema(db);
    stmt.reset(con->createStatement());
    res.reset(stmt->executeQuery("SELECT CONNECTION_ID() as _pid"));
    ASSERT(res->next());
    msg.str("");
    msg << "KILL " << res->getInt("_pid");

    try
    {
      Connection my_con(getConnection());
      my_con->setSchema(db);
      Statement my_stmt(my_con->createStatement());
      my_stmt->execute(msg.str());
      logMsg("... we seem to be lucky, we have killed a connection");
      logMsg(msg.str());
      try
      {
        msg.str("");
        msg << "USE " << db;
        stmt->execute(msg.str());
        stmt->execute("DROP TABLE IF EXISTS test");
        FAIL("Statement object is still usable");
      }
      catch (sql::SQLException &e)
      {
        // Any error message is fine, connection should have been killed
        logMsg(e.what());
      }
    }
    catch (sql::SQLException &/*e*/)
    {
      // KILL has failed - that is OK, we may not have permissions
    }

    connection_properties.erase("OPT_RECONNECT");
    connection_properties["OPT_RECONNECT"]=false;

    created_objects.clear();
    con.reset(driver->connect(connection_properties));
    con->setSchema(db);
    stmt.reset(con->createStatement());
    res.reset(stmt->executeQuery("SELECT CONNECTION_ID() as _pid"));
    ASSERT(res->next());
    msg.str("");
    msg << "KILL " << res->getInt("_pid");

    try
    {
      Connection my_con(getConnection());
      my_con->setSchema(db);
      Statement my_stmt(my_con->createStatement());
      my_stmt->execute(msg.str());
      logMsg("... we seem to be lucky, we have killed a connection");
      logMsg(msg.str());
      try
      {
        stmt.reset(con->createStatement());
        logMsg("... we got a new statement object");
        stmt->execute("DROP TABLE IF EXISTS test");
        FAIL("Statement object is still usable");
      }
      catch (sql::SQLException &e)
      {
        /* Any error message is fine, connection should have been killed */
        logMsg(e.what());
      }
    }
    catch (sql::SQLException &/*e*/)
    {
      /* KILL has failed - that is OK, we may not have permissions */
    }

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

}

void connection::setTransactionIsolation()
{
  logMsg("connection::setTransactionIsolation() - MySQL_Connection::setTransactionIsolation()");
  bool have_innodb=false;
  int cant_be_changed_error= -1;
  int server_dependent_insert= -1;

  stmt.reset(con->createStatement());
  try
  {
    con->setTransactionIsolation(sql::TRANSACTION_READ_COMMITTED);
    ASSERT_EQUALS(sql::TRANSACTION_READ_COMMITTED, con->getTransactionIsolation());
    res.reset(stmt->executeQuery("SHOW VARIABLES LIKE 'transaction_isolation'"));
    checkResultSetScrolling(res);
    res->next();
    ASSERT_EQUALS("READ-COMMITTED", res->getString("Value"));

    con->setTransactionIsolation(sql::TRANSACTION_READ_UNCOMMITTED);
    ASSERT_EQUALS(sql::TRANSACTION_READ_UNCOMMITTED, con->getTransactionIsolation());
    res.reset(stmt->executeQuery("SHOW VARIABLES LIKE 'transaction_isolation'"));
    res->next();
    ASSERT_EQUALS("READ-UNCOMMITTED", res->getString("Value"));

    con->setTransactionIsolation(sql::TRANSACTION_REPEATABLE_READ);
    ASSERT_EQUALS(sql::TRANSACTION_REPEATABLE_READ, con->getTransactionIsolation());
    res.reset(stmt->executeQuery("SHOW VARIABLES LIKE 'transaction_isolation'"));
    res->next();
    ASSERT_EQUALS("REPEATABLE-READ", res->getString("Value"));

    con->setTransactionIsolation(sql::TRANSACTION_SERIALIZABLE);
    ASSERT_EQUALS(sql::TRANSACTION_SERIALIZABLE, con->getTransactionIsolation());
    res.reset(stmt->executeQuery("SHOW VARIABLES LIKE 'transaction_isolation'"));
    res->next();
    ASSERT_EQUALS("SERIALIZABLE", res->getString("Value"));
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  try
  {
    con.reset(getConnection());
    stmt.reset(con->createStatement());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
  try
  {
    con->setAutoCommit(true);
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT) ENGINE = InnoDB");
    have_innodb=true;
  }
  catch (sql::SQLException &)
  {
    have_innodb=false;
  }

  if (have_innodb)
  {
    try
    {
      con->setAutoCommit(false);
      stmt->execute("SET TRANSACTION ISOLATION LEVEL SERIALIZABLE");
      stmt->execute("INSERT INTO test(id) VALUES (1)");
      /* JDBC documentation: If this method is called while
       in the middle of a transaction, any changes up to that point
       will be committed.*/
      stmt->execute("SET TRANSACTION ISOLATION LEVEL REPEATABLE READ");
      // con->setTransactionIsolation(sql::TRANSACTION_REPEATABLE_READ);
      /* According to the JDBC docs the INSERT has been comitted
       and this ROLLBACK must have no impat */
      con->rollback();

      res.reset(stmt->executeQuery("SELECT COUNT(*) AS _num FROM test"));
      res->next();
      server_dependent_insert=res->getInt("_num");
    }
    catch (sql::SQLException &e)
    {
      logMsg("... EXPECTED behaviour - Transaction isolation level can't be changed while a transaction is in progress.");
      logMsg(e.what());
      logMsg("SQLState: " + std::string(e.getSQLState()));
      cant_be_changed_error=e.getErrorCode();
    }

    try
    {
      con.reset(getConnection());
      stmt.reset(con->createStatement());
    }
    catch (sql::SQLException &e)
    {
      logErr(e.what());
      logErr("SQLState: " + std::string(e.getSQLState()));
      fail(e.what(), __FILE__, __LINE__);
    }

    try
    {
      con->setAutoCommit(true);
      stmt->execute("DROP TABLE IF EXISTS test");
      stmt->execute("CREATE TABLE test(id INT) ENGINE = InnoDB");

      con->setAutoCommit(false);
      con->setTransactionIsolation(sql::TRANSACTION_SERIALIZABLE);
      stmt->execute("INSERT INTO test(id) VALUES (1)");
      /* JDBC documentation: If this method is called while
       in the middle of a transaction, any changes up to that point
       will be committed.*/
      con->setTransactionIsolation(sql::TRANSACTION_REPEATABLE_READ);

      if (-1 != cant_be_changed_error)
      {
        FAIL("Changing the transaction level manually has caused an exception. Changing it through API has not!");
      }

      /* According to the JDBC docs the INSERT has been comitted
       and this ROLLBACK must have no impat */
      con->rollback();

      res.reset(stmt->executeQuery("SELECT COUNT(*) AS _num FROM test"));
      res->next();
      ASSERT_EQUALS(server_dependent_insert, res->getInt("_num"));
      stmt->execute("DROP TABLE IF EXISTS test");
    }
    catch (sql::SQLException &e)
    {
      if (e.getErrorCode() != cant_be_changed_error)
      {
        logErr(e.what());
        logErr("SQLState: " + std::string(e.getSQLState()));
        fail(e.what(), __FILE__, __LINE__);
      }
      else
      {
        logMsg("... EXPECTED behaviour - Transaction isolation level can't be changed while a transaction is in progress.");
        logMsg(e.what());
        logMsg("SQLState: " + std::string(e.getSQLState()));
      }
    }
  }

  con->close();
  try
  {
    con->setTransactionIsolation(sql::TRANSACTION_READ_COMMITTED);
    FAIL("Closed connection not detected");
  }
  catch (sql::SQLException &)
  {
  }
}

void connection::rollback()
{
  try
  {
    con->setAutoCommit(false);
    try
    {
      try
      {
        con->setAutoCommit(true);
        con->rollback(con->setSavepoint("foo"));
        FAIL("autoCommit mode not detected");
      }
      catch (sql::InvalidArgumentException &e)
      {
        ASSERT_EQUALS(e.what(), "The connection is in autoCommit mode");
      }
    }
    catch (sql::SQLException &)
    {
      /* no support for savepoints, bad luck... */
    }
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

/* Exploiting bug that different wrapper is currently created if requested for ""
 * and default lib name. just to test that nothing bad happens.
 * Test itself shouldn't fail.
 */
#ifndef MYSQLCLIENT_STATIC_BINDING

void connection::loadSameLibraryTwice()
{
#if defined(_WIN32)
  const sql::SQLString baseName("libmysql.dll");
#elif defined(__APPLE__)
  const sql::SQLString baseName("libmysqlclient_r.dylib");
#elif defined(__hpux) && defined(__hppa)
  const sql::SQLString baseName("libmysqlclient_r.sl");
#else
  const sql::SQLString baseName("libmysqlclient_r.so");
#endif

  sql::ConnectOptionsMap connection_properties;
  connection_properties["hostName"]=url;
  connection_properties["userName"]=user;
  connection_properties["password"]=passwd;

  connection_properties["clientlib"]=baseName;

  con.reset(driver->connect(connection_properties));
}
#endif


/* Test of the OPT_ENABLE_CLEARTEXT_PLUGIN connection of the text
   The test idea - we try to create fake PAM authorized user and try to connect
   using that user first without, and then with the new option selected. In
   first case the error has to be that cleartext plugin could not be loaded, and
   in second case the error has to be different */
void connection::enableClearTextAuth()
{
  int serverVersion=getMySQLVersion(con);

  if ( ((serverVersion < 55027) || (serverVersion > 56000)) && (serverVersion < 56007))
  {
    SKIP("The server does not support tested functionality(cleartext plugin enabling)");
  }

  try
  {
    stmt->executeUpdate("DROP USER 't_ct_user'@'%'");
  }
  catch (sql::SQLException &)
  {
    // Catching exception if user did not exist
  }

  try
  {
    stmt->executeUpdate("GRANT ALL ON 't_ct_plugin' TO 't_ct_user' IDENTIFIED WITH "
                        "'authentication_pam'");
  }
  catch (sql::SQLException &)
  {
    SKIP("The authentication_pam plugin not loaded");
  }

  sql::ConnectOptionsMap opts;
  testsuite::Connection c2;

  opts["userName"]=sql::SQLString("t_ct_user");
  opts["password"]=sql::SQLString("foo");

  /*
    Expecting error CR_AUTH_PLUGIN_CANNOT_LOAD_ERROR
    without option ENABLE_CLEARTEXT_PLUGIN
  */
  try
  {
    c2.reset(getConnection(&opts));
  }
  catch (sql::SQLException &e)
  {
    /* We should have dropped the created user here if assertion fails -
       TODO: Add sort of dropSchemaObject for created users in tearDown */
    ASSERT_EQUALS(2059, e.getErrorCode()/*CR_AUTH_PLUGIN_CANNOT_LOAD_ERROR*/);
  }

  /*
    Expecting error other then CR_AUTH_PLUGIN_CANNOT_LOAD_ERROR
    as option ENABLE_CLEARTEXT_PLUGIN is used
  */
  opts["OPT_ENABLE_CLEARTEXT_PLUGIN"]=true;

  try
  {
    c2.reset(getConnection(&opts));
  }
  catch (sql::SQLException &e)
  {
    ASSERT(e.getErrorCode() != 2059);
  }

  stmt->executeUpdate("DROP USER 't_ct_user'@'%'");
}


void connection::connectAttrAdd()
{
  logMsg("connection::connectAttr - MYSQL_OPT_CONNECT_ATTR_ADD|MYSQL_OPT_CONNECT_ATTR_DELETE");

  //TODO: Enable it after fixing
  SKIP("Removed untill fixed");

  int serverVersion=getMySQLVersion(con);
  if ( serverVersion < 56006)
  {
    SKIP("The server does not support tested functionality(cleartext plugin enabling)");
  }

  try
  {
    testsuite::Connection conn1;
    sql::ConnectOptionsMap opts;
    std::map< sql::SQLString, sql::SQLString > connectAttrMap;
    std::list< std::string > connectAttrList;

    opts["hostName"]=url;
    opts["userName"]=user;
    opts["password"]=passwd;

    connectAttrMap["keyc1"]="value1";
    connectAttrMap["keyc2"]="value2";
    connectAttrMap["keyc3"]="value3";
    connectAttrMap["keyc4"]="value4";
    connectAttrMap["keyc5"]="value5";

    connectAttrList.push_back(std::string("keyc2"));
    connectAttrList.push_back(std::string("keyc5"));

    opts.erase("OPT_CONNECT_ATTR_ADD");
    opts.erase("OPT_CONNECT_ATTR_DELETE");
    opts["OPT_CONNECT_ATTR_ADD"]=connectAttrMap;
    opts["OPT_CONNECT_ATTR_DELETE"]=connectAttrList;

    created_objects.clear();
    conn1.reset(driver->connect(opts));

    stmt.reset(conn1->createStatement());
    res.reset(stmt->executeQuery("SELECT ATTR_NAME, ATTR_VALUE FROM "
                "performance_schema.session_account_connect_attrs WHERE "
                "ATTR_NAME LIKE '%keyc%' ORDER BY ATTR_NAME ASC;"));
    ASSERT(res->next());
    ASSERT_EQUALS(res->getString("ATTR_NAME"), "keyc1");
    ASSERT_EQUALS(res->getString("ATTR_VALUE"), "value1");

    ASSERT(res->next());
    ASSERT_EQUALS(res->getString("ATTR_NAME"), "keyc3");
    ASSERT_EQUALS(res->getString("ATTR_VALUE"), "value3");

    ASSERT(res->next());
    ASSERT_EQUALS(res->getString("ATTR_NAME"), "keyc4");
    ASSERT_EQUALS(res->getString("ATTR_VALUE"), "value4");

    ASSERT(!res->next());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }


  /*
    Check for empty OPT_CONNECT_ATTR_ADD map
    should not result in errors
  */
  try
  {
    testsuite::Connection conn1;
    sql::ConnectOptionsMap opts;
    std::map< sql::SQLString, sql::SQLString > connectAttrMap;

    opts["hostName"]=url;
    opts["userName"]=user;
    opts["password"]=passwd;

    opts.erase("OPT_CONNECT_ATTR_ADD");
    opts["OPT_CONNECT_ATTR_ADD"]=connectAttrMap;

    created_objects.clear();
    conn1.reset(driver->connect(opts));

    stmt.reset(conn1->createStatement());
    res.reset(stmt->executeQuery("SELECT 1"));
    ASSERT(res->next());
    ASSERT_EQUALS(res->getInt(1), 1);

    ASSERT(!res->next());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }


  /*
    Check for empty OPT_CONNECT_ATTR_DELETE list
  */
  try
  {
    testsuite::Connection conn1;
    sql::ConnectOptionsMap opts;
    std::map< sql::SQLString, sql::SQLString > connectAttrMap;
    std::list< std::string > connectAttrList;

    opts["hostName"]=url;
    opts["userName"]=user;
    opts["password"]=passwd;

    connectAttrMap["keya1"]="value1";
    connectAttrMap["keya2"]="value2";

    opts.erase("OPT_CONNECT_ATTR_ADD");
    opts.erase("OPT_CONNECT_ATTR_DELETE");
    opts["OPT_CONNECT_ATTR_ADD"]=connectAttrMap;
    opts["OPT_CONNECT_ATTR_DELETE"]=connectAttrList;

    created_objects.clear();
    conn1.reset(driver->connect(opts));

    stmt.reset(conn1->createStatement());
    res.reset(stmt->executeQuery("SELECT ATTR_NAME, ATTR_VALUE FROM "
                "performance_schema.session_account_connect_attrs WHERE "
                "ATTR_NAME LIKE '%keya%' ORDER BY ATTR_NAME ASC;"));
    ASSERT(res->next());
    ASSERT_EQUALS(res->getString("ATTR_NAME"), "keya1");
    ASSERT_EQUALS(res->getString("ATTR_VALUE"), "value1");

    ASSERT(res->next());
    ASSERT_EQUALS(res->getString("ATTR_NAME"), "keya2");
    ASSERT_EQUALS(res->getString("ATTR_VALUE"), "value2");

    ASSERT(!res->next());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  /*
    Check with inserting max allowed key-value pair i.e. lesser then size
    of performance_schema_session_connect_attrs_size
  */
  try
  {
    testsuite::Connection conn1;
    sql::ConnectOptionsMap opts;
    int max_count;

    opts["hostName"]=url;
    opts["userName"]=user;
    opts["password"]=passwd;

    created_objects.clear();
    conn1.reset(driver->connect(opts));

    stmt.reset(conn1->createStatement());
    res.reset(stmt->executeQuery("SHOW VARIABLES LIKE "
                      "'performance_schema_session_connect_attrs_size';"));

    ASSERT(res->next());
    ASSERT_EQUALS(res->getString("Variable_name"), "performance_schema_session_connect_attrs_size");
    int perf_conn_attr_size= res->getInt("Value");
    if (perf_conn_attr_size < 512) {
      SKIP("The performance_schema_session_connect_attrs_size is less then 512");
    } else if (perf_conn_attr_size >= 512 && perf_conn_attr_size < 1024) {
      max_count= 32;
    } else if (perf_conn_attr_size >= 1024 && perf_conn_attr_size < 2048) {
      max_count= 64;
    } else if (perf_conn_attr_size >= 2048) {
      max_count= 128;
    }

    try
    {
      testsuite::Connection conn2;
      std::map< sql::SQLString, sql::SQLString > connectAttrMap;
      std::list< std::string > connectAttrList;
      std::stringstream skey;
      int i;

      for (i=1; i <= max_count; ++i) {
        skey.str("");
        skey << "keymu" << i;
        connectAttrMap[skey.str()] = "value";
      }

      opts.erase("OPT_CONNECT_ATTR_ADD");
      opts.erase("OPT_CONNECT_ATTR_DELETE");
      opts["OPT_CONNECT_ATTR_ADD"]= connectAttrMap;

      created_objects.clear();
      conn2.reset(driver->connect(opts));

      stmt.reset(conn2->createStatement());
      res.reset(stmt->executeQuery("SELECT ATTR_NAME, ATTR_VALUE FROM "
            "performance_schema.session_account_connect_attrs WHERE "
            "ATTR_NAME LIKE '%keymu%' ORDER BY SUBSTRING(ATTR_NAME, 6)+0 ASC;"));

      i=0;
      while (res->next()) {
        skey.str("");
        skey << "keymu" << ++i;
        ASSERT_EQUALS(res->getString("ATTR_NAME"), skey.str());
        ASSERT_EQUALS(res->getString("ATTR_VALUE"), "value");
      }
      ASSERT(max_count == i);

      ASSERT(!res->next());
    }
    catch (sql::SQLException &e)
    {
      logErr(e.what());
      logErr("SQLState: " + std::string(e.getSQLState()));
      fail(e.what(), __FILE__, __LINE__);
    }
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void connection::connectAttrReset()
{
  logMsg("connection::connectAttr - MYSQL_OPT_CONNECT_ATTR_RESET");
  int serverVersion= getMySQLVersion(con);
  if ( serverVersion < 50606)
  {
    SKIP("The server does not support tested functionality(cleartext plugin enabling)");
  }

  try
  {
    testsuite::Connection conn2;
    sql::ConnectOptionsMap opts;
    std::map< sql::SQLString, sql::SQLString > connectAttrMap;

    opts["hostName"]=url;
    opts["userName"]=user;
    opts["password"]=passwd;

    connectAttrMap["keyd1"]="value1";
    connectAttrMap["keyd2"]="value2";
    connectAttrMap["keyd3"]="value3";

    opts.erase("OPT_CONNECT_ATTR_ADD");
    opts["OPT_CONNECT_ATTR_ADD"]=connectAttrMap;
    opts["OPT_CONNECT_ATTR_RESET"]=0;

    created_objects.clear();
    conn2.reset(driver->connect(opts));

    stmt.reset(conn2->createStatement());
    res.reset(stmt->executeQuery("SELECT ATTR_NAME, ATTR_VALUE FROM "
                "performance_schema.session_account_connect_attrs WHERE "
                "ATTR_NAME LIKE '%keyd%' ORDER BY ATTR_NAME ASC;"));
    ASSERT(!res->next());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void connection::connectCharsetDir()
{
  try
  {
    sql::ConnectOptionsMap opts;
    sql::SQLString charDir("/tmp/");

    opts["hostName"]=url;
    opts["userName"]=user;
    opts["password"]=passwd;
    opts["charsetDir"]=charDir;

    created_objects.clear();
    con.reset(driver->connect(opts));

    sql::SQLString outDir=con->getClientOption("characterSetDirectory");
    ASSERT_EQUALS(charDir, outDir);
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void connection::connectSSLEnforce()
{
  try
  {
    sql::ConnectOptionsMap opts;

    opts["hostName"]=url;
    opts["userName"]=user;
    opts["password"]=passwd;
    opts["sslEnforce"]=true;

    created_objects.clear();
    con.reset(driver->connect(opts));
  }
  catch (sql::SQLException &e)
  {
    ASSERT_EQUALS(2026, e.getErrorCode() /*CR_SSL_CONNECTION_ERROR*/);
  }
}


void connection::setAuthDir()
{
  logMsg("connection::setAuthDir - MYSQL_PLUGIN_DIR");
  int serverVersion=getMySQLVersion(con);
  if ( serverVersion >= 50703 )
  {
    SKIP("Server version >= 5.7.3 needed to run this test");
  }

  try
  {
    testsuite::Connection conn1;
    sql::ConnectOptionsMap opts;
    sql::SQLString in_plugin_dir;

    opts["hostName"]=url;
    opts["userName"]=user;
    opts["password"]=passwd;
#ifdef _WIN32
    in_plugin_dir=sql::SQLString("C:\test_plugin");
#else
    in_plugin_dir=sql::SQLString("\tmp\test_plugin");
#endif //_WIN32

    opts["pluginDir"]=in_plugin_dir;
    created_objects.clear();
    conn1.reset(driver->connect(opts));

    sql::SQLString out_plugin_dir=conn1->getClientOption(sql::SQLString("pluginDir"));

    ASSERT_EQUALS(in_plugin_dir, out_plugin_dir);

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void connection::setDefaultAuth()
{
  logMsg("connection::setDefaultAuth - MYSQL_DEFAULT_AUTH");
  int serverVersion=getMySQLVersion(con);
  if ( serverVersion < 50703 )
  {
    SKIP("Server version >= 5.7.3 needed to run this test");
  }

  try
  {
    testsuite::Connection conn1;
    sql::ConnectOptionsMap opts;
    sql::SQLString in_plugin_dir;
    sql::SQLString def_auth("test_set_default_password");

    opts["hostName"]=url;
    opts["userName"]=user;
    opts["password"]=passwd;
    opts["defaultAuth"]=def_auth;
    created_objects.clear();

    try
    {
      conn1.reset(driver->connect(opts));
    }
    catch (sql::SQLException &e)
    {
      /* Error expected as trying to load unknown authentication plugin */
      ASSERT_EQUALS(2059, e.getErrorCode()/*CR_AUTH_PLUGIN_CANNOT_LOAD_ERROR*/);
    }
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void connection::localInfile()
{
  logMsg("connection::setDefaultAuth - MYSQL_OPT_LOCAL_INFILE");

  struct dataObject
  {
    std::string m_path;

    dataObject(std::string path) : m_path(path)
    {}

    const char *path() { return m_path.c_str(); }
  };

  struct dataDir : public dataObject
  {
    void cleanup()
    {
      std::stringstream cmd;
#if defined(_WIN32)
      cmd << "rd /s /q \"" << m_path << "\"";
#else
      cmd << "rm -rf " << m_path;
#endif
      std::cout << "CLEANING-UP..." << std::endl
        << "EXECUTING SYSTEM COMMAND: " << cmd.str() << " [result:"
        << std::system(cmd.str().c_str()) << "]" << std::endl;
    }

    dataDir(std::string path, bool is_hidden = false) : dataObject(path)
    {
      cleanup();
      std::cout << "CREATING A DIRECTORY: " << path << std::endl;
      try
      {
#if defined(_WIN32)
        if(mkdir(m_path.c_str()))
#else
        if(mkdir(m_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
#endif
        {
          throw "Destination directory could not be created";
        }

#if defined(_WIN32)
        if (is_hidden && !SetFileAttributes(m_path.c_str(), FILE_ATTRIBUTE_HIDDEN))
        {
          throw "Could not set the hidden attribute";
        }
#endif
      }
      catch(...)
      {
        cleanup();
        throw;
      }
    }

    ~dataDir()
    {
      cleanup();
    }

  };

  /*
    Create a data file to be used for LOAD DATA LOCAL INFILE
    It does not need to do a clean-up because the directory destructor will
    take care of it.
  */
  struct dataFile : public dataObject
  {
    dataFile(std::string dir, std::string file_name, bool write_only = false) :
      dataObject(dir + file_name)
    {
      std::ofstream infile;
      infile.open(m_path, std::ios::out|std::ios::binary);
      infile << "1,\"val1\"\n";
      infile << "2,\"val2\"\n";
      infile.close();

#ifndef _WIN32
      /*
        In windows all files are always readable; it is not possible to
        give write-only permission. For this reason the write-only test
        is not done in Windows
      */
      if (write_only)
      {
        std::stringstream sstr;
        sstr << "chmod -r " << m_path;
        if(system(sstr.str().c_str()))
        {
          throw "File permissions could not be set";
        }
      }
#endif
    }
  };

#ifndef _WIN32
  /*
    Windows can create symlinks, but the admin access is required.
    Or the machine must be in Developer Mode.
  */
  struct dataSymlink : public dataObject
  {
    dataSymlink(std::string fpath, std::string spath, bool directory = false)
      : dataObject(spath)
    {
      std::cout << "CREATING SYMLINK: " << spath << " -> " << fpath << std::endl;
      if (symlink(fpath.c_str(), spath.c_str()))
        throw "Error creating symbolic link";
    }
  };
#endif
  struct testLocal
  {
    sql::ConnectOptionsMap m_opts;
    testsuite::Connection m_main_conn;
    testsuite::Statement m_main_stmt;

#ifdef _WIN32
    enum class slash_type { MAKE_FORWARD, MAKE_BACKWARD, MAKE_DOUBLE_BACKWARD };

    static std::string slash_fix(std::string s, slash_type st)
    {
      std::string s_find = "\\";
      std::string s_repl = "/";

      switch(st)
      {
        case slash_type::MAKE_FORWARD:
          s_find = "\\";
          s_repl = "/";
        break;
        case slash_type::MAKE_BACKWARD:
          s_find = "/";
          s_repl = "\\";
        break;
        case slash_type::MAKE_DOUBLE_BACKWARD:
          s_find = "/";
          s_repl = "\\\\";
        break;
      }

      size_t pos = s.find(s_find);
      while (pos != std::string::npos)
      {
        s.replace(pos, 1, s_repl);
        pos = s.find(s_find);
      }
      return s;
    }
#endif

    testLocal(sql::ConnectOptionsMap opts) : m_opts(opts)
    {
      m_main_conn.reset(driver->connect(opts));
      m_main_stmt.reset(m_main_conn->createStatement());
      m_main_stmt->execute("DROP TABLE IF EXISTS test_local_infile");
      m_main_stmt->execute("CREATE TABLE test_local_infile(id INT, value VARCHAR(20))");

    }

    ~testLocal()
    {
      m_main_stmt->execute("DROP TABLE IF EXISTS test_local_infile");
      set_server_infile(false);
    }

    void set_server_infile(bool enabled)
    {
      std::string q = "SET GLOBAL local_infile=";
      m_main_stmt->execute(q.append(enabled ? "1" : "0"));
    }

    /*
      Checks how setting nullptr for OPT_LOAD_DATA_LOCAL_DIR is working.

      1. Set OPT_LOAD_DATA_LOCAL_DIR using load_data_path parameter and
         check the parameter value using getClientOption(). It is expected
         that the value set by setClientOption() is returned by
         getClientOption().

      2. Use setClientOption() to set OPT_LOAD_DATA_LOCAL_DIR to nullptr.
         This can only be done after the connection is made.

      3. Use getClientOption() to check that the current value of
         OPT_LOAD_DATA_LOCAL_DIR is nullptr.

      The function returns true if all results are as expected.

      In windows the path must use the backslashes before the first check
      because it is always returned with the backslash through
      getClientOption()
    */
    bool do_null_test(std::string load_data_path)
    {
      sql::ConnectOptionsMap opts(m_opts);
      opts[OPT_LOCAL_INFILE] = 1;

#ifdef _WIN32
      // Need to convert to backslashes
      load_data_path = slash_fix(load_data_path, slash_type::MAKE_BACKWARD);
#endif

      opts[OPT_LOAD_DATA_LOCAL_DIR] = load_data_path;

      testsuite::Connection conn;
      conn.reset(driver->connect(opts));

      sql::SQLString orig_dir_path = load_data_path;
      sql::SQLString dir_path = conn->getClientOption(OPT_LOAD_DATA_LOCAL_DIR);
      ASSERT_EQUALS(orig_dir_path, dir_path);

      conn->setClientOption(OPT_LOAD_DATA_LOCAL_DIR, nullptr);

      char *out_dir_path = (char*)"a";
      conn->getClientOption(OPT_LOAD_DATA_LOCAL_DIR, &out_dir_path);

      if (nullptr != out_dir_path)
        return false;

      return true;
    }

    /*
      Performs the following test:

      1. Opens new connection with LOCAL_INFILE and LOAD_DATA_LOCAL_DIR
         parameters set to client_local_infile and load_data_path, respectively.

      2. If set_after_connect is true, the LOAD_DATA_LOCAL_DIR parameter
         is set after making connection.

      3. Server's local_infile global variable is set to the
         value of server_local_infile.

      4. A LOAD DATA LOCAL INFILE command is executed for the
         file specified by file_path.

      5. For Windows the function can use forward and backward slashes in the
         directory and file paths depending on use_back_slash parameter

      6. If above succeeds, the data loaded from the file is examined.

      Returns true if test was successful, false otherwise.
      If parameter res is false, then it is expected that LOAD DATA LOCAL
      command in step 4 should fail with error.
    */

    bool do_test(bool server_local_infile, bool client_local_infile,
                 const char *load_data_path, /* Uses forward slash if not null */
                 std::string file_path,
                 bool set_after_connect = false,
                 bool res = true, bool use_back_slash = false)
    {
      bool error_expected = !res;
      std::string t = load_data_path ? load_data_path : "nullptr";
      std::string file_path_query = file_path;

#ifdef _WIN32
      if (use_back_slash)
      {
        t = slash_fix(t, slash_type::MAKE_BACKWARD);
        file_path = slash_fix(file_path, slash_type::MAKE_BACKWARD);

        if (load_data_path && *load_data_path)
          load_data_path = t.c_str(); // use the back-slashed path
        /*
          The file path must be properly escaped for LOAD DATA LOCAL INFILE
          query. Therefore, it must use the double backslashes if needed.
        */
        file_path_query = slash_fix(file_path_query,
                                    slash_type::MAKE_DOUBLE_BACKWARD);
      }
#endif

      std::cout << " OPT_LOCAL_INFILE=" << (int)client_local_infile << std::endl <<
        " OPT_LOAD_DATA_LOCAL_DIR=" << t << std::endl <<
        " FILE_IN=" << file_path << std::endl;

      sql::ConnectOptionsMap opts(m_opts);
      opts["OPT_LOCAL_INFILE"] = (int)client_local_infile;

      if (load_data_path && !set_after_connect)
        opts[OPT_LOAD_DATA_LOCAL_DIR] = load_data_path;

      set_server_infile(server_local_infile);

      try
      {
        testsuite::Connection conn;
        conn.reset(driver->connect(opts));

        if (set_after_connect)
          conn->setClientOption(OPT_LOAD_DATA_LOCAL_DIR, load_data_path);

        testsuite::Statement stmt;
        stmt.reset(conn->createStatement());

        std::stringstream sstr;
        sstr << "LOAD DATA LOCAL INFILE '" << file_path_query <<
            "' INTO TABLE test_local_infile FIELDS TERMINATED BY ',' "
            "OPTIONALLY ENCLOSED BY '\"' LINES TERMINATED BY '\n'";

        std::cout << " Result: ";
        stmt->execute(sstr.str());

        testsuite::ResultSet res;
        res.reset(stmt->executeQuery("SELECT * FROM test_local_infile ORDER BY id ASC;"));
        ASSERT(res->next());
        ASSERT_EQUALS("1", res->getInt(1));
        std::string val1 = res->getString(2);
        ASSERT_EQUALS("val1", val1);

        ASSERT(res->next());
        ASSERT_EQUALS("2", res->getInt(1));
        ASSERT_EQUALS("val2", res->getString(2));
        ASSERT(!res->next());
      }
      catch(...)
      {
        std::cout << "Error (nothing is loaded)" << std::endl;

        if (error_expected)
          return true;

        return false;
      }

      std::cout << "Data is Loaded" << std::endl;
      if (error_expected)
        return false;

      return true;
    }
  };

  try
  {
    testsuite::Connection conn1;
    sql::ConnectOptionsMap opts;
    sql::SQLString in_plugin_dir;
    sql::SQLString schema("test");

    opts["hostName"]=url;
    opts["userName"]=user;
    opts["password"]=passwd;
    opts["schema"]=schema;
    opts["OPT_LOCAL_INFILE"]=1;

    created_objects.clear();

    {
      std::string temp_dir;
      std::string dir;
      std::string file_path;

#ifdef _WIN32
      temp_dir = getenv("TEMP");

      // easier to deal with forward slash
      temp_dir = testLocal::slash_fix(temp_dir,
                                      testLocal::slash_type::MAKE_FORWARD);
      temp_dir.append("/");

      dataDir hidden_dir(temp_dir + "HiddenTest/", true);
      dataFile file_in_hidden_dir(temp_dir + "HiddenTest/", "infile.txt");

#else
      temp_dir = "/tmp/";
#endif

      dir = temp_dir + "test/";
      file_path = dir + "infile.txt";

      dataDir dir_test(dir);
      dataDir dir_link(temp_dir + "test_link/");
      dataDir dir_subdir_link(temp_dir + "test_subdir_link/");

      dataFile infile(dir, "infile.txt");

#ifndef _WIN32
      dataFile infile_wo("/tmp/test/", "infile_wo.txt", true);
      dataSymlink sl(file_path, temp_dir + "test_link/link_infile.txt");
      dataSymlink sld(dir, temp_dir + "test_subdir_link/subdir");
      std::string sld_file = sld.path();
      sld_file.append("/infile.txt");
#endif

      testLocal null_test(opts);
      ASSERT(null_test.do_null_test(dir));

      struct paramtest
      {
        int opt_local_infile;
        const char *opt_load_data_local_dir;
        const char *file_in;
        bool expected_result;
      } params[] = {
        {0,"",                     infile.path(),                   false},
        {0,nullptr,                infile.path(),                   false},
        {0,dir_test.path(),        infile.path(),                   true},
        {1,dir_test.path(),        infile.path(),                   true},
        {0,"invalid_test/",        "invalid_test/infile.txt",       false},
#ifdef _WIN32
        {0,hidden_dir.path(),      file_in_hidden_dir.path(),       true}
#else
        {1,dir_test.path(),        sl.path(),                       true},
        {1,dir_test.path(),        sld_file.c_str(),                true},
        {0,dir_test.path(),        sld_file.c_str(),                true},
        {0,dir_test.path(),        sl.path(),                       true},
        {0,dir_link.path(),        sl.path(),                       false},
        {0,dir_subdir_link.path(), sld_file.c_str(),                false},
        {0,dir_test.path(),        infile_wo.path(),                false},
        {1,nullptr,                infile_wo.path(),                false}
#endif
      };

      for (int k = 0; k < 3; ++k)
      {
        bool server_local_infile = true;
        bool set_after_connect = false;
        std::cout << "---- TESTS SERIES: ";
        switch(k)
        {
          case 0:
            std::cout << "Normal scenario" << std::endl;
            break;

          case 1:
            std::cout << "Set OPT_LOAD_DATA_LOCAL_DIR after connect" << std::endl;
            set_after_connect = true;
            break;

          case 2:
            std::cout << "Disable OPT_LOCAL_INFILE on the server" << std::endl;
            server_local_infile = false;
            break;
        }

        int test_num = 0;
        for (auto elem : params)
        {
          std::cout << "-- TEST" << (++test_num) << std::endl;

          for(bool use_backslash : {false, true})
          {
            testLocal test(opts);
            ASSERT(test.do_test(server_local_infile,
                                (bool)elem.opt_local_infile,
                                elem.opt_load_data_local_dir,
                                elem.file_in,
                                set_after_connect,
                                server_local_infile ? elem.expected_result : false,
                                use_backslash));
#ifndef _WIN32
            break;
#endif
          }
        }

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


void connection::isValid()
{
  logMsg("connection::isValid");

  try
  {
    if (!con->isValid())
    {
      FAIL("Connection is not active");
    }
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void connection::reconnect()
{
  logMsg("connection::connectOptReconnect - OPT_RECONNECT");

  logMsg("OPT_RECONNECT disabled");
  try
  {
    try
    {
      sql::ConnectOptionsMap connection_properties;

      connection_properties["hostName"]=url;
      connection_properties["userName"]=user;
      connection_properties["password"]=passwd;
      connection_properties["OPT_READ_TIMEOUT"]=1;

      connection_properties.erase("OPT_RECONNECT");
      connection_properties["OPT_RECONNECT"]=false;

      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      con->setSchema(db);
      stmt.reset(con->createStatement());
      res.reset(stmt->executeQuery("SELECT sleep(10);"));
      FAIL("Connection didn't timed out");
    }
    catch (sql::SQLException &/*e*/)
    {
      ASSERT(con->reconnect());
      res.reset(stmt->executeQuery("SELECT 1;"));
      ASSERT(res->next());
      ASSERT_EQUALS(res->getInt(1), 1);
    }
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }


  logMsg("OPT_RECONNECT enabled");
  try
  {
    try
    {
      sql::ConnectOptionsMap connection_properties;

      connection_properties["hostName"]=url;
      connection_properties["userName"]=user;
      connection_properties["password"]=passwd;
      connection_properties["OPT_READ_TIMEOUT"]=1;

      connection_properties.erase("OPT_RECONNECT");
      connection_properties["OPT_RECONNECT"]=true;

      created_objects.clear();
      con.reset(driver->connect(connection_properties));
      con->setSchema(db);
      stmt.reset(con->createStatement());
      res.reset(stmt->executeQuery("SELECT sleep(10);"));
      FAIL("Connection didn't timed out");
    }
    catch (sql::SQLException &/*e*/)
    {
      ASSERT(con->reconnect());
      res.reset(stmt->executeQuery("SELECT 1;"));
      ASSERT(res->next());
      ASSERT_EQUALS(res->getInt(1), 1);
    }
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connection::ssl_mode()
{
  logMsg("connection::ssl_mode - OPT_SSL_MODE");

  sql::ConnectOptionsMap connection_properties;

  connection_properties["hostName"]=url;
  connection_properties["userName"]=user;
  connection_properties["password"]=passwd;

  connection_properties["OPT_SSL_MODE"] = sql::SSL_MODE_DISABLED;

  created_objects.clear();
  con.reset(driver->connect(connection_properties));

  con->setSchema(db);
  stmt.reset(con->createStatement());

  res.reset(stmt->executeQuery("SHOW GLOBAL VARIABLES LIKE 'tls_version'"));

  res->next();

  std::string tls_versions = res->getString(2);

  std::cout << "TLS VERSIONS: " <<tls_versions << std::endl;

  if (tls_versions.empty())
  {
    SKIP("Server doesn't support SSL connections");
  }

  res.reset(stmt->executeQuery("SHOW SESSION STATUS LIKE 'Ssl_version'"));

  res->next();

  ASSERT_EQUALS(0, static_cast<int>(res->getString(2).length()));

  connection_properties["OPT_SSL_MODE"] = sql::SSL_MODE_REQUIRED;

  try
  {
    created_objects.clear();
    con.reset(driver->connect(connection_properties));
  }
  catch (std::exception&)
  {
    SKIP("Server doesn't support SSL connections");
  }

  con->setSchema(db);
  stmt.reset(con->createStatement());
  res.reset(stmt->executeQuery("SHOW SESSION STATUS LIKE 'Ssl_version'"));

  res->next();

  ASSERT_GT(0, static_cast<int>(res->getString(2).length()));

  stmt->execute("Drop user if exists ssluser");
  stmt->execute("CREATE USER 'ssluser' IDENTIFIED BY 'sslpass' require SSL");
  stmt->execute("GRANT all on test.* to 'ssluser'");

  connection_properties["hostName"]=url;
  connection_properties["userName"]="ssluser";
  connection_properties["password"]="sslpass";

  connection_properties["OPT_SSL_MODE"] = sql::SSL_MODE_REQUIRED;

  created_objects.clear();
  con.reset(driver->connect(connection_properties));

  connection_properties["OPT_SSL_MODE"] = sql::SSL_MODE_DISABLED;

  //only to trigger setssl which changes SSL_MODE
  connection_properties["sslCA"] = "invalid_path";

  created_objects.clear();

  try
  {
    con.reset(driver->connect(connection_properties));
    FAIL("User requires SSL");
  } catch (std::exception &e)
  {
    std::cout << e.what() << std::endl;
  }

}

void connection::tls_version()
{
  logMsg("connection::tls_version - OPT_TLS_VERSION");

  sql::ConnectOptionsMap connection_properties;

  connection_properties["hostName"]=url;
  connection_properties["userName"]=user;
  connection_properties["password"]=passwd;

  connection_properties["OPT_SSL_MODE"] = sql::SSL_MODE_DISABLED;

  created_objects.clear();
  con.reset(driver->connect(connection_properties));

  con->setSchema(db);
  stmt.reset(con->createStatement());
  res.reset(stmt->executeQuery("SHOW GLOBAL VARIABLES LIKE 'tls_version'"));

  res->next();

  std::string tls_available = res->getString(2);

  std::cout << "TLS VERSIONS: " <<tls_available << std::endl;

  std::vector<std::string> tls_versions;

  size_t begin_pos = 0;

  for ( size_t end_pos = tls_available.find_first_of(',',begin_pos);
       begin_pos != std::string::npos || end_pos != std::string::npos;
       begin_pos = end_pos == std::string::npos ? end_pos : end_pos+1,
       end_pos = tls_available.find_first_of(',',begin_pos))
  {
    tls_versions.push_back(tls_available.substr(begin_pos, end_pos-begin_pos));
  }

  connection_properties["OPT_SSL_MODE"] = sql::SSL_MODE_REQUIRED;

  // Using wrong TLS version... should fail to connect
  connection_properties["OPT_TLS_VERSION"] = sql::SQLString("TLSv999");

  created_objects.clear();
  try
  {
    con.reset(driver->connect(connection_properties));
    FAIL("Wrong TLS version used and still can connect!");
  }
  catch (sql::SQLException &)
  {
    //Should FAIL to connect
  }


  for (std::vector<std::string>::const_iterator version = tls_versions.begin();
       version != tls_versions.end();
       ++version)
  {
    connection_properties["OPT_TLS_VERSION"] = sql::SQLString(*version);

    created_objects.clear();
    try
    {
      con.reset(driver->connect(connection_properties));
    }
    catch (sql::SQLException &e)
    {
      //Server exports TLS_VERSION even if no certs installed...
      //So skipping anyway if error on connect
      std::cout << "SKIP "<< *version << ": " << e.what() << std::endl;
      continue;
    }

    stmt.reset(con->createStatement());
    res.reset(stmt->executeQuery("SHOW SESSION STATUS LIKE 'Ssl_version'"));

    res->next();

    ASSERT_EQUALS(*version, res->getString(2));
  }

}

void connection::cached_sha2_auth()
{

  logMsg("connection::auth - MYSQL_OPT_GET_SERVER_PUBLIC_KEY");

  if (getMySQLVersion(con) < 80000)
  {
    SKIP("Server doesn't support caching_sha2_password");
    return;
  }

  try {
    stmt->execute("DROP USER 'doomuser'@'%';");
  } catch (...) {}


  stmt->execute("CREATE USER 'doomuser'@'%' IDENTIFIED WITH caching_sha2_password BY '!sha2user_pass';");

  sql::ConnectOptionsMap opts;
  opts["hostName"] = url;
  opts["userName"] = "doomuser";
  opts["password"] = "!sha2user_pass";
  opts["OPT_GET_SERVER_PUBLIC_KEY"] = false;
  opts["OPT_SSL_MODE"] = sql::SSL_MODE_DISABLED;

  try {

    // Should fail using unencrypted connection, since we don't have server
    // public key
    created_objects.clear();
    //need to close connection, otherwise will use fast auth!
    con->close();
    con.reset(driver->connect(opts));
    FAIL("caching_sha2_password can't be used on unexcrypted connection");
    throw "caching_sha2_password can't be used on unexcrypted connection";
  }
  catch(std::exception &e)
  {
    std::stringstream err;
    err << "Expected error: ";
    err << e.what();
    logMsg(err.str());
  }

  opts["OPT_GET_SERVER_PUBLIC_KEY"] = true;

  // Now we can connect using unencrypted connection, since we now can ask for
  // the server public key
  con.reset(driver->connect(opts));

  //Now using fast auth!
  con->close();
  opts["OPT_GET_SERVER_PUBLIC_KEY"] = false;
  con.reset(driver->connect(opts));

  // Cleanup
  con.reset(getConnection());
  stmt.reset(con->createStatement());
  stmt->execute("DROP USER 'doomuser'@'%';");
}

void connection::socket()
{
  logMsg("connection::socket - connection using unix socket");

  const char* socket = getenv("MYSQL_SOCKET");
  if(socket)
  {
    sql::ConnectOptionsMap connection_properties;

    connection_properties[OPT_SOCKET]=socket;
    connection_properties[OPT_USERNAME]=user;
    connection_properties[OPT_PASSWORD]=passwd;

    con.reset(driver->connect(connection_properties));
    if(!con)
      FAIL("Could not connect using socket on connection properties");

    std::stringstream uri;

    uri << "unix://" << socket;

    con.reset(driver->connect(uri.str(), user, passwd));
    if(!con)
      FAIL("Could not connect using socket on connection properties");

    con.reset(getConnection());
  }
  else
  {
    logMsg("test skipped...");
  }

}

void connection::dns_srv()
{
  logMsg("connection::dns_srv - connection using dns+srv service");

  const char* dns_srv = getenv("MYSQL_SRV");
  if(dns_srv)
  {
    for(int i = 0; i < 100; ++i)
    {
      sql::ConnectOptionsMap connection_properties;

      connection_properties[OPT_HOSTNAME]=dns_srv;
      connection_properties[OPT_USERNAME]=user;
      connection_properties[OPT_PASSWORD]=passwd;
      connection_properties[OPT_DNS_SRV] = true;

      con.reset(driver->connect(connection_properties));
      if(!con)
        FAIL("Could not connect using socket on connection properties");

      stmt.reset(con->createStatement());

      res.reset(stmt->executeQuery("show session variables like 'server_id'"));

      ASSERT(res->next());

      std::cout << res->getString(2) << std::endl;
    }

    con.reset(getConnection());
  }
}

void connection::mfa()
{
  logMsg("connection::mfa - multi factor authentication");

  try {
    stmt->execute("UNINSTALL PLUGIN cleartext_plugin_server");
  }  catch (...) {
  }

  try {
    stmt->execute("INSTALL PLUGIN cleartext_plugin_server SONAME 'auth_test_plugin.so'");
  }  catch (...) {
    SKIP("Server doesn't support auth test plugin cleartext_plugin_server");
  }

  struct MFA_TEST_DATA
  {
    const char* user;
    const char* pwd;
    const char* pwd1;
    const char* pwd2;
    const char* pwd3;
    bool succeed;
  };


  MFA_TEST_DATA test_data[] =
  {
  // user1 tests
  {"user_1f", "pass1", nullptr, nullptr, nullptr, true  },
  {"user_1f", "pass1", "pass1", nullptr, nullptr, true  },
  {"user_1f", "badp1", "pass1", nullptr, nullptr, true  },
  {"user_1f", nullptr, "pass1", nullptr, nullptr, true  },

  {"user_1f", nullptr, nullptr, nullptr, nullptr, false },
  {"user_1f", "badp1", "badp1", "pass1", nullptr, false },
  {"user_1f", nullptr, nullptr, "pass1", nullptr, false },
  {"user_1f", nullptr, nullptr, nullptr, "pass1", false },

  // user2 tests
  {"user_2f", "pass1", nullptr, "pass2", nullptr, true  },
  {"user_2f", "pass1", "pass1", "pass2", nullptr, true  },
  {"user_2f", "badp1", "pass1", "pass2", nullptr, true  },
  {"user_2f", nullptr, "pass1", "pass2", nullptr, true  },

  {"user_2f", "pass2", nullptr, "pass1", nullptr, false },
  {"user_2f", "pass2", "pass2", "pass1", nullptr, false },
  {"user_2f", "pass2", "badp2", "pass1", nullptr, false },
  {"user_2f", nullptr, "pass2", "pass1", nullptr, false },

  {"user_2f", "pass1", nullptr, nullptr, "pass2", false },
  {"user_2f", "pass1", "pass1", nullptr, "pass2", false },
  {"user_2f", "badp1", "pass1", nullptr, "pass2", false },
  {"user_2f", nullptr, "pass1", nullptr, "pass2", false },

  {"user_2f", "pass1", nullptr   , "badp1", "pass2", false },
  {"user_2f", "pass1", "pass1", "badp1", "pass2", false },
  {"user_2f", "badp1", "pass1", "badp1", "pass2", false },
  {"user_2f", nullptr   , "pass1", "badp1", "pass2", false },

  // user3 tests
  {"user_3f", "pass1", nullptr   , "pass2", "pass3", true  },
  {"user_3f", "pass1", "pass1", "pass2", "pass3", true  },
  {"user_3f", "badp1", "pass1", "pass2", "pass3", true  },
  {"user_3f", nullptr   , "pass1", "pass2", "pass3", true  },

  {"user_3f", "pass1", nullptr   , "pass3", "pass2", false },
  {"user_3f", "pass1", "pass1", "pass3", "pass2", false },
  {"user_3f", "badp1", "pass1", "pass3", "pass2", false },
  {"user_3f", nullptr   , "pass1", "pass3", "pass2", false },

  {"user_3f", "pass3", nullptr   , "badp1", "pass2", false },
  {"user_3f", "pass3", "pass3", "badp1", "pass2", false },
  {"user_3f", "pass3", "badp3", "badp1", "pass2", false },
  {"user_3f", nullptr   , "pass3", "badp1", "pass2", false },

  {"user_3f", "pass1", nullptr   , "pass2", "badp3", false },
  {"user_3f", "pass1", "pass1", "pass2", "badp3", false },
  {"user_3f", "badp1", "pass1", "pass2", "badp3", false },
  {"user_3f", nullptr   , "pass1", "pass2", "badp3", false },

  };


  stmt->execute("drop user if exists  user_1f");
  stmt->execute("drop user if exists  user_2f");
  stmt->execute("drop user if exists  user_3f");

  stmt->execute("create user user_1f IDENTIFIED WITH cleartext_plugin_server BY 'pass1'");
  stmt->execute("create user user_2f IDENTIFIED WITH cleartext_plugin_server BY 'pass1' "
                "AND IDENTIFIED WITH cleartext_plugin_server BY 'pass2'; ");
  stmt->execute("create user user_3f IDENTIFIED WITH cleartext_plugin_server by 'pass1' "
                "AND IDENTIFIED WITH cleartext_plugin_server BY 'pass2' "
                "AND IDENTIFIED WITH cleartext_plugin_server BY 'pass3'; ");


  auto check_connection = [this] (sql::Connection* conn) -> void
  {
    std::unique_ptr<sql::Statement> my_stmt(conn->createStatement());
    std::unique_ptr<sql::ResultSet> my_res(my_stmt->executeQuery("select @@version"));
    my_res->next();
    std::string version = my_res->getString(1);

    logMsg(std::string("Server Version ")+version);

    delete conn;
  };

  for(auto &data : test_data)
  {
    sql::ConnectOptionsMap opt;
    opt[OPT_ENABLE_CLEARTEXT_PLUGIN] = true;
    opt[OPT_USERNAME] = data.user;
    if(data.pwd)
      opt[OPT_PASSWORD] = data.pwd;
    if(data.pwd1)
      opt[OPT_PASSWORD1] = data.pwd1;
    if(data.pwd2)
      opt[OPT_PASSWORD2] = data.pwd2;
    if(data.pwd3)
      opt[OPT_PASSWORD3] = data.pwd3;

    if(data.succeed)
    {
      check_connection(getConnection(&opt));
    }
    else
    {
      try {
        getConnection(&opt);
        FAIL("Should fail to connect");
      }  catch (sql::SQLException&) {
      }
    }
  }

}

} /* namespace connection */
} /* namespace testsuite */
