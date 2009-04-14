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
* Debug/Tracefile generation
*
* Two types of debug/trace files are available with Connector/C++. You can make
* Connector/C++ write a trace file and/or activate tracing on the level of the
* underlying MySQL Client Libarary.
*
* Connector/C++ is based on the MySQL Client Library (C-API, AKA libmysql).
* Connector/C++ is a proxy/wrapper of the MySQL Client Library.
* Debug versions of the MySQL Client Library can write a trace file. On the C-level
* this is done by passing a string with debug instructions to the C function
* mysql_debug(). You can use the Connector/C++ method
* setClientOption("libmysql_debug", const string debug) to pass a string to the
* C-API function mysql_debug(). See the MySQL C-API documentation
*
* Example of a libmysql trace:
* >mysql_stmt_init
* | >_mymalloc
* | | enter: Size: 816
* | | exit: ptr: 0x68e7b8
* | <_mymalloc
* | >init_alloc_root
* | | enter: root: 0x68e7b8
* | | >_mymalloc
* | | | enter: Size: 2064
* | | | exit: ptr: 0x68eb28
* [...]
*
* If you do not have compiled Connector/C++ against a debug version of the MySQL
* Client Library but you have compiled  Connector/C++ with tracing enabled, then
* you may activate the Connector/C++ internal trace using
* setClientOption("clientTrace", bool on_off).
*
* To compile Connector/C++ with tracing enabled, do:
*   cmake -DMYSQLCPPCONN_TRACE_ENABLE:BOOL=1
*
* Example of a Connector/C++ trace:
* |  INF: Tracing enabled
* <MySQL_Connection::setClientOption
* >MySQL_Prepared_Statement::setInt
* |  INF: this=0x69a2e0
* |  >MySQL_Prepared_Statement::checkClosed
* |  <MySQL_Prepared_Statement::checkClosed
* | <MySQL_Prepared_Statement::setInt
*
* Note that the traces will contain your SQL statements. You may not want
* to share sensitive SQL statements.
*
* @link http://dev.mysql.com/doc/refman/5.1/en/mysql-debug.html
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
	const string host(argc >= 2 ? argv[1] : EXAMPLE_HOST);
	const string user(argc >= 3 ? argv[2] : EXAMPLE_USER);
	const string pass(argc >= 4 ? argv[3] : EXAMPLE_PASS);
	const string database(argc >= 5 ? argv[4] : EXAMPLE_DB);

	sql::Driver *driver;
	int i;
	int on_off = 1;

	cout << boolalpha;
	cout << "1..1" << endl;;
	cout << "# Connector/C++ debug / client_options example.." << endl;

	try {
		/* Using the Driver to create a connection */
		driver = get_driver_instance();
		std::auto_ptr< sql::Connection > con(driver->connect(host, user, pass));

		/*
		 Activate debug trace of the MySQL Client Library (C-API)
		Only available with a debug build of the MySQL Client Library!
		*/
		con->setClientOption("libmysql_debug", "d:t:O,client.trace");

		con->setSchema(database);

		std::auto_ptr< sql::Statement > stmt(con->createStatement());
		stmt->execute("DROP TABLE IF EXISTS test");
		stmt->execute("CREATE TABLE test(id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, label CHAR(1))");
		cout << "#\t Test table created" << endl;

		std::auto_ptr< sql::PreparedStatement > prep_stmt(con->prepareStatement("INSERT INTO test(id, label) VALUES (?, ?)"));
		for (i = 0; i < EXAMPLE_NUM_TEST_ROWS; i++) {
			prep_stmt->setInt(1, test_data[i].id);
			prep_stmt->setString(2, test_data[i].label);
			prep_stmt->executeUpdate();

			/*
			Pointless activity to demo how to
			activating and de-activating the Connector/C++ internal trace
			*/
			on_off = (i + 2) % 2;
			con->setClientOption("clientTrace", &on_off);
		}

		stmt->execute("DROP TABLE IF EXISTS test");

	} catch (sql::SQLException &e) {
		/*
		The MySQL Connector/C++ throws three different exceptions:

		- sql::MethodNotImplementedException (derived from sql::SQLException)
		- sql::InvalidArgumentException (derived from sql::SQLException)
		- sql::SQLException (derived from std::runtime_error)
		*/
		cout << endl;
		cout << "# ERR: DbcException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		// Use what(), getErrorCode() and getSQLState()
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
		cout << "not ok 1 - examples/debug.cpp" << endl;

		return EXIT_FAILURE;
	} catch (std::runtime_error &e) {

		cout << endl;
		cout << "# ERR: runtime_error in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what() << endl;
		cout << "not ok 1 - examples/debug.cpp" << endl;

		return EXIT_FAILURE;
	}

	cout << "ok 1 - examples/debug.cpp" << endl;
	return EXIT_SUCCESS;
}
