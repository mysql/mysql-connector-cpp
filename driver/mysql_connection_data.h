/*
   Copyright (C) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_CONNECTION_DATA_H_
#define _MYSQL_CONNECTION_DATA_H_

#include <list>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <cppconn/resultset.h>
#include "mysql_util.h"
#include "mysql_metadata.h"
struct st_mysql;

namespace sql
{
namespace mysql
{

class MySQL_DebugLogger;
class MySQL_ConnectionMetaData;

class MySQL_ConnectionData
{
public:
	MySQL_ConnectionData(boost::shared_ptr< MySQL_DebugLogger > & l)
		: closed(false), autocommit(false), txIsolationLevel(TRANSACTION_READ_COMMITTED),
		  is_valid(false), sql_mode_set(false), cache_sql_mode(false),
		  metadata_use_info_schema(true),
		  defaultStatementResultType(sql::ResultSet::TYPE_SCROLL_INSENSITIVE),
		  defaultPreparedStatementResultType(sql::ResultSet::TYPE_SCROLL_INSENSITIVE),
		  logger(l), meta(NULL) {}

	~MySQL_ConnectionData() {}

	bool closed;
	bool autocommit;
	enum_transaction_isolation txIsolationLevel;

	/* disable compile warnings on Windows */
#ifdef _WIN32
#pragma warning(push)
#pragma warning (disable : 4251)
#endif

	boost::scoped_ptr<const sql::SQLWarning> warnings;

#ifdef _WIN32
#pragma warning (pop)
#endif

	bool is_valid;

	sql::SQLString sql_mode;
	bool sql_mode_set;
	bool cache_sql_mode;
	bool metadata_use_info_schema;

	sql::ResultSet::enum_type defaultStatementResultType;
	sql::ResultSet::enum_type defaultPreparedStatementResultType;

	boost::shared_ptr< MySQL_DebugLogger > logger;

	boost::scoped_ptr< MySQL_ConnectionMetaData > meta;
};

} /* namespace mysql */
} /* namespace sql */

#endif // _MYSQL_CONNECTION_DATA_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
