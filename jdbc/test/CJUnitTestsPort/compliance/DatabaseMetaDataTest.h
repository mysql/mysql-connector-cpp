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



#include "../BaseTestFixture.h"

#include "cppconn/connection.h"
#include "cppconn/metadata.h"
#include "cppconn/resultset.h"
#include "cppconn/resultset_metadata.h"
#include "cppconn/exception.h"

//#include javax.sql.DataSource


/**
 * @author mmatthew
 *
 * To change this generated comment edit the template variable "typecomment":
 * Window>Preferences>Java>Templates.
 * To enable and disable the creation of type comments go to
 * Window>Preferences>Java>Code Generation.
 */

namespace testsuite
{
namespace compliance
{

class DatabaseMetaDataTest : public BaseTestFixture
{
private:
  typedef BaseTestFixture super;
  DatabaseMetaData dbmd;
  //DataSource ds1;
  String dbName;
  String dbUser;
  String drManager;
  String sCatalogName;
  String sFtable;
  String sPtable;
  String sSchemaName;

  /* A method to compare the Column Names & No of Columns Specific to the test */

  /* throws sql::DbcException */

  bool columnCompare(List & sColumnNames, ResultSet & rs);
protected:

  /**
   * @see junit.framework.TestCase#setUp()
   */

  /* throws std::exception * */

  void setUp();
public:

  TEST_FIXTURE(DatabaseMetaDataTest)
  {
    TEST_CASE(testAllProceduresAreCallable);
    TEST_CASE(testAllTablesAreSelectable);
    TEST_CASE(testDataDefinitionCausesTransactionCommit);
    TEST_CASE(testDataDefinitionIgnoredInTransactions);
    TEST_CASE(testDeletesAreDetected1);
    TEST_CASE(testDeletesAreDetected2);
    TEST_CASE(testDeletesAreDetected3);
    TEST_CASE(testDoesMaxRowSizeIncludeBlobs);
    TEST_CASE(testGetBestRowIdentifier1);
    TEST_CASE(testGetBestRowIdentifier2);
    TEST_CASE(testGetBestRowIdentifier3);
    TEST_CASE(testGetBestRowIdentifier4);
    TEST_CASE(testGetBestRowIdentifier5);
    TEST_CASE(testGetBestRowIdentifier6);
    TEST_CASE(testGetBestRowIdentifier7);
    TEST_CASE(testGetCatalogSeparator);
    TEST_CASE(testGetCatalogTerm);
    TEST_CASE(testGetCatalogs);
    TEST_CASE(testGetColumnPrivileges);
    TEST_CASE(testGetColumns);
    TEST_CASE(testGetCrossReference);
    TEST_CASE(testGetDatabaseProductName);
    TEST_CASE(testGetDatabaseProductVersion);
    TEST_CASE(testGetDefaultTransactionIsolation);
    TEST_CASE(testGetDriverMajorVersion);
    TEST_CASE(testGetDriverMinorVersion);
    TEST_CASE(testGetDriverName);
    TEST_CASE(testGetDriverVersion);
    TEST_CASE(testGetExportedKeys);
    TEST_CASE(testGetExtraNameCharacters);
    TEST_CASE(testGetIdentifierQuoteString);
    TEST_CASE(testGetImportedKeys);
    TEST_CASE(testGetIndexInfo1);
    TEST_CASE(testGetIndexInfo2);
    TEST_CASE(testGetIndexInfo3);
    TEST_CASE(testGetIndexInfo4);
    TEST_CASE(testGetIndexInfo5);
    TEST_CASE(testGetMaxBinaryLiteralLength);
    TEST_CASE(testGetMaxCatalogNameLength);
    TEST_CASE(testGetMaxCharLiteralLength);
    TEST_CASE(testGetMaxColumnNameLength);
    TEST_CASE(testGetMaxColumnsInGroupBy);
    TEST_CASE(testGetMaxColumnsInIndex);
    TEST_CASE(testGetMaxColumnsInOrderBy);
    TEST_CASE(testGetMaxColumnsInSelect);
    TEST_CASE(testGetMaxColumnsInTable);
    TEST_CASE(testGetMaxConnections);
    TEST_CASE(testGetMaxCursorNameLength);
    TEST_CASE(testGetMaxIndexLength);
    TEST_CASE(testGetMaxProcedureNameLength);
    TEST_CASE(testGetMaxRowSize);
    TEST_CASE(testGetMaxSchemaNameLength);
    TEST_CASE(testGetMaxStatementLength);
    TEST_CASE(testGetMaxStatements);
    TEST_CASE(testGetMaxTableNameLength);
    TEST_CASE(testGetMaxTablesInSelect);
    TEST_CASE(testGetMaxUserNameLength);
    TEST_CASE(testGetNumericFunctions);
    TEST_CASE(testGetPrimaryKeys);

    TEST_CASE(testGetProcedureTerm);
    TEST_CASE(testGetProcedures);
    TEST_CASE(testGetSQLKeywords);
    TEST_CASE(testGetSchemaTerm);
    TEST_CASE(testGetSchemas);
    TEST_CASE(testGetSearchStringEscape);
    TEST_CASE(testGetStringFunctions);
    TEST_CASE(testGetSystemFunctions);
    TEST_CASE(testGetTablePrivileges);
    TEST_CASE(testGetTableTypes);
    TEST_CASE(testGetTables);
    TEST_CASE(testGetTimeDateFunctions);
    TEST_CASE(testGetTypeInfo);
    TEST_CASE(testGetUDTs);
    TEST_CASE(testGetUDTs01);

    TEST_CASE(testGetUserName);
    TEST_CASE(testGetVersionColumns);
    TEST_CASE(testInsertsAreDetected1);
    TEST_CASE(testInsertsAreDetected2);
    TEST_CASE(testInsertsAreDetected3);
    TEST_CASE(testIsCatalogAtStart);
    TEST_CASE(testIsReadOnly);
    TEST_CASE(testNullPlusNonNullIsNull);
    TEST_CASE(testNullsAreSortedAtEnd);
    TEST_CASE(testNullsAreSortedAtStart);
    TEST_CASE(testNullsAreSortedHigh);
    TEST_CASE(testNullsAreSortedLow);
    TEST_CASE(testOthersDeletesAreVisible1);
    TEST_CASE(testOthersDeletesAreVisible2);
    TEST_CASE(testOthersDeletesAreVisible3);
    TEST_CASE(testOthersInsertsAreVisible1);
    TEST_CASE(testOthersInsertsAreVisible2);
    TEST_CASE(testOthersInsertsAreVisible3);
    TEST_CASE(testOthersUpdatesAreVisible1);
    TEST_CASE(testOthersUpdatesAreVisible2);
    TEST_CASE(testOthersUpdatesAreVisible3);
    TEST_CASE(testOwnDeletesAreVisible1);
    TEST_CASE(testOwnDeletesAreVisible2);
    TEST_CASE(testOwnDeletesAreVisible3);
    TEST_CASE(testOwnInsertsAreVisible1);
    TEST_CASE(testOwnInsertsAreVisible2);
    TEST_CASE(testOwnInsertsAreVisible3);
    TEST_CASE(testOwnUpdatesAreVisible1);
    TEST_CASE(testOwnUpdatesAreVisible2);
    TEST_CASE(testOwnUpdatesAreVisible3);
    TEST_CASE(testStoresLowerCaseIdentifiers);
    TEST_CASE(testStoresLowerCaseQuotedIdentifiers);
    TEST_CASE(testStoresMixedCaseIdentifiers);
    TEST_CASE(testStoresMixedCaseQuotedIdentifiers);
    TEST_CASE(testStoresUpperCaseIdentifiers);
    TEST_CASE(testStoresUpperCaseQuotedIdentifiers);
    TEST_CASE(testSupportsANSI92EntryLevelSQL);
    TEST_CASE(testSupportsANSI92FullSQL);
    TEST_CASE(testSupportsANSI92IntermediateSQL);
    TEST_CASE(testSupportsAlterTableWithAddColumn);
    TEST_CASE(testSupportsAlterTableWithDropColumn);
    TEST_CASE(testSupportsBatchUpdates);
    TEST_CASE(testSupportsCatalogsInDataManipulation);
    TEST_CASE(testSupportsCatalogsInIndexDefinitions);
    TEST_CASE(testSupportsCatalogsInPrivilegeDefinitions);
    TEST_CASE(testSupportsCatalogsInProcedureCalls);
    TEST_CASE(testSupportsCatalogsInTableDefinitions);
    TEST_CASE(testSupportsColumnAliasing);
    TEST_CASE(testSupportsConvert);
    TEST_CASE(testSupportsConvert01);
    TEST_CASE(testSupportsConvert02);
    TEST_CASE(testSupportsConvert03);
    TEST_CASE(testSupportsConvert04);
    TEST_CASE(testSupportsConvert05);
    TEST_CASE(testSupportsConvert06);
    TEST_CASE(testSupportsConvert07);
    TEST_CASE(testSupportsConvert08);
    TEST_CASE(testSupportsConvert09);
    TEST_CASE(testSupportsConvert10);
    TEST_CASE(testSupportsConvert11);
    TEST_CASE(testSupportsConvert12);
    TEST_CASE(testSupportsConvert13);
    //      TEST_CASE( testSupportsConvert14 );
    TEST_CASE(testSupportsConvert15);
    TEST_CASE(testSupportsConvert16);
    TEST_CASE(testSupportsConvert17);
    TEST_CASE(testSupportsConvert18);
    TEST_CASE(testSupportsConvert19);
    TEST_CASE(testSupportsConvert20);
    TEST_CASE(testSupportsConvert21);
    TEST_CASE(testSupportsConvert22);
    TEST_CASE(testSupportsConvert23);
    TEST_CASE(testSupportsConvert24);
    TEST_CASE(testSupportsConvert25);
    TEST_CASE(testSupportsConvert26);
    TEST_CASE(testSupportsConvert27);
    TEST_CASE(testSupportsConvert28);
    TEST_CASE(testSupportsConvert29);
    TEST_CASE(testSupportsConvert30);
    TEST_CASE(testSupportsConvert31);
    TEST_CASE(testSupportsConvert32);
    TEST_CASE(testSupportsConvert33);
    TEST_CASE(testSupportsConvert34);
    TEST_CASE(testSupportsConvert35);
    TEST_CASE(testSupportsConvert36);
    TEST_CASE(testSupportsConvert37);
    TEST_CASE(testSupportsCoreSQLGrammar);
    TEST_CASE(testSupportsCorrelatedSubqueries);
    TEST_CASE(testSupportsDataDefinitionAndDataManipulationTransactions);
    TEST_CASE(testSupportsDataManipulationTransactionsOnly);
    TEST_CASE(testSupportsDifferentTableCorrelationNames);
    TEST_CASE(testSupportsExpressionsInOrderBy);
    TEST_CASE(testSupportsExtendedSQLGrammar);
    TEST_CASE(testSupportsFullOuterJoins);
    TEST_CASE(testSupportsGroupBy);
    TEST_CASE(testSupportsGroupByBeyondSelect);
    TEST_CASE(testSupportsGroupByUnrelated);

    TEST_CASE(testSupportsLikeEscapeClause);
    TEST_CASE(testSupportsLimitedOuterJoins);
    TEST_CASE(testSupportsMinimumSQLGrammar);
    TEST_CASE(testSupportsMixedCaseIdentifiers);
    TEST_CASE(testSupportsMixedCaseQuotedIdentifiers);
    TEST_CASE(testSupportsMultipleResultSets);
    TEST_CASE(testSupportsMultipleTransactions);
    TEST_CASE(testSupportsNonNullableColumns);
    TEST_CASE(testSupportsOpenCursorsAcrossCommit);
    TEST_CASE(testSupportsOpenCursorsAcrossRollback);
    TEST_CASE(testSupportsOpenStatementsAcrossCommit);
    TEST_CASE(testSupportsOpenStatementsAcrossRollback);
    TEST_CASE(testSupportsOrderByUnrelated);
    TEST_CASE(testSupportsOuterJoins);
    TEST_CASE(testSupportsPositionedDelete);
    TEST_CASE(testSupportsPositionedUpdate);

    TEST_CASE(testSupportsResultSetType1);
    TEST_CASE(testSupportsResultSetType2);
    TEST_CASE(testSupportsResultSetType3);
    TEST_CASE(testSupportsSchemasInDataManipulation);
    TEST_CASE(testSupportsSchemasInIndexDefinitions);
    TEST_CASE(testSupportsSchemasInPrivilegeDefinitions);
    TEST_CASE(testSupportsSchemasInProcedureCalls);
    TEST_CASE(testSupportsSchemasInTableDefinitions);
    TEST_CASE(testSupportsSelectForUpdate);
    TEST_CASE(testSupportsStoredProcedures);
    TEST_CASE(testSupportsSubqueriesInComparisons);
    TEST_CASE(testSupportsSubqueriesInExists);
    TEST_CASE(testSupportsSubqueriesInIns);
    TEST_CASE(testSupportsSubqueriesInQuantifieds);
    TEST_CASE(testSupportsTableCorrelationNames);
    TEST_CASE(testSupportsTransactionIsolationLevel1);
    TEST_CASE(testSupportsTransactionIsolationLevel2);
    TEST_CASE(testSupportsTransactionIsolationLevel3);
    TEST_CASE(testSupportsTransactionIsolationLevel4);
    TEST_CASE(testSupportsTransactionIsolationLevel5);
    TEST_CASE(testSupportsTransactions);
    TEST_CASE(testSupportsUnion);
    TEST_CASE(testSupportsUnionAll);
    TEST_CASE(testUpdatesAreDetected1);
    TEST_CASE(testUpdatesAreDetected2);
    TEST_CASE(testUpdatesAreDetected3);

    TEST_CASE(testUsesLocalFilePerTable);

#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
    TEST_CASE(testGetProcedureColumns);
    TEST_CASE(testGetURL);
    TEST_CASE(testSupportsIntegrityEnhancementFacility);
    TEST_CASE(testSupportsResultSetConcurrency1);
    TEST_CASE(testSupportsResultSetConcurrency2);
    TEST_CASE(testSupportsResultSetConcurrency3);
    TEST_CASE(testSupportsResultSetConcurrency4);
    TEST_CASE(testSupportsResultSetConcurrency5);
    TEST_CASE(testSupportsResultSetConcurrency6);
#endif

    dbmd= NULL;

    dbName="";
    dbUser="";
    drManager="";
    sCatalogName="";
    sFtable="";
    sPtable="";
    //                                         ds1               (NULL),
    sSchemaName="";
  }


  /*
   * @testName:         testAllProceduresAreCallable
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The allProceduresAreCallable method must return a boolean value;
   *                    true if the user has the security rights of calling all the
   *                    procedures returned by the method getProcedures and false
   *                    otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the allProceduresAreCallable() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testAllProceduresAreCallable();
  /*
   * @testName:         testAllTablesAreSelectable
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave
   *                    as specified in the JDBC 1.0 and 2.0 specifications. None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The allTablesAreSelectable method must return a boolean value;
   *                    true if the user can use a SELECT statement with all the
   *                    tables returned by the method getTables and false
   *                    otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the allTablesAreSelectable() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testAllTablesAreSelectable();
  /*
   * @testName:         testDataDefinitionCausesTransactionCommit
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The dataDefinitionCausesTransactionCommit() method must return
   *                    a boolean value; true if the data definition statement within a
   *                    transaction is forced the transaction to commit and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the dataDefinitionCausesTransactionCommit() method.
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testDataDefinitionCausesTransactionCommit();
  /*
   * @testName:         testDataDefinitionIgnoredInTransactions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The dataDefinitionIgnoredInTransactions() method must return a
   *                    boolean value; true if a data definition statement within a
   *                    transaction is ignored and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the dataDefinitionIgnoredInTransactions() method.
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testDataDefinitionIgnoredInTransactions();
  /*
   * @testName:         testDeletesAreDetected1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The deletesAreDetected(int resType) method must return a
   *                    boolean value; true if changes are detected by the resultset type
   *                    and false otherwise. The possible values for
   *                    resType are TYPE_FORWARD_ONLY, TYPE_SCROLL_SENSITIVE and
   *                    TYPE_SCROLL_INSENSITIVE.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the deletesAreDetected() method on that object with the
   *                    result set type as ResultSet.TYPE_FORWARD_ONLY.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testDeletesAreDetected1();
  /*
   * @testName:         testDeletesAreDetected2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The deletesAreDetected(int resType) method must return a
   *                    boolean value; true if changes are detected by the resultset type
   *                    and false otherwise. The possible values for
   *                    resType are TYPE_FORWARD_ONLY, TYPE_SCROLL_SENSITIVE and
   *                    TYPE_SCROLL_INSENSITIVE.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the deletesAreDetected() method on that object with the
   *                    result set type as ResultSet.TYPE_SCROLL_INSENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testDeletesAreDetected2();
  /*
   * @testName:         testDeletesAreDetected3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The deletesAreDetected(int resType) method must return a
   *                    boolean value; true if changes are detected by the resultset type
   *                    and false otherwise. The possible values for
   *                    resType are TYPE_FORWARD_ONLY, TYPE_SCROLL_SENSITIVE and
   *                    TYPE_SCROLL_INSENSITIVE.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the deletesAreDetected() method on that object with the
   *                    result set type as ResultSet.TYPE_SCROLL_SENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testDeletesAreDetected3();
  /*
   * @testName:         testDoesMaxRowSizeIncludeBlobs
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The doesMaxRowSizeIncludeBlobs() method must return a boolean
   *                    value; true if maximum row size includes LONGVARCHAR and
   *                    LONGVARBINARY blobs and false otherwise. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the doesMaxRowSizeIncludeBlobs() method.
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testDoesMaxRowSizeIncludeBlobs();
  /*
   * @testName:         testGetBestRowIdentifier1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getBestRowIdentifier(Str cat, Str sch, Str tab,int scope, boo nflg)
   *                    method must return a ResultSet object with each row is a
   *                    description of a column that belongs to a optimal set of columns
   *                    that uniquely identifies a row.  The possible values for scope
   *                    are bestRowTemporary, bestRowTransaction or bestRowSession
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getBestRowIdentifier() method on that object.
   *                    It should return a ResultSet object.
   *
   */

  /* throws std::exception * */

  void testGetBestRowIdentifier1();
  /*
   * @testName:         testGetBestRowIdentifier2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getBestRowIdentifier(Str cat, Str sch, Str tab,int scope, boo nflg)
   *                    method must return a ResultSet object with each row is a
   *                    description of a column that belongs to a optimal set of columns
   *                    that uniquely identifies a row.  The possible values for scope
   *                    are bestRowTemporary, bestRowTransaction or bestRowSession
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getBestRowIdentifier() method on that object.
   *                    It should return a ResultSet object.
   *
   */

  /* throws std::exception * */

  void testGetBestRowIdentifier2();
  /*
   * @testName:         testGetBestRowIdentifier3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getBestRowIdentifier(Str cat, Str sch, Str tab,int scope, boo nflg)
   *                    method must return a ResultSet object with each row is a
   *                    description of a column that belongs to a optimal set of columns
   *                    that uniquely identifies a row.  The possible values for scope
   *                    are bestRowTemporary, bestRowTransaction or bestRowSession
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getBestRowIdentifier() method on that object.
   *                    It should return a ResultSet object.
   *
   */

  /* throws std::exception * */

  void testGetBestRowIdentifier3();
  /*
   * @testName:         testGetBestRowIdentifier4
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getBestRowIdentifier(Str cat, Str sch, Str tab,int scope, boo nflg)
   *                    method must return a ResultSet object with each row is a
   *                    description of a column that belongs to a optimal set of columns
   *                    that uniquely identifies a row.  The possible values for scope
   *                    are bestRowTemporary, bestRowTransaction or bestRowSession
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getBestRowIdentifier() method on that object.
   *                    It should return a ResultSet object.
   *
   */

  /* throws std::exception * */

  void testGetBestRowIdentifier4();
  /*
   * @testName:         testGetBestRowIdentifier5
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getBestRowIdentifier(Str cat, Str sch, Str tab,int scope, boo nflg)
   *                    method must return a ResultSet object with each row is a
   *                    description of a column that belongs to a optimal set of columns
   *                    that uniquely identifies a row.  The possible values for scope
   *                    are bestRowTemporary, bestRowTransaction or bestRowSession
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getBestRowIdentifier() method on that object.
   *                    It should return a ResultSet object.
   *
   */

  /* throws std::exception * */

  void testGetBestRowIdentifier5();
  /*
   * @testName:         testGetBestRowIdentifier6
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getBestRowIdentifier(Str cat, Str sch, Str tab,int scope, boo nflg)
   *                    method must return a ResultSet object with each row is a
   *                    description of a column that belongs to a optimal set of columns
   *                    that uniquely identifies a row.  The possible values for scope
   *                    are bestRowTemporary, bestRowTransaction or bestRowSession
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getBestRowIdentifier() method on that object.
   *                    It should return a ResultSet object
   *
   */

  /* throws std::exception * */

  void testGetBestRowIdentifier6();
  /*
   * @testName:         testGetBestRowIdentifier7
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getBestRowIdentifier(Str cat, Str sch, Str tab,int scope, boo nflg)
   *                    method must return a ResultSet object with each row is a
   *                    description of a column that belongs to a optimal set of columns
   *                    that uniquely identifies a row.  The possible values for scope
   *                    are bestRowTemporary, bestRowTransaction or bestRowSession
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getBestRowIdentifier() method on that object.
   *                    It should return a ResultSet object.
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetBestRowIdentifier7();
  /*
   * @testName:         testGetCatalogSeparator
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getCatalogSeparator() method must return a String value
   *                    representing the separator string. (See JDK 1.2.2 API
   *                    documentation)
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase.
   *                    Make a call to DatabaseMetadata.getCatalogSeparator() on that object.
   *                    It should return a String and NULL if it is not supported.
   *
   */

  /* throws std::exception * */

  void testGetCatalogSeparator();
  /*
   * @testName:         testGetCatalogTerm
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getCatalogTerm() method must return a String object containing
   *                    the vendor term for "catalog". (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getCatalogTerm() method on that object.
   *                    It should return a String and NULL if it cannot be returned.
   *
   */

  /* throws std::exception * */

  void testGetCatalogTerm();
  /*
   * @testName:         testGetCatalogs
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getCatalogs() method must return a ResultSet object with each
   *                    row representing a catalog name available in the database.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getCatalogs() method.
   *                    It should return a ResultSet object
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetCatalogs();
  /*
   * @testName:         testGetColumnPrivileges
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    getColumnPrivileges(String ctlg,String sch,String tab,String colpat)
   *                    method must return a ResultSet object with each row is a
   *                    description of a column's privileges. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getColumnPrivileges() method on that object.
   *                    It should return a ResultSet object.
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetColumnPrivileges();
  /*
   * @testName:         testGetColumns
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getColumns(String clg,String schp,String tnpat,String colNamepat)
   *                    method must return a ResultSet object with each row is a
   *                    description of a table column. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getColumns() method.
   *                    It should return a ResultSet object
   *                          Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetColumns();
  /*
   * @testName:         testGetCrossReference
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getCrossReference(Str prcalg,Str prsch,Str prtab,Str foclg,Str fosch,Str fotab)
   *                    method must return a ResultSet object with each row is a
   *                    description of a foreign key column in a foreign key table.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getCrossReference() method on that object.
   *                    It should return a ResultSet object.
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetCrossReference();
  /*
   * @testName:         testGetDatabaseProductName
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The getDatabaseProductName method must return a String object
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getDatabaseProductName() method
   *                    It should return a String
   *
   */

  /* throws std::exception * */

  void testGetDatabaseProductName();
  /*
   * @testName:         testGetDatabaseProductVersion
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The getDatabaseProductVersion method must return a String object
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getDatabaseProductVersion() method
   *                    It should return a String
   *
   */

  /* throws std::exception * */

  void testGetDatabaseProductVersion();
  /*
   * @testName:         testGetDefaultTransactionIsolation
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getDefaultTransactionIsolation() method must return an integer
   *                    value; the value representing the default Transaction Isolation.
   *                    The value can be any one of the following TRANSACTION_NONE,
   *                    TRANSACTION_READ_UNCOMMITTED, TRANSACTION_READ_COMMITTED,
   *                    TRANSACTION_REPEATABLE_READ and TRANSACTION_SERIALIZABLE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getDefaultTransactionIsolation() method.
   *                    It should return an integer value
   *
   */

  /* throws std::exception * */

  void testGetDefaultTransactionIsolation();
  /*
   * @testName:         testGetDriverMajorVersion
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The getDriverMajorVersion method must return a integer value
   *                    representing the driver major version. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getDriverMajorVersion() method
   *                    It should return a Integer value
   *
   */

  /* throws std::exception * */

  void testGetDriverMajorVersion();
  /*
   * @testName:         testGetDriverMinorVersion
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The getDriverMinorVersion method must return a integer value
   *                    representing the driver major version. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getDriverMinorVersion() method
   *                    It should return a Integer value
   *
   */

  /* throws std::exception * */

  void testGetDriverMinorVersion();
  /*
   * @testName:         testGetDriverName
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The getDriverName method must return a String object
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getDriverName() method
   *                    It should return a String
   *
   */
  /* throws std::exception * */

  void testGetDriverName();


  /*
   * @testName:         testGetDriverVersion
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The getDriverVersion method must return a String object
   *                    representing the driver version including its major and minor
   *                    version numbers. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getDriverVersion() method
   *                    It should return a String
   *
   */
  /* throws std::exception * */

  void testGetDriverVersion();


  /*
   * @testName:         testGetExportedKeys
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getExportedKeys(String calg, String sch, String tab)
   *                    method must return a ResultSet object with each row is a
   *                    description of a foreign key that reference a table's primary
   *                    key columns. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getExportedKeys() method on that object.
   *                    It should return a ResultSet object.
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetExportedKeys();
  /*
   * @testName:         testGetExtraNameCharacters
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getExtraNameCharacters() method must return a String object
   *                    containing the extra characters; (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getExtraNameCharacters() method
   *                    It should return a String
   *
   */

  /* throws std::exception * */

  void testGetExtraNameCharacters();
  /*
   * @testName:         testGetIdentifierQuoteString
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getIdentifierQuoteString() method must return a String
   *                    object representing the quoting string or a space if the database
   *                    does not support quoting identifiers.  A JDBC Compliant driver
   *                    always uses a double quote character ("). (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getIdentifierQuoteString() method
   *                    It should return a String
   *
   */

  /* throws std::exception * */

  void testGetIdentifierQuoteString();
  /*
   * @testName:         testGetImportedKeys
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getImportedKeys(String calg, String sch, String tab)
   *                    method must return a ResultSet object with each row is a
   *                    description of a primary key columns that are referenced by a
   *                    table's foreign key columns. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getImportedKeys() method on that object.
   *                    It should return a ResultSet object.
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetImportedKeys();
  /*
   * @testName:         testGetIndexInfo1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getIndexInfo(Str calg,Str sch,Str tab,boolean uni,boolean approx)
   *                    method must return a ResultSet object with each row is a
   *                    description of an index column. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getIndexInfo() method on that object.
   *                    It should return a ResultSet object.
   *
   */

  /* throws std::exception * */

  void testGetIndexInfo1();
  /*
   * @testName:         testGetIndexInfo2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getIndexInfo(Str calg,Str sch,Str tab,boolean uni,boolean approx)
   *                    method must return a ResultSet object with each row is a
   *                    description of an index column. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getIndexInfo() method on that object.
   *                    It should return a ResultSet object.
   *
   */

  /* throws std::exception * */

  void testGetIndexInfo2();
  /*
   * @testName:         testGetIndexInfo3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getIndexInfo(Str calg,Str sch,Str tab,boolean uni,boolean approx)
   *                    method must return a ResultSet object with each row is a
   *                    description of an index column. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getIndexInfo() method on that object.
   *                    It should return a ResultSet object.
   *
   */

  /* throws std::exception * */

  void testGetIndexInfo3();
  /*
   * @testName:         testGetIndexInfo4
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getIndexInfo(Str calg,Str sch,Str tab,boolean uni,boolean approx)
   *                    method must return a ResultSet object with each row is a
   *                    description of an index column. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getIndexInfo() method on that object.
   *                    It should return a ResultSet object.
   *
   */

  /* throws std::exception * */

  void testGetIndexInfo4();
  /*
   * @testName:         testGetIndexInfo5
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getIndexInfo(Str calg,Str sch,Str tab,boolean uni,boolean approx)
   *                    method must return a ResultSet object with each row is a
   *                    description of an index column. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getIndexInfo() method on that object.
   *                    It should return a ResultSet object.
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetIndexInfo5();
  /*
   * @testName:         testGetMaxBinaryLiteralLength
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxBinaryLiteralLength() method must return a integer
   *                    value; the value representing the maximum number of hex
   *                    characters; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxBinaryLiteralLength() method on that object.
   *                    It should return an integer value
   *
   */

  /* throws std::exception * */

  void testGetMaxBinaryLiteralLength();
  /*
   * @testName:         testGetMaxCatalogNameLength
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxCatalogNameLength() method must return an integer
   *                    value; the value representing the maximum catalog name length
   *                    that is allowed; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxCatalogNameLength() method on that object.
   *                    It should return an integer value.
   *
   */

  /* throws std::exception * */

  void testGetMaxCatalogNameLength();
  /*
   * @testName:         testGetMaxCharLiteralLength
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxCharLiteralLength() method must return a integer
   *                    value; the value representing the maximum number of
   *                    characters; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxCharLiteralLength() method on that object.
   *                    It should return an integer value.
   *
   */

  /* throws std::exception * */

  void testGetMaxCharLiteralLength();
  /*
   * @testName:         testGetMaxColumnNameLength
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxColumnNameLength() method must return a integer
   *                    value; the value representing the maximum number of
   *                    characters allowed as column name length; 0 if there is no limit
   *                    or the limit is unknown. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxColumnNameLength() method on that object.
   *                    It should return an integer value
   *
   */

  /* throws std::exception * */

  void testGetMaxColumnNameLength();
  /*
   * @testName:         testGetMaxColumnsInGroupBy
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxColumnsInGroupBy() method must return a integer
   *                    value; the value representing the maximum number of
   *                    columns in a GROUP BY clause; 0 if there is no limit or
   *                    the limit is unknown. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxColumnsInGroupBy() method on that object.
   *                    It should return an integer value.
   *
   */

  /* throws std::exception * */

  void testGetMaxColumnsInGroupBy();
  /*
   * @testName:         testGetMaxColumnsInIndex
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxColumnsInIndex() method must return a integer
   *                    value; the value representing the maximum number of
   *                    columns in Index; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxColumnsInIndex() method on that object.
   *                    It should return an integer value.
   *
   */

  /* throws std::exception * */

  void testGetMaxColumnsInIndex();
  /*
   * @testName:         testGetMaxColumnsInOrderBy
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxColumnsInOrderBy() method must return a integer
   *                    value; the value representing the maximum number of columns
   *                    in a ORDER BY clause; 0 if there is no limit or the limit is
   *                    unknown. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxColumnsInOrderBy() method on that object.
   *                    It should return an integer value.
   *
   */

  /* throws std::exception * */

  void testGetMaxColumnsInOrderBy();
  /*
   * @testName:         testGetMaxColumnsInSelect
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxColumnsInSelect() method must return a integer
   *                    value; the value representing the maximum number of columns in
   *                    a SELECT list; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxColumnsInSelect() method on that object.
   *                    It should return an integer value.
   *
   */

  /* throws std::exception * */

  void testGetMaxColumnsInSelect();
  /*
   * @testName:         testGetMaxColumnsInTable
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxColumnsInTable() method must return a integer
   *                    value; the value representing the maximum number of columns
   *                    in a Table; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxColumnsInTable() method on that object.
   *                    It should return an integer value.
   *
   */

  /* throws std::exception * */

  void testGetMaxColumnsInTable();
  /*
   * @testName:         testGetMaxConnections
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxConnections() method must return a integer
   *                    value; the value representing the maximum number of active
   *                    connections at a time in a database; 0 if there is no limit or
   *                    the limit is unknown. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxConnections() method on that object.
   *                    It should return an integer value
   *
   */

  /* throws std::exception * */

  void testGetMaxConnections();
  /*
   * @testName:         testGetMaxCursorNameLength
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxCursorNameLength() method must return a integer
   *                    value; the value representing the maximum cursor name length
   *                    in bytes; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxCursorNameLength() method on that object.
   *                    It should return an integer value
   *
   */

  /* throws std::exception * */

  void testGetMaxCursorNameLength();
  /*
   * @testName:         testGetMaxIndexLength
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxIndexLength() method must return a integer
   *                    value; the value representing the maximum index length in
   *                    bytes; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxIndexLength() method on that object.
   *                    It should return an integer value
   *
   */

  /* throws std::exception * */

  void testGetMaxIndexLength();
  /*
   * @testName:         testGetMaxProcedureNameLength
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxProcedureNameLength() method must return a integer
   *                    value; the value representing the maximum procedure name length
   *                    that is allowed; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxProcedureNameLength() method on that object.
   *                    It should return an integer value.
   *
   */

  /* throws std::exception * */

  void testGetMaxProcedureNameLength();
  /*
   * @testName:         testGetMaxRowSize
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxRowSize() method must return a integer
   *                    value; the value representing the maximum length of a single row
   *                    in bytes; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxRowSize() method on that object.
   *                    It should return an integer value
   *
   */

  /* throws std::exception * */

  void testGetMaxRowSize();
  /*
   * @testName:         testGetMaxSchemaNameLength
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxSchemaNameLength() method must return a integer
   *                    value; the value representing the maximum schema name length that
   *                    is allowed; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxSchemaNameLength() method on that object.
   *                    It should return an integer value.
   *
   */

  /* throws std::exception * */

  void testGetMaxSchemaNameLength();
  /*
   * @testName:         testGetMaxStatementLength
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxStatementLength() method must return an integer
   *                    value; the value representing the maximum number of characters
   *                    allowed in  SQL statement; 0 if there is no limit or the limit is
   *                    unknown. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxStatementLength() method.
   *                    It should return an integer value
   *
   */

  /* throws std::exception * */

  void testGetMaxStatementLength();
  /*
   * @testName:         testGetMaxStatements
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxStatements method must return an integer
   *                    value; the value representing the maximum number of statements
   *                    that can be open at one time to the database; 0 if there is no limit
   *                    or the limit is unknown. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxStatements() method.
   *                    It should return an integer value
   *
   */

  /* throws std::exception * */

  void testGetMaxStatements();
  /*
   * @testName:         testGetMaxTableNameLength
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxTableNameLength() method must return an integer
   *                    value; the value representing the maximum number of characters
   *                    allowed in a table name; 0 if there is no limit or the limit
   *                    is unknown. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxTableNameLength() method.
   *                    It should return an integer value
   *
   */

  /* throws std::exception * */

  void testGetMaxTableNameLength();
  /*
   * @testName:         testGetMaxTablesInSelect
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxTablesInSelect() method must return an integer
   *                    value; the value representing the maximum number of tables in
   *                    SELECT clause; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxTablesInSelect() method.
   *                    It should return an integer value
   *
   */

  /* throws std::exception * */

  void testGetMaxTablesInSelect();
  /*
   * @testName:         testGetMaxUserNameLength
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getMaxUserNameLength() method must return an integer
   *                    value; the value representing the maximum of characters allowed
   *                    in a user name; 0 if there is no limit or the limit is unknown.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getMaxUserNameLength() method.
   *                    It should return an integer value
   *
   */

  /* throws std::exception * */

  void testGetMaxUserNameLength();
  /*
   * @testName:         testGetNumericFunctions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getNumericFunctions() method must return a String object
   *                    that is a comma separated list of math functions; These are
   *                    the X/Open CLI math function names used in the JDBC function
   *                    escape clause. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the  getNumericFunctions() method
   *                    It should return a String
   *
   */

  /* throws std::exception * */

  void testGetNumericFunctions();
  /*
   * @testName:         testGetPrimaryKeys
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getPrimaryKeys(String calg, String sch, String tab)
   *                    method must return a ResultSet object with each row is a
   *                    description of the given table's primary key column(s).
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getPrimaryKeys() method on that object.
   *                    It should return a ResultSet object.
   *                      Validate the column names and column ordering.
   */
  /* throws std::exception * */
  void testGetPrimaryKeys();


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  /*
   * @testName:         testGetProcedureColumns
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getProcedureColumns(Str ctlg,Str schp,String prp,String colpa)
   *                    method must return a ResultSet object with each row describes
   *                    the information like parameter for the stored procedure,
   *                    return value of the stored procedure, etc.,
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getProcedureColumns() method.
   *                    It should return a ResultSet object
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */
  void testGetProcedureColumns();
#endif


  /*
   * @testName:         testGetProcedureTerm
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getProcedureTerm() method must return a String object
   *                    containing the vendor term for "procedure".  (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getProcedureTerm() method on that object.
   *                    It should return a String and NULL if it cannot be generated.;
   *
   */

  /* throws std::exception * */

  void testGetProcedureTerm();
  /*
   * @testName:         testGetProcedures
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getProcedures(String catg, String schemapat, String procpat)
   *                    method must return a ResultSet object where catg is the catalog
   *                    name, schemapat is schema pattern and procpat is procedure name
   *                    pattern.  Each row of the ResultSet is the description of the.
   *                    stored procedure. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getProcedures() method.
   *                    It should return a ResultSet object
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetProcedures();
  /*
   * @testName:         testGetSQLKeywords
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getSQLKeywords() method must return a String object that is
   *                    comma separated list of keywords used by the database that are
   *                    not also SQL-92 keywords.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the  getSQLKeywords() method
   *                    It should return a String
   *
   */

  /* throws std::exception * */

  void testGetSQLKeywords();
  /*
   * @testName:         testGetSchemaTerm
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getSchemaTerm() method must return a String object representing
   *                    the vendor term for "schema". (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getSchemaTerm() method on that object.
   *                    It should return a String and NULL if it cannot be generated.
   *
   */

  /* throws std::exception * */

  void testGetSchemaTerm();
  /*
   * @testName:         testGetSchemas
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getSchemas() method must return a ResultSet object, with each row
   *                    representing a schema name available in the database
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getSchemas() method.
   *                    It should return a ResultSet object
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetSchemas();
  /*
   * @testName:         testGetSearchStringEscape
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getSystemFunctions() method must return a String object
   *                    used to escape wildcard characters; (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getSearchStringEscape() method
   *                    It should return a String
   *
   */

  /* throws std::exception * */

  void testGetSearchStringEscape();
  /*
   * @testName:         testGetStringFunctions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getStringFunctions() method must return a String object
   *                    that is a comma separated list of string functions; These are
   *                    the X/Open CLI string function names used in the JDBC function
   *                    escape clause. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getStringFunctions() method
   *                    It should return a String
   *
   */

  /* throws std::exception * */

  void testGetStringFunctions();
  /*
   * @testName:         testGetSystemFunctions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getSystemFunctions() method must return a String object
   *                    that is a comma separated list of string functions; These are
   *                    the X/Open CLI system function names used in the JDBC function
   *                    escape clause. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getSystemFunctions() method
   *                    It should return a String
   *
   */

  /* throws std::exception * */

  void testGetSystemFunctions();
  /*
   * @testName:         testGetTablePrivileges
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    getTablePrivileges(String clg,String schpat,String tabnamepat)
   *                    method must return a ResultSet object with each row is a
   *                    description of the access rights for a table. (See JDK 1.2.2
   *                    API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getTablePrivileges() method on that object.
   *                    It should return a ResultSet object.
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetTablePrivileges();
  /*
   * @testName:         testGetTableTypes
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getTableTypes() method must return a ResultSet object with each
   *                    row representing a table type available in the DBMS.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getTableTypes() method.
   *                    It should return a ResultSet object
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetTableTypes();
  /*
   * @testName:         testGetTables
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getTables() method must return a ResultSet object
   *                    with each row is a description of the table
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getTables() method.
   *                    It should return a ResultSet object
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetTables();
  /*
   * @testName:         testGetTimeDateFunctions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getSystemFunctions method must return a String object
   *                    that is a comma separated list of time and date functions;
   *                    These are the X/Open CLI time and date function names used
   *                    in the JDBC function escape clause. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getTimeDateFunctions() method
   *                    It should return a String
   *
   */

  /* throws std::exception * */

  void testGetTimeDateFunctions();
  /*
   * @testName:         testGetTypeInfo
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getTypeInfo() method must return a ResultSet object with each
   *                    row is a description of a local DBMS type.  (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getTypeInfo() method on that object.
   *                    It should return a ResultSet object
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetTypeInfo();
  /*
   * @testName:         testGetUDTs
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getUDTs() method must return a ResultSet object with each
   *                    row is a description of a UDT (User Defined Type).
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getUDTs() method on that object.
   *                    It should return a ResultSet object.
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetUDTs();
  /*
   * @testName:         testGetUDTs01
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getUDTs() method must return a ResultSet object with each
   *                    row is a description of a UDT (User Defined Type).
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getUDTs() method on that object.
   *                    It should return a ResultSet object.
   *                      Validate the column names and column ordering.
   *
   */
  /* throws std::exception * */
  void testGetUDTs01();


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  /*
   * @testName:         testGetURL
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave
   *                    as specified in the JDBC 1.0 and 2.0 specifications. None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The getURL method must return a String object representing the
   *                    URL for the database; null if it cannot be generated.
   *                    (See JDK 1.2.2 API documentation).
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getURL() method
   *                    It should return a String or null if it cannot be generated
   *
   */
  /* throws std::exception * */
  void testGetURL();
#endif

  /*
   * @testName:         testGetUserName
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The getUserName method must return a String object representing
   *                    the Username of the database. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the getUserName() method
   *                    It should return a String
   *
   */

  /* throws std::exception * */

  void testGetUserName();
  /*
   * @testName:         testGetVersionColumns
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The getVersionColumns(String calg, String sch, String tab)
   *                    method must return a ResultSet object with each row is a
   *                    description of a table column that is automatically updated
   *                    whenever a value in a row is updated.  (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the getVersionColumns() method on that object.
   *                    It should return a ResultSet object.Compare the column names
   *                      Validate the column names and column ordering.
   */

  /* throws std::exception * */

  void testGetVersionColumns();
  /*
   * @testName:         testInsertsAreDetected1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The insertsAreDetected(int resType) method must return a
   *                    boolean value; true if the method ResultSet.rowInserted returns
   *                    true when called on a ResultSet object with a given type that
   *                    contains visible row insertion; false if the method
   *                    ResultSet.rowInserted returns false. The possible values for
   *                    resType are TYPE_FORWARD_ONLY, TYPE_SCROLL_SENSITIVE and
   *                    TYPE_SCROLL_INSENSITIVE.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the insertsAreDetected() method on that object with the
   *                    result set type as ResultSet.TYPE_FORWARD_ONLY.
   *                    It should return a boolean value; either or false.
   *
   */

  /* throws std::exception * */

  void testInsertsAreDetected1();
  /*
   * @testName:         testInsertsAreDetected2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The insertsAreDetected(int resType) method must return a
   *                    boolean value; true if the method ResultSet.rowInserted returns
   *                    true when called on a ResultSet object with a given type that
   *                    contains visible row insertion; false if the method
   *                    ResultSet.rowInserted returns false. The possible values for
   *                    resType are TYPE_FORWARD_ONLY, TYPE_SCROLL_SENSITIVE and
   *                    TYPE_SCROLL_INSENSITIVE.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the insertsAreDetected() method on that object with the
   *                    result set type as ResultSet.TYPE_SCROLL_INSENSITIVE.
   *                    It should return a boolean value; either or false.
   *
   */

  /* throws std::exception * */

  void testInsertsAreDetected2();
  /*
   * @testName:         testInsertsAreDetected3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The insertsAreDetected(int resType) method must return a
   *                    boolean value; true if the method ResultSet.rowInserted returns
   *                    true when called on a ResultSet object with a given type that
   *                    contains visible row insertion; false if the method
   *                    ResultSet.rowInserted returns false. The possible values for
   *                    resType are TYPE_FORWARD_ONLY, TYPE_SCROLL_SENSITIVE and
   *                    TYPE_SCROLL_INSENSITIVE.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the insertsAreDetected() method on that object with the
   *                    result set type as ResultSet.TYPE_SCROLL_SENSITIVE.
   *                    It should return a boolean value; either or false.
   *
   */

  /* throws std::exception * */

  void testInsertsAreDetected3();
  /*
   * @testName:         testIsCatalogAtStart
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The isCatalogAtStart() method must return a boolean value;
   *                    true if the catalog name appears at the start of a fully
   *                    qualified table name; false if it appears at the end.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the isCatalogAtStart() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testIsCatalogAtStart();
  /*
   * @testName:         testIsReadOnly
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The isReadOnly method must return a boolean value; true if
   *                    the database is in read-only mode and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the isReadOnly() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testIsReadOnly();
  /*
   * @testName:         testNullPlusNonNullIsNull
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The nullPlusNonNullIsNull() method must return a boolean value;
   *                    true if the concatenation of a NULL value and a non-NULL value
   *                    results in a NULL value and false otherwise.  A JDBC compliant
   *                    driver must always return true. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the nullPlusNonNullIsNull() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testNullPlusNonNullIsNull();
  /*
   * @testName:         testNullsAreSortedAtEnd
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The nullsAreSortedAtEnd method must return a boolean value;
   *                    true if NULL values are sorted at the end regardless of the
   *                    sort order and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the nullsAreSortedAtEnd() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testNullsAreSortedAtEnd();
  /*
   * @testName:         testNullsAreSortedAtStart
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The nullsAreSortedAtStart method must return a boolean value;
   *                    true if NULL values are sorted at the start regardless of the
   *                    sort order and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the nullsAreSortedAtStart() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testNullsAreSortedAtStart();
  /*
   * @testName:         testNullsAreSortedHigh
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The nullsAreSortedHigh method must return a boolean value;
   *                    true if NULL values are sorted high and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the nullsAreSortedHigh() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testNullsAreSortedHigh();
  /*
   * @testName:         testNullsAreSortedLow
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The nullsAreSortedLow method must return a boolean value;
   *                    true if NULL values are sorted low and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the nullsAreSortedLow() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testNullsAreSortedLow();
  /*
   * @testName:         testOthersDeletesAreVisible1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The othersDeletesAreVisible(int resType) method must return a
   *                    boolean value; true if a rows deleted by others are visible and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the otherDeletesAreVisible(int resType) method on that
   *                    object with ResultSet.TYPE_FORWARD_ONLY.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOthersDeletesAreVisible1();
  /*
   * @testName:         testOthersDeletesAreVisible2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The othersDeletesAreVisible(int resType) method must return a
   *                    boolean value; true if a rows deleted by others are visible and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the otherDeletesAreVisible(int resType) method on that
   *                    object with ResultSet.TYPE_SCROLL_INSENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOthersDeletesAreVisible2();
  /*
   * @testName:         testOthersDeletesAreVisible3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The othersDeletesAreVisible(int resType) method must return a
   *                    boolean value; true if a rows deleted by others are visible and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the otherDeletesAreVisible(int resType) method on that
   *                    object with ResultSet.TYPE_SCROLL_SENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOthersDeletesAreVisible3();
  /*
   * @testName:         testOthersInsertsAreVisible1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The othersInsertsAreVisible(int resType) method must return a
   *                    boolean value; true if a rows inserted by others are visible and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the otherInsertsAreVisible(int resType) method on that
   *                    object with ResultSet.TYPE_FORWARD_ONLY.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOthersInsertsAreVisible1();
  /*
   * @testName:         testOthersInsertsAreVisible2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The othersInsertsAreVisible(int resType) method must return a
   *                    boolean value; true if a rows inserted by others are visible and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the otherInsertsAreVisible(int resType) method on that
   *                    object with ResultSet.TYPE_SCROLL_INSENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOthersInsertsAreVisible2();
  /*
   * @testName:         testOthersInsertsAreVisible3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The othersInsertsAreVisible(int resType) method must return a
   *                    boolean value; true if a rows inserted by others are visible and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the otherInsertsAreVisible(int resType) method on that
   *                    object with ResultSet.TYPE_SCROLL_SENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOthersInsertsAreVisible3();
  /*
   * @testName:         testOthersUpdatesAreVisible1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The othersUpdatesAreVisible(int resType) method must return a
   *                    boolean value; true if a rows updated by others are visible and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the otherUpdatesAreVisible(int resType) method on that
   *                    object with ResultSet.TYPE_FORWARD_ONLY.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOthersUpdatesAreVisible1();
  /*
   * @testName:         testOthersUpdatesAreVisible2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The othersUpdatesAreVisible(int resType) method must return a
   *                    boolean value; true if a rows updated by others are visible and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the otherUpdatesAreVisible(int resType) method on that
   *                    object with ResultSet.TYPE_SCROLL_INSENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOthersUpdatesAreVisible2();
  /*
   * @testName:         testOthersUpdatesAreVisible3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The othersUpdatesAreVisible(int resType) method must return a
   *                    boolean value; true if a rows updated by others are visible and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the otherUpdatesAreVisible(int resType) method on that
   *                    object with ResultSet.TYPE_SCROLL_SENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOthersUpdatesAreVisible3();
  /*
   * @testName:         testOwnDeletesAreVisible1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The ownDeletesAreVisible(int resType) method must return a
   *                    boolean value; true if a ResultSet object's own deletions are
   *                    visible without having to close and reopen the result set and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the ownDeletesAreVisible(int resType) method on that object
   *                    with ResultSet.TYPE_FORWARD_ONLY.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOwnDeletesAreVisible1();
  /*
   * @testName:         testOwnDeletesAreVisible2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The ownDeletesAreVisible(int resType) method must return a
   *                    boolean value; true if a ResultSet object's own deletions are
   *                    visible without having to close and reopen the result set and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the ownDeletesAreVisible(int resType) method on that object
   *                    with ResultSet.TYPE_SCROLL_INSENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOwnDeletesAreVisible2();
  /*
   * @testName:         testOwnDeletesAreVisible3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The ownDeletesAreVisible(int resType) method must return a
   *                    boolean value; true if a ResultSet object's own deletions are
   *                    visible without having to close and reopen the result set and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the ownDeletesAreVisible(int resType) method on that object
   *                    with ResultSet.TYPE_SCROLL_SENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOwnDeletesAreVisible3();
  /*
   * @testName:         testOwnInsertsAreVisible1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The ownInsertsAreVisible(int resType) method must return a
   *                    boolean value; true if a ResultSet object's own insertions are
   *                    visible without having to close and reopen the result set and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the ownInsertsAreVisible(int resType) method on that object
   *                    with ResultSet.TYPE_FORWARD_ONLY.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOwnInsertsAreVisible1();
  /*
   * @testName:         testOwnInsertsAreVisible2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The ownInsertsAreVisible(int resType) method must return a
   *                    boolean value; true if a ResultSet object's own insertions are
   *                    visible without having to close and reopen the result set and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the ownInsertsAreVisible(int resType) method on that object
   *                    with ResultSet.TYPE_SCROLL_INSENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOwnInsertsAreVisible2();
  /*
   * @testName:         testOwnInsertsAreVisible3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The ownInsertsAreVisible(int resType) method must return a
   *                    boolean value; true if a ResultSet object's own insertions are
   *                    visible without having to close and reopen the result set and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the ownInsertsAreVisible(int resType) method on that object
   *                    with ResultSet.TYPE_SCROLL_SENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOwnInsertsAreVisible3();
  /*
   * @testName:         testOwnUpdatesAreVisible1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The ownUpdatesAreVisible(int resType) method must return a
   *                    boolean value; true if a ResultSet object's own updates are
   *                    visible without having to close and reopen the result set and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the ownUpdatesAreVisible(int resType) method on that object
   *                    with ResultSet.TYPE_FORWARD_ONLY.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOwnUpdatesAreVisible1();
  /*
   * @testName:         testOwnUpdatesAreVisible2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The ownUpdatesAreVisible(int resType) method must return a
   *                    boolean value; true if a ResultSet object's own updates are
   *                    visible without having to close and reopen the result set and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the ownUpdatesAreVisible(int resType) method on that object
   *                    with ResultSet.TYPE_SCROLL_INSENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOwnUpdatesAreVisible2();
  /*
   * @testName:         testOwnUpdatesAreVisible3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The ownUpdatesAreVisible(int resType) method must return a
   *                    boolean value; true if a ResultSet object's own updates are
   *                    visible without having to close and reopen the result set and
   *                    false otherwise. The possible values for resType are
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE and TYPE_SCROLL_SENSITIVE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the ownUpdatesAreVisible(int resType) method on that object
   *                    with ResultSet.TYPE_SCROLL_SENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testOwnUpdatesAreVisible3();
  /*
   * @testName:         testStoresLowerCaseIdentifiers
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The storesLowerCaseIdentifiers method must return a boolean
   *                    value; true if DBMS allows mixed case SQL Identifiers in SQL
   *                    statements as case-insensitive and stores them as all lowercases
   *                    in its metadata tables and false otherwise. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the storesLowerCaseIdentifiers() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testStoresLowerCaseIdentifiers();
  /*
   * @testName:         testStoresLowerCaseQuotedIdentifiers
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The storesLowerCaseQuotedIdentifiers() method must return a boolean
   *                    value; true if DBMS allows mixed case quoted SQL Identifiers
   *                    in SQLstatements as case-insensitive and stores them as all
   *                    lower cases in its metadata tables and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the storesLowerCaseQuotedIdentifiers() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testStoresLowerCaseQuotedIdentifiers();
  /*
   * @testName:         testStoresMixedCaseIdentifiers
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The storesMixedCaseIdentifiers() method must return a boolean
   *                    value; true if DBMS allows mixed case SQL Identifiers in SQL
   *                    statements as case-insensitive and stores them as all mixed cases
   *                    in its metadata tables and false otherwise. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the storesMixedCaseIdentifiers() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testStoresMixedCaseIdentifiers();
  /*
   * @testName:         testStoresMixedCaseQuotedIdentifiers
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The storesMixedCaseQuotedIdentifiers() method must return a boolean
   *                    value; true if DBMS allows mixed case quoted SQL Identifiers in SQL
   *                    statements as case-insensitive and stores them as all mixed cases
   *                    in its metadata tables and false otherwise. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the storesMixedCaseQuotedIdentifiers() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testStoresMixedCaseQuotedIdentifiers();
  /*
   * @testName:         testStoresUpperCaseIdentifiers
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The storesUpperCaseIdentifiers method must return a boolean
   *                    value; true if DBMS allows mixed case SQL Identifiers in SQL
   *                    statements as case-insensitive and stores them as all uppercases
   *                    in its metadata tables and false otherwise. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the storesUpperCaseIdentifiers() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testStoresUpperCaseIdentifiers();
  /*
   * @testName:         testStoresUpperCaseQuotedIdentifiers
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The storesUpperCaseQuotedIdentifiers() method must return a boolean
   *                    value; true if DBMS allows mixed case quoted SQL Identifiers in SQL
   *                    statements as case-insensitive and stores them as all upper cases
   *                    in its metadata tables and false otherwise. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the storesUpperCaseQuotedIdentifiers() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testStoresUpperCaseQuotedIdentifiers();
  /*
   * @testName:         testSupportsANSI92EntryLevelSQL
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsANSI92EntryLevelSQL() method must return a boolean
   *                    value; true if the database supports ANSI92 entry level SQL
   *                    grammar and false otherwise.  All JDBC Compliant drivers must
   *                    return true. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsANSI92EntryLevelSQL() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsANSI92EntryLevelSQL();
  /*
   * @testName:         testSupportsANSI92FullSQL
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsANSI92FullSQL() method must return a boolean value
   *                    true if the database supports ANSI92 Full SQL grammar and false
   *                    otherwise.  (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsANSI92FullSQL() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsANSI92FullSQL();
  /*
   * @testName:         testSupportsANSI92IntermediateSQL
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsANSI92IntermediateSQL() method must return a boolean
   *                    value; true if the database supports ANSI92 Intermediate SQL
   *                    grammar and false otherwise.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsANSI92IntermediateSQL() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsANSI92IntermediateSQL();
  /*
   * @testName:         testSupportsAlterTableWithAddColumn
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsAlterTableWithAddColumn() method must return a
   *                    boolean value; true if the database supports ALTER TABLE
   *                    with add Column and false otherwise (See JDK 1.2.2 API
   *                    documentation).
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsAlterTableWithAddColumn() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testSupportsAlterTableWithAddColumn();
  /*
   * @testName:         testSupportsAlterTableWithDropColumn
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsAlterTableWithDropColumn() method must return a
   *                    boolean value; true if the database supports ALTER TABLE
   *                    with drop column and false otherwise (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsAlterTableWithDropColumn() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testSupportsAlterTableWithDropColumn();
  /*
   * @testName:         testSupportsBatchUpdates
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsBatchUpdates() method must return a boolean
   *                    value; true if the driver supports batch updates and false
   *                    otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsBatchUpdates() method.
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testSupportsBatchUpdates();
  /*
   * @testName:         testSupportsCatalogsInDataManipulation
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsCatalogsInDataManipulation() method must return a
   *                    boolean value; true if the database supports using a catalog name
   *                    in a data manipulation statement and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase.
   *                    Call to supportsCatalogsInDataManipulation()on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsCatalogsInDataManipulation();
  /*
   * @testName:         testSupportsCatalogsInIndexDefinitions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsCatalogsInIndexDefinitions() method must return a
   *                    boolean value; true if the database supports using a catalog name
   *                    in a index definition statement and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    Call the supportsCatalogsInIndexDefinitions() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsCatalogsInIndexDefinitions();
  /*
   * @testName:         testSupportsCatalogsInPrivilegeDefinitions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsCatalogsInPrivilegeDefinitions() method must return a
   *                    boolean value; true if the database supports using a catalog name
   *                    in a privilege definition statement and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsCatalogsInPrivilegeDefinitions() method on that
   *                    object. It should return a boolean value; either true or false
   *
   */

  /* throws std::exception * */

  void testSupportsCatalogsInPrivilegeDefinitions();
  /*
   * @testName:         testSupportsCatalogsInProcedureCalls
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsCatalogsInProcedureCalls() method must return a
   *                    boolean value; true if the database supports using a catalog name
   *                    in a procedure call statement and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase.
   *                    Call to supportsCatalogsInProcedureCalls() on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsCatalogsInProcedureCalls();
  /*
   * @testName:         testSupportsCatalogsInTableDefinitions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsCatalogsInTableDefinitions() method must return a
   *                    boolean value; true if the database supports using a catalog name
   *                    in a table definition statement and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    Call the supportsCatalogsInTableDefinitions() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsCatalogsInTableDefinitions();
  /*
   * @testName:         testSupportsColumnAliasing
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsColumnAliasing() method must return a
   *                    boolean value; true if the database supports column aliasing
   *                    and false otherwise.  A JDBC compliant driver must always return
   *                    true. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsColumnAliasing() method
   *                    It should return a true value
   *
   */

  /* throws std::exception * */

  void testSupportsColumnAliasing();
  /*
   * @testName:         testSupportsConvert
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert() method must return a boolean value;
   *                    true if the database supports the scalar function CONVERT for
   *                    for the conversion of one JDBC type to another and false
   *                    otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testSupportsConvert();
  /*
   * @testName:         testSupportsConvert01
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(ARRAY, VARCHAR) method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testSupportsConvert01();
  /*
   * @testName:         testSupportsConvert02
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(BIGINT, VARCHAR) method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testSupportsConvert02();
  /*
   * @testName:         testSupportsConvert03
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(BINARY, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert03();
  /*
   * @testName:         testSupportsConvert04
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation).
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(BIT, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert04();
  /*
   * @testName:         testSupportsConvert05
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(BLOB, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert05();
  /*
   * @testName:         testSupportsConvert06
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(CHAR, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert06();
  /*
   * @testName:         testSupportsConvert07
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(CLOB, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert07();
  /*
   * @testName:         testSupportsConvert08
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(DATE, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert08();
  /*
   * @testName:         testSupportsConvert09
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(DECIMAL, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert09();
  /*
   * @testName:         testSupportsConvert10
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(DISTINCT, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert10();
  /*
   * @testName:         testSupportsConvert11
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(DOUBLE, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert11();
  /*
   * @testName:         testSupportsConvert12
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(FLOAT, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert12();
  /*
   * @testName:         testSupportsConvert13
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(INTEGER, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert13();

  /*
   * @testName:         testSupportsConvert15
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(LONGVARBINARY, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert15();
  /*
   * @testName:         testSupportsConvert16
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(LONGVARCHAR, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert16();
  /*
   * @testName:         testSupportsConvert17
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(NULL, VARCHAR) on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert17();
  /*
   * @testName:         testSupportsConvert18
   * @assertion:        The DatabaseMetaData provides information about the database
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(NUMERIC, VARCHAR) method on that object
   *                    It should return a boolean value; either true or false
   *
   */

  /* throws std::exception * */

  void testSupportsConvert18();
  /*
   * @testName:         testSupportsConvert19
   * @assertion:        The DatabaseMetaData provides information about the database
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(OTHER, VARCHAR) method on that object
   *                    It should return a boolean value; either true or false
   *
   */

  /* throws std::exception * */

  void testSupportsConvert19();
  /*
   * @testName:         testSupportsConvert20
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(REAL, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert20();
  /*
   * @testName:         testSupportsConvert21
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(REF, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert21();
  /*
   * @testName:         testSupportsConvert22
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(SMALLINT, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert22();
  /*
   * @testName:         testSupportsConvert23
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(STRUCT, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert23();
  /*
   * @testName:         testSupportsConvert24
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(TIME, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert24();
  /*
   * @testName:         testSupportsConvert25
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(TIMESTAMP, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert25();
  /*
   * @testName:         testSupportsConvert26
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(TINYINT, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert26();
  /*
   * @testName:         testSupportsConvert27
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(VARBINARY, VARCHAR) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert27();
  /*
   * @testName:         testSupportsConvert28
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(BIGINT, INTEGER) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert28();
  /*
   * @testName:         testSupportsConvert29
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(BIT, INTEGER) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert29();
  /*
   * @testName:         testSupportsConvert30
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(DATE, INTEGER) method onn that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert30();
  /*
   * @testName:         testSupportsConvert31
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(DECIMAL, INTEGER) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert31();
  /*
   * @testName:         testSupportsConvert32
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(DOUBLE, INTEGER) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert32();
  /*
   * @testName:         testSupportsConvert33
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(FLOAT, INTEGER) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert33();
  /*
   * @testName:         testSupportsConvert34
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(NUMERIC, INTEGER) method on that object.
   *                    It should return a boolean value; true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert34();
  /*
   * @testName:         testSupportsConvert35
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(REAL, INTEGER) method on that object.
   *                    It should return a boolean value; either true false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert35();
  /*
   * @testName:         testSupportsConvert36
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(SMALLINT, INTEGER) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert36();
  /*
   * @testName:         testSupportsConvert37
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsConvert(int fromType, int toType) method must return
   *                    a boolean value; true if the database supports the scalar function
   *                    CONVERT for conversions between the JDBC types fromType and toType
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsConvert(TINYINT, INTEGER) method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsConvert37();
  /*
   * @testName:         testSupportsCoreSQLGrammar
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsCoreSQLGrammar() method must return a boolean value
   *                    true if the database supports the ODBC core SQL grammar and
   *                    false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsCoreSQLGrammar() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsCoreSQLGrammar();
  /*
   * @testName:         testSupportsCorrelatedSubqueries
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsCorrelatedSubqueries() method must return a
   *                    boolean value; true if the database supports correlated subqueries
   *                    and false otherwise. A JDBC Compliant driver always returns true.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsCorrelatedSubqueries() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsCorrelatedSubqueries();
  /*
   * @testName:         testSupportsDataDefinitionAndDataManipulationTransactions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsDataDefinitionAndDataManipulationTransactions method
   *                    must return a boolean value; true if both data definition and
   *                    data manipulation statements within a transaction are supported;
   *                    false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsDataDefinitionAndDataManipulationTransactions() method.
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testSupportsDataDefinitionAndDataManipulationTransactions();
  /*
   * @testName:         testSupportsDataManipulationTransactionsOnly
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsDataManipulationTransactionsOnly() method must return
   *                    a boolean value, true if the data manipulation statements within
   *                    a transaction is supported and false otherwise. A JDBC Compliant
   *                    driver should return true. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsDataManipulationTransactionsOnly() method.
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testSupportsDataManipulationTransactionsOnly();
  /*
   * @testName:         testSupportsDifferentTableCorrelationNames
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsDifferentTableCorrelationNames() method must return
   *                    a boolean value; true if the database supports table
   *                    correlation names and false otherwise. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsDifferentTableCorrelationNames() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsDifferentTableCorrelationNames();
  /*
   * @testName:         testSupportsExpressionsInOrderBy
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsExpressionsInOrderBy() method must return
   *                    a boolean value; true if the database supports expressions
   *                    in ORDER BY lists and false otherwise. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsExpressionsInOrderBy() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsExpressionsInOrderBy();
  /*
   * @testName:         testSupportsExtendedSQLGrammar
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsExtendedSQLGrammar() method must return a boolean
   *                    value; true if the database supports the ODBC Extended SQL
   *                    grammar and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsExtendedSQLGrammar() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsExtendedSQLGrammar();
  /*
   * @testName:         testSupportsFullOuterJoins
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsFullOuterJoins() method must return a
   *                    boolean value; true if the database supports full nested outer
   *                    joins and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsFullOuterJoins() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsFullOuterJoins();
  /*
   * @testName:         testSupportsGroupBy
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsGroupBy() method must return a boolean value;
   *                    true if the database supports GROUP BY clause
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsGroupBy() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsGroupBy();
  /*
   * @testName:         testSupportsGroupByBeyondSelect
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsGroupByBeyondSelect() method must return a boolean
   *                    value; true if a GROUP BY clause can use columns that are not
   *                    in the SELECT clause. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsGroupByBeyondSelect() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsGroupByBeyondSelect();
  /*
   * @testName:         testSupportsGroupByUnrelated
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsGroupByUnrelated() method must return a boolean value;
   *                    true if a GROUP BY clause can use columns that are not in the
   *                    SELECT clause and false otherwise (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsGroupByUnrelated() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsGroupByUnrelated();


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  /*
   * @testName:         testSupportsIntegrityEnhancementFacility
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsIntegrityEnhancementFacility() method must return a
   *                    boolean value; true if the database supports the SQL Integrity
   *                    Enhancement Facility and false otherwise.
   *                   (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsIntegrityEnhancementFacility() method onn that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsIntegrityEnhancementFacility();
#endif


  /*
   * @testName:         testSupportsLikeEscapeClause
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsLikeEscapeClause() method must return a boolean value
   *                    true if the database supports specifying a LIKE escape clause
   *                    and false otherwise.  A JDBC compliant driver always return true.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsLikeEscapeClause() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsLikeEscapeClause();


  /*
   * @testName:         testSupportsLimitedOuterJoins
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsLimitedOuterJoins() method must return a
   *                    boolean value; true if the database supports limited outer joins
   *                    and false otherwise. Note if the method supportsFullOuterJoins
   *                    return true, this method will return true. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsLimitedOuterJoins() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsLimitedOuterJoins();


  /*
   * @testName:         testSupportsMinimumSQLGrammar
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsMinimumSQLGrammar() method must return a boolean
   *                    value; true if the database supports the ODBC Minimum SQL
   *                    grammar and false otherwise.  All JDBC Compliant drivers must
   *                    return true. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsMinimumSQLGrammar() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsMinimumSQLGrammar();


  /*
   * @testName:         testSupportsMixedCaseIdentifiers
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The supportsMixedCaseIdentifiers method must return a boolean
   *                    value; true if DBMS allows mixed case SQL Identifiers in SQL
   *                    statements as case-sensitive and false otherwise.  A JDBC
   *                    compliant driver will  always return false.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsMixedCaseIdentifiers() method
   *                    It should return a boolean value
   *
   */
  /* throws std::exception * */
  void testSupportsMixedCaseIdentifiers();


  /*
   * @testName:         testSupportsMixedCaseQuotedIdentifiers
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsMixedCaseQuotedIdentifiers() method must return a boolean
   *                    value; true if DBMS allows mixed case quoted SQL Identifiers in SQL
   *                    statements as case-insensitive and stores them as all mixed cases
   *                    in its metadata tables and false otherwise. A JDBC Compliant
   *                    driver will always return true. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsMixedCaseQuotedIdentifiers() method
   *                    It should return a boolean value
   *
   */
  /* throws std::exception * */
  void testSupportsMixedCaseQuotedIdentifiers();


  /*
   * @testName:         testSupportsMultipleResultSets
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsMultipleResultSets() method must return a boolean value
   *                    true if the database supports multiple result sets from a single
   *                    execute statement and false otherwise. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsMultipleResultSets() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsMultipleResultSets();


  /*
   * @testName:         testSupportsMultipleTransactions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsMultipleTransactions() method must return a boolean
   *                    value; true if multiple transactions can be open at once on
   *                    different connections and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsMultipleTransactions() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsMultipleTransactions();


  /*
   * @testName:         testSupportsNonNullableColumns
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsNonNullableColumns() method must return a boolean
   *                    value; true if the database supports defining columns as
   *                    non-nullable and false otherwise.  (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsNonNullableColumns() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsNonNullableColumns();


  /*
   * @testName:         testSupportsOpenCursorsAcrossCommit
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsOpenCursorsAcrossCommit() method must return a boolean
   *                    value; true if cursors always remain open after commits; false
   *                    if cursors are closed on commit. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsOpenCursorsAcrossCommit() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsOpenCursorsAcrossCommit();


  /*
   * @testName:         testSupportsOpenCursorsAcrossRollback
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsOpenCursorsAcrossRollback() method must return a boolean
   *                    value; true if cursors always remain open after rollbacks; false
   *                    if cursors are closed on rollback. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsOpenCursorsAcrossRollback() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsOpenCursorsAcrossRollback();


  /*
   * @testName:         testSupportsOpenStatementsAcrossCommit
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsOpenStatementsAcrossCommit() method must return a
   *                    boolean value; true if statements always remain open after
   *                    commits; false if statements are closed on commit.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsOpenStatementsAcrossCommit() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsOpenStatementsAcrossCommit();


  /*
   * @testName:         testSupportsOpenStatementsAcrossRollback
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsOpenStatementsAcrossRollback() method must return a
   *                    boolean value; true if statements always remain open after
   *                    rollbacks; false if statements are closed on rollback.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsOpenStatementsAcrossRollback() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsOpenStatementsAcrossRollback();


  /*
   * @testName:         testSupportsOrderByUnrelated
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsOrderByUnrelated() method must return
   *                    a boolean value; true if the database supports ORDER BY
   *                    clause can use columns that are not in the SELECT clause
   *                    and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsOrderByUnrelated() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsOrderByUnrelated();


  /*
   * @testName:         testSupportsOuterJoins
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsOuterJoins() method must return a
   *                    boolean value; true if the database supports form of outer
   *                    joins and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsOuterJoins() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsOuterJoins();


  /*
   * @testName:         testSupportsPositionedDelete
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsPositionedDelete() method must return a
   *                    boolean value; true if the database supports positioned
   *                    DELETE statement and false otherwise.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsPositionedDelete() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsPositionedDelete();


  /*
   * @testName:         testSupportsPositionedUpdate
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsPositionedUpdate() method must return a
   *                    boolean value; true if the database supports positioned
   *                    UPDATE statement and false otherwise.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsPositionedUpdate() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsPositionedUpdate();

#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
  /*
   * @testName:         testSupportsResultSetConcurrency1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsResultSetConcurrency(int rsType, int rsConcur)
   *                    method must return a boolean value where possible values for rsType
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE & TYPE_SCROLL_SENSITIVE
   *                    and values for rsConcur can be CONCUR_READ_ONLY and
   *                    CONCUR_UPDATABLE. The method returns true if the database
   *                    supports the concurrency level rsConcur with the combination
   *                    of rsType and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsResultSetConcurrency(int resType, int rsConcur)
   *                    method on that object with TYPE_FORWARD_ONLY and CONCUR_READ_ONLY.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsResultSetConcurrency1();


  /*
   * @testName:         testSupportsResultSetConcurrency2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsResultSetConcurrency(int rsType, int rsConcur)
   *                    method must return a boolean value where possible values for rsType
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE & TYPE_SCROLL_SENSITIVE
   *                    and values for rsConcur can be CONCUR_READ_ONLY and
   *                    CONCUR_UPDATABLE. The method returns true if the database
   *                    supports the concurrency level rsConcur with the combination
   *                    of rsType and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsResultSetConcurrency(int resType, int rsConcur)
   *                    method on that object with TYPE_FORWARD_ONLY and CONCUR_UPDATABLE.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsResultSetConcurrency2();


  /*
   * @testName:         testSupportsResultSetConcurrency3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsResultSetConcurrency(int rsType, int rsConcur)
   *                    method must return a boolean value where possible values for rsType
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE & TYPE_SCROLL_SENSITIVE
   *                    and values for rsConcur can be CONCUR_READ_ONLY and
   *                    CONCUR_UPDATABLE. The method returns true if the database
   *                    supports the concurrency level rsConcur with the combination
   *                    of rsType and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsResultSetConcurrency(int resType, int rsConcur)
   *                    method on that object with TYPE_SCROLL_INSENSITIVE and CONCUR_READ_ONLY.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsResultSetConcurrency3();


  /*
   * @testName:         testSupportsResultSetConcurrency4
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsResultSetConcurrency(int rsType, int rsConcur)
   *                    method must return a boolean value where possible values for rsType
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE & TYPE_SCROLL_SENSITIVE
   *                    and values for rsConcur can be CONCUR_READ_ONLY and
   *                    CONCUR_UPDATABLE. The method returns true if the database
   *                    supports the concurrency level rsConcur with the combination
   *                    of rsType and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsResultSetConcurrency(int resType, int rsConcur)
   *                    method on that object with TYPE_SCROLL_INSENSITIVE and CONCUR_UPDATABLE.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsResultSetConcurrency4();


  /*
   * @testName:         testSupportsResultSetConcurrency5
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsResultSetConcurrency(int rsType, int rsConcur)
   *                    method must return a boolean value where possible values for rsType
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE & TYPE_SCROLL_SENSITIVE
   *                    and values for rsConcur can be CONCUR_READ_ONLY and
   *                    CONCUR_UPDATABLE. The method returns true if the database
   *                    supports the concurrency level rsConcur with the combination
   *                    of rsType and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsResultSetConcurrency(int resType, int rsConcur)
   *                    method on that object with TYPE_SCROLL_SENSITIVE and CONCUR_READ_ONLY.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsResultSetConcurrency5();


  /*
   * @testName:         testSupportsResultSetConcurrency6
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsResultSetConcurrency(int rsType, int rsConcur)
   *                    method must return a boolean value where possible values for rsType
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE & TYPE_SCROLL_SENSITIVE
   *                    and values for rsConcur can be CONCUR_READ_ONLY and
   *                    CONCUR_UPDATABLE. The method returns true if the database
   *                    supports the concurrency level rsConcur with the combination
   *                    of rsType and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsResultSetConcurrency(int resType, int rsConcur)
   *                    method on that object with TYPE_SCROLL_SENSITIVE and CONCUR_UPDATABLE.
   *                    It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsResultSetConcurrency6();
#endif


  /*
   * @testName:         testSupportsResultSetType1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsResultSetType(int resType) method must return a
   *                    boolean value where resType can be any one of the following
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE & TYPE_SCROLL_SENSITIVE.
   *                    The value is true if the database supports the ResultSet type
   *                    resType and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsResultSetType(int resType) method with Type TYPE_FORWARD_ONLY
   *                    on that object.It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsResultSetType1();


  /*
   * @testName:         testSupportsResultSetType2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsResultSetType(int resType) method must return a
   *                    boolean value where resType can be any one of the following
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE & TYPE_SCROLL_SENSITIVE.
   *                    The value is true if the database supports the ResultSet type
   *                    resType and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsResultSetType() method with Type TYPE_SCROLL_INSENSITIVE
   *                    on that object.It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsResultSetType2();


  /*
   * @testName:         testSupportsResultSetType3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsResultSetType(int resType) method must return a
   *                    boolean value where resType can be any one of the following
   *                    TYPE_FORWARD_ONLY, TYPE_SCROLL_INSENSITIVE & TYPE_SCROLL_SENSITIVE.
   *                    The value is true if the database supports the ResultSet type
   *                    resType and false otherwise. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsResultSetType() method with Type TYPE_SCROLL_SENSITIVE
   *                    on that object.It should return a boolean value; either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsResultSetType3();


  /*
   * @testName:         testSupportsSchemasInDataManipulation
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsSchemasInDataManipulation() method must return a
   *                    boolean value; true if the database supports using a schema name
   *                    in a data manipulation statement and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase.
   *                    Make a call to DatabaseMetadata.supportsSchemasInDataManipulation() on that object.
   *                    It should return a boolean value either true or false.
   *
   */
  /* throws std::exception * */
  void testSupportsSchemasInDataManipulation();


  /*
   * @testName:         testSupportsSchemasInIndexDefinitions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsSchemasInIndexDefinitions() method must return a
   *                    boolean value; true if the database supports using a schema name
   *                    in a index definition statement and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase.
   *                    Call to supportsSchemasInIndexDefinitions() on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsSchemasInIndexDefinitions();
  /*
   * @testName:         testSupportsSchemasInPrivilegeDefinitions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsSchemasInPrivilegeDefinitions() method must return a
   *                    boolean value; true if the database supports using a schema name
   *                    in a privilege definition statement and false otherwise.
   *                    (See JDK 1.2.2 API documentation).
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase.
   *                    Call to supportsSchemasInPrivilegeDefinitions() on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsSchemasInPrivilegeDefinitions();
  /*
   * @testName:         testSupportsSchemasInProcedureCalls
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsSchemasInProcedureCalls() method must return a
   *                    boolean value; true if the database supports using a schema name
   *                    in a procedure call statement and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase.
   *                    Make a call to DatabaseMetadata.supportsSchemasInProcedureCalls() on
   *                    that object.  It should return a boolean value; either true or false
   *
   */

  /* throws std::exception * */

  void testSupportsSchemasInProcedureCalls();
  /*
   * @testName:         testSupportsSchemasInTableDefinitions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsSchemasInTableDefinitions() method must return a
   *                    boolean value; true if the database supports using a schema name
   *                    in a table definition statement and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase.
   *                    Make a call to DatabaseMetadata.supportsSchemasInTableDefinitions()
   *                    on that object.It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsSchemasInTableDefinitions();
  /*
   * @testName:         testSupportsSelectForUpdate
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsSelectForUpdate() method must return a
   *                    boolean value; true if the database supports positioned
   *                    UPDATE statement and false otherwise.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsSelectForUpdate() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsSelectForUpdate();
  /*
   * @testName:         testSupportsStoredProcedures
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave
   *                    as specified in the JDBC 1.0 and 2.0 specifications. None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented.(See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The supportsStoredProcedures method must return a boolean value;
   *                    true if the database supports stored procedure calls and false if
   *                    the database does not support it. (See JDK 1.2.2 API
   *                    documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsStoredprocedures() method
   *                    It should return true value
   *
   */

  /* throws std::exception * */

  void testSupportsStoredProcedures();
  /*
   * @testName:         testSupportsSubqueriesInComparisons
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsSubqueriesInComparisons() method must return a
   *                    boolean value; true if the database supports subqueries in
   *                    comparison expressions and false otherwise.  A JDBC compliant
   *                    driver always returns true. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsSubqueriesInComparisons() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsSubqueriesInComparisons();
  /*
   * @testName:         testSupportsSubqueriesInExists
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsSubqueriesInExists() method must return a
   *                    boolean value; true if the database supports subqueries in
   *                    EXISTS expressions and false otherwise. A JDBC compliant driver
   *                    always returns true. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsSubqueriesInExists() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsSubqueriesInExists();
  /*
   * @testName:         testSupportsSubqueriesInIns
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsSubqueriesInIns() method must return a
   *                    boolean value; true if the database supports subqueries in
   *                    "IN" statements and false otherwise.  A JDBC Compliant driver
   *                    always returns true.  (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsSubqueriesInIns() method on that object.
   *                    It should return a boolean value either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsSubqueriesInIns();
  /*
   * @testName:         testSupportsSubqueriesInQuantifieds
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsSubqueriesInQuantifieds() method must return a
   *                    boolean value; true if the database supports subqueries in
   *                    quantified statement and false otherwise.  A JDBC Compliant driver
   *                    always returns true.  (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsSubqueriesInQuantifieds() method on that object.
   *                    It should return a boolean value either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsSubqueriesInQuantifieds();
  /*
   * @testName:         testSupportsTableCorrelationNames
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsTableCorrelationNames() method must return
   *                    a boolean value; true if the database supports table
   *                    correlation names and false otherwise.  A JDBC compliant
   *                    driver always return true. (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the supportsTableCorrelationNames() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsTableCorrelationNames();
  /*
   * @testName:         testSupportsTransactionIsolationLevel1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsTransactionIsolationLevel(int isolevel) method must
   *                    return a boolean value; true if the database supports the given
   *                    Transaction Isolation Level isolevel and false otherwise.  The
   *                    possible values for isolevel can be TRANSACTION_NONE,
   *                    TRANSACTION_READ_COMMITTED, TRANSACTION_READ_UNCOMMITTED,
   *                    TRANSACTION_REPEATABLE_READ and TRANSACTION_SERIALIZABLE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsTransactionIsolationLevel(int isolevel) method
   *                    on that object with the isolation level TRANSACTION_NONE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsTransactionIsolationLevel1();
  /*
   * @testName:         testSupportsTransactionIsolationLevel2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsTransactionIsolationLevel(int isolevel) method must
   *                    return a boolean value; true if the database supports the given
   *                    Transaction Isolation Level isolevel and false otherwise.  The
   *                    possible values for isolevel can be TRANSACTION_NONE,
   *                    TRANSACTION_READ_COMMITTED, TRANSACTION_READ_UNCOMMITTED,
   *                    TRANSACTION_REPEATABLE_READ and TRANSACTION_SERIALIZABLE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsTransactionIsolationLevel(int isolevel) method
   *                    on that object with the isolation level TRANSACTION_READ_COMMITTED.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsTransactionIsolationLevel2();
  /*
   * @testName:         testSupportsTransactionIsolationLevel3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsTransactionIsolationLevel(int isolevel) method must
   *                    return a boolean value; true if the database supports the given
   *                    Transaction Isolation Level isolevel and false otherwise.  The
   *                    possible values for isolevel can be TRANSACTION_NONE,
   *                    TRANSACTION_READ_COMMITTED, TRANSACTION_READ_UNCOMMITTED,
   *                    TRANSACTION_REPEATABLE_READ and TRANSACTION_SERIALIZABLE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsTransactionIsolationLevel(int isolevel) method
   *                    on that object with the isolation level TRANSACTION_READ_UNCOMMITTED.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsTransactionIsolationLevel3();
  /*
   * @testName:         testSupportsTransactionIsolationLevel4
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsTransactionIsolationLevel(int isolevel) method must
   *                    return a boolean value; true if the database supports the given
   *                    Transaction Isolation Level isolevel and false otherwise.  The
   *                    possible values for isolevel can be TRANSACTION_NONE,
   *                    TRANSACTION_READ_COMMITTED, TRANSACTION_READ_UNCOMMITTED,
   *                    TRANSACTION_REPEATABLE_READ and TRANSACTION_SERIALIZABLE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsTransactionIsolationLevel(int isolevel) method
   *                    on that object with  isolation level TRANSACTION_REPEATABLE_READ.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsTransactionIsolationLevel4();
  /*
   * @testName:         testSupportsTransactionIsolationLevel5
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsTransactionIsolationLevel(int isolevel) method must
   *                    return a boolean value; true if the database supports the given
   *                    Transaction Isolation Level isolevel and false otherwise.  The
   *                    possible values for isolevel can be TRANSACTION_NONE,
   *                    TRANSACTION_READ_COMMITTED, TRANSACTION_READ_UNCOMMITTED,
   *                    TRANSACTION_REPEATABLE_READ and TRANSACTION_SERIALIZABLE.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsTransactionIsolationLevel(int isolevel) method
   *                    on that object with  isolation level TRANSACTION_SERIALIZABLE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsTransactionIsolationLevel5();
  /*
   * @testName:         testSupportsTransactions
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsTransactions() method must return a boolean
   *                    value; true if transactions are supported and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsTransactions() method.
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testSupportsTransactions();
  /*
   * @testName:         testSupportsUnion
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsUnion() method must return a boolean value;
   *                    true if the database supports SQL Union and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsUnion() method on that object.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsUnion();
  /*
   * @testName:         testSupportsUnionAll
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The supportsUnionAll() method must return a boolean value
   *                    true if the database supports SQL Union All and false otherwise.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the supportsUnionAll() method on that object.
   *                    It should return a boolean value; eithet true or false.
   *
   */

  /* throws std::exception * */

  void testSupportsUnionAll();
  /*
   * @testName:         testUpdatesAreDetected1
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The updatesAreDetected(int resType) method must return a
   *                    boolean value; true if the method ResultSet.rowUpdated returns
   *                    true when called on a ResultSet object with a given type that
   *                    contains visible row updates; false if the method
   *                    ResultSet.rowUpdated returns false. The possible values for
   *                    resType are TYPE_FORWARD_ONLY, TYPE_SCROLL_SENSITIVE and
   *                    TYPE_SCROLL_INSENSITIVE.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the updatesAreDetected() method on that object with the
   *                    ResultSet Type as ResultSet.TYPE_FORWARD_ONLY.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testUpdatesAreDetected1();
  /*
   * @testName:         testUpdatesAreDetected2
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The updatesAreDetected(int resType) method must return a
   *                    boolean value; true if the method ResultSet.rowUpdated returns
   *                    true when called on a ResultSet object with a given type that
   *                    contains visible row updates; false if the method
   *                    ResultSet.rowUpdated returns false. The possible values for
   *                    resType are TYPE_FORWARD_ONLY, TYPE_SCROLL_SENSITIVE and
   *                    TYPE_SCROLL_INSENSITIVE.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the updatesAreDetected() method on that object with the
   *                    ResultSet Type as ResultSet.TYPE_SCROLL_INSENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testUpdatesAreDetected2();
  /*
   * @testName:         testUpdatesAreDetected3
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *                    The updatesAreDetected(int resType) method must return a
   *                    boolean value; true if the method ResultSet.rowUpdated returns
   *                    true when called on a ResultSet object with a given type that
   *                    contains visible row updates; false if the method
   *                    ResultSet.rowUpdated returns false. The possible values for
   *                    resType are TYPE_FORWARD_ONLY, TYPE_SCROLL_SENSITIVE and
   *                    TYPE_SCROLL_INSENSITIVE.(See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get a DatabaseMetadata object from the connection to the database
   *                    and call the updatesAreDetected() method on that object with the
   *                    ResultSet Type as ResultSet.TYPE_SCROLL_SENSITIVE.
   *                    It should return a boolean value; either true or false.
   *
   */

  /* throws std::exception * */

  void testUpdatesAreDetected3();
  /*
   * @testName:         testUsesLocalFilePerTable
   * @assertion:        The DatabaseMetaData provides information about the database.
   *                    (See section 15.1 of JDBC 2.0 API Reference & Tutorial 2nd edition)
   *
   *                    A  driver must provide full support for DatabaseMetaData and
   *                    ResultSetMetaData.  This implies that all of the methods in the
   *                    DatabaseMetaData interface must be implemented and must behave as
   *                    specified in the JDBC 1.0 and 2.0 specifications.  None of the
   *                    methods in DatabaseMetaData and ResultSetMetaData may throw an
   *                    exception because they are not implemented. (See section 6.2.2.3
   *                    of Java2 Platform Enterprise Edition (J2EE) Specification v1.2)
   *
   *                    The usesLocalFilePerTable method must return a boolean value;
   *                    true if the database uses a local file for each table.
   *                    (See JDK 1.2.2 API documentation)
   *
   * @test_Strategy:    Get the DataBaseMetaData object from the Connection to the DataBase
   *                    and call the usesLocalFilePerTable() method
   *                    It should return a boolean value
   *
   */

  /* throws std::exception * */

  void testUsesLocalFilePerTable();
};
REGISTER_FIXTURE(DatabaseMetaDataTest);

}
}
