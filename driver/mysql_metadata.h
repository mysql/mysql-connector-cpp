/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates.
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



#ifndef _MYSQL_METADATA_H_
#define _MYSQL_METADATA_H_

#include <boost/shared_ptr.hpp>

#include <cppconn/metadata.h>
#include <map>

namespace sql
{
class ResultSet;

namespace mysql
{

class MySQL_Connection;
class MySQL_DebugLogger;

namespace NativeAPI
{
class IMySQLCAPI;
}


class MySQL_ConnectionMetaData : public sql::DatabaseMetaData
{
  sql::Statement * stmt;
  MySQL_Connection * connection;
  unsigned long server_version;
  boost::shared_ptr< MySQL_DebugLogger > logger;

  std::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy;

  sql::SQLString lower_case_table_names;

  bool use_info_schema;

public:
  MySQL_ConnectionMetaData(sql::Statement * const service, std::shared_ptr<NativeAPI::NativeConnectionWrapper> _capi,
              boost::shared_ptr< MySQL_DebugLogger > & l);

  virtual ~MySQL_ConnectionMetaData();

  bool allProceduresAreCallable();

  bool allTablesAreSelectable();

  bool dataDefinitionCausesTransactionCommit();

  bool dataDefinitionIgnoredInTransactions();

  bool deletesAreDetected(int type);

  bool doesMaxRowSizeIncludeBlobs();

  sql::ResultSet * getAttributes(const sql::SQLString& catalog, const sql::SQLString& schemaPattern, const sql::SQLString& typeNamePattern, const sql::SQLString& attributeNamePattern);

  sql::ResultSet * getBestRowIdentifier(const sql::SQLString& catalog, const sql::SQLString& schema, const sql::SQLString& table, int scope, bool nullable);

  sql::ResultSet * getCatalogs();

  const sql::SQLString& getCatalogSeparator();

  const sql::SQLString& getCatalogTerm();

  sql::ResultSet * getColumnPrivileges(const sql::SQLString& catalog, const sql::SQLString& schema, const sql::SQLString& table, const sql::SQLString& columnNamePattern);

  sql::ResultSet * getColumns(const sql::SQLString& catalog, const sql::SQLString& schemaPattern, const sql::SQLString& tableNamePattern, const sql::SQLString& columnNamePattern);

  sql::Connection * getConnection();

  sql::ResultSet * getCrossReference(const sql::SQLString& primaryCatalog, const sql::SQLString& primarySchema, const sql::SQLString& primaryTable, const sql::SQLString& foreignCatalog, const sql::SQLString& foreignSchema, const sql::SQLString& foreignTable);

  unsigned int getDatabaseMajorVersion();

  unsigned int getDatabaseMinorVersion();

  unsigned int getDatabasePatchVersion();

  const sql::SQLString& getDatabaseProductName();

  SQLString getDatabaseProductVersion();

  int getDefaultTransactionIsolation();

  unsigned int getDriverMajorVersion();

  unsigned int getDriverMinorVersion();

  unsigned int getDriverPatchVersion();

  const sql::SQLString& getDriverName();

  const sql::SQLString& getDriverVersion();

  sql::ResultSet * getExportedKeys(const sql::SQLString& catalog, const sql::SQLString& schema, const sql::SQLString& table);

  const sql::SQLString& getExtraNameCharacters();

  const sql::SQLString& getIdentifierQuoteString();

  sql::ResultSet * getImportedKeys(const sql::SQLString& catalog, const sql::SQLString& schema, const sql::SQLString& table);

  sql::ResultSet * getIndexInfo(const sql::SQLString& catalog, const sql::SQLString& schema, const sql::SQLString& table, bool unique, bool approximate);

  unsigned int getCDBCMajorVersion();

  unsigned int getCDBCMinorVersion();

  unsigned int getMaxBinaryLiteralLength();

  unsigned int getMaxCatalogNameLength();

  unsigned int getMaxCharLiteralLength();

  unsigned int getMaxColumnNameLength();

  unsigned int getMaxColumnsInGroupBy();

  unsigned int getMaxColumnsInIndex();

  unsigned int getMaxColumnsInOrderBy();

  unsigned int getMaxColumnsInSelect();

  unsigned int getMaxColumnsInTable();

  unsigned int getMaxConnections();

  unsigned int getMaxCursorNameLength();

  unsigned int getMaxIndexLength();

  unsigned int getMaxProcedureNameLength();

  unsigned int getMaxRowSize();

  unsigned int getMaxSchemaNameLength();

  unsigned int getMaxStatementLength();

  unsigned int getMaxStatements();

  unsigned int getMaxTableNameLength();

  unsigned int getMaxTablesInSelect();

  unsigned int getMaxUserNameLength();

  const sql::SQLString& getNumericFunctions();

  sql::ResultSet * getPrimaryKeys(const sql::SQLString& catalog, const sql::SQLString& schema, const sql::SQLString& table);

  sql::ResultSet * getUniqueNonNullableKeys(const sql::SQLString& catalog, const sql::SQLString& schema, const sql::SQLString& table);

  sql::ResultSet * getProcedureColumns(const sql::SQLString& catalog, const sql::SQLString& schemaPattern, const sql::SQLString& procedureNamePattern, const sql::SQLString& columnNamePattern);

  sql::ResultSet * getProcedures(const sql::SQLString& catalog, const sql::SQLString& schemaPattern, const sql::SQLString& procedureNamePattern);

  const sql::SQLString& getProcedureTerm();

  int getResultSetHoldability();

  sql::ResultSet * getSchemas();

  const sql::SQLString& getSchemaTerm();

  sql::ResultSet * getSchemaCollation(const sql::SQLString& catalog, const sql::SQLString& schemaPattern);

  sql::ResultSet * getSchemaCharset(const sql::SQLString& catalog, const sql::SQLString& schemaPattern);

  const sql::SQLString& getSearchStringEscape();

  const sql::SQLString& getSQLKeywords();

  int getSQLStateType();

  const sql::SQLString& getStringFunctions();

  sql::ResultSet * getSuperTables(const sql::SQLString& catalog, const sql::SQLString& schemaPattern, const sql::SQLString& tableNamePattern);

  sql::ResultSet * getSuperTypes(const sql::SQLString& catalog, const sql::SQLString& schemaPattern, const sql::SQLString& typeNamePattern);

  const sql::SQLString& getSystemFunctions();

  sql::ResultSet * getTablePrivileges(const sql::SQLString& catalog, const sql::SQLString& schemaPattern, const sql::SQLString& tableNamePattern);

  sql::ResultSet * getTables(const sql::SQLString& catalog, const sql::SQLString& schemaPattern, const sql::SQLString& tableNamePattern, std::list<sql::SQLString> &types);

  sql::ResultSet * getTableTypes();

  sql::ResultSet * getTableCollation(const sql::SQLString& catalog, const sql::SQLString& schemaPattern, const sql::SQLString& tableNamePattern);

  sql::ResultSet * getTableCharset(const sql::SQLString& catalog, const sql::SQLString& schemaPattern, const sql::SQLString& tableNamePattern);

  const sql::SQLString& getTimeDateFunctions();

  sql::ResultSet * getTypeInfo();

  sql::ResultSet * getUDTs(const sql::SQLString& catalog, const sql::SQLString& schemaPattern, const sql::SQLString& typeNamePattern, std::list<int> &types);

  SQLString getURL();

  SQLString getUserName();

  sql::ResultSet * getVersionColumns(const sql::SQLString& catalog, const sql::SQLString& schema, const sql::SQLString& table);

  bool insertsAreDetected(int type);

  bool isCatalogAtStart();

  bool isReadOnly();

  bool locatorsUpdateCopy();

  bool nullPlusNonNullIsNull();

  bool nullsAreSortedAtEnd();

  bool nullsAreSortedAtStart();

  bool nullsAreSortedHigh();

  bool nullsAreSortedLow();

  bool othersDeletesAreVisible(int type);

  bool othersInsertsAreVisible(int type);

  bool othersUpdatesAreVisible(int type);

  bool ownDeletesAreVisible(int type);

  bool ownInsertsAreVisible(int type);

  bool ownUpdatesAreVisible(int type);

  bool storesLowerCaseIdentifiers();

  bool storesLowerCaseQuotedIdentifiers();

  bool storesMixedCaseIdentifiers();

  bool storesMixedCaseQuotedIdentifiers();

  bool storesUpperCaseIdentifiers();

  bool storesUpperCaseQuotedIdentifiers();

  bool supportsAlterTableWithAddColumn();

  bool supportsAlterTableWithDropColumn();

  bool supportsANSI92EntryLevelSQL();

  bool supportsANSI92FullSQL();

  bool supportsANSI92IntermediateSQL();

  bool supportsBatchUpdates();

  bool supportsCatalogsInDataManipulation();

  bool supportsCatalogsInIndexDefinitions();

  bool supportsCatalogsInPrivilegeDefinitions();

  bool supportsCatalogsInProcedureCalls();

  bool supportsCatalogsInTableDefinitions();

  bool supportsColumnAliasing();

  bool supportsConvert();

  bool supportsConvert(int fromType, int toType);

  bool supportsCoreSQLGrammar();

  bool supportsCorrelatedSubqueries();

  bool supportsDataDefinitionAndDataManipulationTransactions();

  bool supportsDataManipulationTransactionsOnly();

  bool supportsDifferentTableCorrelationNames();

  bool supportsExpressionsInOrderBy();

  bool supportsExtendedSQLGrammar();

  bool supportsFullOuterJoins();

  bool supportsGetGeneratedKeys();

  bool supportsGroupBy();

  bool supportsGroupByBeyondSelect();

  bool supportsGroupByUnrelated();

  bool supportsIntegrityEnhancementFacility();

  bool supportsLikeEscapeClause();

  bool supportsLimitedOuterJoins();

  bool supportsMinimumSQLGrammar();

  bool supportsMixedCaseIdentifiers();

  bool supportsMixedCaseQuotedIdentifiers();

  bool supportsMultipleOpenResults();

  bool supportsMultipleResultSets();

  bool supportsMultipleTransactions();

  bool supportsNamedParameters();

  bool supportsNonNullableColumns();

  bool supportsOpenCursorsAcrossCommit();

  bool supportsOpenCursorsAcrossRollback();

  bool supportsOpenStatementsAcrossCommit();

  bool supportsOpenStatementsAcrossRollback();

  bool supportsOrderByUnrelated();

  bool supportsOuterJoins();

  bool supportsPositionedDelete();

  bool supportsPositionedUpdate();

  bool supportsResultSetConcurrency(int type, int concurrency);

  bool supportsResultSetHoldability(int holdability);

  bool supportsResultSetType(int type);

  bool supportsSavepoints();

  bool supportsSchemasInDataManipulation();

  bool supportsSchemasInIndexDefinitions();

  bool supportsSchemasInPrivilegeDefinitions();

  bool supportsSchemasInProcedureCalls();

  bool supportsSchemasInTableDefinitions();

  bool supportsSelectForUpdate();

  bool supportsStatementPooling();

  bool supportsStoredProcedures();

  bool supportsSubqueriesInComparisons();

  bool supportsSubqueriesInExists();

  bool supportsSubqueriesInIns();

  bool supportsSubqueriesInQuantifieds();

  bool supportsTableCorrelationNames();

  bool supportsTransactionIsolationLevel(int level);

  bool supportsTransactions();

  bool supportsTypeConversion();

  bool supportsUnion();

  bool supportsUnionAll();

  bool updatesAreDetected(int type);

  bool usesLocalFilePerTable();

  bool usesLocalFiles();

  sql::ResultSet *getSchemata(const sql::SQLString& catalogName = "");

  sql::ResultSet *getSchemaObjects(const sql::SQLString& catalogName = "", const sql::SQLString& schemaName = "", const sql::SQLString& objectType = "", bool includingDdl = true, const sql::SQLString& objectName = "", const sql::SQLString& contextTableName = "");

  // Returns all schema object types this database supports
  sql::ResultSet *getSchemaObjectTypes();

private:
  bool matchTable(const sql::SQLString& sPattern, const sql::SQLString& tPattern, const sql::SQLString& schema, const sql::SQLString& table);
  bool parseImportedKeys(
    const sql::SQLString& def,
    sql::SQLString & constraint_name,
    std::map< sql::SQLString, sql::SQLString > & keywords_names,
    std::map< sql::SQLString, std::list< sql::SQLString > > & referenced_fields,
    std::map< sql::SQLString, int > & update_delete_action
  );

  /* Prevent use of these */
  MySQL_ConnectionMetaData();
  MySQL_ConnectionMetaData(const MySQL_ConnectionMetaData &);
  void operator=(MySQL_ConnectionMetaData &);
};

} /* namespace mysql */
} /* namespace sql */
#endif // _MYSQL_METADATA_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
