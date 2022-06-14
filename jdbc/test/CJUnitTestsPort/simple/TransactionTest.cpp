/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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
