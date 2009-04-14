/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

/**
* Types of execptions thrown by Connector/C++
*
*/

/* Standard C++ includes */
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
#include <driver/mysql_public_iface.h>
/* Connection parameter and sample data */
#include "examples.h"

using namespace std;

int main(int argc, const char **argv)
{
	static const string host(argc >= 2 ? argv[1] : EXAMPLE_HOST);
	static const string user(argc >= 3 ? argv[2] : EXAMPLE_USER);
	static const string pass(argc >= 4 ? argv[3] : EXAMPLE_PASS);
	static const string database(argc >= 5 ? argv[4] : EXAMPLE_DB);

	// Driver Manager
	sql::Driver *driver;
	int i;

	cout << boolalpha;
	cout << "1..1" << endl;
	cout << "# Connector/C++ types of exceptions.." << endl;

	try {
		/* Using the Driver to create a connection */
		driver = get_driver_instance();
		std::auto_ptr< sql::Connection> con(driver->connect(host, user, pass));

		/* Run in autocommit mode */
		con->setAutoCommit(1);
		std::auto_ptr< sql::Savepoint > savepoint(NULL);
		try {
			// It makes no sense to set a savepoint in autocommit mode
			savepoint.reset(con->setSavepoint(string("before_insert")));
			throw new runtime_error("Setting a savepoint should not be allow in autocommit mode");
		} catch (sql::InvalidArgumentException &e) {
			cout << "#\t Invalid Argument: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}

		con->setSchema(database);

		std::auto_ptr< sql::Statement > stmt(con->createStatement());
		stmt->execute("DROP TABLE IF EXISTS test");
		stmt->execute("CREATE TABLE test(id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, label CHAR(1))");
		cout << "#\t Test table created" << endl;

		con->setAutoCommit(0);
		try {
			savepoint.reset(con->setSavepoint(string("")));
		} catch (sql::InvalidArgumentException &e) {
			cout << "#\t Invalid Argument: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}
		savepoint.reset(con->setSavepoint(string("before_insert")));

		{
			std::auto_ptr< sql::PreparedStatement > prep_stmt(con->prepareStatement("INSERT INTO test(id, label) VALUES (?, ?)"));
			for (i = 0; i < EXAMPLE_NUM_TEST_ROWS; i++) {
				prep_stmt->setInt(1, test_data[i].id);
				prep_stmt->setString(2, test_data[i].label);
				prep_stmt->executeUpdate();
			}
		}

		try {
			std::auto_ptr< sql::PreparedStatement > prep_stmt(con->prepareStatement("INSERT INTO test(id, label) VALUES (?, ?)"));
			prep_stmt->setInt(1, test_data[0].id);
			/* This will cause a duplicate index error */
			prep_stmt->executeUpdate();
			throw new runtime_error("Query not executed or duplicate key not detected");
		} catch (sql::SQLException &e) {
			cout << "#\t SQL Exception: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}

		/* Switching back to auto commit will invalidate the savepoint!	*/
		con->setAutoCommit(1);
		try {
			con->releaseSavepoint(savepoint.get());
			throw new runtime_error("Releasing a savepoint should not be allow in autocommit mode");
		} catch (sql::InvalidArgumentException &e) {
			cout << "#\t Invalid Argument: " << e.what();
			cout << " (MySQL error code: " << e.getErrorCode();
			cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		}

		stmt->execute("DROP TABLE IF EXISTS test");
		cout << "#done!" << endl;

	} catch (sql::SQLException &e) {
		/*
		The MySQL Connector/C++ throws three different exceptions:

		- sql::MethodNotImplementedException (derived from sql::SQLException)
		- sql::InvalidArgumentException (derived from sql::SQLException)
		- sql::SQLException (derived from std::runtime_error)

                 sql::SQLException is the base class.
		*/
		cout << endl;
		cout << "# ERR: DbcException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		/* Use what(), getErrorCode() and getSQLState() */
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;

		if (e.getErrorCode() == 1047) {
			/*
			Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
			Message: Unknown command
			*/
			cout << "# ERR: Your server seems not to support PS at all because its MYSQL <4.1" << endl;
		}
		cout << "not ok 1 - examples/exceptions.cpp" << endl;

		return EXIT_FAILURE;
	} catch (std::runtime_error &e) {

		cout << endl;
		cout << "# ERR: runtime_error in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what() << endl;
		cout << "not ok 1 - examples/exceptions.cpp" << endl;

		return EXIT_FAILURE;
	}

	cout << "ok 1 - examples/exceptions.cpp" << endl;
	return EXIT_SUCCESS;
}


bool prepare_execute(sql::Connection *con, const char *sql)
{
	sql::PreparedStatement *prep_stmt;

	prep_stmt = con->prepareStatement(sql);
	prep_stmt->execute();
	delete prep_stmt;

	return true;
}


sql::Statement* emulate_prepare_execute(sql::Connection *con, const char *sql)
{
	sql::PreparedStatement *prep_stmt;
	sql::Statement *stmt = NULL;

	cout << "#\t\t 'emulation': " << sql << endl;

	try {

		prep_stmt = con->prepareStatement(sql);
		prep_stmt->execute();
		cout << "#\t\t 'emulation': use of sql::PreparedStatement possible" << endl;
		// safe upcast - PreparedStatement is derived from Statement
		stmt = prep_stmt;

	} catch (sql::SQLException &e) {
		/*
		Maybe the command is not supported by the MySQL Server?

		http://dev.mysql.com/doc/refman/5.1/en/error-messages-server.html
		Error: 1295 SQLSTATE: HY000 (ER_UNSUPPORTED_PS)

		Message: This command is not supported in the prepared statement protocol yet
		*/
		if (e.getErrorCode() != 1295) {
			/*
			The MySQL Server should be able to prepare the statement
			but something went wrong. Let the caller handle the error.
			*/
			throw ;
		}
		cout << "#\t\t 'emulation': ER_UNSUPPORTED_PS and fallback to sql::Statement" << endl;
		cout << "#\t\t ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;

		stmt = con->createStatement();
		stmt->execute(sql);
	}

	return stmt;
}
