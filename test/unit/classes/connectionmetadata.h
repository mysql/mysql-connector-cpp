/*
 Copyright 2008 - 2009 Sun Microsystems, Inc.

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

#include "../unit_fixture.h"
#include <vector>

/**
 * Example of a collection of tests
 *
 */

namespace testsuite
{
namespace classes
{

class connectionmetadata : public unit_fixture
{
private:
  typedef unit_fixture super;
  /* Helper function to check a FK definition */
  void checkForeignKey(Connection &con, ResultSet &res);

protected:

public:

  EXAMPLE_TEST_FIXTURE(connectionmetadata)
  {    
    TEST_CASE(getSchemata);
    TEST_CASE(getAttributes);
    TEST_CASE(getBestRowIdentifier);
    TEST_CASE(getCatalogs);
    TEST_CASE(getCatalogSeparator);
    TEST_CASE(getCatalogTerm);
    TEST_CASE(getCrossReference);
    TEST_CASE(getColumnPrivileges);
    TEST_CASE(getColumns);
    TEST_CASE(getConnection);
    TEST_CASE(getDatabaseVersions);
    TEST_CASE(getDriverVersions);
    TEST_CASE(getDefaultTransactionIsolation);
    TEST_CASE(getExtraNameCharacters);
    TEST_CASE(getExportedKeys);
    TEST_CASE(getIdentifierQuoteString);
    TEST_CASE(getImportedKeys);
    TEST_CASE(getIndexInfo);
    TEST_CASE(getLimitsAndStuff);
    TEST_CASE(getPrimaryKeys);
    TEST_CASE(getProcedures);
    TEST_CASE(getProcedureColumns);
    TEST_CASE(getProcedureTerm);
    TEST_CASE(getResultSetHoldability);
    TEST_CASE(getSchemaTerm);
    TEST_CASE(getSearchStringEscape);
    TEST_CASE(getSQLKeywords);
    TEST_CASE(getSuperTables);
    TEST_CASE(getSuperTypes);     
    TEST_CASE(classAttributes);
  }

  /**
   * Test for DatabaseMetaData:getSchemata()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getSchemata();

  /**
   * Test for DatabaseMetaData:getAttributes()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getAttributes();

  /**
   * Test for DatabaseMetaData:getBestRowIdentifier()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getBestRowIdentifier();

  /**
   * Test for DatabaseMetaData:getCatalogs()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getCatalogs();

  /**
   * Test for DatabaseMetaData:getCatalogSeparator()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getCatalogSeparator();

  /**
   * Test for DatabaseMetaData:getCatalogTerm()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getCatalogTerm();

  /**
   * Test for DatabaseMetaData:getCrossReference()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getCrossReference();

  /**
   * Test for DatabaseMetaData:getColumns()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getColumns();

  /**
   * Test for DatabaseMetaData:getColumnPrivileges()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getColumnPrivileges();

  /**
   * Test for DatabaseMetaData:getConnection
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getConnection();

  /**
   * Test for DatabaseMetaData:getDatabaseMajorVersion, *MinorVersion,  *PatchVersion()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getDatabaseVersions();

  /**
   * Test for DatabaseMetaData:getDriverMajorVersion, *MinorVersion,  *PatchVersion()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getDriverVersions();

  /**
   * Test for DatabaseMetaData:getDefaultTransactionIsolation()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getDefaultTransactionIsolation();

  /**
   * Test for DatabaseMetaData:getExtraNameCharacters()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getExtraNameCharacters();

  /**
   * Test for DatabaseMetaData:getExportedKeys()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getExportedKeys();

  /**
   * Test for DatabaseMetaData:getIdentifierQuoteString()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getIdentifierQuoteString();

  /**
   * Test for DatabaseMetaData:getImportedKeys()
   *
   * Create two tables parent and child and check if the method detects the FK properly.
   */
  void getImportedKeys();

  /**
   * Test for DatabaseMetaData:getIndexInfo()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getIndexInfo();

  /**
   * Test for DatabaseMetaData:getLimitsAndStuff()
   *
   * Test for assorted get*Something() methods
   */
  void getLimitsAndStuff();

  /**
   * Test for DatabaseMetaData:getPrimaryKeys()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getPrimaryKeys();

  /**
   * Test for DatabaseMetaData:getProcedures()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getProcedures();

  /**
   * Test for DatabaseMetaData:getProcedureColumns()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getProcedureColumns();

  /**
   * Test for DatabaseMetaData:getProcedureTerm()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getProcedureTerm();

  /**
   * Test for DatabaseMetaData:getResultSetHoldability()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getResultSetHoldability();

  /**
   * Test for DatabaseMetaData::getSchemaTerm()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getSchemaTerm();

  /**
   * Test for DatabaseMetaData::getSearchStringEscape()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getSearchStringEscape();

  /**
   * Test for DatabaseMetaData::getSQLKeywords()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getSQLKeywords();

  /**
   * Test for DatabaseMetaData::getSuperTables()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getSuperTables();

  /**
   * Test for DatabaseMetaData::getSuperTypes()
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void getSuperTypes();

  /**
   * Test of DatabaseMetaData class attributes
   *
   * Focus on code coverage: invalid parameter, invalid resultset
   * JDBC compliance tests should take care that it does what its supposed to do
   */
  void classAttributes();

};

REGISTER_FIXTURE(connectionmetadata);
} /* namespace classes */
} /* namespace testsuite */

