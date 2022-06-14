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



#include "DatabaseMetaDataTest.h"

namespace testsuite
{
namespace compliance
{
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

/* throws Exception */
void DatabaseMetaDataTest::testAllProceduresAreCallable()
{

  logMsg("Calling allProceduresAreCallable on DatabaseMetaData");
  bool retValue=dbmd->allProceduresAreCallable();
  if (retValue) {
    logMsg(
           "allProceduresAreCallable method called by the current user");
  } else {
    logMsg(
           "allProceduresAreCallable method not called by the current user");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testAllTablesAreSelectable()
{
  logMsg("Calling allTablesAreSelectable on DatabaseMetaData");
  bool retValue=dbmd->allTablesAreSelectable();
  if (retValue) {
    logMsg(
           "allTablesAreSelectable method SELECTed by the current user");
  } else {
    logMsg(
           "allTablesAreSelectable method not SELECTed by the current user");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testDataDefinitionCausesTransactionCommit()
{
  logMsg(
         "Calling DatabaseMetaData.dataDefinitionCausesTransactionCommit");
  bool retValue=dbmd->dataDefinitionCausesTransactionCommit();
  if (retValue) {
    logMsg(
           "Data definition statement forces the transaction to commit");
  } else {
    logMsg(
           "Data definition statement does not forces the transaction to commit");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testDataDefinitionIgnoredInTransactions()
{

  logMsg(
         "Calling DatabaseMetaData.dataDefinitionIgnoredInTransactions");
  bool retValue=dbmd->dataDefinitionIgnoredInTransactions();
  if (retValue) {
    logMsg("Data definition statement is ignored in a transaction");
  } else {
    logMsg(
           "Data definition statement is not ignored in a transaction");
  }


}

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
 *                    result set type as sql::ResultSet::TYPE_FORWARD_ONLY.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testDeletesAreDetected1()
{

  logMsg(
         "Calling DatabaseMetaData.deletesAreDetected(TYPE_FORWARD_ONLY)");
  bool retValue=dbmd->deletesAreDetected(sql::ResultSet::TYPE_FORWARD_ONLY);
  if (retValue) {
    logMsg(
           "Visible row delete can be detected for TYPE_FORWARD_ONLY");
  } else {
    logMsg(
           "Visible row delete cannot be detected for TYPE_FORWARD_ONLY");
  }


}

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
 *                    result set type as sql::ResultSet::TYPE_SCROLL_INSENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testDeletesAreDetected2()
{

  logMsg(
         "Calling DatabaseMetaData.deletesAreDetected(TYPE_SCROLL_INSENSITIVE)");
  bool retValue=dbmd->deletesAreDetected(sql::ResultSet::TYPE_SCROLL_INSENSITIVE);
  if (retValue) {
    logMsg(
           "Visible row delete can be detected for TYPE_SCROLL_INSENSITIVE");
  } else {
    logMsg(
           "Visible row delete cannot be detected for TYPE_SCROLL_INSENSITIVE");
  }


}

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
 *                    result set type as sql::ResultSet::TYPE_SCROLL_SENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testDeletesAreDetected3()
{

  logMsg(
         "Calling DatabaseMetaData.deletesAreDetected(TYPE_SCROLL_SENSITIVE)");
  bool retValue=dbmd->deletesAreDetected(sql::ResultSet::TYPE_SCROLL_SENSITIVE);
  if (retValue) {
    logMsg(
           "Visible row delete can be detected for TYPE_SCROLL_SENSITIVE");
  } else {
    logMsg(
           "Visible row delete cannot be detected for TYPE_SCROLL_SENSITIVE");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testDoesMaxRowSizeIncludeBlobs()
{

  logMsg("Calling  DatabaseMetaData.doesMaxRowSizeIncludeBlobs");
  bool retValue=dbmd->doesMaxRowSizeIncludeBlobs();
  if (retValue) {
    logMsg("MaxRowSize includes blobs");
  } else {
    logMsg("MaxRowSize does not include blobs");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetBestRowIdentifier1()
{

  logMsg(
         "Calling DatabaseMetaData.getBestRowIdentifier with scope bestRowTemporary with NULLable columns");

  ResultSet oRet_ResultSet(dbmd->getBestRowIdentifier(sCatalogName,
                                                      sSchemaName, sFtable, sql::DatabaseMetaData::bestRowTemporary,
                                                      true));
  String sRetStr;
  sRetStr="";
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(2) + ",");
  if (sRetStr == "") {
    logMsg(
           "getBestRowIdentifier with scope bestRowTemporary did not return any columns");
  } else {
    logMsg("The columns returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetBestRowIdentifier2()
{

  logMsg(
         "Calling DatabaseMetaData.getBestRowIdentifier with scope bestRowTransaction with NULLable columns");
  ResultSet oRet_ResultSet(dbmd->getBestRowIdentifier(sCatalogName,
                                                      sSchemaName, sFtable, sql::DatabaseMetaData::bestRowTransaction,
                                                      true));
  String sRetStr;
  sRetStr="";
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(2) + ",");
  if (sRetStr == "") {
    logMsg(
           "getBestRowIdentifier with scope bestRowTransaction did not return any columns");
  } else {
    logMsg("The columns returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetBestRowIdentifier3()
{

  logMsg(
         "Calling DatabaseMetaData.getBestRowIdentifier with scope bestRowSession with NULLable columns");
  ResultSet oRet_ResultSet(dbmd->getBestRowIdentifier(sCatalogName,
                                                      sSchemaName, sFtable, sql::DatabaseMetaData::bestRowSession, true));
  String sRetStr;
  sRetStr="";
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(2) + ",");
  if (sRetStr == "") {
    logMsg(
           "getBestRowIdentifier with scope bestRowSession did not return any columns");
  } else {
    logMsg("The columns returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetBestRowIdentifier4()
{

  logMsg(
         "Calling DatabaseMetaData.getBestRowIdentifier with scope bestRowTemporary without NULLable columns");
  ResultSet oRet_ResultSet(dbmd->getBestRowIdentifier(sCatalogName,
                                                      sSchemaName, sFtable, sql::ResultSet::TYPE_SCROLL_SENSITIVE,
                                                      false));
  String sRetStr;
  sRetStr="";
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(2) + ",");
  if (sRetStr == "") {
    logMsg(
           "getBestRowIdentifier with scope bestRowTemporary did not return any columns");
  } else {
    logMsg("The columns returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetBestRowIdentifier5()
{

  logMsg(
         "Calling DatabaseMetaData.getBestRowIdentifier with scope bestRowTransaction without NULLable columns");
  ResultSet oRet_ResultSet(dbmd->getBestRowIdentifier(sCatalogName,
                                                      sSchemaName, sFtable, sql::DatabaseMetaData::bestRowTransaction,
                                                      true));
  String sRetStr;
  sRetStr="";
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(2) + ",");
  if (sRetStr == "") {
    logMsg(
           "getBestRowIdentifier with scope bestRowTransaction did not return any columns");
  } else {
    logMsg("The columns returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }

}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetBestRowIdentifier6()
{

  logMsg(
         "Calling DatabaseMetaData.getBestRowIdentifier with scope bestRowSession without NULLable columns");
  ResultSet oRet_ResultSet(dbmd->getBestRowIdentifier(sCatalogName,
                                                      sSchemaName, sFtable, sql::DatabaseMetaData::bestRowSession, true));
  String sRetStr;
  sRetStr="";
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(2) + ",");
  if (sRetStr == "") {
    logMsg(
           "getBestRowIdentifier with scope bestRowSession did not return any columns");
  } else {
    logMsg("The columns returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetBestRowIdentifier7()
{
  List sColumnNames;
  sColumnNames.push_back("SCOPE");
  sColumnNames.push_back("COLUMN_NAME");
  sColumnNames.push_back("DATA_TYPE");
  sColumnNames.push_back("TYPE_NAME");
  sColumnNames.push_back("COLUMN_SIZE");
  sColumnNames.push_back("BUFFER_LENGTH");
  sColumnNames.push_back("DECIMAL_DIGITS");
  sColumnNames.push_back("PSEUDO_COLUMN");

  bool test_status=true;
  logMsg(
         "Calling DatabaseMetaData.getBestRowIdentifier with scope bestRowSession without NULLable columns");
  ResultSet oRet_ResultSet(dbmd->getBestRowIdentifier(sCatalogName,
                                                      sSchemaName, sFtable, sql::DatabaseMetaData::bestRowSession, true));
  test_status=columnCompare(sColumnNames, oRet_ResultSet);

  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getBestRowIdentifier7 Failed!");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetCatalogSeparator()
{
  logMsg("Calling DatabaseMetaData.getCatalogSeparator");
  String sRetValue=dbmd->getCatalogSeparator();
  if (sRetValue.empty()) {
    logMsg("getCatalogSeparator is not supported");
  } else {
    logMsg(String("getCatalogSeparator returns ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetCatalogTerm()
{
  logMsg("Calling getCatalogTerm on DatabaseMetaData");
  String sRetValue=dbmd->getCatalogTerm();
  if (sRetValue.empty()) {
    logMsg(
           "getCatalogTerm method does not returns the vendor's preferred term for catalog ");
  } else {
    logMsg(String("getCatalogTerm method returns:  ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetCatalogs()
{
  List sColumnNames;
  sColumnNames.push_back("TABLE_CAT");
  bool test_status=true;
  logMsg("Calling DatabaseMetaData.getCatalogs");
  ResultSet oRet_ResultSet(dbmd->getCatalogs());
  String sRetStr;
  sRetStr="";
  test_status=columnCompare(sColumnNames, oRet_ResultSet);
  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getCatalogs Failed!");
  }

  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(1) + ",");
  if (sRetStr == "") {
    logMsg("getCatalogs did not return any catalog names");
  } else {
    logMsg("The Catalog names returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetColumnPrivileges()
{

  List sColumnNames;
  sColumnNames.push_back("TABLE_CAT");
  sColumnNames.push_back("TABLE_SCHEM");
  sColumnNames.push_back("TABLE_NAME");
  sColumnNames.push_back("COLUMN_NAME");
  sColumnNames.push_back("GRANTOR");
  sColumnNames.push_back("GRANTEE");
  sColumnNames.push_back("PRIVILEGE");
  sColumnNames.push_back("IS_GRANTABLE");

  bool test_status=true;
  logMsg("Calling DatabaseMetaData.getColumnPrivileges");
  String tmp("%");
  ResultSet oRet_ResultSet(dbmd->getColumnPrivileges(sCatalogName,
                                                     sSchemaName, sFtable, tmp));
  test_status=columnCompare(sColumnNames, oRet_ResultSet);
  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getColumnPrivileges Failed!");
  }
  if (oRet_ResultSet->next()) {
    logMsg("getColumnPrivileges returned some column names");
  } else {
    logMsg("getColumnPrivileges did not return any column names");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetColumns()
{

  List sColumnNames;

  sColumnNames.push_back("TABLE_CAT");
  sColumnNames.push_back("TABLE_SCHEM");
  sColumnNames.push_back("TABLE_NAME");
  sColumnNames.push_back("COLUMN_NAME");
  sColumnNames.push_back("DATA_TYPE");
  sColumnNames.push_back("TYPE_NAME");
  sColumnNames.push_back("COLUMN_SIZE");
  sColumnNames.push_back("BUFFER_LENGTH");
  sColumnNames.push_back("DECIMAL_DIGITS");
  sColumnNames.push_back("NUM_PREC_RADIX");
  sColumnNames.push_back("NULLABLE");
  sColumnNames.push_back("REMARKS");
  sColumnNames.push_back("COLUMN_DEF");
  sColumnNames.push_back("SQL_DATA_TYPE");
  sColumnNames.push_back("SQL_DATETIME_SUB");
  sColumnNames.push_back("CHAR_OCTET_LENGTH");
  sColumnNames.push_back("ORDINAL_POSITION");
  sColumnNames.push_back("IS_NULLABLE");

  bool test_status=true;
  logMsg("Calling DatabaseMetaData.getColumns");
  String tmp("%");
  ResultSet oRet_ResultSet(dbmd->getColumns(sCatalogName, sSchemaName, tmp, tmp));
  test_status=columnCompare(sColumnNames, oRet_ResultSet);
  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getColumns Failed!");
  }
  if (oRet_ResultSet->next()) {
    logMsg("getColumns returned some column names");
  } else {
    logMsg("getColumns did not return any column names");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetCrossReference()
{

  List sColumnNames;

  sColumnNames.push_back("PKTABLE_CAT");
  sColumnNames.push_back("PKTABLE_SCHEM");
  sColumnNames.push_back("PKTABLE_NAME");
  sColumnNames.push_back("PKCOLUMN_NAME");
  sColumnNames.push_back("FKTABLE_CAT");
  sColumnNames.push_back("FKTABLE_SCHEM");
  sColumnNames.push_back("FKTABLE_NAME");
  sColumnNames.push_back("FKCOLUMN_NAME");
  sColumnNames.push_back("KEY_SEQ");
  sColumnNames.push_back("UPDATE_RULE");
  sColumnNames.push_back("DELETE_RULE");
  sColumnNames.push_back("FK_NAME");
  sColumnNames.push_back("PK_NAME");
  sColumnNames.push_back("DEFERRABILITY");

  bool test_status=true;
  logMsg("Calling DatabaseMetaData.getCrossReference");
  ResultSet oRet_ResultSet(dbmd->getCrossReference(sCatalogName,
                                                   sSchemaName, sPtable, sCatalogName, sSchemaName, sFtable));
  String sRetStr;
  sRetStr="";
  test_status=columnCompare(sColumnNames, oRet_ResultSet);
  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getCrossReference Failed!");
  }
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(8) + ",");
  if (sRetStr == "") {
    logMsg("getCrossReference did not return any columns");
  } else {
    logMsg("The columns returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetDatabaseProductName()
{

  logMsg("Calling getDatabaseProductName on DatabaseMetaData");
  String sRetValue=dbmd->getDatabaseProductName();
  if (sRetValue.empty()) {
    logMsg(
           "getDatabaseProductName method does not returns database product name ");
  } else {
    logMsg(String("getDatabaseProductName method returns:  ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetDatabaseProductVersion()
{

  logMsg("Calling getDatabaseProductVersion on DatabaseMetaData");
  String sRetValue=dbmd->getDatabaseProductVersion();
  if (sRetValue.empty()) {
    logMsg(
           "getDatabaseProductVersion  method does not returns a database product version ");
  } else {
    logMsg("getDatabaseProductVersion method returns:  "
           + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetDefaultTransactionIsolation()
{

  logMsg("Calling DatabaseMetaData.getDefaultTransactionIsolation");
  int nRetval=dbmd->getDefaultTransactionIsolation();
  if ((nRetval != sql::TRANSACTION_NONE)
      && (nRetval != sql::TRANSACTION_READ_UNCOMMITTED)
      && (nRetval != sql::TRANSACTION_READ_COMMITTED)
      && (nRetval != sql::TRANSACTION_REPEATABLE_READ)
      && (nRetval != sql::TRANSACTION_SERIALIZABLE)) {
      std::stringstream msg;
      msg << "getDefaultTransactionIsolation returns an invalid value "
          << nRetval;
    logErr(msg.str());
    FAIL("Call to getDefaultTransactionIsolation is Failed!");
  } else {
    std::stringstream msg;
    msg << "getDefaultTransactionIsolation returns a valid Isolation level "
        << nRetval;
    logMsg(msg.str());
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetDriverMajorVersion()
{
  logMsg("Calling getDriverMajorVersion on DatabaseMetaData");
  int drMajorVersion=dbmd->getDriverMajorVersion();
  if (drMajorVersion >= 0) {
    std::stringstream msg;
    msg << "getDriverMajorVersion method returns: "
        << drMajorVersion;
    logMsg(msg.str());
  } else {
    logMsg(" getDriverMajorVersion method returns a negative value");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetDriverMinorVersion()
{
  logMsg("Calling getDriverMinorVersion on DatabaseMetaData");
  int drMinorVersion=dbmd->getDriverMinorVersion();
  if (drMinorVersion >= 0) {
    std::stringstream msg;
    msg << " getDriverMinorVersion method returns: "
        << drMinorVersion;
    logMsg(msg.str());
  } else {
    std::stringstream msg;
    msg << " getDriverMinorVersion method returns a negative value: "
        << drMinorVersion;
    logMsg(msg.str());
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetDriverName()
{

  logMsg("Calling getDriverName on DatabaseMetaData");
  String sRetValue=dbmd->getDriverName();
  if (sRetValue.empty()) {
    logMsg(
           "getDriverName method does not returns a Driver Name ");
  } else {
    logMsg(String("getDriverName method returns:  ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetDriverVersion()
{

  logMsg("Calling getDriverVersion on DatabaseMetaData");
  String sRetValue=dbmd->getDriverVersion();
  if (sRetValue.empty()) {
    logMsg(
           "getDriverVersion method does not returns a Driver Version ");
  } else {
    logMsg(String("getDriverVersion returns:  ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetExportedKeys()
{

  List sColumnNames;

  sColumnNames.push_back("PKTABLE_CAT");
  sColumnNames.push_back("PKTABLE_SCHEM");
  sColumnNames.push_back("PKTABLE_NAME");
  sColumnNames.push_back("PKCOLUMN_NAME");
  sColumnNames.push_back("FKTABLE_CAT");
  sColumnNames.push_back("FKTABLE_SCHEM");
  sColumnNames.push_back("FKTABLE_NAME");
  sColumnNames.push_back("FKCOLUMN_NAME");
  sColumnNames.push_back("KEY_SEQ");
  sColumnNames.push_back("UPDATE_RULE");
  sColumnNames.push_back("DELETE_RULE");
  sColumnNames.push_back("FK_NAME");
  sColumnNames.push_back("PK_NAME");
  sColumnNames.push_back("DEFERRABILITY");

  bool test_status=true;
  logMsg("Calling DatabaseMetaData.getExportedKeys");
  ResultSet oRet_ResultSet(dbmd->getExportedKeys(sCatalogName,
                                                 sSchemaName, sPtable));
  String sRetStr;
  sRetStr="";
  test_status=columnCompare(sColumnNames, oRet_ResultSet);
  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getExportedKeys Failed!");
  }
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(8) + ",");
  if (sRetStr == "") {
    logMsg("getExportedKeys did not return any columns");
  } else {
    logMsg("The columns returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetExtraNameCharacters()
{

  logMsg("Calling getExtraNameCharacters on DatabaseMetaData");
  String sRetValue=dbmd->getExtraNameCharacters();
  if (sRetValue.empty()) {
    logMsg(
           "getExtraNameCharacters method does not returns the string containing the extra characters ");
  } else {
    logMsg(String("getExtraNameCharacters method returns: ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetIdentifierQuoteString()
{

  logMsg("Calling getIdentifierQuoteString on DatabaseMetaData");
  String sRetValue=dbmd->getIdentifierQuoteString();
  if (sRetValue == "") {
    logMsg("The database does not support quoting identifiers");
  } else /*if (dynamic_cast<String>( sRetValue ) != NULL)*/
  {
    logMsg(String("getIdentifierQuoteString method returns ") + sRetValue);
  }
  /*
  else if (sRetValue == NULL) {
                  logErr("getIdentifierQuoteString returns an Invalid value");
                  FAIL("Call to getIdentfierQuoteString is Failed!");
              }*/
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetImportedKeys()
{

  List sColumnNames;

  sColumnNames.push_back("PKTABLE_CAT");
  sColumnNames.push_back("PKTABLE_SCHEM");
  sColumnNames.push_back("PKTABLE_NAME");
  sColumnNames.push_back("PKCOLUMN_NAME");
  sColumnNames.push_back("FKTABLE_CAT");
  sColumnNames.push_back("FKTABLE_SCHEM");
  sColumnNames.push_back("FKTABLE_NAME");
  sColumnNames.push_back("FKCOLUMN_NAME");
  sColumnNames.push_back("KEY_SEQ");
  sColumnNames.push_back("UPDATE_RULE");
  sColumnNames.push_back("DELETE_RULE");
  sColumnNames.push_back("FK_NAME");
  sColumnNames.push_back("PK_NAME");
  sColumnNames.push_back("DEFERRABILITY");

  bool test_status=true;

  logMsg("Calling DatabaseMetaData.getImportedKeys");
  ResultSet oRet_ResultSet(dbmd->getImportedKeys(sCatalogName,
                                                 sSchemaName, sFtable));
  String sRetStr;
  sRetStr="";
  test_status=columnCompare(sColumnNames, oRet_ResultSet);
  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getImportedKeys Failed!");
  }
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(4) + ",");
  if (sRetStr == "") {
    logMsg("getImportedKeys did not return any columns");
  } else {
    logMsg("The columns returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetIndexInfo1()
{

  logMsg(
         "Calling DatabaseMetaData.getIndexInfo with approximate data and without accurate results");
  ResultSet oRet_ResultSet(dbmd->getIndexInfo(sCatalogName,
                                              sSchemaName, sFtable, true, true));
  String sRetStr;
  sRetStr="";
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(6) + ",");
  if (sRetStr == "") {
    logMsg("getIndexInfo did not return any index names");
  } else {
    logMsg("The index names returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetIndexInfo2()
{

  logMsg(
         "Calling DatabaseMetaData.getIndexInfo with approximate data and with accurate results");
  ResultSet oRet_ResultSet(dbmd->getIndexInfo(sCatalogName,
                                              sSchemaName, sFtable, true, false));
  String sRetStr;
  sRetStr="";
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(6) + ",");
  if (sRetStr == "") {
    logMsg("getIndexInfo did not return any index names");
  } else {
    logMsg("The index names returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetIndexInfo3()
{

  logMsg(
         "Calling DatabaseMetaData.getIndexInfo without approximate data and with accurate results");
  ResultSet oRet_ResultSet(dbmd->getIndexInfo(sCatalogName,
                                              sSchemaName, sFtable, false, false));
  String sRetStr;
  sRetStr="";
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(6) + ",");
  if (sRetStr == "") {
    logMsg("getIndexInfo did not return any index names");
  } else {
    logMsg("The index names returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetIndexInfo4()
{

  logMsg(
         "Calling DatabaseMetaData.getIndexInfo without approximate data and without accurate results");
  ResultSet oRet_ResultSet(dbmd->getIndexInfo(sCatalogName,
                                              sSchemaName, sFtable, false, true));
  String sRetStr;
  sRetStr="";
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(6) + ",");
  if (sRetStr == "") {
    logMsg("getIndexInfo did not return any index names");
  } else {
    logMsg("The index names returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetIndexInfo5()
{
  List sColumnNames;
  sColumnNames.push_back("TABLE_CAT");
  sColumnNames.push_back("TABLE_SCHEM");
  sColumnNames.push_back("TABLE_NAME");
  sColumnNames.push_back("NON_UNIQUE");
  sColumnNames.push_back("INDEX_QUALIFIER");
  sColumnNames.push_back("INDEX_NAME");
  sColumnNames.push_back("TYPE");
  sColumnNames.push_back("ORDINAL_POSITION");
  sColumnNames.push_back("COLUMN_NAME");
  sColumnNames.push_back("ASC_OR_DESC");
  sColumnNames.push_back("CARDINALITY");
  sColumnNames.push_back("PAGES");
  sColumnNames.push_back("FILTER_CONDITION");

  bool test_status=true;
  logMsg(
         "Calling DatabaseMetaData.getIndexInfo without approximate data and without accurate results");
  ResultSet oRet_ResultSet(dbmd->getIndexInfo(sCatalogName,
                                              sSchemaName, sFtable, false, true));
  test_status=columnCompare(sColumnNames, oRet_ResultSet);
  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getCrossReference Failed!");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxBinaryLiteralLength()
{
  logMsg("Calling DatabaseMetaData.getMaxBinaryLiteralLength");
  int nRetval=dbmd->getMaxBinaryLiteralLength();
  if (nRetval < 0) {
    logErr("getMaxBinaryLiteralLength returns a negative value");
  } else {
    TestsListener::messagesLog()
            << "getMaxBinaryLiteralLength returns " << nRetval << std::endl;
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxCatalogNameLength()
{
  logMsg("Calling DatabaseMetaData.getMaxCatalogNameLength");
  int nRetval=dbmd->getMaxCatalogNameLength();
  if (nRetval < 0) {
    logErr("getMaxCatalogNameLength returns a negative value");
  } else {
    TestsListener::messagesLog()
            << "getMaxCatalogNameLength returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxCharLiteralLength()
{

  logMsg("Calling DatabaseMetaData.getMaxCharLiteralLength");
  int nRetval=dbmd->getMaxCharLiteralLength();
  if (nRetval < 0) {
    logErr("getMaxCharLiteralLength returns a negative value");
  } else {
    TestsListener::messagesLog()
            << "getMaxCharLiteralLength returns " << nRetval << std::endl;
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxColumnNameLength()
{

  logMsg("Calling DatabaseMetaData.getMaxColumnNameLength");
  int nRetval=dbmd->getMaxColumnNameLength();
  if (nRetval < 0) {
    logErr("getMaxColumnNameLength returns a negative value");
  } else {
    TestsListener::messagesLog() <<
            "getMaxColumnNameLength returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxColumnsInGroupBy()
{

  logMsg("Calling DatabaseMetaData.getMaxColumnsInGroupBy");
  int nRetval=dbmd->getMaxColumnsInGroupBy();
  if (nRetval < 0) {
    logErr("getMaxColumnsInGroupBy returns a negative value");
  } else {
    TestsListener::messagesLog()
            << "getMaxColumnsInGroupBy returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxColumnsInIndex()
{

  logMsg("Calling DatabaseMetaData.getMaxColumnsInIndex");
  int nRetval=dbmd->getMaxColumnsInIndex();
  if (nRetval < 0) {
    logErr("getMaxColumnsInIndex returns a negative value");
  } else {
    TestsListener::messagesLog()
            << "getMaxColumnsInIndex returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxColumnsInOrderBy()
{

  logMsg("Calling DatabaseMetaData.getMaxColumnsInOrderBy");
  int nRetval=dbmd->getMaxColumnsInOrderBy();
  if (nRetval < 0) {
    logErr("getMaxColumnsInOrderBy returns a negative value");
  } else {
    TestsListener::messagesLog()
            << "getMaxColumnsInOrderBy returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxColumnsInSelect()
{

  logMsg("Calling DatabaseMetaData.getMaxColumnsInSelect");
  int nRetval=dbmd->getMaxColumnsInSelect();
  if (nRetval < 0) {
    logErr("getMaxColumnsInSelect returns a negative value");
  } else {
    TestsListener::messagesLog()
            << "getMaxColumnsInSelect returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxColumnsInTable()
{
  logMsg("Calling DatabaseMetaData.getMaxColumnsInTable");
  int nRetval=dbmd->getMaxColumnsInTable();
  if (nRetval < 0) {
    logErr("getMaxColumnsInTable returns a negative value");
  } else {
    TestsListener::messagesLog()
            << "getMaxColumnsInTable returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxConnections()
{

  logMsg("Calling DatabaseMetaData.getMaxConnections");
  int nRetval=dbmd->getMaxConnections();
  if (nRetval < 0) {
    logErr("getMaxConnections returns a negative value");
  } else {
    TestsListener::messagesLog()
            << "getMaxConnections returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxCursorNameLength()
{

  logMsg("Calling DatabaseMetaData.getMaxCursorNameLength");
  int nRetval=dbmd->getMaxCursorNameLength();
  if (nRetval < 0) {
    logErr("getMaxCursorNameLength returns a negative value");
  } else {
    TestsListener::messagesLog()
            << "getMaxCursorNameLength returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxIndexLength()
{

  logMsg("Calling DatabaseMetaData.getMaxIndexLength");
  int nRetval=dbmd->getMaxIndexLength();
  if (nRetval < 0) {
    logErr("getMaxIndexLength returns a negative value");
  } else {
    TestsListener::messagesLog()
            << "getMaxIndexLength returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxProcedureNameLength()
{

  logMsg("Calling DatabaseMetaData.getMaxProcedureNameLength");
  int nRetval=dbmd->getMaxProcedureNameLength();
  if (nRetval < 0) {
    logErr("getMaxProcedureNameLength returns a negative value");
  } else {
    TestsListener::messagesLog()
            << "getMaxProcedureNameLength returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxRowSize()
{

  logMsg("Calling DatabaseMetaData.getMaxRowSize");
  int nRetval=dbmd->getMaxRowSize();
  if (nRetval < 0) {
    logErr("getMaxRowSize returns a negative value");
  } else {
    TestsListener::messagesLog() << "getMaxRowSize returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxSchemaNameLength()
{

  logMsg("Calling DatabaseMetaData.getMaxSchemaNameLength");
  int nRetval=dbmd->getMaxSchemaNameLength();
  if (nRetval < 0) {
    logErr("getMaxSchemaNameLength returns a negative value");
  } else {
    TestsListener::messagesLog() << "getMaxSchemaNameLength returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxStatementLength()
{

  logMsg("Calling DatabaseMetaData.getMaxStatementLength");
  int nRetval=dbmd->getMaxStatementLength();
  if (nRetval < 0) {
    logErr("getMaxStatementLength returns a negative value");
  } else {
    TestsListener::messagesLog() << "getMaxStatementLength returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxStatements()
{

  logMsg("Calling DatabaseMetaData.getMaxStatements");
  int nRetval=dbmd->getMaxStatements();
  if (nRetval < 0) {
    logErr("getMaxStatements returns a negative value");
  } else {
    TestsListener::messagesLog() << "getMaxStatements returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxTableNameLength()
{

  logMsg("Calling DatabaseMetaData.getMaxTableNameLength");
  int nRetval=dbmd->getMaxTableNameLength();
  if (nRetval < 0) {
    logErr("getMaxTableNameLength returns a negative value");
  } else {
    TestsListener::messagesLog() << "getMaxTableNameLength returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxTablesInSelect()
{

  logMsg("Calling DatabaseMetaData.getMaxTablesInSelect");
  int nRetval=dbmd->getMaxTablesInSelect();
  if (nRetval < 0) {
    logErr("getMaxTablesInSelect returns a negative value");
  } else {
    TestsListener::messagesLog() << "getMaxTablesInSelect returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetMaxUserNameLength()
{

  logMsg("Calling DatabaseMetaData.getMaxUserNameLength");
  int nRetval=dbmd->getMaxUserNameLength();
  if (nRetval < 0) {
    logErr("getMaxUserNameLength returns a negative value");
  } else {
    TestsListener::messagesLog() << "getMaxUserNameLength returns " << nRetval << std::endl;
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetNumericFunctions()
{

  logMsg("Calling getNumericFunctions on DatabaseMetaData");
  String sRetValue=dbmd->getNumericFunctions();
  if (sRetValue.empty()) {
    logMsg(
           "getNumericFunctions method does not returns the comma-separated list of math functions ");
  } else {
    logMsg(String("getNumericFunctions method returns: ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetPrimaryKeys()
{

  List sColumnNames;

  sColumnNames.push_back("TABLE_CAT");
  sColumnNames.push_back("TABLE_SCHEM");
  sColumnNames.push_back("TABLE_NAME");
  sColumnNames.push_back("COLUMN_NAME");
  sColumnNames.push_back("KEY_SEQ");
  sColumnNames.push_back("PK_NAME");

  bool test_status=true;
  logMsg("Calling DatabaseMetaData.getPrimaryKeys");
  ResultSet oRet_ResultSet(dbmd->getPrimaryKeys(sCatalogName,
                                                sSchemaName, sFtable));
  String sRetStr;
  sRetStr="";
  test_status=columnCompare(sColumnNames, oRet_ResultSet);
  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getPrimaryKeys Failed!");
  }
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(4) + ",");
  if (sRetStr == "") {
    logMsg("getPrimaryKeys did not return any columns");
  } else {
    logMsg("The columns returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}


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
/* throws Exception */
void DatabaseMetaDataTest::testGetProcedureColumns()
{
  if (hasSps) {

    List sColumnNames;

    sColumnNames.push_back("PROCEDURE_CAT");
    sColumnNames.push_back("PROCEDURE_SCHEM");
    sColumnNames.push_back("PROCEDURE_NAME");
    sColumnNames.push_back("COLUMN_NAME");
    sColumnNames.push_back("COLUMN_TYPE");
    sColumnNames.push_back("DATA_TYPE");
    sColumnNames.push_back("TYPE_NAME");
    sColumnNames.push_back("PRECISION");
    sColumnNames.push_back("LENGTH");
    sColumnNames.push_back("SCALE");
    sColumnNames.push_back("RADIX");
    sColumnNames.push_back("NULLABLE");
    sColumnNames.push_back("REMARKS");

    bool test_status=true;

    logMsg("Calling DatabaseMetaData.getProcedureColumns");
    String tmp("%");
    ResultSet oRet_ResultSet(dbmd->getProcedureColumns(sCatalogName, sSchemaName, tmp, tmp));
    String sRetStr;
    sRetStr="";
    test_status=columnCompare(sColumnNames, oRet_ResultSet);
    if (test_status == false)
    {
      logMsg(
             "Columns return are not same either in order or name");
      FAIL("Call to getProcedureColumns Failed!");
    }
    if (oRet_ResultSet->next())
    {
      logMsg("getProcedureColumns returned some column names");
    } else {
      logMsg(
             "getProcedureColumns did not return any column names");
    }
  }
}
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

/* throws Exception */
void DatabaseMetaDataTest::testGetProcedureTerm()
{

  logMsg("Calling getProcedureTerm on DatabaseMetaData");
  String sRetValue=dbmd->getProcedureTerm();
  if (sRetValue.empty()) {
    logMsg(
           "getProcedureTerm method does not returns the vendor's preferred term for procedure ");
  } else {
    logMsg(String("getProcedureTerm method returns:  ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetProcedures()
{
  if (!this->hasSps) {
    return;
  }

  List sColumnNames;

  sColumnNames.push_back("PROCEDURE_CAT");
  sColumnNames.push_back("PROCEDURE_SCHEM");
  sColumnNames.push_back("PROCEDURE_NAME");
  sColumnNames.push_back("RESERVED1");
  sColumnNames.push_back("RESERVED2");
  sColumnNames.push_back("RESERVED3");
  sColumnNames.push_back("REMARKS");
  sColumnNames.push_back("PROCEDURE_TYPE");

  bool  statusColumnCount=  true;
  bool  statusColumnMatch=  true;
  int   iColumnNamesLength= static_cast<int>(sColumnNames.size());

  logMsg("Calling DatabaseMetaData.getProcedures");

  String tmp("%");

  ResultSet oRet_ResultSet(dbmd->getProcedures(sCatalogName, sSchemaName, tmp));
  String sRetStr;

  sRetStr="";
  ResultSetMetaData rsmd(oRet_ResultSet->getMetaData());

  int iCount=rsmd->getColumnCount();
  TestsListener::messagesLog()
          << "Minimum Column Count is:" << iColumnNamesLength << std::endl;
  if (iColumnNamesLength <= iCount) {
    iCount=iColumnNamesLength;
    statusColumnCount=true;
  } else {
    statusColumnCount=false;
  }
  logMsg("Comparing Column Names...");

  while (iColumnNamesLength > 0) {
    if ((iCount < 4) || (iCount > 6))
    {
      if (ciString(sColumnNames[iColumnNamesLength - 1].c_str())
          == rsmd->getColumnName(iCount).c_str())
      {
        statusColumnMatch=true;
      } else
      {
        statusColumnMatch=false;
        break;
      }
    }

    iCount--;
    iColumnNamesLength--;
  }
  if ((statusColumnCount == false) || (statusColumnMatch == false)) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getProcedures Failed!");
  }
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(3) + ",");
  if (sRetStr == "") {
    logMsg("getProcedures did not return any procedure names");
  } else {
    logMsg("The Procedure names returned are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetSQLKeywords()
{

  logMsg("Calling getSQLKeywords on DatabaseMetaData");
  String sRetValue=dbmd->getSQLKeywords();
  if (sRetValue.empty()) {
    logMsg(
           "getSQLKeywords method does not returns the list of SQLKeywords ");
  } else {
    logMsg(String("getSQLKeywords method returns: ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetSchemaTerm()
{
  logMsg("Calling getSchemaTerm on DatabaseMetaData");
  String sRetValue=dbmd->getSchemaTerm();
  if (sRetValue.empty()) {
    logMsg(
           "getSchemaTerm method does not returns the vendor's preferred term for schema ");
  } else {
    logMsg(String("getSchemaTerm method returns:  ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetSchemas()
{
  List sColumnNames;
  sColumnNames.push_back("TABLE_SCHEM");

  bool test_status=true;

  logMsg("Calling DatabaseMetaData.getSchemas");

  ResultSet oRet_ResultSet(dbmd->getSchemas());

  String sRetStr;
  sRetStr="";
  test_status=columnCompare(sColumnNames, oRet_ResultSet);
  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getSchemas Failed!");
  }
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(1) + ",");
  if (sRetStr == "") {
    logMsg("getSchemas did not return any schema names");
  } else {
    logMsg("The Schema names returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetSearchStringEscape()
{

  logMsg("Calling getSearchStringEscape on DatabaseMetaData");
  String sRetValue=dbmd->getSearchStringEscape();
  if (sRetValue.empty()) {
    logMsg(
           "getSearchStringEscape  method does not returns the string used to escape wildcard characters ");
  } else {
    logMsg(String("getSearchStringEscape method returns: ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetStringFunctions()
{

  logMsg("Calling getStringFunctions on DatabaseMetaData");
  String sRetValue=dbmd->getStringFunctions();
  if (sRetValue.empty()) {
    logMsg(
           "getStringFunctions method does not returns the comma-separated list of string functions ");
  } else {
    logMsg(String("getStringFunctions method returns: ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetSystemFunctions()
{

  logMsg("Calling getSystemFunctions on DatabaseMetaData");
  String sRetValue=dbmd->getSystemFunctions();
  if (sRetValue.empty()) {
    logMsg(
           "getSystemFunctions methd does not returns the comma-separated list of system functions ");
  } else {
    logMsg(String("getSystemFunctions method returns: ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetTablePrivileges()
{
  List sColumnNames;

  sColumnNames.push_back("TABLE_CAT");
  sColumnNames.push_back("TABLE_SCHEM");
  sColumnNames.push_back("TABLE_NAME");
  sColumnNames.push_back("GRANTOR");
  sColumnNames.push_back("GRANTEE");
  sColumnNames.push_back("PRIVILEGE");
  sColumnNames.push_back("IS_GRANTABLE");

  bool test_status=true;

  logMsg("Calling DatabaseMetaData.getTablePrivileges");

  ResultSet oRet_ResultSet(dbmd->getTablePrivileges(sCatalogName,
                                                    sSchemaName, sFtable));
  String sRetStr;
  sRetStr="";
  test_status=columnCompare(sColumnNames, oRet_ResultSet);
  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getTablePrivileges Failed!");
  }
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(6) + ",");
  if (sRetStr == "") {
    logMsg("getTablePrivileges did not return any privileges");
  } else {
    logMsg("The privileges returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetTableTypes()
{
  List sColumnNames;
  sColumnNames.push_back("TABLE_TYPE");

  bool test_status=true;

  logMsg("Calling DatabaseMetaData.getTableTypes");

  ResultSet oRet_ResultSet(dbmd->getTableTypes());
  String sRetStr;
  sRetStr="";
  test_status=columnCompare(sColumnNames, oRet_ResultSet);

  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getTableTypes Failed!");
  }

  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(1) + ",");

  if (sRetStr == "") {
    logMsg("getTableTypes did not return any table types");
  } else {
    logMsg("The Table Types returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetTables()
{
  List sColumnNames;

  sColumnNames.push_back("TABLE_CAT");
  sColumnNames.push_back("TABLE_SCHEM");
  sColumnNames.push_back("TABLE_NAME");
  sColumnNames.push_back("TABLE_TYPE");
  sColumnNames.push_back("REMARKS");

  bool test_status=true;

  logMsg("Calling DatabaseMetaData.getTables");
  String tmp("%");
  std::list< sql::SQLString > tmp2;
  ResultSet oRet_ResultSet(dbmd->getTables(sCatalogName, sSchemaName, tmp, tmp2));

  String sRetStr;
  sRetStr="";

  test_status=columnCompare(sColumnNames, oRet_ResultSet);

  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getTables Failed!");
  }

  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(3) + ",");

  if (sRetStr == "") {
    logMsg("getTables did not return any table names");
  } else {
    logMsg("The Table names returned are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetTimeDateFunctions()
{

  logMsg("Calling getTimeDateFunctions on DatabaseMetaData");
  String sRetValue=dbmd->getTimeDateFunctions();
  if (sRetValue.empty()) {
    logMsg(
           "getTimeDateFunctions method does not returns the comma-separated list of time and date functions ");
  } else {
    logMsg(String("getTimeDateFunctions method returns: ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetTypeInfo()
{
  List sColumnNames;

  sColumnNames.push_back("TYPE_NAME");
  sColumnNames.push_back("DATA_TYPE");
  sColumnNames.push_back("PRECISION");
  sColumnNames.push_back("LITERAL_PREFIX");
  sColumnNames.push_back("LITERAL_SUFFIX");
  sColumnNames.push_back("CREATE_PARAMS");
  sColumnNames.push_back("NULLABLE");
  sColumnNames.push_back("CASE_SENSITIVE");
  sColumnNames.push_back("SEARCHABLE");
  sColumnNames.push_back("UNSIGNED_ATTRIBUTE");
  sColumnNames.push_back("FIXED_PREC_SCALE");
  sColumnNames.push_back("AUTO_INCREMENT");
  sColumnNames.push_back("LOCAL_TYPE_NAME");
  sColumnNames.push_back("MINIMUM_SCALE");
  sColumnNames.push_back("MAXIMUM_SCALE");
  sColumnNames.push_back("SQL_DATA_TYPE");
  sColumnNames.push_back("SQL_DATETIME_SUB");
  sColumnNames.push_back("NUM_PREC_RADIX");

  bool    statusColumnMatch= true;
  bool    statusColumnCount= true;
  String  sRetStr("");

  int     iColumnNamesLength= static_cast<int>(sColumnNames.size());

  logMsg("Calling DatabaseMetaData.getTypeInfo");

  ResultSet oRet_ResultSet(dbmd->getTypeInfo());

  ResultSetMetaData rsmd(oRet_ResultSet->getMetaData());

  int iCount=rsmd->getColumnCount();
  TestsListener::messagesLog()
          << "Minimum Column Count is:" << iColumnNamesLength << std::endl;

  if (iColumnNamesLength > iCount) {
    statusColumnCount=false;
  } else if (iColumnNamesLength < iCount) {
    iCount=iColumnNamesLength;
    statusColumnCount=true;
  } else {
    statusColumnCount=true;
  }

  logMsg("Comparing Column Names...");

  while (iColumnNamesLength > 0) {
    if (ciString(sColumnNames[iColumnNamesLength - 1].c_str())
        == rsmd->getColumnName(iCount).c_str())
    {
      statusColumnMatch=true;
    } else {
      statusColumnMatch=false;
      break;
    }
    iCount--;
    iColumnNamesLength--;
  }
  if ((statusColumnMatch == false) && (statusColumnCount == true)) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getTypeInfo Failed!");
  }
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(1) + ",");
  if (sRetStr == "") {
    logMsg("getTypeInfo did not return any type names");
  } else {
    logMsg(String("The Type names returned are : ") + sRetStr);
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetUDTs()
{

  logMsg("Calling DatabaseMetaData.getUDTs");
  String tmp("%");
  std::list<int> tmp2;
  ResultSet oRet_ResultSet(dbmd->getUDTs(sCatalogName, sSchemaName, tmp, tmp2));
  String sRetStr;
  sRetStr="";
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(3) + ",");
  if (sRetStr == "") {
    logMsg("getUDTs did not return any user defined types");
  } else {
    logMsg("The type names returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetUDTs01()
{
  List sColumnNames;

  sColumnNames.push_back("TYPE_CAT");
  sColumnNames.push_back("TYPE_SCHEM");
  sColumnNames.push_back("TYPE_NAME");
  sColumnNames.push_back("CLASS_NAME");
  sColumnNames.push_back("DATA_TYPE");
  sColumnNames.push_back("REMARKS");

  bool statusColumnMatch=true;
  bool statusColumnCount=true;
  String sRetStr;

  sRetStr="";
  int iColumnNamesLength= static_cast<int>( sColumnNames.size() );
  logMsg("Calling DatabaseMetaData.getUDTs");

  String tmp("%");
  std::list<int> tmp2;

  ResultSet oRet_ResultSet(dbmd->getUDTs(sCatalogName, sSchemaName, tmp, tmp2));
  ResultSetMetaData rsmd(oRet_ResultSet->getMetaData());

  int iCount=rsmd->getColumnCount();

  TestsListener::messagesLog()
          << "Minimum Column Count is:" << iColumnNamesLength << std::endl;

  if (iColumnNamesLength > iCount) {
    statusColumnCount=false;
    logMsg("Different number of columns");
    }
    else if (iColumnNamesLength < iCount)
    {
        iCount = iColumnNamesLength;
    }

  logMsg("Comparing Column Names...");

  while (iColumnNamesLength > 0) {
    if (ciString(sColumnNames[iColumnNamesLength - 1].c_str())
        == rsmd->getColumnName(iCount).c_str())
    {
      statusColumnMatch=true;
    } else {
      statusColumnMatch=false;
      logMsg("Different columns, dumping expected and returned");
      logMsg(sColumnNames[iColumnNamesLength - 1]);
      logMsg(rsmd->getColumnName(iCount));
      break;
        }
        --iCount;
        --iColumnNamesLength;
    }

  if ((statusColumnMatch == false) && (statusColumnCount == true)) {
    logMsg("Column names or order wrong.");
    FAIL("Call to getUDTs Failed!");
  }

  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(3) + ",");
  if (sRetStr == "") {
    logMsg("getUDTs did not return any user defined types");
  } else {
    logMsg("The type names returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}


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
/* throws Exception */
void DatabaseMetaDataTest::testGetURL()
{
  logMsg("Calling getURL on DatabaseMetaData");
  String sRetValue=dbmd->getURL();
  if (sRetValue.empty()) {
    logMsg("getURL method return a NULL value ");
  } else {
    logMsg(String("getURL method returns:  ") + sRetValue);
  }
}
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

/* throws Exception */
void DatabaseMetaDataTest::testGetUserName()
{

  logMsg("Calling getUserName on DatabaseMetaData");
  String sRetValue=dbmd->getUserName();
  if (sRetValue.empty()) {
    logMsg("getUserName method does not returns user name ");
  } else {
    logMsg(String("getUserName method returns: ") + sRetValue);
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testGetVersionColumns()
{

  List sColumnNames;

  sColumnNames.push_back("SCOPE");
  sColumnNames.push_back("COLUMN_NAME");
  sColumnNames.push_back("DATA_TYPE");
  sColumnNames.push_back("TYPE_NAME");
  sColumnNames.push_back("COLUMN_SIZE");
  sColumnNames.push_back("BUFFER_LENGTH");
  sColumnNames.push_back("DECIMAL_DIGITS");
  sColumnNames.push_back("PSEUDO_COLUMN");

  bool test_status=true;

  logMsg("Calling DatabaseMetaData.getVersionColumns");

  ResultSet oRet_ResultSet(dbmd->getVersionColumns(sCatalogName,
                                                   sSchemaName, sFtable));

  String sRetStr;
  sRetStr="";
  test_status=columnCompare(sColumnNames, oRet_ResultSet);
  if (test_status == false) {
    logMsg("Columns return are not same either in order or name");
    FAIL("Call to getVersionColumns Failed!");
  }
  while (oRet_ResultSet->next())
    sRetStr+=(oRet_ResultSet->getString(2) + ",");
  if (sRetStr == "") {
    logMsg("getVersionColumns did not return any columns");
  } else {
    logMsg("The columns returned Are : "
           + sRetStr.substr(0, sRetStr.length() - 1));
  }


}

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
 *                    result set type as sql::ResultSet::TYPE_FORWARD_ONLY.
 *                    It should return a boolean value; either or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testInsertsAreDetected1()
{

  logMsg(
         "Calling DatabaseMetaData.insertsAreDetected(TYPE_FORWARD_ONLY)");
  bool retValue=dbmd->insertsAreDetected(sql::ResultSet::TYPE_FORWARD_ONLY);
  if (retValue) {
    logMsg(
           "Visible row insert can be detected for TYPE_FORWARD_ONLY");
  } else {
    logMsg(
           "Visible row insert cannot be detected for TYPE_FORWARD_ONLY");
  }


}

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
 *                    result set type as sql::ResultSet::TYPE_SCROLL_INSENSITIVE.
 *                    It should return a boolean value; either or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testInsertsAreDetected2()
{

  logMsg(
         "Calling DatabaseMetaData.insertsAreDetected(TYPE_SCROLL_INSENSITIVE)");
  bool retValue=dbmd->insertsAreDetected(sql::ResultSet::TYPE_SCROLL_INSENSITIVE);
  if (retValue) {
    logMsg(
           "Visible row insert can be detected for TYPE_SCROLL_INSENSITIVE");
  } else {
    logMsg(
           "Visible row insert cannot be detected for TYPE_SCROLL_INSENSITIVE");
  }


}

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
 *                    result set type as sql::ResultSet::TYPE_SCROLL_SENSITIVE.
 *                    It should return a boolean value; either or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testInsertsAreDetected3()
{

  logMsg(
         "Calling DatabaseMetaData.insertsAreDetected(TYPE_SCROLL_SENSITIVE)");
  bool retValue=dbmd->insertsAreDetected(sql::ResultSet::TYPE_SCROLL_SENSITIVE);
  if (retValue) {
    logMsg(
           "Visible row insert can be detected for TYPE_SCROLL_SENSITIVE");
  } else {
    logMsg(
           "Visible row insert cannot be detected for TYPE_SCROLL_SENSITIVE");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testIsCatalogAtStart()
{

  logMsg("Calling isCatalogAtStart on DatabaseMetaData");
  bool retValue=dbmd->isCatalogAtStart();
  if (retValue) {
    logMsg(
           "isCatalogAtStart metohd returns catalog appear at the start");
  } else {
    logMsg(
           "isCatalogAtStart metohd returns catalog appear at the end");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testIsReadOnly()
{

  logMsg("Calling isReadOnly on DatabaseMetaData");
  bool retValue=dbmd->isReadOnly();
  if (retValue) {
    logMsg("IsReadOnly method is in read-only mode");
  } else {
    logMsg("IsReadOnly method is not in read-only mode");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testNullPlusNonNullIsNull()
{
  logMsg("Calling nullPlusNonNullIsNull on DatabaseMetaData");

  bool retValue=dbmd->nullPlusNonNullIsNull();

  if (retValue) {
    logMsg(
           "nullPlusNonNullIsNull method returns a NULL value for the concatenations between NULL and non-NULL");
  } else {
    logMsg(
           "nullPlusNonNullIsNull method does not returns a NULL value for the concatenations between NULL and non-NULL");
    FAIL("nullPlusNonNullIsNull method should always return true!");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testNullsAreSortedAtEnd()
{

  logMsg("Calling NullsAreSortedAtEnd on DatabaseMetaData");
  bool retValue=dbmd->nullsAreSortedAtEnd();
  if (retValue) {
    logMsg(
           "nullsAreSortedAtEnd method returns NULL values sorted at the end");
  } else {
    logMsg(
           "nullsAreSortedAtEnd method returns NULL values not sorted at the end");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testNullsAreSortedAtStart()
{

  logMsg("Calling nullsAreSortedAtStart on DatabaseMetaData");
  bool retValue=dbmd->nullsAreSortedAtStart();
  if (retValue) {
    logMsg(
           "nullsAreSortedAtStart method returns NULL values sorted at the start");
  } else {
    logMsg(
           "nullsAreSortedAtStart method returns NULL values not sorted at the start");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testNullsAreSortedHigh()
{

  logMsg("Calling nullsAreSortedHigh on DatabaseMetaData");
  bool retValue=dbmd->nullsAreSortedHigh();
  if (retValue) {
    logMsg(
           "nullsAreSortedHigh method returns NULL values sorted  high");
  } else {
    logMsg(
           "nullsAreSortedHigh method returns NULL values not sorted high");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testNullsAreSortedLow()
{
  logMsg("Calling nullsAreSortedLow on DatabaseMetaData");
  bool retValue=dbmd->nullsAreSortedLow();
  if (retValue) {
    logMsg(
           "nullsAreSortedLow method returns NULL values sorted low");
  } else {
    logMsg(
           "nullsAreSortedLow method returns NULL values not sorted low");
  }
}

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
 *                    object with sql::ResultSet::TYPE_FORWARD_ONLY.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOthersDeletesAreVisible1()
{

  logMsg(
         "Calling DatabaseMetaData.othersDeletesAreVisible(TYPE_FORWARD_ONLY)");
  bool retValue=dbmd->othersDeletesAreVisible(sql::ResultSet::TYPE_FORWARD_ONLY);
  if (retValue) {
    logMsg(
           "Deletes made by others are visible for TYPE_FORWARD_ONLY");
  } else {
    logMsg(
           "Deletes made by others are not visible for TYPE_FORWARD_ONLY");
  }


}

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
 *                    object with sql::ResultSet::TYPE_SCROLL_INSENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOthersDeletesAreVisible2()
{

  logMsg(
         "Calling DatabaseMetaData.othersDeletesAreVisible(TYPE_SCROLL_INSENSITIVE)");
  bool retValue=dbmd->othersDeletesAreVisible(sql::ResultSet::TYPE_SCROLL_INSENSITIVE);
  if (retValue) {
    logMsg(
           "Deletes made by others are visible for TYPE_SCROLL_INSENSITIVE");
  } else {
    logMsg(
           "Deletes made by others are not visible for TYPE_SCROLL_INSENSITIVE");
  }


}

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
 *                    object with sql::ResultSet::TYPE_SCROLL_SENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOthersDeletesAreVisible3()
{

  logMsg(
         "Calling DatabaseMetaData.othersDeletesAreVisible(TYPE_SCROLL_SENSITIVE)");
  bool retValue=dbmd->othersDeletesAreVisible(sql::ResultSet::TYPE_SCROLL_SENSITIVE);
  if (retValue) {
    logMsg(
           "Deletes made by others are visible for TYPE_SCROLL_SENSITIVE");
  } else {
    logMsg(
           "Deletes made by others are not visible for TYPE_SCROLL_SENSITIVE");
  }


}

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
 *                    object with sql::ResultSet::TYPE_FORWARD_ONLY.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOthersInsertsAreVisible1()
{

  logMsg(
         "Calling DatabaseMetaData.othersInsertsAreVisible(TYPE_FORWARD_ONLY)");
  bool retValue=dbmd->othersInsertsAreVisible(sql::ResultSet::TYPE_FORWARD_ONLY);
  if (retValue) {
    logMsg(
           "Inserts made by others are visible for TYPE_FORWARD_ONLY");
  } else {
    logMsg(
           "Inserts made by others are not visible for TYPE_FORWARD_ONLY");
  }


}

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
 *                    object with sql::ResultSet::TYPE_SCROLL_INSENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOthersInsertsAreVisible2()
{

  logMsg(
         "Calling DatabaseMetaData.othersInsertsAreVisible(TYPE_SCROLL_INSENSITIVE)");
  bool retValue=dbmd->othersInsertsAreVisible(sql::ResultSet::TYPE_SCROLL_INSENSITIVE);
  if (retValue) {
    logMsg(
           "Inserts made by others are visible for TYPE_SCROLL_INSENSITIVE");
  } else {
    logMsg(
           "Inserts made by others are not visible for TYPE_SCROLL_INSENSITIVE");
  }


}

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
 *                    object with sql::ResultSet::TYPE_SCROLL_SENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOthersInsertsAreVisible3()
{

  logMsg(
         "Calling DatabaseMetaData.othersInsertsAreVisible(TYPE_SCROLL_SENSITIVE)");
  bool retValue=dbmd->othersInsertsAreVisible(sql::ResultSet::TYPE_SCROLL_SENSITIVE);
  if (retValue) {
    logMsg(
           "Inserts made by others are visible for TYPE_SCROLL_SENSITIVE");
  } else {
    logMsg(
           "Inserts made by others are not visible for TYPE_SCROLL_SENSITIVE");
  }


}

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
 *                    object with sql::ResultSet::TYPE_FORWARD_ONLY.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOthersUpdatesAreVisible1()
{

  logMsg(
         "Calling DatabaseMetaData.othersUpdatesAreVisible(TYPE_FORWARD_ONLY)");
  bool retValue=dbmd->othersUpdatesAreVisible(sql::ResultSet::TYPE_FORWARD_ONLY);
  if (retValue) {
    logMsg(
           "Updates made by others are visible for TYPE_FORWARD_ONLY");
  } else {
    logMsg(
           "Updates made by others are not visible for TYPE_FORWARD_ONLY");
  }


}

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
 *                    object with sql::ResultSet::TYPE_SCROLL_INSENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOthersUpdatesAreVisible2()
{

  logMsg(
         "Calling DatabaseMetaData.othersUpdatesAreVisible(TYPE_SCROLL_INSENSITIVE)");
  bool retValue=dbmd->othersUpdatesAreVisible(sql::ResultSet::TYPE_SCROLL_INSENSITIVE);
  if (retValue) {
    logMsg(
           "Updates made by others are visible for TYPE_SCROLL_INSENSITIVE");
  } else {
    logMsg(
           "Updates made by others are not visible for TYPE_SCROLL_INSENSITIVE");
  }


}

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
 *                    object with sql::ResultSet::TYPE_SCROLL_SENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOthersUpdatesAreVisible3()
{

  logMsg(
         "Calling DatabaseMetaData.othersUpdatesAreVisible(TYPE_SCROLL_SENSITIVE)");
  bool retValue=dbmd->othersUpdatesAreVisible(sql::ResultSet::TYPE_SCROLL_SENSITIVE);
  if (retValue) {
    logMsg(
           "Updates made by others are visible for TYPE_SCROLL_SENSITIVE");
  } else {
    logMsg(
           "Updates made by others are not visible for TYPE_SCROLL_SENSITIVE");
  }


}

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
 *                    with sql::ResultSet::TYPE_FORWARD_ONLY.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOwnDeletesAreVisible1()
{

  logMsg(
         "Calling DatabaseMetaData.ownDeletesAreVisible(TYPE_FORWARD_ONLY)");
  bool retValue=dbmd->ownDeletesAreVisible(sql::ResultSet::TYPE_FORWARD_ONLY);
  if (retValue) {
    logMsg(
           "Result Set's own deletes are visible for TYPE_FORWARD_ONLY");
  } else {
    logMsg(
           "Result Set's own deletes are not visible for TYPE_FORWARD_ONLY");
  }


}

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
 *                    with sql::ResultSet::TYPE_SCROLL_INSENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOwnDeletesAreVisible2()
{

  logMsg(
         "Calling DatabaseMetaData.ownDeletesAreVisible(TYPE_SCROLL_INSENSITIVE)");
  bool retValue=dbmd->ownDeletesAreVisible(sql::ResultSet::TYPE_SCROLL_INSENSITIVE);
  if (retValue) {
    logMsg(
           "Result Set's own deletes are visible for TYPE_SCROLL_INSENSITIVE");
  } else {
    logMsg(
           "Result Set's own deletes are not visible for TYPE_SCROLL_INSENSITIVE");
  }


}

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
 *                    with sql::ResultSet::TYPE_SCROLL_SENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOwnDeletesAreVisible3()
{

  logMsg(
         "Calling DatabaseMetaData.ownDeletesAreVisible(TYPE_SCROLL_SENSITIVE)");
  bool retValue=dbmd->ownDeletesAreVisible(sql::ResultSet::TYPE_SCROLL_SENSITIVE);
  if (retValue) {
    logMsg(
           "Result Set's own deletes are visible for TYPE_SCROLL_SENSITIVE");
  } else {
    logMsg(
           "Result Set's own deletes are not visible for TYPE_SCROLL_SENSITIVE");
  }


}

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
 *                    with sql::ResultSet::TYPE_FORWARD_ONLY.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOwnInsertsAreVisible1()
{

  logMsg(
         "Calling DatabaseMetaData.ownInsertsAreVisible(TYPE_FORWARD_ONLY)");
  bool retValue=dbmd->ownInsertsAreVisible(sql::ResultSet::TYPE_FORWARD_ONLY);
  if (retValue) {
    logMsg(
           "Result Set's own inserts are visible for TYPE_FORWARD_ONLY");
  } else {
    logMsg(
           "Result Set's own inserts are not visible for TYPE_FORWARD_ONLY");
  }


}

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
 *                    with sql::ResultSet::TYPE_SCROLL_INSENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOwnInsertsAreVisible2()
{

  logMsg(
         "Calling DatabaseMetaData.ownInsertsAreVisible(TYPE_SCROLL_INSENSITIVE)");
  bool retValue=dbmd->ownInsertsAreVisible(sql::ResultSet::TYPE_SCROLL_INSENSITIVE);
  if (retValue) {
    logMsg(
           "Result Set's own inserts are visible for TYPE_SCROLL_INSENSITIVE");
  } else {
    logMsg(
           "Result Set's own inserts are not visible for TYPE_SCROLL_INSENSITIVE");
  }


}

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
 *                    with sql::ResultSet::TYPE_SCROLL_SENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOwnInsertsAreVisible3()
{

  logMsg(
         "Calling DatabaseMetaData.ownInsertsAreVisible(TYPE_SCROLL_SENSITIVE)");
  bool retValue=dbmd->ownInsertsAreVisible(sql::ResultSet::TYPE_SCROLL_SENSITIVE);
  if (retValue) {
    logMsg(
           "Result Set's own inserts are visible for TYPE_SCROLL_SENSITIVE");
  } else {
    logMsg(
           "Result Set's own inserts are not visible for TYPE_SCROLL_SENSITIVE");
  }


}

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
 *                    with sql::ResultSet::TYPE_FORWARD_ONLY.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOwnUpdatesAreVisible1()
{

  logMsg(
         "Calling DatabaseMetaData.ownUpdatesAreVisible(TYPE_FORWARD_ONLY)");
  bool retValue=dbmd->ownUpdatesAreVisible(sql::ResultSet::TYPE_FORWARD_ONLY);
  if (retValue) {
    logMsg(
           "Result Set's own updates are visible for TYPE_FORWARD_ONLY");
  } else {
    logMsg(
           "Result Set's own updates are not visible for TYPE_FORWARD_ONLY");
  }


}

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
 *                    with sql::ResultSet::TYPE_SCROLL_INSENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOwnUpdatesAreVisible2()
{

  logMsg(
         "Calling DatabaseMetaData.ownUpdatesAreVisible(TYPE_SCROLL_INSENSITIVE)");
  bool retValue=dbmd->ownUpdatesAreVisible(sql::ResultSet::TYPE_SCROLL_INSENSITIVE);
  if (retValue) {
    logMsg(
           "Result Set's own updates are visible for TYPE_SCROLL_INSENSITIVE");
  } else {
    logMsg(
           "Result Set's own updates are not visible for TYPE_SCROLL_INSENSITIVE");
  }


}

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
 *                    with sql::ResultSet::TYPE_SCROLL_SENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testOwnUpdatesAreVisible3()
{

  logMsg(
         "Calling DatabaseMetaData.ownUpdatesAreVisible(TYPE_SCROLL_SENSITIVE)");
  bool retValue=dbmd->ownUpdatesAreVisible(sql::ResultSet::TYPE_SCROLL_SENSITIVE);
  if (retValue) {
    logMsg(
           "Result Set's own updates are visible for TYPE_SCROLL_SENSITIVE");
  } else {
    logMsg(
           "Result Set's own updates are not visible for TYPE_SCROLL_SENSITIVE");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testStoresLowerCaseIdentifiers()
{
  logMsg("Calling storesLowerCaseIdentifiers on DatabaseMetaData");
  bool retValue=dbmd->storesLowerCaseIdentifiers();
  if (retValue) {
    logMsg(
           "storesLowerCaseIdentifiers method returns unquoted SQL identifiers stored as lower case");
  } else {
    logMsg(
           "storesLowerCaseIdentifiers returns unquoted SQL identifiers not stored as lower case");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testStoresLowerCaseQuotedIdentifiers()
{

  logMsg(
         "Calling storesLowerCaseQuotedIdentifiers on DatabaseMetaData");
  bool retValue=dbmd->storesLowerCaseQuotedIdentifiers();
  if (retValue) {
    logMsg(
           "storesLowerCaseQuotedIdentifiers method returns SQL identifiers stored as lower case");
  } else {
    logMsg(
           "storesLowerCaseQuotedIdentifiers method returns SQL identifiers not stored as lower case");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testStoresMixedCaseIdentifiers()
{

  logMsg("Calling storesMixedCaseIdentifiers on DatabaseMetaData");
  bool retValue=dbmd->storesMixedCaseIdentifiers();
  if (retValue) {
    logMsg(
           "storesMixedCaseIdentifiers method returns unquoted SQL identifiers stored as mixed case");
  } else {
    logMsg(
           "storesMixedCaseIdentifiers method returns unquoted SQL identifiers not stored as mixed case");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testStoresMixedCaseQuotedIdentifiers()
{

  logMsg(
         "Calling storesMixedCaseQuotedIdentifiers on DatabaseMetaData");
  bool retValue=dbmd->storesMixedCaseQuotedIdentifiers();
  if (retValue) {
    logMsg(
           "storesMixedCaseQuotedIdentifiers method returns SQL identifiers stored as mixed case");
  } else {
    logMsg(
           "storesMixedCaseQuotedIdentifiers method returns SQL identifiers not stored as mixed case");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testStoresUpperCaseIdentifiers()
{

  logMsg("Calling storesUpperCaseIdentifiers on DatabaseMetaData");
  bool retValue=dbmd->storesUpperCaseIdentifiers();
  if (retValue) {
    logMsg(
           "storesUpperCaseIdentifiers method returns unquoted SQL identifiers stored as upper case");
  } else {
    logMsg(
           "storesUpperCaseIdentifiers method returns unquoted SQL identifiers not stored as upper case");
  }

}

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

/* throws Exception */
void DatabaseMetaDataTest::testStoresUpperCaseQuotedIdentifiers()
{

  logMsg(
         "Calling storesUpperCaseQuotedIdentifiers on DatabaseMetaData");
  bool retValue=dbmd->storesUpperCaseQuotedIdentifiers();
  if (retValue) {
    logMsg(
           "storesUpperCaseQuotedIdentifiers method returns SQL identifiers stored as upper case");
  } else {
    logMsg(
           "storesUpperCaseQuotedIdentifiers method returns SQL identifiers not stored as upper case");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsANSI92EntryLevelSQL()
{

  logMsg("Calling supportsANSI92EntryLevelSQL on DatabaseMetaData");
  bool retValue=dbmd->supportsANSI92EntryLevelSQL();
  if (retValue) {
    logMsg("supportsANSI92EntryLevelSQL method is supported");
  } else {
    logMsg("supportsANSI92EntryLevelSQL method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsANSI92FullSQL()
{

  bool retValue=dbmd->supportsANSI92FullSQL();
  logMsg("Calling supportsANSI92FullSQL on DatabaseMetaData");
  if (retValue) {
    logMsg("supportsANSI92FullSQL method is supported");
  } else {
    logMsg("supportsANSI92FullSQL method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsANSI92IntermediateSQL()
{

  logMsg("Calling supportsANSI92IntermediateSQL on DatabaseMetaData");
  bool retValue=dbmd->supportsANSI92IntermediateSQL();
  if (retValue) {
    logMsg("supportsANSI92IntermediateSQL method is supported");
  } else {
    logMsg("supportsANSI92IntermediateSQL method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsAlterTableWithAddColumn()
{

  logMsg(
         "Calling supportsAlterTableWithAddColumn on DatabaseMetaData");
  bool retValue=dbmd->supportsAlterTableWithAddColumn();
  if (retValue) {
    logMsg("supportsAlterTableWithAddColumn is supported");
  } else {
    logMsg("supportsAlterTableWithAddColumn is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsAlterTableWithDropColumn()
{

  logMsg(
         "Calling supportsAlterTableWithDropColumn on DatabaseMetaData");
  bool retValue=dbmd->supportsAlterTableWithDropColumn();
  if (retValue) {
    logMsg("supportsAlterTableWithDropColumn is supported");
  } else {
    logMsg("supportsAlterTableWithDropColumn is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsBatchUpdates()
{

  logMsg("Calling DatabaseMetaData.supportsBatchUpdates");
  bool retValue=dbmd->supportsBatchUpdates();
  if (retValue) {
    logMsg("supportsBatchUpdates is supported");
  } else {
    logMsg("supportsBatchUpdates is not supported");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsCatalogsInDataManipulation()
{

  logMsg(
         "Calling DatabaseMetaData.supportsCatalogsInDataManipulation");
  bool retValue=dbmd->supportsCatalogsInDataManipulation();
  if (retValue) {
    logMsg("supportsCatalogsInDataManipulation is supported");
  } else {
    logMsg("supportsCatalogsInDataManipulation is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsCatalogsInIndexDefinitions()
{

  logMsg(
         "Calling DatabaseMetaData.supportsCatalogsInIndexDefinitions");
  bool retValue=dbmd->supportsCatalogsInIndexDefinitions();
  if (retValue) {
    logMsg("supportsCatalogsInIndexDefinitions is supported");
  } else {
    logMsg("supportsCatalogsInIndexDefinitions is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsCatalogsInPrivilegeDefinitions()
{

  logMsg(
         "Calling DatabaseMetaData.supportsCatalogsInPrivilegeDefinitions");
  bool retValue=dbmd->supportsCatalogsInPrivilegeDefinitions();
  if (retValue) {
    logMsg("supportsCatalogsInPrivilegeDefinitions is supported");
  } else {
    logMsg(
           "supportsCatalogsInPrivilegeDefinitions is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsCatalogsInProcedureCalls()
{

  logMsg("Calling DatabaseMetaData.supportsCatalogsInProcedureCalls");
  bool retValue=dbmd->supportsCatalogsInProcedureCalls();
  if (retValue) {
    logMsg("supportsCatalogsInProcedureCalls is supported");
  } else {
    logMsg("supportsCatalogsInProcedureCalls is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsCatalogsInTableDefinitions()
{

  logMsg(
         "Calling DatabaseMetaData.supportsCatalogsInTableDefinitions()");
  bool retValue=dbmd->supportsCatalogsInTableDefinitions();
  if (retValue) {
    logMsg("supportsCatalogsInTableDefinitions is supported");
  } else {
    logMsg("supportsCatalogsInTableDefinitions is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsColumnAliasing()
{

  logMsg("Calling supportsColumnAliasing on DatabaseMetaData");
  bool retValue=dbmd->supportsColumnAliasing();
  if (retValue) {
    logMsg("supportsColumnAliasing is supported");
  } else {
    logMsg("supportsColumnAliasing is not supported");
    FAIL("supportsColumnAliasing should always return true!");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert()
{

  logMsg("Calling supportsConvert on DatabaseMetaData");
  bool retValue=dbmd->supportsConvert();
  if (retValue) {
    logMsg("supportsConvert method is supported");
  } else {
    logMsg("supportsConvert method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert01()
{
  logMsg(
         "Calling supportsConvert(ARRAY, VARCHAR) on DatabaseMetaData");

#ifdef WE_HAVE_ALL_DATATYPES_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::ARRAY
                                      , sql::DataType::VARCHAR);

  if (retValue) {
    logMsg("supportsConvert(ARRAY, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(ARRAY, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(ARRAY, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert02()
{
  logMsg("Calling supportsConvert(BIGINT, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::BIGINT, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(BIGINT, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(BIGINT, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(BIGINT, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert03()
{

#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::BINARY, sql::DataType::VARCHAR);
  logMsg(
         "Calling supportsConvert(BINARY, VARCHAR) on DatabaseMetaData");
  if (retValue) {
    logMsg("supportsConvert(BINARY, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(BINARY, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(BINARY, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert04()
{
  logMsg("Calling supportsConvert(BIT, VARCHAR) on DatabaseMetaData");
  bool retValue=dbmd->supportsConvert(sql::DataType::BIT, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(BIT, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(BIT, VARCHAR) method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert05()
{

  logMsg("Calling supportsConvert(BLOB, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_ALL_DATATYPES_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::BLOB, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(BLOB, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(BLOB, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(BLOB, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert06()
{

  logMsg("Calling supportsConvert(CHAR, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::CHAR, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(CHAR, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(CHAR, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(CHAR, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert07()
{

  logMsg("Calling supportsConvert(CLOB, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_ALL_DATATYPES_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::CLOB, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(CLOB, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(CLOB, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(CLOB, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert08()
{

  logMsg("Calling supportsConvert(DATE, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::DATE, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(DATE, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(DATE, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(DATE, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert09()
{

  logMsg("Calling supportsConvert(DECIMAL, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::DECIMAL, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(DECIMAL, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(DECIMAL, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(DECIMAL, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert10()
{

  logMsg(
         "Calling supportsConvert(DISTINCT, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_ALL_DATATYPES_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::DISTINCT,
                                      sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(DISTINCT, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(DISTINCT, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(DISTINCT, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert11()
{

  logMsg(
         "Calling supportsConvert(DOUBLE, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::DOUBLE, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(DOUBLE, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(DOUBLE, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(DOUBLE, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert12()
{

  logMsg(
         "Calling supportsConvert(FLOAT, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_ALL_DATATYPES_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::FLOAT, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(FLOAT, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(FLOAT, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(FLOAT, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert13()
{

  logMsg(
         "Calling supportsConvert(INTEGER, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::INTEGER, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(INTEGER, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(INTEGER, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(INTEGER, VARCHAR) method is not supported");
#endif
}

/*
 * @testName:         testSupportsConvert14
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
 *                    and call the supportsConvert(JAVA_OBJECT, VARCHAR) method on that object.
 *                    It should return a boolean value; either true or false.
 *
 */

/* Doesn't make sense in C/C++*/
/*
  void DatabaseMetaDataTest::testSupportsConvert14() {

            logMsg(
                "Calling supportsConvert(JAVA_OBJECT, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
            bool retValue = dbmd->supportsConvert(sql::DataType::JAVA_OBJECT,
                    sql::DataType::VARCHAR);
            if (retValue) {
                logMsg(
                    "supportsConvert(JAVA_OBJECT, VARCHAR) method is supported");
            } else {
                logMsg("supportsConvert(JAVA_OBJECT, VARCHAR) method is not supported");
            }
#else
                logMsg("supportsConvert(JAVA_OBJECT, VARCHAR) method is not supported");
#endif
  }
 */


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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert15()
{

  logMsg(
         "Calling supportsConvert(LONGVARBINARY, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::LONGVARBINARY,
                                      sql::DataType::VARCHAR);
  if (retValue) {
    logMsg(
           "supportsConvert(LONGVARBINARY, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(LONGVARBINARY, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(LONGVARBINARY, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert16()
{

  logMsg(
         "Calling supportsConvert(LONGVARCHAR, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::LONGVARCHAR,
                                      sql::DataType::VARCHAR);
  if (retValue) {
    logMsg(
           "supportsConvert(LONGVARCHAR, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(LONGVARCHAR, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(LONGVARCHAR, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert17()
{
  logMsg("Calling supportsConvert(NULL, VARCHAR) on DatabaseMetaData");


#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::SQLNULL, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(NULL, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(NULL, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(NULL, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert18()
{

  logMsg(
         "Calling supportsConvert(NUMERIC, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::NUMERIC, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(NUMERIC, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(NUMERIC, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(NUMERIC, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert19()
{

  logMsg(
         "Calling supportsConvert(OTHER, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_ALL_DATATYPES_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::OTHER, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(OTHER, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(OTHER, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(OTHER, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert20()
{

  logMsg("Calling supportsConvert(REAL, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::REAL, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(REAL, VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(REAL, VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(REAL, VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert21()
{

  logMsg("Calling supportsConvert(REF, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_ALL_DATATYPES_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::REF, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(REF VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(REF VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(REF VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert22()
{

  logMsg("Calling supportsConvert(SMALLINT, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::SMALLINT,
                                      sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(SMALLINT VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(SMALLINT VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(SMALLINT VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert23()
{

  logMsg("Calling supportsConvert(STRUCT, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_ALL_DATATYPES_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::STRUCT, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(STRUCT VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(STRUCT VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(STRUCT VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert24()
{

  logMsg("Calling supportsConvert(TIME, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::TIME, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(TIME VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(TIME VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(TIME VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert25()
{

  logMsg("Calling supportsConvert(TIMESTAMP, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::TIMESTAMP,
                                      sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(TIMESTAMP VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(TIMESTAMP VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(TIMESTAMP VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert26()
{

  logMsg("Calling supportsConvert(TINYINT, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::TINYINT, sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(TINYINT VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(TINYINT VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(TINYINT VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert27()
{

  logMsg("Calling supportsConvert(VARBINARY, VARCHAR) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::VARBINARY,
                                      sql::DataType::VARCHAR);
  if (retValue) {
    logMsg("supportsConvert(VARBINARY VARCHAR) method is supported");
  } else {
    logMsg("supportsConvert(VARBINARY VARCHAR) method is not supported");
  }
#else
  logMsg("supportsConvert(VARBINARY VARCHAR) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert28()
{

  logMsg("Calling supportsConvert(BIGINT, INTEGER) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::BIGINT, sql::DataType::INTEGER);
  if (retValue) {
    logMsg("supportsConvert(BIGINT, INTEGER) method is supported");
  } else {
    logMsg("supportsConvert(BIGINT, INTEGER) method is not supported");
  }
#else
  logMsg("supportsConvert(BIGINT, INTEGER) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert29()
{

  logMsg("Calling supportsConvert(BIT, INTEGER) on DatabaseMetaData");

  bool retValue=dbmd->supportsConvert(sql::DataType::BIT, sql::DataType::INTEGER);
  if (retValue) {
    logMsg("supportsConvert(BIT, INTEGER) method is supported");
  } else {
    logMsg("supportsConvert(BIT, INTEGER) method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert30()
{

  logMsg("Calling supportsConvert(DATE, INTEGER) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::DATE, sql::DataType::INTEGER);
  if (retValue) {
    logMsg("supportsConvert(DATE, INTEGER) method is supported");
  } else {
    logMsg("supportsConvert(DATE, INTEGER) method is not supported");
  }
#else
  logMsg("supportsConvert(DATE, INTEGER) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert31()
{

  logMsg("Calling supportsConvert(DECIMAL, INTEGER) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::DECIMAL, sql::DataType::INTEGER);
  if (retValue) {
    logMsg("supportsConvert(DECIMAL, INTEGER) method is supported");
  } else {
    logMsg("supportsConvert(DECIMAL, INTEGER) method is not supported");
  }
#else
  logMsg("supportsConvert(DECIMAL, INTEGER) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert32()
{

  logMsg("Calling supportsConvert(DOUBLE, INTEGER) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::DOUBLE, sql::DataType::INTEGER);
  if (retValue) {
    logMsg("supportsConvert(DOUBLE, INTEGER) method is supported");
  } else {
    logMsg("supportsConvert(DOUBLE, INTEGER) method is not supported");
  }
#else
  logMsg("supportsConvert(DOUBLE, INTEGER) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert33()
{

  logMsg("Calling supportsConvert(FLOAT, INTEGER) on DatabaseMetaData");
#ifdef WE_HAVE_ALL_DATATYPES_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::FLOAT, sql::DataType::INTEGER);
  if (retValue) {
    logMsg("supportsConvert(FLOAT, INTEGER) method is supported");
  } else {
    logMsg("supportsConvert(FLOAT, INTEGER) method is not supported");
  }
#else
  logMsg("supportsConvert(FLOAT, INTEGER) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert34()
{

  logMsg("Calling supportsConvert(NUMERIC, INTEGER) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::NUMERIC, sql::DataType::INTEGER);
  if (retValue) {
    logMsg("supportsConvert(NUMERIC, INTEGER) method is supported");
  } else {
    logMsg("supportsConvert(NUMERIC, INTEGER) method is not supported");
  }
#else
  logMsg("supportsConvert(NUMERIC, INTEGER) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert35()
{

  logMsg("Calling supportsConvert(REAL, INTEGER) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::REAL, sql::DataType::INTEGER);
  if (retValue) {
    logMsg("supportsConvert(REAL, INTEGER) method is supported");
  } else {
    logMsg("supportsConvert(REAL, INTEGER) method is not supported");
  }
#else
  logMsg("supportsConvert(REAL, INTEGER) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert36()
{

  logMsg("Calling supportsConvert(SMALLINT, INTEGER) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::SMALLINT,
                                      sql::DataType::INTEGER);
  if (retValue) {
    logMsg("supportsConvert(SMALLINT, INTEGER) method is supported");
  } else {
    logMsg("supportsConvert(SMALLINT, INTEGER) method is not supported");
  }
#else
  logMsg("supportsConvert(SMALLINT, INTEGER) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsConvert37()
{

  logMsg("Calling supportsConvert(TINYINT, INTEGER) on DatabaseMetaData");
#ifdef WE_HAVE_DATATYPE_AGAIN
  bool retValue=dbmd->supportsConvert(sql::DataType::TINYINT, sql::DataType::INTEGER);
  if (retValue) {
    logMsg("supportsConvert(TINYINT, INTEGER) method is supported");
  } else {
    logMsg("supportsConvert(TINYINT, INTEGER) method is not supported");
  }
#else
  logMsg("supportsConvert(TINYINT, INTEGER) method is not supported");
#endif
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsCoreSQLGrammar()
{

  logMsg("Calling supportsCoreSQLGrammar on DatabaseMetaData");
  bool retValue=dbmd->supportsCoreSQLGrammar();
  if (retValue) {
    logMsg("supportsCoreSQLGrammar method is supported");
  } else {
    logMsg("supportsCoreSQLGrammar method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsCorrelatedSubqueries()
{

  logMsg("Calling DatabaseMetaData.supportsCorrelatedSubqueries");
  bool retValue=dbmd->supportsCorrelatedSubqueries();
  if (retValue) {
    logMsg("supportsCorrelatedSubqueries is supported");
  } else {
    logMsg("supportsCorrelatedSubqueries is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsDataDefinitionAndDataManipulationTransactions()
{
  logMsg(
         "Calling DatabaseMetaData.supportsDataDefinitionAndDataManipulationTransactions");
  bool retValue=dbmd->supportsDataDefinitionAndDataManipulationTransactions();
  if (retValue) {
    logMsg(
           "supportsDataDefinitionAndDataManipulationTransactions is supported");
  } else {
    logMsg(
           "supportsDataDefinitionAndDataManipulationTransactions is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsDataManipulationTransactionsOnly()
{

  logMsg(
         "Calling DatabaseMetaData.supportsDataManipulationTransactionsOnly");
  bool retValue=dbmd->supportsDataManipulationTransactionsOnly();
  if (retValue) {
    logMsg("supportsDataManipulationTransactionsOnly is supported");
  } else {
    logMsg(
           "supportsDataManipulationTransactionsOnly is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsDifferentTableCorrelationNames()
{

  logMsg(
         "Calling supportsDifferentTableCorrelationNames on DatabaseMetaData");
  bool retValue=dbmd->supportsDifferentTableCorrelationNames();
  if (retValue) {
    logMsg(
           "supportsDifferentTableCorrelationNames method is supported");
  } else {
    logMsg(
           "supportsDifferentTableCorrelationNames method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsExpressionsInOrderBy()
{

  logMsg("Calling supportsExpressionsInOrderBy on DatabaseMetaData");
  bool retValue=dbmd->supportsExpressionsInOrderBy();
  if (retValue) {
    logMsg("supportsExpressionsInOrderBy method is supported");
  } else {
    logMsg("supportsExpressionsInOrderBy method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsExtendedSQLGrammar()
{

  logMsg("Calling supportsExtendedSQLGrammar on DatabaseMetaData");
  bool retValue=dbmd->supportsExtendedSQLGrammar();
  if (retValue) {
    logMsg("supportsExtendedSQLGrammar method is supported");
  } else {
    logMsg("supportsExtendedSQLGrammar method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsFullOuterJoins()
{

  logMsg("Calling supportsFullOuterJoins on DatabaseMetaData");
  bool retValue=dbmd->supportsFullOuterJoins();
  if (retValue) {
    logMsg("supportsFullOuterJoins method is supported");
    if (!dbmd->supportsLimitedOuterJoins())
    {
      FAIL("supportsLimitedOuterJoins() must "
           + "be true if supportsFullOuterJoins() " + "is true!");
    }
  } else {
    logMsg("supportsFullOuterJoins method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsGroupBy()
{

  logMsg("Calling supportsGroupBy on DatabaseMetaData");
  bool retValue=dbmd->supportsGroupBy();
  if (retValue) {
    logMsg("supportsGroupBy method is supported");
  } else {
    logMsg("supportsGroupBy method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsGroupByBeyondSelect()
{

  logMsg("Calling supportsGroupByBeyondSelect on DatabaseMetaData");
  bool retValue=dbmd->supportsGroupByBeyondSelect();
  if (retValue) {
    logMsg("supportsGroupByBeyondSelect method is supported");
  } else {
    logMsg("supportsGroupByBeyondSelect method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsGroupByUnrelated()
{

  logMsg("Calling supportsGroupByUnrelated on DatabaseMetaData");
  bool retValue=dbmd->supportsGroupByUnrelated();
  if (retValue) {
    logMsg("supportsGroupByUnrelated method is supported");
  } else {
    logMsg("supportsGroupByUnrelated method is not supported");
  }
}


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
/* throws Exception */
void DatabaseMetaDataTest::testSupportsIntegrityEnhancementFacility()
{
  logMsg(
         "Calling supportsIntegrityEnhancementFacility on DatabaseMetaData");
  bool retValue=dbmd->supportsIntegrityEnhancementFacility();
  if (retValue) {
    logMsg(
           "supportsIntegrityEnhancementFacility method is supported");
  } else {
    logMsg(
           "supportsIntegrityEnhancementFacility method is not supported");
  }
}
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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsLikeEscapeClause()
{
  logMsg("Calling supportsLikeEscapeClause on DatabaseMetaData");
  bool retValue=dbmd->supportsLikeEscapeClause();
  if (retValue) {
    logMsg("supportsLikeEscapeClause method is supported");
  } else {
    logMsg("supportsLikeEscapeClause method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsLimitedOuterJoins()
{
  logMsg("Calling supportsLimitedOuterJoins on DatabaseMetaData");
  bool retValue=dbmd->supportsLimitedOuterJoins();
  if (retValue) {
    logMsg("supportsLimitedOuterJoins method is supported");
  } else {
    logMsg("supportsLimitedOuterJoins method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsMinimumSQLGrammar()
{
  logMsg("Calling supportsMinimumSQLGrammar on DatabaseMetaData");
  bool retValue=dbmd->supportsMinimumSQLGrammar();
  if (retValue) {
    logMsg("supportsMinimumSQLGrammar method is supported");
  } else {
    logMsg("supportsMinimumSQLGrammar method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsMixedCaseIdentifiers()
{
  logMsg("Calling supportsMixedCaseIdentifiers on DatabaseMetaData");
  bool retValue=dbmd->supportsMixedCaseIdentifiers();
  if (retValue) {
    logMsg("supportsMixedCaseIdentifiers method is supported");
  } else {
    logMsg("supportsMixedCaseIdentifiers method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsMixedCaseQuotedIdentifiers()
{

  logMsg(
         "Calling supportsMixedCaseQuotedIdentifiers on DatabaseMetaData");
  bool retValue=dbmd->supportsMixedCaseQuotedIdentifiers();
  if (retValue) {
    logMsg("supportsMixedCaseQuotedIdentifiers method is supported");
  } else {
    logMsg(
           "supportsMixedCaseQuotedIdentifiers method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsMultipleResultSets()
{

  logMsg("Calling supportsMultipleResultSets on DatabaseMetaData");
  bool retValue=dbmd->supportsMultipleResultSets();
  if (retValue) {
    logMsg("supportsMultipleResultSets method is supported");
  } else {
    logMsg("supportsMultipleResultSets method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsMultipleTransactions()
{
  logMsg("Calling supportsMultipleTransactions on DatabaseMetaData");
  bool retValue=dbmd->supportsMultipleTransactions();
  if (retValue) {
    logMsg("supportsMultipleTransactions method is supported");
  } else {
    logMsg("supportsMultipleTransactions method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsNonNullableColumns()
{

  logMsg("Calling supportsNonNullableColumns on DatabaseMetaData");
  bool retValue=dbmd->supportsNonNullableColumns();
  if (retValue) {
    logMsg("supportsNonNullableColumns method is supported");
  } else {
    logMsg("supportsNonNullableColumns method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsOpenCursorsAcrossCommit()
{

  logMsg("Calling DatabaseMetaData.supportsOpenCursorsAcrossCommit");
  bool retValue=dbmd->supportsOpenCursorsAcrossCommit();
  if (retValue) {
    logMsg("supportsOpenCursorsAcrossCommit is supported");
  } else {
    logMsg("supportsOpenCursorsAcrossCommit is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsOpenCursorsAcrossRollback()
{

  logMsg("Calling DatabaseMetaData.supportsOpenCursorsAcrossRollback");
  bool retValue=dbmd->supportsOpenCursorsAcrossRollback();
  if (retValue) {
    logMsg("supportsOpenCursorsAcrossRollback is supported");
  } else {
    logMsg("supportsOpenCursorsAcrossRollback is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsOpenStatementsAcrossCommit()
{

  logMsg(
         "Calling DatabaseMetaData.supportsOpenStatementsAcrossCommit");
  bool retValue=dbmd->supportsOpenStatementsAcrossCommit();
  if (retValue) {
    logMsg("supportsOpenStatementsAcrossCommit is supported");
  } else {
    logMsg("supportsOpenStatementsAcrossCommit is not supported");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsOpenStatementsAcrossRollback()
{
  logMsg(
         "Calling DatabaseMetaData.supportsOpenStatementsAcrossRollback");
  bool retValue=dbmd->supportsOpenStatementsAcrossRollback();
  if (retValue) {
    logMsg("supportsOpenStatementsAcrossRollback is supported");
  } else {
    logMsg("supportsOpenStatementsAcrossRollback is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsOrderByUnrelated()
{

  logMsg("Calling supportsOrderByUnrelated on DatabaseMetaData");
  bool retValue=dbmd->supportsOrderByUnrelated();
  if (retValue) {
    logMsg("supportsOrderByUnrelated method is supported");
  } else {
    logMsg("supportsOrderByUnrelated method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsOuterJoins()
{

  logMsg("Calling supportsOuterJoins on DatabaseMetaData");
  bool retValue=dbmd->supportsOuterJoins();
  if (retValue) {
    logMsg("supportsOuterJoins method is supported");
  } else {
    logMsg("supportsOuterJoins method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsPositionedDelete()
{

  logMsg("Calling DatabaseMetaData.supportsPositionedDelete");
  bool retValue=dbmd->supportsPositionedDelete();
  if (retValue) {
    logMsg("supportsPositionedDelete is supported");
  } else {
    logMsg("supportsPositionedDelete is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsPositionedUpdate()
{

  logMsg("Calling DatabaseMetaData.supportsPositionedUpdate");
  bool retValue=dbmd->supportsPositionedUpdate();
  if (retValue) {
    logMsg("supportsPositionedUpdate is supported");
  } else {
    logMsg("supportsPositionedUpdate is not supported");
  }
}


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
/* throws Exception */
void DatabaseMetaDataTest::testSupportsResultSetConcurrency1()
{

  logMsg(
         "Calling DatabaseMetaData.supportsResultSetConcurrency(TYPE_FORWARD_ONLY, CONCUR_READ_ONLY)");
  bool retValue=dbmd->supportsResultSetConcurrency(sql::ResultSet::TYPE_FORWARD_ONLY,
                                                   sql::ResultSet::CONCUR_READ_ONLY);
  if (retValue) {
    logMsg(
           "supportsResultSetConcurrency(TYPE_FORWARD_ONLY, CONCUR_READ_ONLY) is supported");
  } else {
    logMsg(
           "supportsResultSetConcurrency(TYPE_FORWARD_ONLY, CONCUR_READ_ONLY) is not supported");
  }
}


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
/* throws Exception */
void DatabaseMetaDataTest::testSupportsResultSetConcurrency2()
{
  logMsg(
         "Calling DatabaseMetaData.supportsResultSetConcurrency(TYPE_FORWARD_ONLY, CONCUR_UPDATABLE)");
  bool retValue=dbmd->supportsResultSetConcurrency(sql::ResultSet::TYPE_FORWARD_ONLY,
                                                   sql::ResultSet::CONCUR_UPDATABLE);
  if (retValue) {
    logMsg(
           "supportsResultSetConcurrency(TYPE_FORWARD_ONLY, CONCUR_UPDATABLE) is supported");
  } else {
    logMsg(
           "supportsResultSetConcurrency(TYPE_FORWARD_ONLY, CONCUR_UPDATABLE) is not supported");
  }
}


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
/* throws Exception */
void DatabaseMetaDataTest::testSupportsResultSetConcurrency3()
{
  logMsg(
         "Calling DatabaseMetaData.supportsResultSetConcurrency(TYPE_SCROLL_INSENSITIVE, CONCUR_READ_ONLY)");
  bool retValue=dbmd->supportsResultSetConcurrency(sql::ResultSet::TYPE_SCROLL_INSENSITIVE,
                                                   sql::ResultSet::CONCUR_READ_ONLY);
  if (retValue) {
    logMsg(
           "supportsResultSetConcurrency(TYPE_SCROLL_INSENSITIVE, CONCUR_READ_ONLY) is supported");
  } else {
    logMsg(
           "supportsResultSetConcurrency(TYPE_SCROLL_INSENSITIVE, CONCUR_READ_ONLY) is not supported");
  }
}


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
/* throws Exception */
void DatabaseMetaDataTest::testSupportsResultSetConcurrency4()
{
  logMsg(
         "Calling DatabaseMetaData.supportsResultSetConcurrency(TYPE_SCROLL_INSENSITIVE, CONCUR_UPDATABLE)");
  bool retValue=dbmd->supportsResultSetConcurrency(sql::ResultSet::TYPE_SCROLL_INSENSITIVE,
                                                   sql::ResultSet::CONCUR_UPDATABLE);
  if (retValue) {
    logMsg(
           "supportsResultSetConcurrency(TYPE_SCROLL_INSENSITIVE, CONCUR_UPDATABLE) is supported");
  } else {
    logMsg(
           "supportsResultSetConcurrency(TYPE_SCROLL_INSENSITIVE, CONCUR_UPDATABLE) is not supported");
  }
}


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
/* throws Exception */
void DatabaseMetaDataTest::testSupportsResultSetConcurrency5()
{
  logMsg(
         "Calling DatabaseMetaData.supportsResultSetConcurrency(TYPE_SCROLL_SENSITIVE, CONCUR_READ_ONLY)");
  bool retValue=dbmd->supportsResultSetConcurrency(sql::ResultSet::TYPE_SCROLL_SENSITIVE,
                                                   sql::ResultSet::CONCUR_READ_ONLY);
  if (retValue) {
    logMsg(
           "supportsResultSetConcurrency(TYPE_SCROLL_SENSITIVE, CONCUR_READ_ONLY) is supported");
  } else {
    logMsg(
           "supportsResultSetConcurrency(TYPE_SCROLL_SENSITIVE, CONCUR_READ_ONLY) is not supported");
  }
}


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
/* throws Exception */
void DatabaseMetaDataTest::testSupportsResultSetConcurrency6()
{
  logMsg(
         "Calling DatabaseMetaData.supportsResultSetConcurrency(TYPE_SCROLL_SENSITIVE, CONCUR_UPDATABLE)");
  bool retValue=dbmd->supportsResultSetConcurrency(sql::ResultSet::TYPE_SCROLL_SENSITIVE,
                                                   sql::ResultSet::CONCUR_UPDATABLE);
  if (retValue) {
    logMsg(
           "supportsResultSetConcurrency(TYPE_SCROLL_SENSITIVE, CONCUR_UPDATABLE) is supported");
  } else {
    logMsg(
           "supportsResultSetConcurrency(TYPE_SCROLL_SENSITIVE, CONCUR_UPDATABLE) is not supported");
  }
}
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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsResultSetType1()
{

  logMsg(
         "Calling DatabaseMetaData.supportsResultSetType(TYPE_FORWARD_ONLY)");
  bool retValue=dbmd->supportsResultSetType(sql::ResultSet::TYPE_FORWARD_ONLY);
  if (retValue) {
    logMsg("TYPE_FORWARD_ONLY ResultSetType is supported");
  } else {
    logMsg("TYPE_FORWARD_ONLY ResultSetType is not supported");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsResultSetType2()
{

  logMsg(
         "Calling DatabaseMetaData.supportsResultSetType(TYPE_SCROLL_INSENSITIVE)");
  bool retValue=dbmd->supportsResultSetType(sql::ResultSet::TYPE_SCROLL_INSENSITIVE);
  if (retValue) {
    logMsg("TYPE_SCROLL_INSENSITIVE ResultSetType is supported");
  } else {
    logMsg("TYPE_SCROLL_INSENSITIVE ResultSetType is not supported");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsResultSetType3()
{

  logMsg(
         "Calling DatabaseMetaData.supportsResultSetType(TYPE_SCROLL_SENSITIVE)");
  bool retValue=dbmd->supportsResultSetType(sql::ResultSet::TYPE_SCROLL_SENSITIVE);
  if (retValue) {
    logMsg("TYPE_SCROLL_SENSITIVE ResultSetType is supported");
  } else {
    logMsg("TYPE_SCROLL_SENSITIVE ResultSetType is not supported");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsSchemasInDataManipulation()
{

  logMsg("Calling DatabaseMetaData.supportsSchemasInDataManipulation");
  bool retValue=dbmd->supportsSchemasInDataManipulation();
  if (retValue) {
    logMsg("supportsSchemasInDataManipulation is supported");
  } else {
    logMsg("supportsSchemasInDataManipulation is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsSchemasInIndexDefinitions()
{

  logMsg("Calling DatabaseMetaData.supportsSchemasInIndexDefinitions");
  bool retValue=dbmd->supportsSchemasInIndexDefinitions();
  if (retValue) {
    logMsg("supportsSchemasInIndexDefinitions is supported");
  } else {
    logMsg("supportsSchemasInIndexDefinitions is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsSchemasInPrivilegeDefinitions()
{
  logMsg(
         "Calling DatabaseMetaData.supportsSchemasInPrivilegeDefinitions");
  bool retValue=dbmd->supportsSchemasInPrivilegeDefinitions();
  if (retValue) {
    logMsg("supportsSchemasInPrivilegeDefinitions is supported");
  } else {
    logMsg("supportsSchemasInPrivilegeDefinitions is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsSchemasInProcedureCalls()
{

  logMsg("Calling DatabaseMetaData.supportsSchemasInProcedureCalls");
  bool retValue=dbmd->supportsSchemasInProcedureCalls();
  if (retValue) {
    logMsg("supportsSchemasInProcedureCalls is supported");
  } else {
    logMsg("supportsSchemasInProcedureCalls is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsSchemasInTableDefinitions()
{

  logMsg("Calling DatabaseMetaData.supportsSchemasInTableDefinitions");
  bool retValue=dbmd->supportsSchemasInTableDefinitions();
  if (retValue) {
    logMsg("supportsSchemasInTableDefinitions is supported");
  } else {
    logMsg("supportsSchemasInTableDefinitions is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsSelectForUpdate()
{

  logMsg("Calling DatabaseMetaData.supportsSelectForUpdate");
  bool retValue=dbmd->supportsSelectForUpdate();
  if (retValue) {
    logMsg("supportsSelectForUpdate is supported");
  } else {
    logMsg("supportsSelectForUpdate is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsStoredProcedures()
{
  if (hasSps) {

    logMsg("Calling supportsStoredProcedures on DatabaseMetaData");
    bool retValue=dbmd->supportsStoredProcedures();
    if (retValue)
    {
      logMsg("SupportsStoredProcedures is supported");
    } else {
      logErr("SupportsStoredProcedures is not supported");
      FAIL("supportsStoredProcedures should always return true!");
    }
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsSubqueriesInComparisons()
{

  logMsg("Calling DatabaseMetaData.supportsSubqueriesInComparisons");
  bool retValue=dbmd->supportsSubqueriesInComparisons();
  if (retValue) {
    logMsg("supportsSubqueriesInComparisons is supported");
  } else {
    logMsg("supportsSubqueriesInComparisons is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsSubqueriesInExists()
{

  logMsg("Calling DatabaseMetaData.supportsSubqueriesInExists");
  bool retValue=dbmd->supportsSubqueriesInExists();
  if (retValue) {
    logMsg("supportsSubqueriesInExists is supported");
  } else {
    logMsg("supportsSubqueriesInExists is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsSubqueriesInIns()
{

  logMsg("Calling DatabaseMetaData.supportsSubqueriesInIns");
  bool retValue=dbmd->supportsSubqueriesInIns();
  if (retValue) {
    logMsg("supportsSubqueriesInIns is supported");
  } else {
    logMsg("supportsSubqueriesInIns is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsSubqueriesInQuantifieds()
{
  logMsg("Calling DatabaseMetaData.supportsSubqueriesInQuantifieds");
  bool retValue=dbmd->supportsSubqueriesInQuantifieds();

  if (retValue) {
    logMsg("supportsSubqueriesInQuantifieds is supported");
  } else {
    logMsg("supportsSubqueriesInQuantifieds is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsTableCorrelationNames()
{

  logMsg("Calling supportsTableCorrelationNames on DatabaseMetaData");
  bool retValue=dbmd->supportsTableCorrelationNames();
  if (retValue) {
    logMsg("supportsTableCorrelationNames method is supported");
  } else {
    logMsg("supportsTableCorrelationNames method is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsTransactionIsolationLevel1()
{
  logMsg(
         "Calling DatabaseMetaData.supportsTransactionIsolationLevel(sql::TRANSACTION_NONE)");
  bool retValue=dbmd->supportsTransactionIsolationLevel(sql::TRANSACTION_NONE);
  if (retValue) {
    logMsg(
           "supportsTransactionIsolationLevel(sql::TRANSACTION_NONE) is supported");
  } else {
    logMsg(
           "supportsTransactionIsolationLevel(sql::TRANSACTION_NONE) is not supported");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsTransactionIsolationLevel2()
{
  logMsg(
         "Calling DatabaseMetaData.supportsTransactionIsolationLevel(sql::TRANSACTION_READ_COMMITTED)");
  bool retValue=dbmd->supportsTransactionIsolationLevel(sql::TRANSACTION_READ_COMMITTED);
  if (retValue) {
    logMsg(
           "supportsTransactionIsolationLevel(sql::TRANSACTION_READ_COMMITTED) is supported");
  } else {
    logMsg(
           "supportsTransactionIsolationLevel(sql::TRANSACTION_READ_COMMITTED) is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsTransactionIsolationLevel3()
{

  logMsg(
         "Calling DatabaseMetaData.supportsTransactionIsolationLevel(sql::TRANSACTION_READ_UNCOMMITTED)");
  bool retValue=dbmd->supportsTransactionIsolationLevel(sql::TRANSACTION_READ_UNCOMMITTED);
  if (retValue) {
    logMsg(
           "supportsTransactionIsolationLevel(sql::TRANSACTION_READ_UNCOMMITTED) is supported");
  } else {
    logMsg(
           "supportsTransactionIsolationLevel(sql::TRANSACTION_READ_UNCOMMITTED ) is not supported");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsTransactionIsolationLevel4()
{

  logMsg(
         "Calling DatabaseMetaData.supportsTransactionIsolationLevel(sql::TRANSACTION_REPEATABLE_READ)");
  bool retValue=dbmd->supportsTransactionIsolationLevel(sql::TRANSACTION_REPEATABLE_READ);
  if (retValue) {
    logMsg(
           "supportsTransactionIsolationLevel(sql::TRANSACTION_REPEATABLE_READ) is supported");
  } else {
    logMsg(
           "supportsTransactionIsolationLevel(sql::TRANSACTION_REPEATABLE_READ) is not supported");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsTransactionIsolationLevel5()
{

  logMsg(
         "Calling DatabaseMetaData.supportsTransactionIsolationLevel(sql::TRANSACTION_SERIALIZABLE)");
  bool retValue=dbmd->supportsTransactionIsolationLevel(sql::TRANSACTION_SERIALIZABLE);
  if (retValue) {
    logMsg(
           "supportsTransactionIsolationLevel(sql::TRANSACTION_SERIALIZABLE) is supported");
  } else {
    logMsg(
           "supportsTransactionIsolationLevel(sql::TRANSACTION_SERIALIZABLE) is not supported");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsTransactions()
{

  logMsg("Calling DatabaseMetaData.supportsTransactions");
  bool retValue=dbmd->supportsTransactions();
  if (retValue) {
    logMsg("supportsTransactions is supported");
  } else {
    logMsg("supportsTransactions is not supported");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsUnion()
{

  logMsg("Calling DatabaseMetaData.supportsUnion");
  bool retValue=dbmd->supportsUnion();
  if (retValue) {
    logMsg("supportsUnion is supported");
  } else {
    logMsg("supportsUnion is not supported");
  }
}

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

/* throws Exception */
void DatabaseMetaDataTest::testSupportsUnionAll()
{

  logMsg("Calling DatabaseMetaData.supportsUnionAll");
  bool retValue=dbmd->supportsUnionAll();
  if (retValue) {
    logMsg("supportsUnionAll is supported");
  } else {
    logMsg("supportsUnionAll is not supported");
  }
}

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
 *                    ResultSet Type as sql::ResultSet::TYPE_FORWARD_ONLY.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testUpdatesAreDetected1()
{

  logMsg(
         "Calling DatabaseMetaData.updatesAreDetected(TYPE_FORWARD_ONLY)");
  bool retValue=dbmd->updatesAreDetected(sql::ResultSet::TYPE_FORWARD_ONLY);
  if (retValue) {
    logMsg(
           "Visible row update can be detected for TYPE_FORWARD_ONLY");
  } else {
    logMsg(
           "Visible row update cannot be detected for TYPE_FORWARD_ONLY");
  }


}

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
 *                    ResultSet Type as sql::ResultSet::TYPE_SCROLL_INSENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testUpdatesAreDetected2()
{

  logMsg(
         "Calling DatabaseMetaData.updatesAreDetected(TYPE_SCROLL_INSENSITIVE)");
  bool retValue=dbmd->updatesAreDetected(sql::ResultSet::TYPE_SCROLL_INSENSITIVE);
  if (retValue) {
    logMsg(
           "Visible row update can be detected for TYPE_SCROLL_INSENSITIVE");
  } else {
    logMsg(
           "Visible row update cannot be detected for TYPE_SCROLL_INSENSITIVE");
  }


}

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
 *                    ResultSet Type as sql::ResultSet::TYPE_SCROLL_SENSITIVE.
 *                    It should return a boolean value; either true or false.
 *
 */

/* throws Exception */
void DatabaseMetaDataTest::testUpdatesAreDetected3()
{

  logMsg(
         "Calling DatabaseMetaData.updatesAreDetected(TYPE_SCROLL_SENSITIVE)");
  bool retValue=dbmd->updatesAreDetected(sql::ResultSet::TYPE_SCROLL_SENSITIVE);
  if (retValue) {
    logMsg(
           "Visible row update can be detected for TYPE_SCROLL_SENSITIVE");
  } else {
    logMsg(
           "Visible row update cannot be detected for TYPE_SCROLL_SENSITIVE");
  }


}

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

/* throws Exception */
void DatabaseMetaDataTest::testUsesLocalFilePerTable()
{
  logMsg("Calling usesLocalFilePerTable on DatabaseMetaData");
  bool retValue=dbmd->usesLocalFilePerTable();
  if (retValue) {
    logMsg(
           "usesLocalFilePerTable method returns database uses a local file");
  } else {
    logMsg(
           "usesLocalFilePerTable method returns database not uses a local file");
  }
}

/**
 * @see junit.framework.TestCase#setUp()
 */

/* throws Exception */
void DatabaseMetaDataTest::setUp()
{
  super::setUp();

  sPtable="CTSTABLE1";

  Properties::const_iterator cit=sqlProps.find("ptable");

  if (cit != sqlProps.end())
    sPtable=cit->second;

  sFtable="CTSTABLE2";

  cit=sqlProps.find("ftable");

  if (cit != sqlProps.end())
    sFtable=cit->second;

  if (sPtable.length() == 0) {
    FAIL("Invalid Primary table");
  }
  if (sFtable.length() == 0) {
    FAIL("Invalid Foreign table");
  }
  sSchemaName="";
  dbmd= conn->getMetaData();
}

/* A private method to compare the Column Names & No of Columns Specific to the test */

/* throws SQLException */
bool DatabaseMetaDataTest::columnCompare(List & sColumnNames, ResultSet & rset)
{
  bool test_status=       false;
  bool statusColumnCount= true;
  bool statusColumnMatch= true;
  int iColumnNamesLength= static_cast<int>(sColumnNames.size());

  ResultSetMetaData rsmd(rset->getMetaData());

  int iCount=rsmd->getColumnCount();

  TestsListener::messagesLog() << "Minimum Column Count is:"
          << iColumnNamesLength << std::endl;

  if (iColumnNamesLength <= iCount) {
    iCount=iColumnNamesLength;
    statusColumnCount=true;
    logMsg("Different numbers of columns");
  } else {
    statusColumnCount=false;
  }

  logMsg("Comparing Column Names...");

  while (iColumnNamesLength > 0) {
    if (ciString(sColumnNames[iColumnNamesLength - 1].c_str()) ==
        rsmd->getColumnName(iCount).c_str())
    {
      statusColumnMatch=true;
    } else {
      statusColumnMatch=false;
      logMsg("Wrong column value, dumping expected and returned data");
      logMsg(sColumnNames[iColumnNamesLength - 1]);
      logMsg(rsmd->getColumnName(iCount));
      break;
    }

    iCount--;
    iColumnNamesLength--;
  }

  if ((statusColumnCount == true) && (statusColumnMatch == true)) {
    test_status=true;
  }

  return test_status;
}

} // namespace compliance
} // namespace testsuite
