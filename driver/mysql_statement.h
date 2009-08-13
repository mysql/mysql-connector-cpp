/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_STATEMENT_H_
#define _MYSQL_STATEMENT_H_

#include <boost/shared_ptr.hpp>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include "mysql_util.h"

namespace sql
{
namespace mysql
{
class MySQL_Connection;
class MySQL_DebugLogger;


namespace NativeAPI
{
class NativeResultsetWrapper;
class NativeConnectionWrapper;
}

class MySQL_Statement : public sql::Statement
{
protected:
	sql::SQLWarning * warnings;
	MySQL_Connection * connection;
	boost::shared_ptr< NativeAPI::NativeConnectionWrapper > proxy;

	void do_query(const char *q, size_t length);
	bool isClosed;

	uint64_t last_update_count;

	boost::shared_ptr< MySQL_DebugLogger > logger;

	sql::ResultSet::enum_type resultset_type;

	virtual boost::shared_ptr< NativeAPI::NativeResultsetWrapper > get_resultset();
	virtual void checkClosed();

public:
	MySQL_Statement(MySQL_Connection * conn, boost::shared_ptr< NativeAPI::NativeConnectionWrapper > & _proxy,
					sql::ResultSet::enum_type rset_type, boost::shared_ptr< MySQL_DebugLogger > & l);

	~MySQL_Statement();

	sql::Connection * getConnection();

	void cancel();

	void clearWarnings();

	void close();

	bool execute(const sql::SQLString& sql);

	sql::ResultSet * executeQuery(const sql::SQLString& sql);

	int executeUpdate(const sql::SQLString& sql);

	size_t getFetchSize();

	unsigned int getMaxFieldSize();

	uint64_t getMaxRows();

	bool getMoreResults();

	unsigned int getQueryTimeout();

	sql::ResultSet * getResultSet();

	sql::ResultSet::enum_type getResultSetType();

	uint64_t getUpdateCount();

	const SQLWarning * getWarnings();/* should return differen type */

	Statement * setBuffered();

	void setCursorName(const sql::SQLString & name);

	void setEscapeProcessing(bool enable);

	void setFetchSize(size_t rows);

	void setMaxFieldSize(unsigned int max);

	void setMaxRows(unsigned int max);

	void setQueryTimeout(unsigned int seconds);

	sql::Statement * setResultSetType(sql::ResultSet::enum_type type);
private:
	/* Prevent use of these */
	MySQL_Statement(const MySQL_Statement &);
	void operator=(MySQL_Statement &);
};

} /* namespace mysql */
} /* namespace sql */
#endif // _MYSQL_STATEMENT_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
