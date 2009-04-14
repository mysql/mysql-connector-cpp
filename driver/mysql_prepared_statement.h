/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_PREPARED_STATEMENT_H_
#define _MYSQL_PREPARED_STATEMENT_H_

#include <cppconn/prepared_statement.h>
#include <cppconn/parameter_metadata.h>

#include "mysql_private_iface.h"


namespace sql
{
namespace mysql
{
namespace util {template<class T> class my_shared_ptr; }; // forward declaration.
class MySQL_DebugLogger;
class MySQL_ParamBind;
class MySQL_ParameterMetaData;
class MySQL_Prepared_ResultSetMetaData;
class MySQL_ResultBind;

class MySQL_Prepared_Statement : public sql::PreparedStatement
{
protected:
	sql::Connection * connection;
	MYSQL_STMT * stmt;
	std::auto_ptr< MySQL_ParamBind > param_bind;
	unsigned int param_count;

	int resultSetConcurrency;
	int resultSetType;

	std::auto_ptr< sql::SQLWarning > warnings;

	bool isClosed;

	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger;

	std::auto_ptr< MySQL_Prepared_ResultSetMetaData > res_meta;
	std::auto_ptr< MySQL_ParameterMetaData > param_meta;

	sql::ResultSet::enum_type resultset_type;

	virtual void do_query();
	virtual void checkClosed();
	virtual void closeIntern();

	bool sendLongDataBeforeParamBind();
	void setBlob_intern(unsigned int parameterIndex, std::istream * blob, bool deleteBlobAfterExecute);

public:

	MySQL_Prepared_Statement(MYSQL_STMT *s, sql::Connection * conn, sql::ResultSet::enum_type rset_type, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * log);
	virtual ~MySQL_Prepared_Statement();

	sql::Connection *getConnection();

	void clearParameters();

	void clearWarnings();

	void close();

	bool execute();
	bool execute(const std::string& sql);

	sql::ResultSet *executeQuery();
	sql::ResultSet *executeQuery(const std::string& sql);

	int executeUpdate();
	int executeUpdate(const std::string& sql);

	sql::ResultSetMetaData * getMetaData();

	bool getMoreResults();

	sql::ParameterMetaData * getParameterMetaData();

	sql::ResultSet * getResultSet();

	sql::ResultSet::enum_type getResultSetType();

	uint64_t getUpdateCount();

	const SQLWarning * getWarnings();/* should return differen type */

	Statement * setBuffered();

	void setBlob(unsigned int parameterIndex, std::istream * blob);

	void setBoolean(unsigned int parameterIndex, bool value);

	void setBigInt(unsigned int parameterIndex, const std::string& value);

	void setDateTime(unsigned int parameterIndex, const std::string& value);

	void setDouble(unsigned int parameterIndex, double value);

	void setInt(unsigned int parameterIndex, int32_t value);

	void setUInt(unsigned int parameterIndex, uint32_t value);

	void setInt64(unsigned int parameterIndex, int64_t value);

	void setUInt64(unsigned int parameterIndex, uint64_t value);

	void setNull(unsigned int parameterIndex, int sqlType);

	void setResultSetConcurrency(int concurrencyFlag);

	void setString(unsigned int parameterIndex, const std::string& value);

	sql::PreparedStatement * setResultSetType(sql::ResultSet::enum_type type);

private:
	/* Prevent use of these */
	MySQL_Prepared_Statement(const MySQL_Prepared_Statement &);
	void operator=(MySQL_Prepared_Statement &);
};

} /* namespace mysql */
} /* namespace sql */
#endif // _MYSQL_PREPARED_STATEMENT_H_


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
