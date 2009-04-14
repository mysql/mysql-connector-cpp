/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "../unit_fixture.h"

/**
 * Test of the JDBC ResultSet class counterpart
 *
 */

namespace testsuite
{
namespace classes
{

class resultset : public unit_fixture
{
private:
  typedef unit_fixture super;

protected:
#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  void doNotImplemented();
#endif

public:

  EXAMPLE_TEST_FIXTURE(resultset)
  {
    TEST_CASE(fetchBigint);
    TEST_CASE(getInt);
    TEST_CASE(getTypes);
    TEST_CASE(getResultSetType);
    TEST_CASE(getTypesMinorIssues);

#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
    TEST_CASE(notImplemented);
#endif
  }

  /**
   * Test for resultset::getInt*()
   *
   * Test of the assorted methods to fetch integers
   */
  void getInt();

  /**
   * Test for resultset::get*()
   *
   * Test of the assorted methods to fetch data
   */
  void getTypes();


  /**
   * Test for resultset::getResultSetType()
   *
   * Test of the assorted methods to fetch data
   */
  void getResultSetType();


  /**
   * Test for resultset::get*()
   *
   * TODO - hack to temporarily seperate test failures from TODO for beta by having more fine grained test
   */
  void getTypesMinorIssues();

#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  /**
   * Calling methods which throw "not implemented" to detect API changes
   */
  void notImplemented();
#endif

  /**
   * Calling methods which throw "not implemented" to detect API changes
   */
  void fetchBigint();

};

REGISTER_FIXTURE(resultset);
} /* namespace classes */
} /* namespace testsuite */
