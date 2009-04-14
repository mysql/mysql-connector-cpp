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
* Handling binary data using getString()
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

int main(int argc, const char **argv)
{
	static const string url(argc >= 2 ? argv[1] : EXAMPLE_HOST);
	static const string user(argc >= 3 ? argv[2] : EXAMPLE_USER);
	static const string pass(argc >= 4 ? argv[3] : EXAMPLE_PASS);
	static const string database(argc >= 5 ? argv[4] : EXAMPLE_DB);

	/* Driver Manager */
	sql::Driver *driver;

	/* sql::ResultSet.rowsCount() returns size_t */
	stringstream sql;
	int i;
	struct _test_data min, max;

	cout << boolalpha;
	cout << "1..1" << endl;
	cout << "# Connector/C++ result set.." << endl;

	try {
		/* Using the Driver to create a connection */
		driver = get_driver_instance();
		std::auto_ptr< sql::Connection > con(driver->connect(url, user, pass));

		con->setSchema(database);

		/* Creating a "simple" statement - "simple" = not a prepared statement */
		std::auto_ptr< sql::Statement > stmt(con->createStatement());
		stmt->execute("DROP TABLE IF EXISTS test");
		stmt->execute("CREATE TABLE test(id INT, label CHAR(1), col_binary BINARY(4), col_varbinary VARBINARY(10))");
		cout << "#\t Test table created" << endl;

		/* Populate the test table with data */
		min = max = test_data[0];
		for (i = 0; i < EXAMPLE_NUM_TEST_ROWS; i++) {
			/* Remember mnin/max values for further testing */
			if (test_data[i].id < min.id) {
				min = test_data[i];
			}
			if (test_data[i].id > max.id) {
				max = test_data[i];
			}

			/*
			KLUDGE: You should take measures against SQL injections!
			*/
			sql.str("");
			sql << "INSERT INTO test(id, label, col_binary, col_varbinary) VALUES (";
			sql << test_data[i].id << ", '" << test_data[i].label << "', ";
			sql << "\"a\\0b\", '" << i * 5 << "\\0abc')";
			stmt->execute(sql.str());
		}
		cout << "#\t Test table populated" << endl;

		cout << "#\t Testing sql::Statement based resultset" << endl;
		{
			std::auto_ptr< sql::ResultSet > res(stmt->executeQuery("SELECT * FROM test ORDER BY id ASC"));
			int row = 0;
			while (res->next()) {
				cout << "#\t\t Row " << row << ", getRow() " << res->getRow();
				cout << " id = " << res->getInt("id");
				cout << ", label = '" << res->getString("label") << "'";
				/* cout might "hide" \0 and other special characters from the output */
				cout << ", col_binary = '" << res->getString("col_binary") << "'";
				cout << ", col_varbinary = '" << res->getString("col_varbinary") << "'" << endl;

				/* fixed length column - length = size of the column! */
				if (res->getString("col_binary").length() != 4)
					throw runtime_error("BINARY(n) should return std::string of length n regardless how long the value stored in the column is.");

				if (res->getString("col_binary").compare(0, 1, "a"))
					throw runtime_error("First sign from BINARY(n) seems wrong");

				if (res->getString("col_binary").compare(2, 1, "b"))
					throw runtime_error("Third sign from BINARY(n) seems wrong");

				if (res->getString("col_varbinary").length() != 5 &&
					res->getString("col_varbinary").length() != 6)
					throw runtime_error("VARBINARY(n) should return std::string of length n which holds the length of the actual column value.");

				sql::ResultSetMetaData * meta = res->getMetaData();
				cout << "#\t\t COLUMN_SIZE = " << meta->getColumnDisplaySize(3) << endl;
				row++;
			}
		}

		/* Clean up */
		/* stmt->execute("DROP TABLE IF EXISTS test");*/
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
		cout << "not ok 1 - examples/resultset_binary.cpp" << endl;

		return EXIT_FAILURE;
	} catch (std::runtime_error &e) {

		cout << "# ERR: runtime_error in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what() << endl;
		cout << "not ok 1 - examples/resultset_binary.cpp" << endl;

		return EXIT_FAILURE;
	}

	cout << "ok 1 - examples/resultset_binary.cpp" << endl;
	return EXIT_SUCCESS;
}
