/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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
    TEST_CASE(fetchBitAsInt);
    TEST_CASE(getInt);
    TEST_CASE(getTypes);
    TEST_CASE(getResultSetType);
    TEST_CASE(getTypesMinorIssues);
    TEST_CASE(JSON_support);

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

  /**
   * Fetching BIT values as integers - edge cases
   */
  void fetchBitAsInt();

  /**
   * Test for resultset::getString() on JSON type columns
   *
   * Test of the assorted methods to fetch JSON strings
   */
  void JSON_support();


};

REGISTER_FIXTURE(resultset);
} /* namespace classes */
} /* namespace testsuite */
