/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include "mysql_warning.h"

#include <cppconn/statement.h>
#include <cppconn/resultset.h>

namespace sql
{
namespace mysql
{

/*
* TODO: implement it. Probably it's not the right place for this function
*/
const std::string &
errCode2SqlState(int /* errCode */)
{
	static const std::string state = "";

	return state;
}


sql::SQLWarning *
loadMysqlWarnings(sql::Connection * connection)
{
	SQLWarning * first = NULL, * current = NULL;

	if (connection != NULL) {
		std::auto_ptr<sql::Statement> stmt(connection->createStatement());
		std::auto_ptr<sql::ResultSet> rset(stmt->executeQuery("SHOW WARNINGS"));

		while (rset->next()) {
			// 1 - Level
			// 2 - Code
			// 3 - Message
			int32_t errCode = rset->getInt(2);

			if (current == NULL) {
				first = current = new SQLWarning(std::string(rset->getString(3)), errCode2SqlState(errCode), errCode);
			} else {
				SQLWarning * tmp = new SQLWarning(std::string(rset->getString(3)), errCode2SqlState(errCode), errCode);
				current->setNextWarning(tmp);
				current = tmp;
			}
		}
	}

	return first;
}

} /* namespace mysql */
} /* namespace sql   */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */


