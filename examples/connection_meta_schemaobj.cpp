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
* Basic example demonstrating connect and simple queries
*
*/


/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <list>

/* Public interface of the MySQL Connector/C++ */
#include <driver/mysql_public_iface.h>
/* Connection parameter and sample data */
#include "examples.h"

using namespace std;

/**
* Usage example for Driver, Connection, (simple) Statement, ResultSet
*/
int main(int argc, const char **argv)
{

	string url(argc >= 2 ? argv[1] : EXAMPLE_HOST);
	const string user(argc >= 3 ? argv[2] : EXAMPLE_USER);
	const string pass(argc >= 4 ? argv[3] : EXAMPLE_PASS);
	const string database(argc >= 5 ? argv[4] : EXAMPLE_DB);

	static std::list<std::string> table_types;
	table_types.push_back("TABLE");

	unsigned int column, row;
	string ddl;

	cout << boolalpha;
	cout << "1..1" << endl;
	cout << "# Connector/C++ connection meta data example.." << endl;
	cout << "#" << endl;


	try {
		/* Using the Driver to create a connection */
		sql::Driver * driver = get_driver_instance();
		cout << "# " << driver->getName() << ", version ";
		cout << driver->getMajorVersion() << "." << driver->getMinorVersion();
                cout << "." << driver->getPatchVersion() << endl;

		std::auto_ptr< sql::Connection > con(driver->connect(url, user, pass));
		sql::DatabaseMetaData * con_meta = con->getMetaData();

		cout << "# CDBC (API) major version = " << con_meta->getCDBCMajorVersion() << endl;
		if (con_meta->getCDBCMajorVersion() <= 0) {
			throw runtime_error("API major version must not be 0");
		}
		cout << "# CDBC (API) minor version = " << con_meta->getCDBCMinorVersion() << endl;

		{
			/*
			Which schemata/databases exist?
			NOTE Connector C++ defines catalog = n/a, schema = MySQL database
			*/
			cout << "# List of available schemata/databases: ";
			std::auto_ptr< sql::ResultSet > res(con_meta->getSchemas());

			/* just for fun... of course you can scroll and fetch in whatever order you want */
			res->afterLast();
			while (res->previous()) {
				cout << res->getString("TABLE_SCHEM");
				if (!res->isFirst()) {
					cout << ", ";
				}
			}
			cout << endl;
			if (!res->isBeforeFirst() || res->isFirst())
				throw runtime_error("Cursor should be positioned before first row");
		}

		{
			/* What object types does getSchemaObjects support? */
			cout << "# Supported Object types: ";
			std::auto_ptr< sql::ResultSet > res(con_meta->getSchemaObjectTypes());
			while (res->next()) {
				cout << res->getString(1);
				if (!res->isLast()) {
					cout << ", ";
				}
			}
			cout << endl;
			if (!res->isAfterLast() || res->isLast())
				throw runtime_error("Cursor should be positioned after last row");
		}


		std::auto_ptr< sql::Statement > stmt(con->createStatement());
		stmt->execute("USE " + database);
		stmt->execute("DROP TABLE IF EXISTS test1");
		stmt->execute("CREATE TABLE test1(id INT, label CHAR(1))");
		stmt->execute("DROP TABLE IF EXISTS test2");
		stmt->execute("CREATE TABLE test2(id INT, label CHAR(1))");

		/* "" = empty string requests all types of objects */
		std::auto_ptr< sql::ResultSet > res(con_meta->getSchemaObjects(con->getCatalog(), con->getSchema(), ""));
		row = 1;
		sql::ResultSetMetaData * res_meta = res->getMetaData();
		while (res->next()) {
			if (row++ > 2)
				break;

			cout << "#\t Object " << res->getRow() << " (" << res->getString("OBJECT_TYPE") << ")" << endl;
			if (res->getString(1) != res->getString("OBJECT_TYPE")) {
				throw runtime_error("Fetch using numeric/non-numeric index failed for OBJECT_TYPE");
			}
			if (res->getString("OBJECT_TYPE").empty()) {
				throw runtime_error("OBJECT_TYPE must not be empty");
			}

			cout << "#\t Catalog = " << res->getString("CATALOG");
			cout << " (empty: " << res->getString("CATALOG").empty() << ")" << endl;
			if (res->getString(2) != res->getString("CATALOG")) {
				throw runtime_error("Fetch using numeric/non-numeric index failed for CATALOG");
			}

			cout << "#\t Schema = " << res->getString("SCHEMA");
			cout << " (empty: " << res->getString("SCHEMA").empty() << ")" << endl;
			if (res->getString(3) != res->getString("SCHEMA")) {
				throw runtime_error("Fetch using numeric/non-numeric index failed for SCHEMA");
			}

			cout << "#\t Name = " << res->getString("NAME");
			cout << " (empty: " << res->getString("NAME").empty() << ")" << endl;
			if (res->getString(4) != res->getString("NAME")) {
				throw runtime_error("Fetch using numeric/non-numeric index failed for NAME");
			}

			ddl = res->getString("DDL");
			cout << "#\t DDL = " << setw(40);
			cout << ddl.substr(0, ddl.find_first_of("\n", 1) - 1) << "..." << endl;
			cout << "#\t DDL is empty: " << ddl.empty() << endl;
			if (res->getString(5) != res->getString("DDL")) {
				throw runtime_error("Fetch using numeric/non-numeric index failed for DDL");
			}

		}
		cout << "#" << endl;
		cout << "#\t\t Using different getter methods at the example of the DDL column" << endl;
		cout << "#\t\t Column DDL is of type " << res_meta->getColumnTypeName(5);
		cout << " / Code: " << res_meta->getColumnType(5) << endl;
		/* scroll back to last row in set */
		res->previous();
		cout << "#\t\t DDL (as String) = " << setw(30) << ddl.substr(0, ddl.find_first_of("\n", 1) - 1) << "..." << endl;
		cout << "#\t\t DDL (as Boolean) = " << res->getBoolean("DDL") << "/" << res->getBoolean(5) << endl;
		cout << "#\t\t DDL (as Double) = " << res->getDouble("DDL") << "/" << res->getDouble(5) << endl;
		cout << "#\t\t DDL (as Int) = " << res->getInt("DDL") << "/" << res->getInt(5) << endl;
		cout << "#\t\t DDL (as Long) = " << res->getInt64("DDL") << "/" << res->getInt64(5) << endl;
		cout << "#" << endl;

		cout << "#\t\t Meta data on the result set at the example of the DDL column" << endl;
		cout << "#\t\t Column count = " << res_meta->getColumnCount() << " (Columns: ";
		for (column = 1; column <= res_meta->getColumnCount(); column++) {
			cout << res_meta->getColumnName(column);
			if (column < res_meta->getColumnCount()) {
				cout << ", ";
			}
		}
		cout << ")" << endl;

		cout << "#\t\t getCatalogName() = " << res_meta->getCatalogName(5) << endl;
		/* cout << "#\t\t getColumnDisplaySize() = " << res_meta->getDisplaySize("DDL") << endl; */
		cout << "#\t\t getColumnLabel() = " << res_meta->getColumnLabel(5) << endl;
		cout << "#\t\t getColumnName() = " << res_meta->getColumnName(5) << endl;
		if (res_meta->getColumnName(5) != res_meta->getColumnLabel(5)) {
			throw runtime_error("column names differ for column DDL");
		}

		cout << "#\t\t getColumnType() = " << res_meta->getColumnType(5) << endl;
		cout << "#\t\t getColumnTypeName() " << res_meta->getColumnTypeName(5) << endl;
		/* cout << "#\t\t getPrecision() = " << res_meta->getPrecision(5) << endl; */
		cout << "#\t\t getSchemaName() = " << res_meta->getSchemaName(5) << endl;
		cout << "#\t\t getTableName() = " << res_meta->getTableName(5) << endl;
		cout << "#\t\t isAutoIncrement() = " << res_meta->isAutoIncrement(5) << endl;
		cout << "#\t\t isCaseSensitive() = " << res_meta->isCaseSensitive(5) << endl;
		cout << "#\t\t isCurrency() = " << res_meta->isCurrency(5) << endl;
		cout << "#\t\t isDefinitelyWritable() = " << res_meta->isDefinitelyWritable(5) << endl;
		cout << "#\t\t isNullable() = " << res_meta->isNullable(5) << endl;
		cout << "#\t\t isReadOnly() = " << res_meta->isReadOnly(5) << endl;
		cout << "#\t\t isSearchable() = " << res_meta->isSearchable(5) << endl;
		cout << "#\t\t isSigned() = " << res_meta->isSigned(5) << endl;
		cout << "#\t\t isWritable() = " << res_meta->isWritable(5) << endl;

		std::auto_ptr< sql::ResultSet > res_tables(con_meta->getTables(con->getCatalog(), database, "t%", table_types));
		sql::ResultSetMetaData * res_meta_tables = res_tables->getMetaData();

		cout << "#" << endl;
		cout << "# Tables with names like 't%':" << endl;;
		cout << "#\t rowsCount() = " << res_tables->rowsCount() << endl;
		cout << "#\t getColumnCount() = " << res_meta_tables->getColumnCount() << endl;
		cout << "#\t" << endl;
		while (res_tables->next()) {
			cout << "#\t ";
			for (column = 1; column < res_meta_tables->getColumnCount(); column++) {
				cout << "'" << res_tables->getString(column) << "' ";
			}
			cout <<  endl;
		}
		cout << "#" << endl;

	  	res_tables->first();
		std::auto_ptr< sql::ResultSet > res_columns(con_meta->getColumns(con->getCatalog(), database, "test1", "%"));

		cout << "#" << "Columns in the table 'test1'..." << endl;
		cout << "#\t rowsCount() = " << res_columns->rowsCount() << endl;
		if (res_columns->rowsCount() != 2)
			throw runtime_error("Table test1 has two columns, no more!");

		while (res_columns->next()) {
			cout << "#\t Column name = '" << res_columns->getString("COLUMN_NAME") << "'" << endl;
		}
		cout << "#" << endl;

		cout << "# done!" << endl;
		cout << "ok 1 - examples/connection_meta_schemaobj.cpp" << endl;

	} catch (sql::SQLException &e) {
		/*
		The MySQL Connector/C++ throws three different exceptions:

		- sql::MethodNotImplementedException (derived from sql::SQLException)
		- sql::InvalidArgumentException (derived from sql::SQLException)
		- sql::SQLException (derived from std::runtime_error)
		*/
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		/* Use what() (derived from std::runtime_error) */
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		cout << "not ok 1 - examples/connection_meta_schemaobj.cpp" << endl;

		return EXIT_FAILURE;
	} catch (std::runtime_error &e) {

		cout << "# ERR: runtime_error in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what() << endl;
		cout << "not ok 1 - examples/connection_meta_schemaobj.cpp" << endl;

		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
