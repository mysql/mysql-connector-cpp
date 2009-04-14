/*
Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPL
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPL as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/


#include "SubqueriesRegressionTest.h"

namespace testsuite
{
namespace regression
{
  const int SubqueriesRegressionTest::REPETITIONS = 100;


  /*
	 * (non-Javadoc)
	 * 
	 * @see junit.framework.TestCase#setUp()
	 */
  /* throws Exception */
  void SubqueriesRegressionTest::setUp() 
  {
    super::setUp();
    createTables();
  }


  /*
	 * (non-Javadoc)
	 * 
	 * @see junit.framework.TestCase#tearDown()
	 */
  /* throws Exception */
  void SubqueriesRegressionTest::tearDown() 
  {
    dropTables();
    super::tearDown();
  }


  /**
	 * DOCUMENT ME!
	 * 
	 * @throws Exception
	 *             DOCUMENT ME!
	 */
  /* throws Exception */
  void SubqueriesRegressionTest::testSubQuery1() 
  {
    if (versionMeetsMinimum(4, 1))
    {
      for (int i = 0; i < REPETITIONS; i++)
      {
        rs.reset( stmt->executeQuery("select t3.colA from t3, t1 "\
                                     "where t3.colA = 'bbbb' "\
                                        "and t3.colB = t1.colA "\
                                        "and exists (select 'X' "\
                                                    "from t2 "\
                                                    "where t2.colB = t1.colB)"));

        ASSERT(rs->next());
        ASSERT_EQUALS( "bbbb", rs->getString(1) );
        ASSERT(!rs->next());
      }
    }
  }


  /**
	 * DOCUMENT ME!
	 * 
	 * @throws Exception
	 *             DOCUMENT ME!
	 */
  /* throws Exception */
  void SubqueriesRegressionTest::testSubQuery2() 
  {
    if (versionMeetsMinimum(4, 1))
    {
      for (int i = 0; i < REPETITIONS; i++)
      {
        rs.reset( stmt->executeQuery("select t3.colA from t3, t1"\
                                    " where t3.colA = 'bbbb' "\
                                        "and t3.colB = t1.colA "\
                                        "and exists (select 'X' from t2 "\
                                                    "where t2.colB = 2)") );
        ASSERT(rs->next());
        ASSERT_EQUALS( "bbbb", rs->getString(1) );
        ASSERT(!rs->next());
      }
    }
  }


  /**
	 * DOCUMENT ME!
	 * 
	 * @throws Exception
	 *             DOCUMENT ME!
	 */
  /* throws Exception */
  void SubqueriesRegressionTest::testSubQuery3() 
  {
    if (versionMeetsMinimum(4, 1))
    {
      for (int i = 0; i < REPETITIONS; i++)
      {
        rs.reset( stmt->executeQuery("select * from t1 where t1.colA = 'efgh' "\
                                        "and exists (select 'X' from t2 "\
                                                    "where t2.colB = t1.colB)") );
        ASSERT(rs->next());
        ASSERT_EQUALS( "efgh" , rs->getString(1) );
        ASSERT_EQUALS( "2"    , rs->getString(2) );
        ASSERT(!rs->next());
      }
    }
  }


  /**
	 * DOCUMENT ME!
	 * 
	 * @throws Exception
	 *             DOCUMENT ME!
	 */
  /* throws Exception */
  void SubqueriesRegressionTest::testSubQuery4() 
  {
    if ( versionMeetsMinimum(4, 1) )
    {
      for (int i = 0; i < REPETITIONS; i++)
      {
        rs.reset( stmt->executeQuery("select colA, '' from t2"\
                                    " union select colA, colB from t3") );
        ASSERT(rs->next());
        ASSERT_EQUALS("type1", rs->getString(1) );
        ASSERT_EQUALS(""     , rs->getString(2) );

        ASSERT(rs->next());
        ASSERT_EQUALS("type2", rs->getString(1) );
        ASSERT_EQUALS(""     , rs->getString(2) );

        ASSERT(rs->next());
        ASSERT_EQUALS("type3", rs->getString(1) );
        ASSERT_EQUALS(""     , rs->getString(2) );

        ASSERT(rs->next());
        ASSERT_EQUALS("aaaa", rs->getString(1));
        ASSERT_MESSAGE( rs->getString(2) == "abcd", String( "'" ) + rs->getString(2)
            + "' != expected of 'abcd'" );

        ASSERT(rs->next());
        ASSERT_EQUALS("bbbb", rs->getString(1) );
        ASSERT_EQUALS("efgh", rs->getString(2) );

        ASSERT(rs->next());
        ASSERT_EQUALS("cccc", rs->getString(1) );
        ASSERT_MESSAGE(rs->getString(2) == "ijkl", String( "'" ) + rs->getString(2)
            + "' != expected of 'ijkl'" );

        ASSERT(!rs->next());
      }
    }
  }


  /**
	 * DOCUMENT ME!
	 * 
	 * @throws Exception
	 *             DOCUMENT ME!
	 */
  /* throws Exception */
  void SubqueriesRegressionTest::testSubQuery5() 
  {
    if ( versionMeetsMinimum(4, 1) )
    {
      for (int i = 0; i < REPETITIONS; i++)
      {
        rs.reset( stmt->executeQuery("select t1.colA from t1, t4 where t4.colA = t1.colA "\
                          "and exists (select 'X' from t2 where t2.colA = t4.colB)") );
        ASSERT(rs->next());
        ASSERT_EQUALS( "abcd", rs->getString(1) );

        ASSERT(rs->next());
        ASSERT_EQUALS( "efgh", rs->getString(1) );

        ASSERT(rs->next());
        ASSERT_EQUALS( "ijkl", rs->getString(1) );

        ASSERT(!rs->next());
      }
    }
  }


/* throws Exception */
  void SubqueriesRegressionTest::createTables() 
  {
    stmt->executeUpdate("drop table if exists t1");
    stmt->executeUpdate("drop table if exists t2");
    stmt->executeUpdate("drop table if exists t3");
    stmt->executeUpdate("drop table if exists t4");

    stmt->executeUpdate("create table t1(colA varchar(10), colB decimal(3,0))");
    stmt->executeUpdate("create table t2(colA varchar(10), colB varchar(10))");
    stmt->executeUpdate("create table t3(colA varchar(10), colB varchar(10))");
    stmt->executeUpdate("create table t4(colA varchar(10), colB varchar(10))");

    stmt->executeUpdate("insert into t1 values ('abcd', 1), ('efgh', 2)"\
                                            ", ('ijkl', 3)" );
    stmt->executeUpdate("insert into t2 values ('type1', '1'), ('type2', '2')"\
                                            ", ('type3', '3')");
    stmt->executeUpdate("insert into t3 values ('aaaa', 'abcd')"\
                                            ", ('bbbb', 'efgh')"\
                                            ", ('cccc', 'ijkl')");
    stmt->executeUpdate("insert into t4 values ('abcd', 'type1')"\
                                            ", ('efgh', 'type2')"\
                                            ", ('ijkl', 'type3')");
  }


/* throws Exception */
  void SubqueriesRegressionTest::dropTables() 
  {
    stmt->executeUpdate("drop table if exists t1");
    stmt->executeUpdate("drop table if exists t2");
    stmt->executeUpdate("drop table if exists t3");
    stmt->executeUpdate("drop table if exists t4");
  }

}
}
