/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

/* *
* Example of sql::Statement - "simple" (not prepared) statements
*
*/

/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

/* Public interface of the MySQL Connector/C++ */
#include <driver/mysql_public_iface.h>
/* Connection parameter and sample data */
#include "examples.h"

using namespace std;

/**
* Example of statements - not to be confused with prepared statements
*/
int main(int argc, const char **argv)
{
	static const string url(argc >= 2 ? argv[1] : EXAMPLE_HOST);
	static const string user(argc >= 3 ? argv[2] : EXAMPLE_USER);
	static const string pass(argc >= 4 ? argv[3] : EXAMPLE_PASS);
	static const string database(argc >= 5 ? argv[4] : EXAMPLE_DB);

	/* Driver Manager */
	sql::Driver *driver;

	/* sql::ResultSet.rowsCount() returns size_t */
	size_t row;
	stringstream sql;
	int i;
	bool ok;

	cout << boolalpha;
	cout << "1..1" << endl;
	cout << "# Connector/C++ (simple) statement example.." << endl;

	try {
		/* Using the Driver to create a connection */
		driver = get_driver_instance();
		std::auto_ptr< sql::Connection > con(driver->connect(url, user, pass));
		con->setSchema(database);

		/* Creating a "simple" statement - "simple" = not a prepared statement */
		std::auto_ptr< sql::Statement > stmt(con->createStatement());

		/* Create a test table demonstrating the use of sql::Statement.execute() */
		stmt->execute("DROP TABLE IF EXISTS test");
		stmt->execute("CREATE TABLE test(id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, label CHAR(1))");
		cout << "#\t Test table created" << endl;

		/* Populate the test table with data */
		for (i = 0; i < EXAMPLE_NUM_TEST_ROWS; i++) {
			// KLUDGE: You should take measures against SQL injections!
			// example.h contains the test data
			sql.str("");
			sql << "INSERT INTO test(id, label) VALUES (";
			sql << test_data[i].id << ", '" << test_data[i].label << "')";
			stmt->execute(sql.str());
		}
		cout << "#\t Test table populated" << endl;

		/*
		NOTE: Use execute() instead of the more convenient executeQuery()
		See the other example file for executeQuery() and executeUpdate() examples
		However, if you are executing SQL dynamically, you might have to use execute()
		*/
		ok = stmt->execute("SELECT id, label FROM test ORDER BY id ASC");
		cout << "#\t stmt->execute('SELECT id, label FROM test ORDER BY id ASC') = ";
		cout << ok << endl;
		if (ok == true) {
			/* The first result is a result set */
			cout << "#\t\t Fetching results" << endl;
			/*
			NOTE: If stmt.getMoreResults() would be implemented already one
			would use a do { ... } while (stmt.getMoreResults()) loop
			*/
			std::auto_ptr< sql::ResultSet > res(stmt->getResultSet());
			row = 0;
			while (res->next()) {
				cout << "#\t\t Row " << row << " - id = " << res->getInt("id");
				cout << ", label = '" << res->getString("label") << "'" << endl;
				row++;
			}

		} else if (ok == false) {
			/* The first result is an update count */
			throw runtime_error("Expecting regular result set");
		}

		/* Clean up */
		stmt->execute("DROP TABLE IF EXISTS test");
		cout << "# done!" << endl;

	} catch (sql::SQLException &e) {
		/*
		The MySQL Connector/C++ throws three different exceptions:

		- sql::MethodNotImplementedException (derived from sql::SQLException)
		- sql::InvalidArgumentException (derived from sql::SQLException)
		- sql::SQLException (derived from std::runtime_error)
		*/
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		// Use what(), getErrorCode() and getSQLState()
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		cout << "not ok 1 - examples/statement.cpp" << endl;

		return EXIT_FAILURE;
	} catch (std::runtime_error &e) {

		cout << "# ERR: runtime_error in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what() << endl;
		cout << "not ok 1 - examples/statement.cpp" << endl;

		return EXIT_FAILURE;
	}

	cout << "ok 1 - examples/statement.cpp" << endl;
	return EXIT_SUCCESS;
}
