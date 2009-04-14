/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_RESULTBIND_H_
#define _MYSQL_RESULTBIND_H_

#include <cppconn/prepared_statement.h>
#include <cppconn/parameter_metadata.h>

#include "mysql_private_iface.h"
#include "mysql_util.h"

namespace sql
{
namespace mysql
{

class MySQL_ResultBind
{

	unsigned int num_fields;
	sql::mysql::util::my_array_guard< my_bool > is_null;
	sql::mysql::util::my_array_guard< my_bool > err;
	sql::mysql::util::my_array_guard< unsigned long > len;

	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger;
	MYSQL_STMT * stmt;

public:
	sql::mysql::util::my_array_guard< MYSQL_BIND > rbind;


	MySQL_ResultBind(MYSQL_STMT * s, sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * log)
		: num_fields(0), is_null(NULL), err(NULL), len(NULL), logger(log), stmt(s), rbind(NULL) {}
	~MySQL_ResultBind();

	void bindResult();

};

} /* namespace mysql */
} /* namespace sql */

#endif /* _MYSQL_RESULTBIND_H_ */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

