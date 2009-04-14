
/*
Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPL
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPL as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include "TransactionTest.h"
#include <math.h>

namespace testsuite
{
namespace simple
{
  const double TransactionTest::DOUBLE_CONST= 25.4312;

  /**
	 * DOCUMENT ME!
	 *
	 * @throws Exception
	 *             DOCUMENT ME!
	 */
  /* throws Exception */
  void TransactionTest::setUp()
  {
    super::setUp();
    createTestTable();
  }

  /**
	 * DOCUMENT ME!
	 *
	 * @throws SQLException
	 *             DOCUMENT ME!
	 */
  /* throws SQLException */
  void TransactionTest::testTransaction()
  {
    bool prevAutoCommit= conn->getAutoCommit();

    conn->setAutoCommit( false );

    stmt->executeUpdate("INSERT INTO trans_test (id, decdata) VALUES (1, 1.0)");
    conn->rollback();

    rs.reset( stmt->executeQuery("SELECT * from trans_test") );

    bool hasResults = rs->next();

    ASSERT_MESSAGE((hasResults != true)
      , "Results returned, rollback to empty table failed" );

    std::ostringstream str;

    str << "INSERT INTO trans_test (id, decdata) VALUES (2, " << DOUBLE_CONST << ")";

    stmt->executeUpdate( str.str() );
    conn->commit();

    rs.reset( stmt->executeQuery("SELECT * from trans_test where id=2") );

    hasResults = rs->next();

    ASSERT_MESSAGE( hasResults, "No rows in table after INSERT" );

    double doubleVal = rs->getDouble(2);
    str.str("");

    str << "Double value returned != " << DOUBLE_CONST;

    ASSERT_EQUALS( DOUBLE_CONST, doubleVal );

    //no need to setAutoCommit to old value - connection reset after each test.
    conn->setAutoCommit( prevAutoCommit );
  }


/* throws SQLException */
  void TransactionTest::createTestTable()
  {
    try
    {
      stmt->executeUpdate("DROP TABLE trans_test");
    }
    catch ( sql::SQLException & /*sqlEx*/ )
    {
    }

    stmt->executeUpdate("CREATE TABLE trans_test (id INT NOT NULL PRIMARY KEY, decdata DOUBLE) ENGINE=InnoDB");
  }

}
}
