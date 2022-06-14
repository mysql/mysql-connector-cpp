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



#include "../unit_fixture.h"
#include <iostream>
#include <climits>

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
  bool createSP(std::string sp_code);

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
    TEST_CASE(callSPInOut);
    TEST_CASE(callSPWithPS);
    TEST_CASE(callSPMultiRes);
    TEST_CASE(getWarnings);
    TEST_CASE(blob);
    TEST_CASE(executeQuery);
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
   * Calls a stored procedure with IN and OUT parameters
   */
  void callSPInOut();

  /**
   * Calls a stored procedure which contains a prepared statement
   */
  void callSPWithPS();

  /**
   * Calls a stored procedure which returns multiple result sets
   */
  void callSPMultiRes();

  /**
   * TODO - temporary to isolate a crash, remove after fix!
   */
  void crash();

  /**
   * Check get|clearWarnings()
   */
  void getWarnings();

  /**
   * Check BLOB/LOB handling
   */
  void blob();

  /**
   * Check executeQuery() and invalid fetch mode
   *
   * TODO - the test does focus on code coverage not functionality
   */
  void executeQuery();


};

REGISTER_FIXTURE(preparedstatement);
} /* namespace classes */
} /* namespace testsuite */
