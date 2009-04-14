/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include <cppconn/warning.h>

#include "connection.h"
#include <stdlib.h>
#include <cppconn/connection.h>
#include <driver/mysql_connection.h>
#include <cppconn/exception.h>

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
    bool input_value=true;
    bool output_value=false;
    void * input;
    void * output;

    input=(static_cast<bool *> (&input_value));
    output=(static_cast<bool *> (&output_value));

    con->setClientOption("metadataUseInfoSchema", input);
    con->getClientOption("metadataUseInfoSchema", output);

    ASSERT_EQUALS(input_value, output_value);

    input_value=false;
    con->setClientOption("metadataUseInfoSchema", input);
    con->getClientOption("metadataUseInfoSchema", output);
    ASSERT_EQUALS(input_value, output_value);
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
    std::auto_ptr< sql::mysql::MySQL_Connection > my_con(dynamic_cast<sql::mysql::MySQL_Connection*> (driver->connect(url, user, passwd)));
    value=my_con->getSessionVariable("sql_mode");

    my_con->setSessionVariable("sql_mode", "ANSI");
    // The server will translate ANSI into something that is version dependent - 
    // ASSERT_EQUALS(my_con->getSessionVariable("sql_mode"), "");

    my_con->setSessionVariable("sql_mode", value);
    ASSERT_EQUALS(value, my_con->getSessionVariable("sql_mode"));

    value=my_con->getSessionVariable("sql_warnings");

    std::string on("ON");
    std::string off("OFF");
    try
    {
      my_con->setSessionVariable("sql_warnings", "0");
      on="1";
      off="0";
    }
    catch (sql::SQLException &)
    {
    }

    try
    {
      my_con->setSessionVariable("sql_warnings", on);
      ASSERT_EQUALS(on, my_con->getSessionVariable("sql_warnings"));
      my_con->setSessionVariable("sql_warnings", off);
      ASSERT_EQUALS(off, my_con->getSessionVariable("sql_warnings"));
    }
    catch (sql::SQLException &)
    {
    }

    my_con->setSessionVariable("sql_warnings", value);
    ASSERT_EQUALS(value, my_con->getSessionVariable("sql_warnings"));
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
      std::string reason(exceptionIsOK(e, "HY000", 1000));
      if (!reason.empty())
        FAIL(reason);
    }


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
    std::map<std::string, sql::ConnectPropertyVal> connection_properties;

    {
      sql::ConnectPropertyVal tmp;
      /* url comes from the unit testing framework */
      tmp.str.val=url.c_str();
      tmp.str.len=url.length();
      connection_properties[std::string("hostName")]=tmp;
    }

    {
      sql::ConnectPropertyVal tmp;
      /* user comes from the unit testing framework */
      tmp.str.val=user.c_str();
      tmp.str.len=user.length();
      connection_properties[std::string("userName")]=tmp;
    }

    {
      sql::ConnectPropertyVal tmp;
      tmp.str.val=passwd.c_str();
      tmp.str.len=passwd.length();
      connection_properties[std::string("password")]=tmp;
    }

    {
      sql::ConnectPropertyVal tmp;
      tmp.bval= !TestsRunner::getStartOptions()->getBool("dont-use-is");
      connection_properties[std::string("metadataUseInfoSchema")]=tmp;
    }

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
          sql::ConnectPropertyVal tmp;
          tmp.lval=(long long) port;
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
          sql::ConnectPropertyVal tmp;
          tmp.lval=(long long) port;
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
        sql::ConnectPropertyVal tmp;
        tmp.lval=(long long) port;
        connection_properties[std::string("port")]=tmp;
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

        sql::ConnectPropertyVal tmp;
        tmp.str.val=schema.c_str();
        tmp.str.len=schema.length();
        connection_properties[std::string("schema")]=tmp;

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
        tmp.str.val=myschema.c_str();
        tmp.str.len=myschema.length();
        connection_properties[std::string("schema")]=tmp;

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
        sql::ConnectPropertyVal tmp;
        tmp.str.val=myschema.c_str();
        tmp.str.len=myschema.length();
        connection_properties[std::string("schema")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.str.val=sql.c_str();
      tmp.str.len=sql.length();
      connection_properties[std::string("sslKey")]=tmp;
      connection_properties[std::string("sslCert")]=tmp;
      connection_properties[std::string("sslCA")]=tmp;
      connection_properties[std::string("sslCAPath")]=tmp;
      connection_properties[std::string("sslCipher")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.bval=true;
      connection_properties[std::string("CLIENT_COMPRESS")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties[std::string("CLIENT_COMPRESS")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_COMPRESS");
      tmp.lval=(long long) false;
      connection_properties[std::string("CLIENT_COMPRESS")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.bval=true;
      connection_properties[std::string("CLIENT_FOUND_ROWS")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties[std::string("CLIENT_FOUND_ROWS")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_FOUND_ROWS");
      tmp.lval=(long long) false;
      connection_properties[std::string("CLIENT_FOUND_ROWS")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.bval=true;
      connection_properties[std::string("CLIENT_IGNORE_SIGPIPE")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties[std::string("CLIENT_IGNORE_SIGPIPE")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_IGNORE_SIGPIPE");
      tmp.lval=(long long) false;
      connection_properties[std::string("CLIENT_IGNORE_SIGPIPE")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.bval=true;
      connection_properties[std::string("CLIENT_IGNORE_SPACE")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties[std::string("CLIENT_IGNORE_SPACE")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_IGNORE_SPACE");
      tmp.lval=(long long) false;
      connection_properties[std::string("CLIENT_IGNORE_SPACE")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.bval=true;
      connection_properties[std::string("CLIENT_INTERACTIVE")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties[std::string("CLIENT_INTERACTIVE")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_INTERACTIVE");
      tmp.lval=(long long) false;
      connection_properties[std::string("CLIENT_INTERACTIVE")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.bval=true;
      connection_properties[std::string("CLIENT_LOCAL_FILES")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties[std::string("CLIENT_LOCAL_FILES")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_LOCAL_FILES");
      tmp.lval=(long long) false;
      connection_properties[std::string("CLIENT_LOCAL_FILES")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.bval=true;
      connection_properties[std::string("CLIENT_MULTI_RESULTS")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties[std::string("CLIENT_MULTI_RESULTS")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_MULTI_RESULTS");
      tmp.lval=(long long) false;
      connection_properties[std::string("CLIENT_MULTI_RESULTS")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.bval=true;
      connection_properties[std::string("CLIENT_MULTI_STATEMENTS")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties[std::string("CLIENT_MULTI_STATEMENTS")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_MULTI_STATEMENTS");
      tmp.lval=(long long) false;
      connection_properties[std::string("CLIENT_MULTI_STATEMENTS")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.bval=true;
      connection_properties[std::string("CLIENT_NO_SCHEMA")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties[std::string("CLIENT_NO_SCHEMA")]=tmp;
      try
      {
        created_objects.clear();
        con.reset(driver->connect(connection_properties));
      }
      catch (sql::SQLException &)
      {
      }

      connection_properties.erase("CLIENT_NO_SCHEMA");
      tmp.lval=(long long) false;
      connection_properties[std::string("CLIENT_NO_SCHEMA")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      /*
       C-API does not care about the actual value, its passed down to the OS,
       The OS may or may not detect bogus values such as negative values.
       */
      tmp.lval=(long long) 1;
      connection_properties[std::string("OPT_CONNECT_TIMEOUT")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      /*
       C-API does not care about the actual value, its passed down to the OS,
       The OS may or may not detect bogus values such as negative values.
       */
      tmp.lval=(long long) 1;
      connection_properties[std::string("OPT_READ_TIMEOUT")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      /* C-API does not care about the actual value */
      tmp.lval=(long long) 1;
      connection_properties[std::string("OPT_WRITE_TIMEOUT")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      /* C-API does not care about the actual value */
      tmp.lval=(long long) 1;
      connection_properties[std::string("OPT_RECONNECT")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      /* C-API does not care about the actual value */
      tmp.str.val=charset.c_str();
      tmp.str.len=charset.length();
      connection_properties[std::string("OPT_SET_CHARSET_NAME")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      /* C-API does not care about the actual value */
      tmp.str.val=charset.c_str();
      tmp.str.len=charset.length();
      connection_properties[std::string("REPORT_DATA_TRUNCATION")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.bval=true;
      connection_properties[std::string("metadataUseInfoSchema")]=tmp;
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
      tmp.bval=false;
      connection_properties[std::string("metadataUseInfoSchema")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.lval=sql::ResultSet::TYPE_FORWARD_ONLY;
      connection_properties[std::string("defaultStatementResultType")]=tmp;
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
      tmp.lval=sql::ResultSet::TYPE_SCROLL_INSENSITIVE;
      connection_properties[std::string("defaultStatementResultType")]=tmp;
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
      tmp.lval=sql::ResultSet::TYPE_SCROLL_SENSITIVE;
      connection_properties[std::string("defaultStatementResultType")]=tmp;
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

#ifdef CPPWIN_WIN32
    /* 21) OPT_NAMED_PIPE - handled but ignored! */
    connection_properties.erase("OPT_NAMED_PIPE");
    {
      logMsg("... testing OPT_NAMED_PIPE");
      sql::ConnectPropertyVal tmp;
      std::string pipe("IGNORED");
      tmp.str.val=pipe.c_str();
      tmp.str.len=pipe.length();
      connection_properties[std::string("OPT_NAMED_PIPE")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      std::string charset("utf8");
      tmp.str.val=charset.c_str();
      tmp.str.len=charset.length();
      connection_properties[std::string("OPT_CHARSET_NAME")]=tmp;
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
      sql::ConnectPropertyVal tmp;
      tmp.bval=true;
      connection_properties[std::string("OPT_REPORT_DATA_TRUNCATION")]=tmp;
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
      tmp.bval=false;
      connection_properties[std::string("OPT_REPORT_DATA_TRUNCATION")]=tmp;
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
    res.reset(stmt->executeQuery("SHOW VARIABLES LIKE 'tx_isolation'"));
    checkResultSetScrolling(res);
    res->next();
    ASSERT_EQUALS("READ-COMMITTED", res->getString("Value"));

    con->setTransactionIsolation(sql::TRANSACTION_READ_UNCOMMITTED);
    ASSERT_EQUALS(sql::TRANSACTION_READ_UNCOMMITTED, con->getTransactionIsolation());
    res.reset(stmt->executeQuery("SHOW VARIABLES LIKE 'tx_isolation'"));
    res->next();
    ASSERT_EQUALS("READ-UNCOMMITTED", res->getString("Value"));

    con->setTransactionIsolation(sql::TRANSACTION_REPEATABLE_READ);
    ASSERT_EQUALS(sql::TRANSACTION_REPEATABLE_READ, con->getTransactionIsolation());
    res.reset(stmt->executeQuery("SHOW VARIABLES LIKE 'tx_isolation'"));
    res->next();
    ASSERT_EQUALS("REPEATABLE-READ", res->getString("Value"));

    con->setTransactionIsolation(sql::TRANSACTION_SERIALIZABLE);
    ASSERT_EQUALS(sql::TRANSACTION_SERIALIZABLE, con->getTransactionIsolation());
    res.reset(stmt->executeQuery("SHOW VARIABLES LIKE 'tx_isolation'"));
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

} /* namespace connection */
} /* namespace testsuite */
