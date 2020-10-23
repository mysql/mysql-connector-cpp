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



/**
* Basic example demonstrating connect and simple queries
*
*/


// Standard C++ includes
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

/*
  Public interface of the MySQL Connector/C++.
  You might not use it but directly include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include <jdbc.h>
/* Connection parameter and sample data */
#include "examples.h"

using namespace std;

/**
* Usage example for Driver, Connection, (simple) Statement, ResultSet
*/
int main(int argc, const char **argv)
{
  const char* mysql_host = getenv("MYSQL_HOST");
  const char* mysql_port = getenv("MYSQL_PORT");
  const char* mysql_user = getenv("MYSQL_USER");
  const char* mysql_password = getenv("MYSQL_PASSWORD");
  const char* ldap_user = getenv("LDAP_USER");
  const char* ldap_user_dn = getenv("LDAP_USER_DN");
  const char* ldap_simple_pwd = getenv("LDAP_SIMPLE_PWD");
  const char* ldap_scram_pwd = getenv("LDAP_SCRAM_PWD");
  const char* plugin_dir = getenv("PLUGIN_DIR");

  string url(EXAMPLE_HOST);
  string user(EXAMPLE_USER);
  string pass(EXAMPLE_PASS);
  string database(argc >= 5 ? argv[4] : EXAMPLE_DB);

  if(argc >= 2)
  {
    url = argv[1];
  }
  else if(mysql_host || mysql_port)
  {
    url = "mysql://";
    url+= mysql_host ? mysql_host : "localhost";
    url+= ":";
    url+= mysql_port ? mysql_port : "3306";
  }

  if(argc >= 3)
  {
    user = argv[2];
  }
  else if(mysql_user)
  {
    user = mysql_user;
  }

  if(argc >= 4)
  {
    pass = argv[3];
  }
  else if(mysql_password)
  {
    pass = mysql_password;
  }

  /* sql::ResultSet.rowsCount() returns size_t */
  size_t row;
  stringstream sql;
  stringstream msg;
  int i, affected_rows;

  cout << boolalpha;
  cout << "1..1" << endl;
  cout << "# Connector/C++ connect basic usage example.." << endl;
  cout << "#" << endl;

  cout << "# " << url  << endl;
  cout << "# " << user << endl;
  cout << "# " << pass << endl;

  try {
    sql::Driver * driver = sql::mysql::get_driver_instance();

    /* Using the Driver to create a connection */
    std::unique_ptr< sql::Connection > con(driver->connect(url, user, pass));

    /* Creating a "simple" statement - "simple" = not a prepared statement */
    std::unique_ptr< sql::Statement > stmt(con->createStatement());

    /* Create a test table demonstrating the use of sql::Statement.execute() */
    stmt->execute("USE " + database);
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT, label CHAR(1))");
    cout << "#\t Test table created" << endl;

    /* Populate the test table with data */
    for (i = 0; i < EXAMPLE_NUM_TEST_ROWS; i++) {
      /*
      KLUDGE: You should take measures against SQL injections!
      example.h contains the test data
      */
      sql.str("");
      sql << "INSERT INTO test(id, label) VALUES (";
      sql << test_data[i].id << ", '" << test_data[i].label << "')";
      stmt->execute(sql.str());
    }
    cout << "#\t Test table populated" << endl;

    {
      /*
      Run a query which returns exactly one result set like SELECT
      Stored procedures (CALL) may return more than one result set
      */
      std::unique_ptr< sql::ResultSet > res(stmt->executeQuery("SELECT id, label FROM test ORDER BY id ASC"));
      cout << "#\t Running 'SELECT id, label FROM test ORDER BY id ASC'" << endl;

      /* Number of rows in the result set */
      cout << "#\t\t Number of rows\t";
      cout << "res->rowsCount() = " << res->rowsCount() << endl;
      if (res->rowsCount() != EXAMPLE_NUM_TEST_ROWS) {
        msg.str("");
        msg << "Expecting " << EXAMPLE_NUM_TEST_ROWS << "rows, found " << res->rowsCount();
        throw runtime_error(msg.str());
      }

      /* Fetching data */
      row = 0;
      while (res->next()) {
        cout << "#\t\t Fetching row " << row << "\t";
        /* You can use either numeric offsets... */
        cout << "id = " << res->getInt(1);
        /* ... or column names for accessing results. The latter is recommended. */
        cout << ", label = '" << res->getString("label") << "'" << endl;
        row++;
      }
    }

    {
      /* Fetching again but using type convertion methods */
      std::unique_ptr< sql::ResultSet > res(stmt->executeQuery("SELECT id FROM test ORDER BY id DESC"));
      cout << "#\t Fetching 'SELECT id FROM test ORDER BY id DESC' using type conversion" << endl;
      row = 0;
      while (res->next()) {
        cout << "#\t\t Fetching row " << row;
        cout << "#\t id (int) = " << res->getInt("id");
        cout << "#\t id (boolean) = " << res->getBoolean("id");
        cout << "#\t id (long) = " << res->getInt64("id") << endl;
        row++;
      }
    }

    /* Usage of UPDATE */
    stmt->execute("INSERT INTO test(id, label) VALUES (100, 'z')");
    affected_rows = stmt->executeUpdate("UPDATE test SET label = 'y' WHERE id = 100");
    cout << "#\t UPDATE indicates " << affected_rows << " affected rows" << endl;
    if (affected_rows != 1) {
      msg.str("");
      msg << "Expecting one row to be changed, but " << affected_rows << "change(s) reported";
      throw runtime_error(msg.str());
    }

    {
      std::unique_ptr< sql::ResultSet > res(stmt->executeQuery("SELECT id, label FROM test WHERE id = 100"));

      res->next();
      if ((res->getInt("id") != 100) || (res->getString("label") != "y")) {
        msg.str("Update must have failed, expecting 100/y got");
        msg << res->getInt("id") << "/" << res->getString("label");
        throw runtime_error(msg.str());
      }

      cout << "#\t\t Expecting id = 100, label = 'y' and got id = " << res->getInt("id");
      cout << ", label = '" << res->getString("label") << "'" << endl;
    }

    /* Clean up */
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt.reset(NULL); /* free the object inside  */

    cout << "#" << endl;
    cout << "#\t Demo of connection URL syntax" << endl;

    //Lets first create ldap and sasl users
    if(ldap_user || ldap_user_dn)
    {
      if(ldap_user_dn)
      {
        stringstream user_create;
        stmt.reset(con->createStatement());
        user_create << "CREATE USER ldap_simple IDENTIFIED WITH "
                       "authentication_ldap_simple AS \""
                    << ldap_user_dn
                    << "\"";
        try{
        stmt->execute(user_create.str());
        }catch(...)
        {}
      }

      if(ldap_user)
      {
        stringstream user_create;
        stmt.reset(con->createStatement());
        user_create << "CREATE USER "
                    << ldap_user
                    << " IDENTIFIED WITH authentication_ldap_sasl";
        try{
        stmt->execute(user_create.str());
        }catch(...)
        {}
      }
    }

    if(ldap_simple_pwd)
    {
      //AUTH USING SASL client side plugin
      try {
        /*
         When using ldap simple authentication, we need to enable cleartext
         plugin
        */

        sql::ConnectOptionsMap opts;
        opts[OPT_HOSTNAME] = url;
        opts[OPT_USERNAME] = "ldap_simple";
        opts[OPT_PASSWORD] = ldap_simple_pwd;

        opts[OPT_ENABLE_CLEARTEXT_PLUGIN] = true;

        con.reset(driver->connect(opts));

        auto *stmt = con->createStatement();
        auto  *res = stmt->executeQuery("select 'Hello Simple LDAP'");

        res->next();

        std::cout << res->getString(1) << std::endl;

        con->close();

      } catch (sql::SQLException &e) {
        cout << "#\t\t " << url << " caused expected exception when connecting using ldap_simple" << endl;
        cout << "#\t\t " << e.what() << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        throw;
      }
    }

    if(ldap_user && ldap_scram_pwd)
    {
      //AUTH USING SASL client side plugin
      try {
        /*
         When using client side plugins (this case for SCRAM-SHA1 SASL
         authentication, plugin_dir might have to be set (if not using default
         location)
        */

        sql::ConnectOptionsMap opts;
        opts[OPT_HOSTNAME] = url;
        opts[OPT_USERNAME] = ldap_user;
        opts[OPT_PASSWORD] = ldap_scram_pwd;

        if(plugin_dir)
        {
          opts[OPT_PLUGIN_DIR] = plugin_dir;
        }

        con.reset(driver->connect(opts));

        auto *stmt = con->createStatement();
        auto  *res = stmt->executeQuery("select 'Hello SASL'");

        res->next();

        std::cout << res->getString(1) << std::endl;

        con->close();

      } catch (sql::SQLException &e) {
        cout << "#\t\t " << url << " caused expected exception when connecting using "<< ldap_user << endl;
        cout << "#\t\t " << e.what() << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        throw;
      }
    }


    try {
      /*s This will implicitly assume that the host is 'localhost' */
      url = "unix://path_to_mysql_socket.sock";
      con.reset(driver->connect(url, user, pass));
    } catch (sql::SQLException &e) {
      cout << "#\t\t unix://path_to_mysql_socket.sock caused expected exception" << endl;
      cout << "#\t\t " << e.what() << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    try {
      url = "tcp://hostname_or_ip[:port]";
      con.reset(driver->connect(url, user, pass));
    } catch (sql::SQLException &e) {
      cout << "#\t\t tcp://hostname_or_ip[:port] caused expected exception" << endl;
      cout << "#\t\t " << e.what() << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    try {
      /*
      Note: in the MySQL C-API host = localhost would cause a socket connection!
      Not so with the C++ Connector. The C++ Connector will translate
      tcp://localhost into tcp://127.0.0.1 and give you a TCP connection
      url = "tcp://localhost[:port]";
      */
      con.reset(driver->connect(url, user, pass));
    } catch (sql::SQLException &e) {
      cout << "#\t\t tcp://hostname_or_ip[:port] caused expected exception" << endl;
      cout << "#\t\t " << e.what() << " (MySQL error code: " << e.getErrorCode();
      cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    cout << "# done!" << endl;

  } catch (sql::SQLException &e) {
    /*
    The MySQL Connector/C++ throws three different exceptions:

    - sql::MethodNotImplementedException (derived from sql::SQLException)
    - sql::InvalidArgumentException (derived from sql::SQLException)
    - sql::SQLException (derived from std::runtime_error)
    */
    cout << "# ERR: SQLException in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    /* Use what() (derived from std::runtime_error) to fetch the error message */
    cout << "# ERR: " << e.what();
    cout << " (MySQL error code: " << e.getErrorCode();
    cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    cout << "not ok 1 - examples/connect.php" << endl;

    return EXIT_FAILURE;
  } catch (std::runtime_error &e) {

    cout << "# ERR: runtime_error in " << __FILE__;
    cout << "(" << EXAMPLE_FUNCTION << ") on line " << __LINE__ << endl;
    cout << "# ERR: " << e.what() << endl;
    cout << "not ok 1 - examples/connect.php" << endl;

    return EXIT_FAILURE;
  }

  cout << "ok 1 - examples/connect.php" << endl;
  return EXIT_SUCCESS;
}
