/* Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.

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

#ifndef _MYSQL_METADATA_H_
#define _MYSQL_METADATA_H_

#include <cppconn/metadata.h>

namespace sql
{
class ResultSet;

namespace mysql
{

class MySQL_Connection;
class MySQL_DebugLogger;
namespace util {template<class T> class my_shared_ptr; }; // forward declaration.

class MySQL_ConnectionMetaData : public sql::DatabaseMetaData
{
	MySQL_Connection * connection;
	unsigned long server_version;
	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger;

	std::string lower_case_table_names;
public:
	MySQL_ConnectionMetaData(MySQL_Connection * conn, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l);

	virtual ~MySQL_ConnectionMetaData();

	bool allProceduresAreCallable();

	bool allTablesAreSelectable();

	bool dataDefinitionCausesTransactionCommit();

	bool dataDefinitionIgnoredInTransactions();

	bool deletesAreDetected(int type);

	bool doesMaxRowSizeIncludeBlobs();

	sql::ResultSet * getAttributes(const std::string& catalog, const std::string& schemaPattern, const std::string& typeNamePattern, const std::string& attributeNamePattern);

	sql::ResultSet * getBestRowIdentifier(const std::string& catalog, const std::string& schema, const std::string& table, int scope, bool nullable);

	sql::ResultSet * getCatalogs();

	const std::string& getCatalogSeparator();

	const std::string& getCatalogTerm();

	sql::ResultSet * getColumnPrivileges(const std::string& catalog, const std::string& schema, const std::string& table, const std::string& columnNamePattern);

	sql::ResultSet * getColumns(const std::string& catalog, const std::string& schemaPattern, const std::string& tableNamePattern, const std::string& columnNamePattern);

	sql::Connection * getConnection();

	sql::ResultSet * getCrossReference(const std::string& primaryCatalog, const std::string& primarySchema, const std::string& primaryTable, const std::string& foreignCatalog, const std::string& foreignSchema, const std::string& foreignTable);

	int getDatabaseMajorVersion();

	int getDatabaseMinorVersion();

	int getDatabasePatchVersion();

	const std::string& getDatabaseProductName();

	const std::string& getDatabaseProductVersion();

	int getDefaultTransactionIsolation();

	int getDriverMajorVersion();

	int getDriverMinorVersion();

	int getDriverPatchVersion();

	const std::string& getDriverName();

	const std::string& getDriverVersion();

	sql::ResultSet * getExportedKeys(const std::string& catalog, const std::string& schema, const std::string& table);

	const std::string& getExtraNameCharacters();

	const std::string& getIdentifierQuoteString();

	sql::ResultSet * getImportedKeys(const std::string& catalog, const std::string& schema, const std::string& table);

	sql::ResultSet * getIndexInfo(const std::string& catalog, const std::string& schema, const std::string& table, bool unique, bool approximate);

	int getCDBCMajorVersion();

	int getCDBCMinorVersion();

	int getMaxBinaryLiteralLength();

	int getMaxCatalogNameLength();

	int getMaxCharLiteralLength();

	int getMaxColumnNameLength();

	int getMaxColumnsInGroupBy();

	int getMaxColumnsInIndex();

	int getMaxColumnsInOrderBy();

	int getMaxColumnsInSelect();

	int getMaxColumnsInTable();

	int getMaxConnections();

	int getMaxCursorNameLength();

	int getMaxIndexLength();

	int getMaxProcedureNameLength();

	int getMaxRowSize();

	int getMaxSchemaNameLength();

	int getMaxStatementLength();

	int getMaxStatements();

	int getMaxTableNameLength();

	int getMaxTablesInSelect();

	int getMaxUserNameLength();

	std::string getNumericFunctions();

	sql::ResultSet * getPrimaryKeys(const std::string& catalog, const std::string& schema, const std::string& table);

	sql::ResultSet * getProcedureColumns(const std::string& catalog, const std::string& schemaPattern, const std::string& procedureNamePattern, const std::string& columnNamePattern);

	sql::ResultSet * getProcedures(const std::string& catalog, const std::string& schemaPattern, const std::string& procedureNamePattern);

	const std::string& getProcedureTerm();

	int getResultSetHoldability();

	sql::ResultSet * getSchemas();

	const std::string& getSchemaTerm();

	const std::string& getSearchStringEscape();

	const std::string& getSQLKeywords();

	int getSQLStateType();

	const std::string& getStringFunctions();

	sql::ResultSet * getSuperTables(const std::string& catalog, const std::string& schemaPattern, const std::string& tableNamePattern);

	sql::ResultSet * getSuperTypes(const std::string& catalog, const std::string& schemaPattern, const std::string& typeNamePattern);

	const std::string& getSystemFunctions();

	sql::ResultSet * getTablePrivileges(const std::string& catalog, const std::string& schemaPattern, const std::string& tableNamePattern);

	sql::ResultSet * getTables(const std::string& catalog, const std::string& schemaPattern, const std::string& tableNamePattern, std::list<std::string> &types);

	sql::ResultSet * getTableTypes();

	const std::string& getTimeDateFunctions();

	sql::ResultSet * getTypeInfo();

	sql::ResultSet * getUDTs(const std::string& catalog, const std::string& schemaPattern, const std::string& typeNamePattern, std::list<int> &types);

	std::string getURL();

	std::string getUserName();

	sql::ResultSet * getVersionColumns(const std::string& catalog, const std::string& schema, const std::string& table);

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

	sql::ResultSet *getSchemata(const std::string& catalogName = "");

	sql::ResultSet *getSchemaObjects(const std::string& catalogName = "", 
								const std::string& schemaName = "",
								const std::string& objectType = "");

	// Returns all schema object types this database supports
	sql::ResultSet *getSchemaObjectTypes();

private:
	bool matchTable(std::string &sPattern, std::string & tPattern, std::string & schema, std::string & table);
	bool parseImportedKeys(std::string& token, std::string & quoteIdentifier, std::list< std::string > &fields);

	/* Prevent use of these */
	MySQL_ConnectionMetaData();
	MySQL_ConnectionMetaData(const MySQL_ConnectionMetaData &);
	void operator=(MySQL_ConnectionMetaData &);
};

}; /* namespace mysql */
}; /* namespace sql */
#endif // _MYSQL_METADATA_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
