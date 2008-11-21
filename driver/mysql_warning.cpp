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
const std::string & errCode2SqlState (int errCode)
{
	static const std::string state = "";

	return state;
}

sql::SQLWarning * loadMysqlWarnings(MySQL_Connection * connection)
{
	SQLWarning * first = NULL, * current = NULL;

	if (connection != NULL) {
		std::auto_ptr<sql::Statement> stmt(connection->createStatement());
		std::auto_ptr<sql::ResultSet> rset(stmt->executeQuery("SHOW WARNINGS"));

		while (rset->next()) {
			int errCode = rset->getInt(2);

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


