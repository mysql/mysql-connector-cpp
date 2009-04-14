/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_METADATA_H_
#define _MYSQL_METADATA_H_

#include <cppconn/metadata.h>
#include <string>
#include <map>

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

	bool use_info_schema;
public:
	MySQL_ConnectionMetaData(MySQL_Connection * const conn, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l);

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

	unsigned int getDatabaseMajorVersion();

	unsigned int getDatabaseMinorVersion();

	unsigned int getDatabasePatchVersion();

	const std::string& getDatabaseProductName();

	std::string getDatabaseProductVersion();

	int getDefaultTransactionIsolation();

	unsigned int getDriverMajorVersion();

	unsigned int getDriverMinorVersion();

	unsigned int getDriverPatchVersion();

	const std::string& getDriverName();

	const std::string& getDriverVersion();

	sql::ResultSet * getExportedKeys(const std::string& catalog, const std::string& schema, const std::string& table);

	const std::string& getExtraNameCharacters();

	const std::string& getIdentifierQuoteString();

	sql::ResultSet * getImportedKeys(const std::string& catalog, const std::string& schema, const std::string& table);

	sql::ResultSet * getIndexInfo(const std::string& catalog, const std::string& schema, const std::string& table, bool unique, bool approximate);

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

	const std::string& getNumericFunctions();

	sql::ResultSet * getPrimaryKeys(const std::string& catalog, const std::string& schema, const std::string& table);

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

	std::string getUserName();

	sql::ResultSet * getVersionColumns(const std::string& catalog, const std::string& schema, const std::string& table);

	bool insertsAreDetected(int type);

	bool isCatalogAtStart();

	bool isReadOnly();

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

	sql::ResultSet *getSchemaObjects(const std::string& catalogName = "", const std::string& schemaName = "", const std::string& objectType = "");

	// Returns all schema object types this database supports
	sql::ResultSet *getSchemaObjectTypes();

private:
	bool matchTable(const std::string& sPattern, const std::string& tPattern, const std::string& schema, const std::string& table);
	bool parseImportedKeys(
		const std::string& def,
		std::string & constraint_name,
		std::map< std::string, std::string > & keywords_names,
		std::map< std::string, std::list< std::string > > & referenced_fields,
		std::map< std::string, int > & update_cascade
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
