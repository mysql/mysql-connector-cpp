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

#ifndef _MYSQL_STATEMENT_H_
#define _MYSQL_STATEMENT_H_

#include <cppconn/statement.h>
#include "mysql_util.h"

namespace sql
{
namespace mysql
{
namespace util {template<class T> class my_shared_ptr; }; // forward declaration.
class MySQL_Connection;
class MYSQL_RES_Wrapper;
class MySQL_DebugLogger;

class MySQL_Statement : public sql::Statement
{
protected:
  std::auto_ptr<sql::SQLWarning> warnings;
	MySQL_Connection *connection;

	void do_query(const char *q, size_t length);
	bool isClosed;

	unsigned long long last_update_count;

	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger;

	virtual MYSQL_RES_Wrapper * get_resultset();
	virtual void checkClosed();

public:
	MySQL_Statement(MySQL_Connection * conn, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * l);
	~MySQL_Statement();

	sql::Connection *getConnection();

	void cancel();

	void clearWarnings();

	void close();

	bool execute(const std::string& sql);

	sql::ResultSet * executeQuery(const std::string& sql);

	int executeUpdate(const std::string& sql);

	unsigned int getFetchSize();

	unsigned int getMaxFieldSize();

	unsigned long long getMaxRows();

	bool getMoreResults();

	unsigned int getQueryTimeout();

	sql::ResultSet * getResultSet();

	long long getUpdateCount();

	const SQLWarning * getWarnings();/* should return differen type */

	void setCursorName(const std::string & name);

	void setEscapeProcessing(bool enable);

	void setFetchSize(unsigned int rows);

	void setMaxFieldSize(unsigned int max);

	void setMaxRows(unsigned int max);

	void setQueryTimeout(unsigned int seconds);

private:
	/* Prevent use of these */
	MySQL_Statement(const MySQL_Statement &);
	void operator=(MySQL_Statement &);
};

}; /* namespace mysql */
}; /* namespace sql */
#endif // _MYSQL_STATEMENT_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
