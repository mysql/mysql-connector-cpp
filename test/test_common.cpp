/* Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.

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

#include <list>
#include <map>
#include <memory>

// Portable __FUNCTION__
#ifndef __FUNCTION__
 #ifdef __func__
   #define CPPCONN_FUNC __func__
 #else
   #define CPPCONN_FUNC "(function n/a)"
 #endif
#else
#define CPPCONN_FUNC __FUNCTION__
#endif

#ifndef __LINE__
  #define __LINE__ "(line number n/a)"
#endif

#define ensure(msg, stmt)				do {total_tests++;if(!(stmt)){printf("\n# Error! line=%d: %s\n# ",__LINE__,msg);total_errors++;throw sql::SQLException("error");} else { printf(".");}} while (0)
#define ensure_equal(msg, op1, op2)	do {total_tests++;if((op1)!=(op2)){printf("\n# Error! line=%d: %s\n# ",__LINE__,msg);total_errors++;throw sql::SQLException("error");} else { printf(".");}}while(0)
#define ensure_equal_int(msg, op1, op2)	do {total_tests++;if((op1)!=(op2)){printf("\n# Error! line=%d: %s Op1=%d Op2=%d\n# ",__LINE__,msg,op1,op2);total_errors++;throw sql::SQLException("error");} else { printf(".");}}while(0)

static int total_errors = 0;
static int total_tests = 0;
static int silent = 1;

#define USED_DATABASE "test"

#define ENTER_FUNCTION()		if (!silent) printf("# >>>>   %s\n# ", CPPCONN_FUNC);
#define LEAVE_FUNCTION()		if (!silent) printf("# <<<<   %s\n# ", CPPCONN_FUNC); else printf("\n# ");

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(disable:4251)
#pragma warning(disable:4800)
#endif


extern "C"
{
#if defined(_WIN32) || defined(_WIN64)
#include "my_global.h"
#endif

#include "mysql.h"
}
/* mysql.h introduces bool */
#undef bool

#if defined(_WIN32) || defined(_WIN64)
#pragma warning(disable:4251)
#endif

#include <stdio.h>

class TestBlob : public sql::Blob
{
	std::string value;
	size_t position;
public:
	TestBlob(std::string & s) : value(s), position(0) {}
	virtual ~TestBlob() {}

	std::string readChunk(size_t chunkSize);
	void writeChunk(const std::string & chunk);
};


/* {{{ TestBlob::readChunk() */
std::string TestBlob::readChunk(size_t chunkSize)
{
	try {
		size_t tmp = position;
		position+=chunkSize;
		return value.substr(tmp, chunkSize);
	} catch (std::out_of_range) {
		return std::string("");
	}
}
/* }}} */


/* {{{ TestBlob::writeChunk() */
void TestBlob::writeChunk(const std::string & chunk)
{
	value.append(chunk);
}
/* }}} */


/* {{{	*/
static bool populate_blob_table(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	std::auto_ptr<sql::Statement> stmt(conn->createStatement());
	ensure("stmt is NULL", stmt.get() != NULL);

	stmt->execute("USE " + database);
	stmt->execute("DROP TABLE IF EXISTS test_blob");
	if (true == stmt->execute("CREATE TABLE test_blob (a longblob) ENGINE=MYISAM")) {
		return false;
	}
	return true;
}
/* }}} */


/* {{{ */
static bool populate_insert_data(sql::Statement * stmt)
{
	return stmt->execute("INSERT INTO test_function (a,b,c,d,e) VALUES(1, 111, NULL, \"222\", \"xyz\")");
}
/* }}} */


/* {{{	*/
static bool populate_test_table(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	std::auto_ptr<sql::Statement> stmt(conn->createStatement());
	ensure("stmt is NULL", stmt.get() != NULL);

	stmt->execute("USE " + database);
	stmt->execute("DROP TABLE IF EXISTS test_function");
	if (true == stmt->execute("CREATE TABLE test_function (a integer unsigned not null, b integer, c integer default null, d char(10), e varchar(10) character set utf8 collate utf8_bin) ENGINE=MYISAM")) {
		return false;
	}

	if (true == populate_insert_data(stmt.get())) {
		stmt->execute("DROP TABLE test_function");
		return false;
	}
	return true;
}
/* }}} */


/* {{{	*/
static bool populate_TX_insert_data(sql::Statement * stmt)
{
	return stmt->execute("INSERT INTO test_function_tx (a,b,c,d,e) VALUES(1, 111, NULL,  \"222\", \"xyz\")");
}
/* }}} */


/* {{{	*/
static bool populate_TX_test_table(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	std::auto_ptr<sql::Statement> stmt(conn->createStatement());
	ensure("stmt is NULL", stmt.get() != NULL);

	stmt->execute("USE " + database);
	stmt->execute("DROP TABLE IF EXISTS test_function_tx");
	if (true == stmt->execute("CREATE TABLE test_function_tx(a integer unsigned not null, b integer, c integer default null, d char(10), e varchar(10) character set utf8 collate utf8_bin) engine = innodb")) {
		return false;
	}

	if (true == populate_TX_insert_data(stmt.get())) {
		stmt->execute("DROP TABLE test_function_tx");
		return false;
	}
	stmt->getConnection()->commit();
	return true;
}
/* }}} */


/* {{{	*/
static bool populate_test_table_PS(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
	ensure("stmt1 is NULL", stmt1.get() != NULL);
	stmt1->execute("USE " + database);

	std::auto_ptr<sql::PreparedStatement> stmt2(conn->prepareStatement("DROP TABLE IF EXISTS test_function"));
	ensure("stmt2 is NULL", stmt2.get() != NULL);
	stmt2->executeUpdate();

	std::auto_ptr<sql::PreparedStatement> stmt3(conn->prepareStatement("CREATE TABLE test_function(a integer unsigned not null, b integer, c integer default null, d char(10), e varchar(10) character set utf8 collate utf8_bin)"));
	ensure("stmt3 is NULL", stmt3.get() != NULL);
	stmt3->executeUpdate();

	std::auto_ptr<sql::PreparedStatement> stmt4(conn->prepareStatement("INSERT INTO test_function (a,b,c,d,e) VALUES(1, 111, NULL, \"222\", \"xyz\")"));
	ensure("stmt4 is NULL", stmt4.get() != NULL);
	stmt4->executeUpdate();

	return true;
}
/* }}} */


/* {{{	*/
static bool populate_TX_test_table_PS(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
	ensure("stmt is NULL", stmt1.get() != NULL);
	stmt1->execute("USE " + database);

	std::auto_ptr<sql::PreparedStatement> stmt2(conn->prepareStatement("DROP TABLE IF EXISTS test_function_tx"));
	ensure("stmt2 is NULL", stmt2.get() != NULL);
	stmt2->executeUpdate();

	std::auto_ptr<sql::PreparedStatement> stmt3(conn->prepareStatement("CREATE TABLE test_function_tx(a integer unsigned not null, b integer, c integer default null, d char(10), e varchar(10) character set utf8 collate utf8_bin) engine = innodb"));
	ensure("stmt3 is NULL", stmt3.get() != NULL);
	stmt3->executeUpdate();

	std::auto_ptr<sql::PreparedStatement> stmt4(conn->prepareStatement("INSERT INTO test_function_tx (a,b,c,d,e) VALUES(1, 111, NULL, \"222\", \"xyz\")"));
	ensure("stmt4 is NULL", stmt4.get() != NULL);
	stmt4->executeUpdate();

	return true;
}
/* }}} */


/* {{{	*/
static void test_autocommit(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {
		conn->setAutoCommit(1);
		ensure("AutoCommit", conn->getAutoCommit() == true);

		conn->setAutoCommit(0);
		ensure("AutoCommit", conn->getAutoCommit() == false);
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{	*/
static void test_connection_0(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {
		char buff[64];

		std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
		ensure("stmt1 is NULL", stmt1.get() != NULL);
		std::auto_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT CONNECTION_ID()"));
		ensure("res1 is NULL", rset1.get() != NULL);

		ensure("res1 is empty", rset1->next() != false);

		ensure("connection is closed", !conn->isClosed());

		sprintf(buff, "KILL %d", rset1->getInt(1));

		try {
			stmt1->execute(buff);
		} catch (sql::SQLException &) {
			/*
			  If this is mac, we will get an error.
			  MySQL on Mac closes the connection without sending response
			*/
		}
		try {
			std::auto_ptr<sql::ResultSet> rset2(stmt1->executeQuery("SELECT CONNECTION_ID()"));
			ensure("no exception", false);
		} catch (sql::SQLException &) {
			ensure("Exception correctly thrown", true);
		}
		ensure("connection is open", conn->isClosed() == false);
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{	*/
static void test_connection_1(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
		ensure("stmt1 is NULL", stmt1.get() != NULL);
		ensure("connection is closed", !conn->isClosed());
		conn->setAutoCommit(false);

		ensure("Data not populated", true == populate_TX_test_table(conn, database));

		std::auto_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
		ensure("res1 is NULL", rset1.get() != NULL);
		ensure("res1 is empty", rset1->next() != false);
		int count_full_before = rset1->getInt(1);
		ensure_equal_int("res1 has more rows ", rset1->next(), false);

		std::string savepointName("firstSavePoint");
		std::auto_ptr<sql::Savepoint> savepoint(conn->setSavepoint(savepointName));

		populate_TX_insert_data(stmt1.get());
		std::auto_ptr<sql::ResultSet> rset2(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
		ensure("res2 is NULL", rset2.get() != NULL);
		ensure_equal_int("res2 is empty", rset2->next(), true);
		int count_full_after = rset2->getInt(1);
		ensure_equal_int("res2 has more rows ", rset2->next(), false);
		ensure_equal_int("wrong number of rows", count_full_after, (count_full_before * 2));

		conn->rollback(savepoint.get());
		std::auto_ptr<sql::ResultSet> rset3(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
		ensure("res3 is NULL", rset3.get() != NULL);
		ensure_equal_int("res3 is empty", rset3->next(), true);
		int count_full_after_rollback = rset3->getInt(1);
		ensure_equal_int("res3 has more rows ", rset3->next(), false);
		ensure_equal_int("wrong number of rows", count_full_after_rollback, count_full_before);

		conn->releaseSavepoint(savepoint.get());
		try {
			/* The second call should throw an exception */
			conn->releaseSavepoint(savepoint.get());
			total_errors++;
		} catch (sql::SQLException &) {}

		/* Clean */
		stmt1->execute("USE " + database);
		stmt1->execute("DROP TABLE test_function_tx");
	} catch (sql::MethodNotImplementedException &) {
		printf("\n# SKIP: RELEASE SAVEPOINT not available in this MySQL version\n");
		printf("# ");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{	*/
static void test_connection_2(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt1 is NULL", stmt.get() != NULL);

		ensure("Wrong catalog", conn->getCatalog() == "");
		stmt->execute("USE " + database);
		std::string newCatalog(conn->getSchema());
		ensure(std::string("Wrong catalog '" + newCatalog + "'/'" + database + "'").c_str(), newCatalog == std::string(database));

		try {
			conn->setCatalog(std::string("doesnt_actually_exist"));
			total_errors++;
		} catch (sql::SQLException &) {}
		conn->setSchema(std::string("information_schema"));
		std::string newCatalog2(conn->getSchema());
		ensure("Wrong catalog", newCatalog2 == std::string("information_schema"));
	} catch (sql::SQLException &e) {
		/* Error: 1049 SQLSTATE: 42000  (ER_BAD_DB_ERROR)  - information_schema not available */
		if (e.getErrorCode() != 1049) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
			printf("# ");
			total_errors++;
		}
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{	*/
static void test_connection_3(std::auto_ptr<sql::Connection> & conn, std::string user)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::DatabaseMetaData> meta(conn->getMetaData());
		ensure("getUserName() failed", user == meta->getUserName().substr(0, user.length()));
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{	*/
static void test_statement_0(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("AutoCommit", conn.get() == stmt->getConnection());
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test simple update statement against statement object */
static void test_statement_1(std::auto_ptr<sql::Connection> & conn, std::auto_ptr<sql::Connection> & conn2, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		ensure("Data not populated", true == populate_test_table(conn, database));
		if (false == stmt->execute("SELECT * FROM test_function"))
			ensure("False returned for SELECT", false);

		/* Clean */
		std::auto_ptr<sql::Statement> stmt2(conn2->createStatement());
		ensure("stmt is NULL", stmt2.get() != NULL);
		stmt2->execute("USE " + database);
		stmt2->execute("DROP TABLE test_function");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test simple query against statement object */
static void test_statement_2(std::auto_ptr<sql::Connection> & conn, std::auto_ptr<sql::Connection> & conn2, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		ensure("Data not populated", true == populate_test_table(conn, database));
		if (false == stmt->execute("SELECT * FROM test_function"))
			ensure("False returned for SELECT", false);

		/* Clean */
		std::auto_ptr<sql::Statement> stmt2(conn2->createStatement());
		ensure("stmt is NULL", stmt2.get() != NULL);
		stmt2->execute("USE " + database);
		stmt2->execute("DROP TABLE test_function");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test executeQuery() - returning a result set*/
static void test_statement_3(std::auto_ptr<sql::Connection> & conn, std::auto_ptr<sql::Connection> & conn2, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		ensure("Data not populated", true == populate_test_table(conn, database));
		/* Get a result set */
		try {
			std::auto_ptr<sql::ResultSet> rset(stmt->executeQuery("SELECT * FROM test_function"));
			ensure("NULL returned for result set", rset.get() != NULL);
		} catch (sql::SQLException &e) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
			total_errors++;
			printf("# ");
		}

		/* Clean */
		std::auto_ptr<sql::Statement> stmt2(conn2->createStatement());
		ensure("stmt is NULL", stmt2.get() != NULL);
		stmt2->execute("USE " + database);
		stmt2->execute("DROP TABLE test_function");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test executeQuery() - returning empty result set */
static void test_statement_4(std::auto_ptr<sql::Connection> & conn, std::auto_ptr<sql::Connection> & conn2, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		ensure("Data not populated", true == populate_test_table(conn, database));
		/* Get a result set */
		try {
			std::auto_ptr<sql::ResultSet> rset(stmt->executeQuery("SELECT * FROM test_function WHERE 1=2"));
			ensure("NULL returned for result set", rset.get() != NULL);
			ensure_equal_int("Non-empty result set", false, rset->next());

		} catch (sql::SQLException &e) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
			printf("# ");
			total_errors++;
		}

		/* Clean */
		std::auto_ptr<sql::Statement> stmt2(conn2->createStatement());
		ensure("stmt is NULL", stmt2.get() != NULL);
		stmt2->execute("USE " + database);
		stmt2->execute("DROP TABLE test_function");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test executeQuery() - use it for inserting, should generate an exception */
static void test_statement_5(std::auto_ptr<sql::Connection> & conn, std::auto_ptr<sql::Connection> & conn2, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		ensure("Data not populated", true == populate_test_table(conn, database));
		/* Get a result set */
		try {
			std::auto_ptr<sql::ResultSet> rset(stmt->executeQuery("INSERT INTO test_function VALUES(2,200)"));
			ensure("NULL returned for result set", rset.get() == NULL);
			ensure_equal_int("Non-empty result set", false, rset->next());
		} catch (sql::SQLException &) {
		} catch (...) {
			printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
			printf("# ");
			total_errors++;
		}
		/* Clean */
		std::auto_ptr<sql::Statement> stmt2(conn2->createStatement());
		ensure("stmt is NULL", stmt2.get() != NULL);
		stmt2->execute("USE " + database);
		stmt2->execute("DROP TABLE test_function");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test executeUpdate() - check the returned value */
static void test_statement_6(std::auto_ptr<sql::Connection> & conn, std::auto_ptr<sql::Connection> & conn2, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		ensure("Data not populated", true == populate_test_table(conn, database));
		/* Get a result set */
		try {
			ensure("Number of updated rows", stmt->executeUpdate("UPDATE test_function SET a = 123") == 1);
		} catch (sql::SQLException &e) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
			total_errors++;
		}

		/* Clean */
		std::auto_ptr<sql::Statement> stmt2(conn2->createStatement());
		ensure("stmt is NULL", stmt2.get() != NULL);
		stmt2->execute("USE " + database);
		stmt2->execute("DROP TABLE test_function");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test executeUpdate() - execute a SELECT, should get an exception */
static void test_statement_7(std::auto_ptr<sql::Connection> & conn, std::auto_ptr<sql::Connection> & conn2, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		ensure("Data not populated", true == populate_test_table(conn, database));
		/* Get a result set */
		try {
			stmt->executeUpdate("SELECT * FROM test_function");
			ensure("No exception thrown", false);
		} catch (sql::SQLException &) {
		} catch (...) {
			printf("\n# ERR: Incorrectly sql::SQLException ist not thrown\n");
			printf("# ");
			total_errors++;
		}

		/* Clean */
		std::auto_ptr<sql::Statement> stmt2(conn2->createStatement());
		ensure("stmt is NULL", stmt2.get() != NULL);
		stmt2->execute("USE " + database);
		stmt2->execute("DROP TABLE test_function");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


#if 0
/* {{{ Test getFetchSize() - should return int value */
/* XXX: Test fails because getFetchSize() is not implemented*/
static void test_statement_xx(std::auto_ptr<sql::Connection> & conn, std::auto_ptr<sql::Connection> & conn2)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		ensure("fetchSize is negative", stmt->getFetchSize() > 0);
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */

/* {{{ Test setFetchSize() - set and get the value */
/* XXX: Doesn't pass because setFetchSize() is unimplemented */
static void test_statement_xx(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		int setFetchSize = 50;

		stmt->setFetchSize(setFetchSize);

		ensure_equal("Non-equal", setFetchSize, stmt->getFetchSize());
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
}
/* }}} */


/* {{{ Test setFetchSize() - set negative value and expect an exception */
/* XXX: Doesn't pass because setFetchSize() is unimplemented */
static void test_statement_xx(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		try {
			stmt->setFetchSize(-1);
			ensure("No exception", false);
		} catch (sql::InvalidArgumentException) {
			printf("INFO: Caught sql::InvalidArgumentException\n");
		}
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test setQueryTimeout() - set negative value and expect an exception */
/* XXX: Doesn't pass because setQueryTimeout() is unimplemented */
static void test_statement_xx(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		try {
			stmt->setQueryTimeout(-1);
			printf("\n# ERR:No exception\n");
		} catch (sql::InvalidArgumentException &) {}
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */
#endif


/* {{{ Test getResultSet() - execute() a query and get the result set */
static void test_statement_8(std::auto_ptr<sql::Connection> & conn, std::auto_ptr<sql::Connection> & conn2, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		ensure("Data not populated", true == populate_test_table(conn, database));

		ensure("sql::Statement::execute returned false", true == stmt->execute("SELECT * FROM test_function"));

		std::auto_ptr<sql::ResultSet> rset(stmt->getResultSet());
		ensure("rset is NULL", rset.get() != NULL);

		/* Clean */
		std::auto_ptr<sql::Statement> stmt2(conn2->createStatement());
		ensure("stmt is NULL", stmt2.get() != NULL);
		stmt2->execute("USE " + database);
		stmt2->execute("DROP TABLE test_function");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
}
/* }}} */


/* {{{ Test getResultSet() - execute() an update query and get the result set - should be empty */
static void test_statement_9(std::auto_ptr<sql::Connection> & conn, std::auto_ptr<sql::Connection> & conn2, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("stmt is NULL", stmt.get() != NULL);

		ensure("Data not populated", true == populate_test_table(conn, database));

		ensure("sql::Statement::execute returned true", false == stmt->execute("UPDATE test_function SET a = 222"));

		std::auto_ptr<sql::ResultSet> rset(stmt->getResultSet());
		ensure("rset is not NULL", rset.get() == NULL);

		/* Clean */
		std::auto_ptr<sql::Statement> stmt2(conn2->createStatement());
		ensure("stmt is NULL", stmt2.get() != NULL);
		stmt2->execute("USE " + database);
		stmt2->execute("DROP TABLE test_function");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{	*/
static void test_result_set_0(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());
		ensure("AutoCommit", conn.get() == stmt->getConnection());

		std::auto_ptr<sql::ResultSet> result(stmt->executeQuery("SELECT 1, 2, 3"));

		ensure_equal_int("isFirst", result->isFirst(), false);

		ensure_equal_int("isLast", result->isLast(), false);
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{	*/
static void test_result_set_1(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
		ensure("stmt1 is NULL", stmt1.get() != NULL);

		std::auto_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT 1"));
		ensure("res1 is NULL", rset1.get() != NULL);

		std::auto_ptr<sql::ResultSet> rset2(stmt1->executeQuery("SELECT 1"));
		ensure("res2 is NULL", rset2.get() != NULL);

		ensure("res1 is empty", rset1->next() != false);
		ensure("res2 is empty", rset2->next() != false);
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{	*/
static void test_result_set_2(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
		ensure("stmt1 is NULL", stmt1.get() != NULL);

		ensure("Data not populated", true == populate_test_table(conn, database));

		std::auto_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT 1"));
		ensure("res1 is NULL", rset1.get() != NULL);
		ensure_equal_int("res1 is empty", rset1->next(), true);
		ensure_equal_int("res1 is empty", rset1->next(), false);

		ensure("No rows updated", stmt1->executeUpdate("UPDATE test_function SET a = 2") > 0);

		stmt1->execute("DROP TABLE test_function");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{	*/
static void test_result_set_3(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
		ensure("stmt1 is NULL", stmt1.get() != NULL);

		ensure_equal("sql::Connection differs", conn.get(), stmt1->getConnection());
		int old_commit_mode = conn->getAutoCommit();
		conn->setAutoCommit(0);

		ensure("Data not populated", true == populate_TX_test_table(conn, database));

		std::auto_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
		ensure("res1 is NULL", rset1.get() != NULL);
		ensure("res1 is empty", rset1->next() != false);
		int count_full_before = rset1->getInt(1);
		ensure("res1 has more rows ", rset1->next() == false);

		/* Let's delete and then rollback */
		ensure_equal("Deleted less rows",
									stmt1->executeUpdate("DELETE FROM test_function_tx WHERE 1"),
									count_full_before);

		std::auto_ptr<sql::ResultSet> rset2(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
		ensure("res2 is NULL", rset2.get() != NULL);
		ensure("res2 is empty", rset2->next() != false);
		ensure("Table not empty after delete", rset2->getInt(1) == 0);
		ensure("res2 has more rows ", rset2->next() == false);

		stmt1->getConnection()->rollback();

		std::auto_ptr<sql::ResultSet> rset3(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
		ensure("res3 is NULL", rset3.get() != NULL);
		ensure("res3 is empty", rset3->next() != false);
		int count_full_after = rset3->getInt(1);
		ensure("res3 has more rows ", rset3->next() == false);

		ensure("Rollback didn't work", count_full_before == count_full_after);

		/* Now let's delete and then commit */
		ensure_equal("Deleted less rows",
									stmt1->executeUpdate("DELETE FROM test_function_tx WHERE 1"),
									count_full_before);
		stmt1->getConnection()->commit();

		std::auto_ptr<sql::ResultSet> rset4(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
		ensure("res4 is NULL", rset4.get() != NULL);
		ensure("res4 is empty", rset4->next() != false);
		ensure("Table not empty after delete", rset4->getInt(1) == 0);
		ensure("res4 has more rows ", rset4->next() == false);

		stmt1->execute("DROP TABLE test_function_tx");

		conn->setAutoCommit(old_commit_mode);
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test commit and rollback (autocommit on) */
static void test_result_set_4(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
		ensure("stmt1 is NULL", stmt1.get() != NULL);

		ensure_equal("sql::Connection differs", conn.get(), stmt1->getConnection());

		int old_commit_mode = conn->getAutoCommit();
		conn->setAutoCommit(true);
		ensure_equal_int("Data not populated", true, populate_TX_test_table(conn, database));


		std::auto_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
		ensure("res1 is NULL", rset1.get() != NULL);
		ensure_equal_int("res1 is empty", rset1->next(), true);
		int count_full_before = rset1->getInt(1);
		ensure_equal_int("res1 has more rows ", rset1->next(), false);

		/* Let's delete and then rollback */
		ensure_equal_int("Deleted less rows",
									stmt1->executeUpdate("DELETE FROM test_function_tx WHERE 1"),
									count_full_before);

		std::auto_ptr<sql::ResultSet> rset2(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
		ensure("res2 is NULL", rset2.get() != NULL);
		ensure_equal_int("res2 is empty", rset2->next(), true);
		ensure_equal_int("Table not empty after delete", rset2->getInt(1), 0);
		ensure_equal_int("res2 has more rows ", rset2->next(), false);

		/* In autocommit on, this is a no-op */
		stmt1->getConnection()->rollback();

		std::auto_ptr<sql::ResultSet> rset3(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
		ensure("res3 is NULL", rset3.get() != NULL);
		ensure_equal_int("res3 is empty", rset3->next(), true);
		ensure_equal_int("Rollback didn't work", rset3->getInt(1), 0);
		ensure_equal_int("res3 has more rows ", rset3->next(), false);

		ensure("Data not populated", true == populate_TX_test_table(conn, database));

		/* Now let's delete and then commit */
		ensure_equal("Deleted less rows",
									stmt1->executeUpdate("DELETE FROM test_function_tx WHERE 1"),
									count_full_before);
		/* In autocommit on, this is a no-op */
		stmt1->getConnection()->commit();

		std::auto_ptr<sql::ResultSet> rset4(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
		ensure("res4 is NULL", rset4.get() != NULL);
		ensure_equal_int("res4 is empty", rset4->next(), true);
		ensure_equal_int("Table not empty after delete", rset4->getInt(1), 0);
		ensure_equal_int("res4 has more rows ", rset4->next(), false);

		conn->setAutoCommit(false);
		ensure("Data not populated", true == populate_TX_test_table(conn, database));
		ensure_equal("Deleted less rows",
									stmt1->executeUpdate("DELETE FROM test_function_tx WHERE 1"),
									count_full_before);
		/* In autocommit iff, this is an op */
		stmt1->getConnection()->rollback();
		std::auto_ptr<sql::ResultSet> rset5(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx"));
		ensure("res5 is NULL", rset5.get() != NULL);
		ensure_equal_int("res5 is empty", rset5->next(), true);
		ensure_equal_int("Table empty after delete", rset5->getInt(1), count_full_before);
		ensure_equal_int("res5 has more rows ", rset5->next(), false);

		stmt1->execute("DROP TABLE test_function_tx");

		conn->setAutoCommit(old_commit_mode);
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test multistatement off - send two queries in one call */
static void test_result_set_5(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
		ensure("stmt1 is NULL", stmt1.get() != NULL);

		try {
			std::auto_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT COUNT(*) FROM test_function_tx; DELETE FROM test_function_tx"));
			ensure("ERR: Exception not thrown", false);
		} catch (sql::SQLException &) {}
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


static void test_result_set_check_out_of_bound(sql::ResultSet *rset1)
{
	ensure("res1 is empty", rset1->next() != false);
	try {
		rset1->getInt(-123);
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {
		rset1->getInt(123);
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {
		rset1->getInt("no_such_column");
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {
		rset1->getString(-123);
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {
		rset1->getString(123);
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {
		rset1->getString("no_such_column");
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {
		rset1->getDouble(-123);
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {
		rset1->getDouble(123);
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {
		rset1->getDouble("no_such_column");
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {
		rset1->getInt(rset1->getInt(1) + 1000);
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {
		rset1->isNull(-123);
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {
		rset1->isNull(123);
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {
		rset1->isNull("no_such_column");
		ensure("ERR: No sql::InvalidArgumentException thrown", false);
	} catch (sql::InvalidArgumentException &) {}
	try {

		ensure_equal_int("res1 has more rows ", rset1->getInt(1), 1);
		ensure_equal_int("res1 has more rows ", rset1->getInt("count of rows"), 1);

//		ensure("res1 has more rows ", rset1->getDouble(1) - 1 < 0.1);
//		ensure("res1 has more rows ", rset1->getDouble("count of rows") - 1 < 0.1);
//		with libmysq we don't support these conversions,  on the fly :(
//		ensure("res1 has more rows ", rset1->getString(1).compare("1"));
//		ensure("res1 has more rows ", rset1->getString("count of rows").compare("1"));

		ensure_equal_int("c is not null", rset1->isNull(1), false);

		ensure_equal_int("res1 has more rows ", rset1->next(), false);
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
}


/* {{{ Test out of bound extraction of data */
static void test_result_set_6(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
		ensure("stmt1 is NULL", stmt1.get() != NULL);

		ensure_equal("sql::Connection differs", conn.get(), stmt1->getConnection());

		ensure("Data not populated", true == populate_TX_test_table(conn, database));

		std::auto_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT COUNT(*) AS 'count of rows' FROM test_function_tx"));
		ensure("res1 is NULL", rset1.get() != NULL);

		test_result_set_check_out_of_bound(rset1.get());

		stmt1->execute("DROP TABLE test_function_tx");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test out of bound extraction of data - PS version */
static void test_result_set_7(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	ENTER_FUNCTION();
	try {
		ensure("Data not populated", true == populate_TX_test_table(conn, database));

		std::auto_ptr<sql::PreparedStatement> stmt1(conn->prepareStatement("SELECT COUNT(*) AS 'count of rows' FROM test_function_tx"));
		ensure("stmt1 is NULL", stmt1.get() != NULL);
		ensure_equal("sql::Connection differs", conn.get(), stmt1->getConnection());

		std::auto_ptr<sql::ResultSet> rset1(stmt1->executeQuery());
		ensure("res1 is NULL", rset1.get() != NULL);

		test_result_set_check_out_of_bound(rset1.get());

		std::auto_ptr<sql::PreparedStatement> stmt2(conn->prepareStatement("DROP TABLE test_function_tx"));
		stmt2->execute();
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test commit and rollback (autocommit on) - PS version */
static void test_result_set_8(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	ENTER_FUNCTION();
	try {
		int count_full_before;
		std::auto_ptr<sql::PreparedStatement> stmt0(conn->prepareStatement("SELECT 1"));
		ensure("stmt0 is NULL", stmt0.get() != NULL);

		ensure_equal("sql::Connection differs", conn.get(), stmt0->getConnection());

		int old_commit_mode = conn->getAutoCommit();
		conn->setAutoCommit(true);
		ensure("Data not populated", true == populate_TX_test_table_PS(conn, database));

		std::auto_ptr<sql::PreparedStatement> stmt1(conn->prepareStatement("SELECT COUNT(*) FROM test_function_tx"));
		std::auto_ptr<sql::ResultSet> rset1(stmt1->executeQuery());
		ensure("res1 is NULL", rset1.get() != NULL);
		ensure_equal_int("res1 is empty", rset1->next(), true);
		count_full_before = rset1->getInt(1);
		ensure_equal_int("res1 has more rows ", rset1->next(), false);

		std::auto_ptr<sql::PreparedStatement> stmt2(conn->prepareStatement("DELETE FROM test_function_tx WHERE 1"));
		/* Let's delete and then rollback */
		ensure_equal_int("Deleted less rows", stmt2->executeUpdate(), count_full_before);

		std::auto_ptr<sql::PreparedStatement> stmt3(conn->prepareStatement("SELECT COUNT(*) FROM test_function_tx"));
		std::auto_ptr<sql::ResultSet> rset2(stmt3->executeQuery());
		ensure("res2 is NULL", rset2.get() != NULL);
		ensure_equal_int("res2 is empty", rset2->next(), true);
		ensure_equal_int("Table not empty after delete", rset2->getInt(1), 0);
		ensure_equal_int("res2 has more rows ", rset2->next(), false);

		/* In autocommit on, this is a no-op */
		stmt1->getConnection()->rollback();

		std::auto_ptr<sql::PreparedStatement> stmt4(conn->prepareStatement("SELECT COUNT(*) FROM test_function_tx"));
		std::auto_ptr<sql::ResultSet> rset3(stmt4->executeQuery());
		ensure("res3 is NULL", rset3.get() != NULL);
		ensure_equal_int("res3 is empty", rset3->next(), true);
		ensure_equal_int("Rollback didn't work", rset3->getInt(1), 0);
		ensure_equal_int("res3 has more rows ", rset3->next(), false);

		ensure("Data not populated", true == populate_TX_test_table_PS(conn, database));

		std::auto_ptr<sql::PreparedStatement> stmt5(conn->prepareStatement("DELETE FROM test_function_tx WHERE 1"));
		/* Let's delete and then rollback */
		ensure_equal_int("Deleted less rows", stmt5->executeUpdate(), count_full_before);

		/* In autocommit on, this is a no-op */
		stmt1->getConnection()->commit();

		std::auto_ptr<sql::PreparedStatement> stmt6(conn->prepareStatement("SELECT COUNT(*) FROM test_function_tx"));
		std::auto_ptr<sql::ResultSet> rset4(stmt6->executeQuery());
		ensure("res4 is NULL", rset4.get() != NULL);
		ensure_equal_int("res4 is empty", rset4->next(), true);
		ensure_equal_int("Rollback didn't work", rset4->getInt(1), 0);
		ensure_equal_int("res4 has more rows ", rset4->next(), false);

		conn->setAutoCommit(false);
		ensure("Data not populated", true == populate_TX_test_table_PS(conn, database));
		std::auto_ptr<sql::PreparedStatement> stmt7(conn->prepareStatement("DELETE FROM test_function_tx WHERE 1"));
		/* Let's delete and then rollback */
		ensure("Deleted less rows", stmt7->executeUpdate() == count_full_before);
		/* In autocommit iff, this is an op */
		stmt1->getConnection()->rollback();

		std::auto_ptr<sql::PreparedStatement> stmt8(conn->prepareStatement("SELECT COUNT(*) FROM test_function_tx"));
		std::auto_ptr<sql::ResultSet> rset5(stmt8->executeQuery());
		ensure("res5 is NULL", rset5.get() != NULL);
		ensure_equal_int("res5 is empty", rset5->next(), true);
		ensure_equal_int("Rollback didn't work", rset5->getInt(1), 0);
		ensure_equal_int("res5 has more rows ", rset5->next(), false);

		std::auto_ptr<sql::PreparedStatement> stmt9(conn->prepareStatement("DROP TABLE test_function_tx"));
		stmt1->execute();

		conn->setAutoCommit(old_commit_mode);
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test multistatement off - send two queries in one call - PS version */
static void test_result_set_9(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {

		try {
			std::auto_ptr<sql::PreparedStatement> stmt1(conn->prepareStatement("SELECT COUNT(*) FROM test_function_tx; DELETE FROM test_function_tx"));
			ensure("ERR: Exception not thrown", false);
		} catch (sql::SQLException &) {}
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test multiresults - SP with normal and prepared statement */
static void test_result_set_10(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt0(conn->createStatement());
		ensure("stmt0 is NULL", stmt0.get() != NULL);
		stmt0->execute("USE " + database);

#if 0
	/* Doesn't work with libmysql - a limitation of the library, might work with mysqlnd if it lies under */
		{
			/* Create procedure is not supported for preparing */
			std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
			stmt1->execute("DROP PROCEDURE IF EXISTS CPP1");
			stmt1->execute("CREATE PROCEDURE CPP1() SELECT 42");

			std::auto_ptr<sql::PreparedStatement> stmt2(conn->prepareStatement("CALL CPP1()"));
			stmt2->execute();

			std::auto_ptr<sql::ResultSet> rset1(stmt2->getResultSet());
			ensure("res1 is NULL", rset1.get() != NULL);
			ensure_equal_int("res1 is empty", rset1->next(), true);
			eensure_equal_intsure("Wrong data", rset1->getInt(1), 42);
			ensure_equal_int("res1 has more rows ", rset1->next(), false);

			/* Here comes the status result set*/
			std::auto_ptr<sql::ResultSet> rset2(stmt2->getResultSet());
			ensure("res2 is not NULL", rset2.get() == NULL);

			/* drop procedure is not supported for preparing */
			stmt1->execute("DROP PROCEDURE CPP1");
		}

		{
			std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
			stmt1->execute("DROP PROCEDURE IF EXISTS CPP1");
			stmt1->execute("CREATE PROCEDURE CPP1() SELECT 42");

			stmt1->execute("CALL CPP1()");
			std::auto_ptr<sql::ResultSet> rset1(stmt1->getResultSet());
			ensure("res1 is NULL", rset1.get() != NULL);
			ensure_equal_int("res1 is empty", rset1->next(), true);
			ensure_equal_int("Wrong data", rset1->getInt(1), 42);
			ensure_equal_int("res1 has more rows ", rset1->next(), false);

			/* Here comes the status result set*/
			std::auto_ptr<sql::ResultSet> rset2(stmt1->getResultSet());
			ensure_equal_int("res2 is not NULL", rset2.get(), NULL);

			stmt1->execute("DROP PROCEDURE CPP1");
		}
#endif
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ getMetadata() */
static void test_result_set_11(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt1(conn->createStatement());
		ensure("stmt1 is NULL", stmt1.get() != NULL);

		ensure("Data not populated", true == populate_test_table(conn, database));

		std::auto_ptr<sql::ResultSet> rset1(stmt1->executeQuery("SELECT * FROM test_function"));
		ensure("res1 is NULL", rset1.get() != NULL);
		ensure("res1 is empty", rset1->next() != false);

		std::auto_ptr<sql::ResultSetMetaData> meta1(rset1->getMetaData());
		ensure("column name differs", !meta1->getColumnName(1).compare("a"));
		ensure("column name differs", !meta1->getColumnName(2).compare("b"));
		ensure("column name differs", !meta1->getColumnName(3).compare("c"));
		ensure("column name differs", !meta1->getColumnName(4).compare("d"));
		ensure("column name differs", !meta1->getColumnName(5).compare("e"));

		ensure_equal_int("bad case sensitivity", meta1->isCaseSensitive(1), false);
		ensure_equal_int("bad case sensitivity", meta1->isCaseSensitive(2), false);
		ensure_equal_int("bad case sensitivity", meta1->isCaseSensitive(3), false);
		ensure_equal_int("bad case sensitivity", meta1->isCaseSensitive(4), false);
		ensure_equal_int("bad case sensitivity", meta1->isCaseSensitive(5), true);

		ensure_equal_int("bad case sensitivity", meta1->isCurrency(1), false);
		ensure_equal_int("bad case sensitivity", meta1->isCurrency(2), false);
		ensure_equal_int("bad case sensitivity", meta1->isCurrency(3), false);
		ensure_equal_int("bad case sensitivity", meta1->isCurrency(4), false);
		ensure_equal_int("bad case sensitivity", meta1->isCurrency(5), false);

		try {
			meta1->getColumnName(0);
			meta1->isCaseSensitive(0);
			meta1->isCurrency(0);
			ensure("Exception not correctly thrown", false);
		} catch (sql::SQLException &) {
			ensure("Exception correctly thrown", true);
		}
		try {
			meta1->getColumnName(100);
			meta1->isCaseSensitive(100);
			meta1->isCurrency(100);
			ensure("Exception not correctly thrown", false);
		} catch (sql::SQLException &) {
			ensure("Exception correctly thrown", true);
		}
		/*
			a integer unsigned not null,
			b integer,
			c integer default null,
			d char(10),
			e varchar(10) character set utf8 collate utf8_bin
		*/


	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */

#if 0
/* {{{ General test 0 */
static void test_general_0(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::DatabaseMetaData> meta(conn->getMetaData());
		std::auto_ptr<sql::ResultSet> rset(meta->getSchemata());

		while (rset->next()) {
			std::auto_ptr<sql::ResultSet> rset2(meta->getSchemaObjects("", rset->getString("schema_name")));

			while (rset2->next())  {
				rset2->getString("object_type").c_str();
				rset2->getString("name").c_str();
				rset2->getString("ddl").c_str();
			}
		}
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ General test 1 */
static void test_general_1(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::Statement> stmt(conn->createStatement());

		stmt->execute("DROP TABLE IF EXISTS test.product");
		stmt->execute("CREATE TABLE test.product(idproduct INT NOT NULL AUTO_INCREMENT PRIMARY KEY, name VARCHAR(80))");

		conn->setAutoCommit(0);


		std::auto_ptr<sql::PreparedStatement> prepStmt(conn->prepareStatement("INSERT INTO test.product (idproduct, name) VALUES(?, ?)"));
		prepStmt->setInt(1, 1);
		prepStmt->setString(2, "The answer is 42");
		prepStmt->executeUpdate();

		std::auto_ptr<sql::ResultSet> rset1(stmt->executeQuery("SELECT * FROM test.product"));

		ensure_equal_int("Empty result set", rset1->next(), true);
		ensure("Wrong data", !rset1->getString(2).compare("The answer is 42"));
		ensure("Wrong data", !rset1->getString("name").compare("The answer is 42"));
		ensure_equal_int("Non-Empty result set", rset1->next(), false);

		conn->rollback();

		std::auto_ptr<sql::ResultSet> rset2(stmt->executeQuery("SELECT * FROM test.product"));

		ensure_equal_int("Non-Empty result set", rset1->next(), false);

		stmt->execute("DROP TABLE IF EXISTS test.product");
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */
#endif

/* {{{	*/
static void test_prep_statement_0(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();
	try {
		try {
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("SELECT 1"));
			stmt->execute();
			std::auto_ptr<sql::ResultSet> rset1(stmt->getResultSet());
		} catch (sql::SQLException &e) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
			printf("# ");
			total_errors++;
		}

		try {
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("SELECT ?"));
		} catch (sql::SQLException &e) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
			printf("# ");
			total_errors++;
		}

		/* Bind but don't execute. There should be no leak */
		try {
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("SELECT ?, ?, ?"));
			stmt->setInt(1, 1);
		} catch (sql::SQLException &e) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
			printf("# ");
			total_errors++;
		}

		/* Bind two different types for the same column. There should be no leak */
		try {
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("SELECT ?"));
			stmt->setString(1, "Hello MySQL");
			stmt->setInt(1, 42);
			stmt->execute();
		} catch (sql::SQLException &e) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
			printf("# ");
			total_errors++;
		}

		/* Execute without fetching the result set. The connector should clean the wire */
		try {
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("SELECT ?, ?, ?, ?"));
			stmt->setInt(1, 1);
			stmt->setDouble(2, 2.25);
			stmt->setString(3, "Здрасти МySQL");
			stmt->setDateTime(4, "2006-11-10 16:17:18");
			stmt->execute();
		} catch (sql::SQLException &e) {			
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
			printf("# ");
			total_errors++;
		}

#if 0
		/* Bind one parameter less than needed - NULL should be sent to the server . Check also multibyte fetching. */
		try {
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("SELECT ? as \"Здравей_МySQL\" , ?, ?"));
			stmt->setInt(3, 42);
			stmt->setString(1, "Здравей МySQL! Как си?");
			stmt->execute();
			std::auto_ptr<sql::ResultSet> rset(stmt->getResultSet());
			ensure("No result set", rset.get() != NULL);
			ensure("Result set is empty", rset->next() != false);
			ensure("Incorrect value for col 1", rset->getInt(2) == 0 && true == rset->wasNull());

			ensure("Incorrect value for col 0", !rset->getString(1).compare("Здравей МySQL! Как си?") && false == rset->wasNull());
			ensure("Incorrect value for col 0", !rset->getString("Здравей_МySQL").compare("Здравей МySQL! Как си?") && false == rset->wasNull());

			ensure("Incorrect value for col 2", rset->getInt(3) == 42 && false == rset->wasNull());
			ensure("Incorrect value for col 2", !rset->getString(3).compare("42") && false == rset->wasNull());
		} catch (sql::SQLException &e) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
			printf("# ");
			total_errors++;
		}
#endif
		/* try double ::execute() */
		try {
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("SELECT ?"));
			stmt->setString(1, "Hello World");
			for (int i = 0; i < 100; i++) {
				std::auto_ptr<sql::ResultSet> rset(stmt->executeQuery());
			}
		} catch (sql::SQLException &e) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
			printf("# ");
			total_errors++;
		}

		/* try clearParameters() call */
		{
#if 0
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("SELECT ?, ?, ?, ?"));
			/* Step 1 */
			try {
				stmt->setInt(3, 13);
				stmt->setString(2, "Hello WORLD");
				stmt->setDouble(4, 1.25);

				stmt->clearParameters();

				stmt->execute();

				std::auto_ptr<sql::ResultSet> rset(stmt->getResultSet());

				ensure("No result set", rset.get() != NULL);
				ensure("Result set is empty", rset->next() != false);
				ensure("Incorrect value for col 1", rset->getInt(2) == 0 && true == rset->wasNull());

				ensure("Incorrect value for col 2", !rset->getString(1).compare("") && true == rset->wasNull());

				ensure("Incorrect value for col 2", rset->getInt(3) == 0 && true == rset->wasNull());
				ensure("Incorrect value for col 2", !rset->getString(3).compare("") && true == rset->wasNull());

			} catch (sql::SQLException &e) {
				printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
				printf("# ");
				total_errors++;
			}
			/* Step 2 */
#endif
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("SELECT ?, ?, ?, NULL"));
			try {
				stmt->setInt(3, 42);
				stmt->setString(1, "Hello MYSQL");
				stmt->setDouble(2, 1.25);
				std::auto_ptr<sql::ResultSet> rset(stmt->executeQuery());
				ensure("No result set", rset.get() != NULL);
				ensure("Result set is empty", rset->next() != false);
				ensure("Incorrect value for col 1", !rset->getString(4).compare("") && true == rset->wasNull());

				ensure("Incorrect value for col 0", !rset->getString(1).compare("Hello MYSQL") && false == rset->wasNull());

				ensure("Incorrect value for col 2", rset->getInt(3) == 42 && false == rset->wasNull());
//				ensure("Incorrect value for col 2", !rset->getString(3).compare("42") && false == rset->wasNull());

				ensure("Incorrect value for col 3", rset->getDouble(2) == 1.25 && false == rset->wasNull());
			} catch (sql::SQLException &e) {
				printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
				printf("# ");
				total_errors++;
			}
		}
		try {
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("SELECT ?"));
			stmt->setInt(1, 1);
			stmt->execute();
			std::auto_ptr<sql::ResultSet> rset(stmt->getResultSet());
		} catch (sql::SQLException &) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d\n", CPPCONN_FUNC, __LINE__);
			printf("# ");
			total_errors++;
		}

	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test simple update statement against statement object */
static void test_prep_statement_1(std::auto_ptr<sql::Connection> & conn, std::auto_ptr<sql::Connection> & conn2, const std::string database)
{
	ENTER_FUNCTION();
	try {
		std::auto_ptr<sql::PreparedStatement> stmt0(conn->prepareStatement("SELECT 1, 2, 3"));
		ensure("stmt0 is NULL", stmt0.get() != NULL);

		ensure("Data not populated", true == populate_test_table_PS(conn, database));

		std::auto_ptr<sql::PreparedStatement> stmt1(conn->prepareStatement("SELECT * FROM test_function"));
		ensure("stmt1 is NULL", stmt1.get() != NULL);
		if (false == stmt1->execute())
			ensure("False returned for SELECT", false);
		std::auto_ptr<sql::ResultSet> rset(stmt1->getResultSet());

		/* Clean */
		std::auto_ptr<sql::Statement> stmt2(conn2->createStatement());
		ensure("stmt2 is NULL", stmt2.get() != NULL);
		stmt2->execute("USE " + database);

		std::auto_ptr<sql::PreparedStatement> stmt3(conn2->prepareStatement("DROP TABLE test_function"));
		ensure("stmt3 is NULL", stmt3.get() != NULL);
		stmt3->execute();
	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Test simple update statement against statement object */
static void test_prep_statement_2(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	ENTER_FUNCTION();
	try {
		try {
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("ELECT 1"));
			ensure("ERR: Exception not thrown", false);
		} catch (sql::SQLException &) {}

		try {
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("SELECT '1"));
			ensure("ERR: Exception not thrown", false);
		} catch (sql::SQLException &) {}

		/* USE still cannot be prepared */
		try {
			std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("USE " + database));
			ensure("ERR: Exception not thrown", false);
		} catch (sql::SQLException &) {}

	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{ Tests blob with PS */
static void test_prep_statement_blob(std::auto_ptr<sql::Connection> & conn, std::string database)
{
	ENTER_FUNCTION();
	try {
		populate_blob_table(conn, database);
		/* USE still cannot be prepared */
		std::auto_ptr<sql::Statement> use_stmt(conn->createStatement());
		use_stmt->execute("USE " + database);

		std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("INSERT INTO test_blob VALUES(?)"));
		std::string value("This is blob's value");
		TestBlob myTestBlob(value);
		stmt->setBlob(1, &myTestBlob);
		stmt->execute();

	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */

/* {{{	Invoke as many "not implemented" methods as possible for better code coverage (and to make sure we keep CHANGES current) */
static void test_not_implemented_connection(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();

	std::string bar("bar");
	int int_array[] = {1, 2, 3};
	std::string string_array[] = {"a", "b", "c"};

	try {
		
		try {
			total_tests++;
			conn->getClientOption("foo", &bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		try {
			total_tests++;
			conn->isReadOnly();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// prepareStatement(const std::string& /* sql */, int /* autoGeneratedKeys */)
		try {
			total_tests++;
			conn->prepareStatement(bar, 1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// prepareStatement(const std::string& /* sql */, int /* columnIndexes */ [])
		try {
			total_tests++;
			conn->prepareStatement(bar, int_array);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// prepareStatement(const std::string& /* sql */, int /* resultSetType */, int /* 
		try {
			total_tests++;
			conn->prepareStatement(bar, 1, 1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// prepareStatement(const std::string& /* sql */, int /* resultSetType */, int /* resultSetConcurrency */, int /* resultSetHoldability */)
		try {
			total_tests++;
			conn->prepareStatement(bar, 1, 1, 1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// prepareStatement(const std::string& /* sql */, std::string /* columnNames*/ [])
		try {
			total_tests++;
			conn->prepareStatement(bar, string_array);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setHoldability(int /* holdability */)
		try {
			total_tests++;
			conn->setHoldability(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setReadOnly(bool /* readOnly */)
		try {
			total_tests++;
			conn->setReadOnly(true);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}
		
		// setSavepoint()
		try {
			total_tests++;
			conn->setSavepoint();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */

static void test_not_implemented_statement(std::auto_ptr<sql::Connection> & conn, const std::string database)
{
	ENTER_FUNCTION();

	std::string bar("foo");
	std::auto_ptr<sql::Statement> stmt(conn->createStatement());
	stmt->execute("USE " + database);

	try {
		// cancel()
		try {
			total_tests++;
			stmt->cancel();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getFetchSize()
		try {
			total_tests++;
			stmt->getFetchSize();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setFetchSize(unsigned int)
		try {
			total_tests++;
			stmt->setFetchSize(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setQueryTimeout(unsigned int)
		try {
			total_tests++;
			stmt->setQueryTimeout(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getMaxFieldSize()
		try {
			total_tests++;
			stmt->getMaxFieldSize();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getMaxRows()
		try {
			total_tests++;
			stmt->getMaxRows();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getQueryTimeout()
		try {
			total_tests++;
			stmt->getQueryTimeout();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setCursorName(const std::string &)
		try {
			total_tests++;
			stmt->setCursorName(bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setEscapeProcessing(bool)
		try {
			total_tests++;
			stmt->setEscapeProcessing(true);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setMaxFieldSize(unsigned int)
		try {
			total_tests++;
			stmt->setMaxFieldSize(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setMaxRows(unsigned int)
		try {
			total_tests++;
			stmt->setMaxRows(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */

static void test_not_implemented_conn_meta(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();

	std::string bar("foo");
	std::auto_ptr<sql::DatabaseMetaData> conn_meta(conn->getMetaData());

	try {

		// :getColumnPrivileges(const std::string& /*catalog*/, const std::string& /*schema*/,        const std::string& /*table*/, const std::string& /*columnNamePattern*/) const
		try {
			total_tests++;
			conn_meta->getColumnPrivileges(bar, bar, bar, bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getCrossReference(const std::string& /*primaryCatalog*/, const std::string& /*primarySchema*/, const std::string& /*primaryTable*/, const std::string& /*foreignCatalog*/, const std::string& /*foreignSchema*/, const std::string& /*foreignTable*/)
		try {
			total_tests++;
			conn_meta->getCrossReference(bar, bar, bar, bar, bar, bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getExportedKeys(const std::string& /*catalog*/, const std::string& /*schema*/, const std::string& /*table*/)
		try {
			total_tests++;
			conn_meta->getExportedKeys(bar, bar, bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getProcedureColumns(const std::string& /* catalog */, const std::string& /*schemaPattern*/, const std::string& /*procedureNamePattern*/, const std::string& /*columnNamePattern*/)
		try {
			total_tests++;
			conn_meta->getProcedureColumns(bar, bar, bar, bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getURL()
		try {
			total_tests++;
			conn_meta->getURL();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// locatorsUpdateCopy()
		try {
			total_tests++;
			conn_meta->locatorsUpdateCopy();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// supportsConvert(int /* fromType */, int /* toType */)
		try {
			total_tests++;
			conn_meta->supportsConvert(1, 2);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// supportsIntegrityEnhancementFacility()
		try {
			total_tests++;
			conn_meta->supportsIntegrityEnhancementFacility();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// supportsResultSetConcurrency(int /* type */, int /* concurrency */)
		try {
			total_tests++;
			conn_meta->supportsResultSetConcurrency(1, 2);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */

static void test_not_implemented_ps(std::auto_ptr<sql::Connection> & conn, const std::string database)
{
	ENTER_FUNCTION();

	std::string bar("jedervernunft");
	std::auto_ptr<sql::Statement> stmt(conn->createStatement());	
	stmt->execute("USE " + database);

	std::auto_ptr<sql::PreparedStatement> ps1(conn->prepareStatement("SELECT 1"));
	std::auto_ptr<sql::PreparedStatement> ps2(conn->prepareStatement("SELECT ?"));
	ps2->setInt(1, 2);

	try {
	
		// clearParameters()
		try {
			ps1->clearParameters();
			ensure("ERR: Exception not thrown", true);
			total_tests++;
			ps2->clearParameters();
		} catch (sql::MethodNotImplementedException &) {}

		// execute(const std::string&)
		try {
			total_tests++;
			ps1->execute(bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// executeQuery(const std::string&)
		try {
			total_tests++;
			ps1->executeQuery(bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// executeUpdate(const std::string&)
		try {
			total_tests++;
			ps1->executeUpdate(bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// cancel()
		try {
			total_tests++;
			ps2->cancel();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getFetchSize()
		try {
			total_tests++;
			ps2->getFetchSize();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setFetchSize(unsigned int)
		try {
			total_tests++;
			ps2->setFetchSize(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setQueryTimeout(unsigned int)
		try {
			total_tests++;
			ps2->setQueryTimeout(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getMaxFieldSize()
		try {
			total_tests++;
			ps2->getMaxFieldSize();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getMaxRows()
		try {
			total_tests++;
			ps2->getMaxRows();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getMoreResult
		try {
			total_tests++;
			ps2->getMoreResults();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getQueryTimeout()
		try {
			total_tests++;
			ps2->getQueryTimeout();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getUpdateCount()
		try {
			total_tests++;
			ps2->getUpdateCount();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setCursorName(const std::string &)
		try {
			total_tests++;
			ps2->setCursorName(bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setEscapeProcessing(bool)
		try {
			total_tests++;
			ps2->setEscapeProcessing(true);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setMaxFieldSize(unsigned int)
		try {
			total_tests++;
			ps2->setMaxFieldSize(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setMaxRows(unsigned int)
		try {
			total_tests++;
			ps2->setMaxRows(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


static void test_not_implemented_resultset(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();

	std::string bar("foo");
	std::auto_ptr<sql::Statement> stmt(conn->createStatement());	
	std::auto_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT 1 AS 'a'"));

	try {
		// cancelRowUpdates()
		try {
			total_tests++;
			res->cancelRowUpdates();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getConcurrency()
		try {
			total_tests++;
			res->getConcurrency();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getCursorName()
		try {
			total_tests++;
			res->getCursorName();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getFetchDirection()
		try {
			total_tests++;
			res->getFetchDirection();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getFetchSize()
		try {
			total_tests++;
			res->getFetchSize();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getHoldability()
		try {
			total_tests++;
			res->getHoldability();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getRowId(unsigned int)
		try {
			total_tests++;
			res->getRowId(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getRowId(const std::string &)
		try {
			total_tests++;
			res->getRowId(bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// insertRow()
		try {
			total_tests++;
			res->insertRow();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// moveToCurrentRow()
		try {
			total_tests++;
			res->moveToCurrentRow();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// moveToInsertRow()
		try {
			total_tests++;
			res->moveToInsertRow();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// refreshRow()
		try {
			total_tests++;
			res->refreshRow();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// rowDeleted()
		try {
			total_tests++;
			res->rowDeleted();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// rowInserted()
		try {
			total_tests++;
			res->rowInserted();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// rowUpdated()
		try {
			total_tests++;
			res->rowUpdated();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setFetchSize(size_t /* rows */)
		try {
			total_tests++;
			res->setFetchSize(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


static void test_not_implemented_ps_resultset(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();

	std::string bar("foo");
	std::auto_ptr<sql::PreparedStatement> stmt(conn->prepareStatement("SELECT 1 AS 'a'"));
	std::auto_ptr<sql::ResultSet> res(stmt->executeQuery());

	try {
		// cancelRowUpdates()
		try {
			total_tests++;
			res->cancelRowUpdates();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getConcurrency()
		try {
			total_tests++;
			res->getConcurrency();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getCursorName()
		try {
			total_tests++;
			res->getCursorName();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getFetchDirection()
		try {
			total_tests++;
			res->getFetchDirection();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getFetchSize()
		try {
			total_tests++;
			res->getFetchSize();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getHoldability()
		try {
			total_tests++;
			res->getHoldability();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getRowId(unsigned int)
		try {
			total_tests++;
			res->getRowId(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getRowId(const std::string &)
		try {
			total_tests++;
			res->getRowId(bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// insertRow()
		try {
			total_tests++;
			res->insertRow();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// moveToCurrentRow()
		try {
			total_tests++;
			res->moveToCurrentRow();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// moveToInsertRow()
		try {
			total_tests++;
			res->moveToInsertRow();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// refreshRow()
		try {
			total_tests++;
			res->refreshRow();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// rowDeleted()
		try {
			total_tests++;
			res->rowDeleted();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// rowInserted()
		try {
			total_tests++;
			res->rowInserted();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// rowUpdated()
		try {
			total_tests++;
			res->rowUpdated();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setFetchSize(size_t /* rows */)
		try {
			total_tests++;
			res->setFetchSize(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */

static void test_not_implemented_cs_resultset(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();

	std::string bar("foo");	
	std::auto_ptr<sql::DatabaseMetaData> conn_meta(conn->getMetaData());
	std::auto_ptr<sql::ResultSet> res(conn_meta->getSchemaObjectTypes());

	try {
		// cancelRowUpdates()
		try {
			total_tests++;
			res->cancelRowUpdates();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getConcurrency()
		try {
			total_tests++;
			res->getConcurrency();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getCursorName()
		try {
			total_tests++;
			res->getCursorName();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getFetchDirection()
		try {
			total_tests++;
			res->getFetchDirection();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getFetchSize()
		try {
			total_tests++;
			res->getFetchSize();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getHoldability()
		try {
			total_tests++;
			res->getHoldability();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getRowId(unsigned int)
		try {
			total_tests++;
			res->getRowId(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getRowId(const std::string &)
		try {
			total_tests++;
			res->getRowId(bar);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// insertRow()
		try {
			total_tests++;
			res->insertRow();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// moveToCurrentRow()
		try {
			total_tests++;
			res->moveToCurrentRow();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// moveToInsertRow()
		try {
			total_tests++;
			res->moveToInsertRow();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// refreshRow()
		try {
			total_tests++;
			res->refreshRow();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// rowDeleted()
		try {
			total_tests++;
			res->rowDeleted();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// rowInserted()
		try {
			total_tests++;
			res->rowInserted();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// rowUpdated()
		try {
			total_tests++;
			res->rowUpdated();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// setFetchSize(size_t /* rows */)
		try {
			total_tests++;
			res->setFetchSize(1);
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */

static void test_not_implemented_rs_meta(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();

	std::string bar("foo");
	std::auto_ptr<sql::Statement> stmt(conn->createStatement());	
	std::auto_ptr<sql::ResultSet> res(stmt->executeQuery("SELECT 1 AS 'a'"));
	std::auto_ptr<sql::ResultSetMetaData> meta(res->getMetaData());

	try {
		try {
			total_tests++;
			res->cancelRowUpdates();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


static void test_not_implemented_cs_rs_meta(std::auto_ptr<sql::Connection> & conn)
{
	ENTER_FUNCTION();

	std::string bar("foo");
	std::auto_ptr<sql::DatabaseMetaData> conn_meta(conn->getMetaData());
	std::auto_ptr<sql::ResultSet> res(conn_meta->getSchemaObjectTypes());
	std::auto_ptr<sql::ResultSetMetaData> meta(res->getMetaData());

	try {
		// getColumnDisplaySize(unsigned int columnIndex) 
		try {
			total_tests++;
			meta->getColumnDisplaySize(1);
			res->cancelRowUpdates();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getPrecision(unsigned int columnIndex) 
		try {
			total_tests++;
			meta->getPrecision(1);
			res->cancelRowUpdates();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

		// getScale(unsigned int columnIndex)
		try {
			total_tests++;
			meta->getScale(1);
			res->cancelRowUpdates();
			ensure("ERR: Exception not thrown", false);
		} catch (sql::MethodNotImplementedException &) {}

	} catch (sql::SQLException &e) {
		printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
		printf("# ");
		total_errors++;
	} catch (...) {
		printf("\n# ERR: Caught unknown exception at %s::%d\n", CPPCONN_FUNC, __LINE__);
		printf("# ");
		total_errors++;
	}
	LEAVE_FUNCTION();
}
/* }}} */


/* {{{	*/
int run_tests(int argc, const char **argv)
{

	printf("1..%d\n#\n", loops);	
#ifndef DRIVER_TEST
	printf("# %s\n", mysql_get_client_info());
#endif

	std::auto_ptr<sql::Connection> conn, conn2;
	int last_error_total = 0;
	int i;

	const std::string user(argc >=3? argv[2]:"root");
	const std::string pass(argc >=4? argv[3]:"root");
	const std::string database(argc >=5? argv[4]:USED_DATABASE);
	
	for (i = 0 ; i < loops; i++) {
		last_error_total = total_errors;
		const std::string host(argc >=2? argv[1]:"tcp://127.0.0.1");
		printf("#---------------  %d -----------------\n", i + 1);
		printf("# ");

		try {
			conn.reset(get_connection(host, user, pass));
		} catch (sql::SQLException &e) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());		
			printf("not ok\n");
			return 1;
		}
		/* XXX : Doing upcast, not the best thing, but tests getMySQLVariable */
//		ensure("Testing getSessionVariable",
//					0 == ((sql::mysql::MySQL_Connection *) conn)->getSessionVariable("version").compare(
//								mysql_get_server_info(((sql::mysql::MySQL_Connection *) conn)->getMySQLHandle())
//																							)
//			);
//		printf("\n");
//		printf("# Server %s\n", ((sql::mysql::MySQL_Connection *) conn)->getSessionVariable("version").c_str());
//		printf("# ");

		try {		
			std::auto_ptr<sql::Statement> stmt(conn->createStatement());
			stmt->execute("SHOW ENGINES");
			std::auto_ptr<sql::ResultSet> rset(stmt->getResultSet());
			int found = 0;
			while (rset->next()) {
				if (rset->getString("Engine") == "InnoDB" && rset->getString("Support") == "YES") {
					found = 1;
					break;
				}
			}
			if (found == 0) {
				printf("\n#ERR: InnoDB Storage engine not available or disabled\n");
				printf("not ok\n");
				return 1;
			}

			stmt->execute("USE " + database);
		} catch (sql::SQLException &e) {
			printf("\n# ERR: Caught sql::SQLException at %s::%d  %s (%d/%s)\n", CPPCONN_FUNC, __LINE__, e.what(), e.getErrorCode(), e.getSQLState().c_str());
			printf("# ");
			return 1;
		}
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_connection_0(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_connection_1(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_connection_2(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_connection_3(conn, user);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_autocommit(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_statement_0(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		conn2.reset(get_connection(host, user, pass));
		test_statement_1(conn, conn2, database);
		conn.reset(NULL);
		conn2.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		conn2.reset(get_connection(host, user, pass));
		test_statement_2(conn, conn2, database);
		conn.reset(NULL);
		conn2.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		conn2.reset(get_connection(host, user, pass));
		test_statement_3(conn, conn2, database);
		conn.reset(NULL);
		conn2.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		conn2.reset(get_connection(host, user, pass));
		test_statement_4(conn, conn2, database);
		conn.reset(NULL);
		conn2.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		conn2.reset(get_connection(host, user, pass));
		test_statement_5(conn, conn2, database);
		conn.reset(NULL);
		conn2.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		conn2.reset(get_connection(host, user, pass));
		test_statement_6(conn, conn2, database);
		conn.reset(NULL);
		conn2.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		conn2.reset(get_connection(host, user, pass));
		test_statement_7(conn, conn2, database);
		conn.reset(NULL);
		conn2.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		conn2.reset(get_connection(host, user, pass));
		test_statement_8(conn, conn2, database);
		conn.reset(NULL);
		conn2.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		conn2.reset(get_connection(host, user, pass));
		test_statement_9(conn, conn2, database);
		conn.reset(NULL);
		conn2.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_result_set_0(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_result_set_1(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_result_set_2(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_result_set_3(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_result_set_4(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_result_set_5(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_result_set_6(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_result_set_7(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_result_set_8(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_result_set_9(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_result_set_10(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_result_set_11(conn, database);
		conn.reset(NULL);
#if 0
		test_general_0(conn); delete conn;
		test_general_1(conn); delete conn;
#endif
		conn.reset(get_connection(host, user, pass));
		test_prep_statement_0(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		conn2.reset(get_connection(host, user, pass));
		test_prep_statement_1(conn, conn2, database);
		conn.reset(NULL);
		conn2.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_prep_statement_2(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_prep_statement_blob(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_not_implemented_connection(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_not_implemented_statement(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_not_implemented_conn_meta(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_not_implemented_ps(conn, database);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_not_implemented_resultset(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_not_implemented_ps_resultset(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_not_implemented_cs_resultset(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_not_implemented_rs_meta(conn);
		conn.reset(NULL);

		conn.reset(get_connection(host, user, pass));
		test_not_implemented_cs_rs_meta(conn);
		conn.reset(NULL);

		printf("\n#---------------  %d -----------------\n", i + 1);	
		if ((total_errors - last_error_total) == 0)
			printf("ok\n");
		else 
			printf("not ok\n");
		printf("# ");
		
	}
	printf("\n# Loops=%2d Tests= %4d Failures= %3d \n", loops, total_tests, total_errors);

	return 0;
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
