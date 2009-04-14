/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "../unit_fixture.h"

/**
 * Example of a collection of tests
 *
 */

namespace testsuite
{
namespace classes
{

class preparedstatement : public unit_fixture
{
private:
  typedef unit_fixture super;

public:

  EXAMPLE_TEST_FIXTURE(preparedstatement)
  {
    TEST_CASE(crash);    
    TEST_CASE(anonymousSelect);
    TEST_CASE(InsertSelectAllTypes);
    TEST_CASE(assortedSetType);
    TEST_CASE(setNull);
    TEST_CASE(checkClosed);
    TEST_CASE(getMetaData);
    TEST_CASE(callSP);
    TEST_CASE(getWarnings);
  }

  /**
   * SELECT ' ', NULL as string
   */
  void anonymousSelect();

  /**
   * Loops over all kinds of column types and inserts/fetches a value
   */
  void InsertSelectAllTypes();

  /**
   * Loops over assorted column types and uses setXYZ to insert a value
   */
  void assortedSetType();

  /**
   * Loops over assorted column types and uses setXYZ to insert a value
   */
  void setNull();

  /**
   * Calling close()
   */
  void checkClosed();

  /**
   * Compare PS and Non-PS Metadata.
   */
  void getMetaData();

  /**
   * Calls a stored procedure
   */
  void callSP();

  /**
   * TODO - temporary to isolate a crash, remove after fix!
   */
  void crash();

  /**
   * Check get|clearWarnings()
   */
  void getWarnings();

};

REGISTER_FIXTURE(preparedstatement);
} /* namespace classes */
} /* namespace testsuite */
