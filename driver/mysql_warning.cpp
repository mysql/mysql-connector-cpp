/*
  Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

  The MySQL Connector/C++ is licensed under the terms of the GPLv2
  <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
  MySQL Connectors. There are special exceptions to the terms and
  conditions of the GPLv2 as it is applied to this software, see the
  FLOSS License Exception
  <http://www.mysql.com/about/legal/licensing/foss-exception.html>.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published
  by the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

#include "mysql_warning.h"
#include <boost/scoped_ptr.hpp>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>

namespace sql
{
namespace mysql
{
	MySQL_Warning::MySQL_Warning(const sql::SQLString& reason, const sql::SQLString& SQLState, int vendorCode)
		:sql_state(SQLState), errNo(vendorCode), descr(reason), next(NULL)
	{
	}

	MySQL_Warning::MySQL_Warning(const sql::SQLString& reason, const sql::SQLString& SQLState)
		:sql_state (SQLState), errNo(0), descr(reason), next(NULL)
	{
	}

	MySQL_Warning::MySQL_Warning(const sql::SQLString& reason)
		: sql_state ("HY000"), errNo(0), descr(reason), next(NULL)
	{
	}

	MySQL_Warning::MySQL_Warning() : sql_state ("HY000"), errNo(0), next(NULL) {}


	const sql::SQLString & MySQL_Warning::getMessage() const
	{
		return descr;
	}


	const sql::SQLString & MySQL_Warning::getSQLState() const
	{
		return sql_state;
	}

	int MySQL_Warning::getErrorCode() const
	{
		return errNo;
	}

	const SQLWarning * MySQL_Warning::getNextWarning() const
	{
		return next.get();
	}

	MySQL_Warning::~MySQL_Warning()
	{
	}


    /* We don't really want it to be called, but we need to implement it */
	void MySQL_Warning::setNextWarning(const SQLWarning * _next)
	{
		if (_next)
		{
			next.reset(new MySQL_Warning(*_next));
		}
		else
		{
			next.reset();
		}
	}


	void MySQL_Warning::setNextWarning(MySQL_Warning * _next)
	{
		next.reset(_next);
	}

	MySQL_Warning::MySQL_Warning(const MySQL_Warning& w)
		: sql_state(w.sql_state), errNo(w.errNo), next(w.next.get()), descr(w.descr) {}

	MySQL_Warning::MySQL_Warning(const ::sql::SQLWarning & w)
		:	sql_state	(w.getSQLState()),
			errNo		(w.getErrorCode()),
			descr		(w.getMessage())
	{

		setNextWarning(w.getNextWarning());
	}


/*
*
*/
const sql::SQLString &
errCode2SqlState(int32_t errCode, sql::SQLString & state)
{
	switch (errCode) {
		case 1037:
		case 1038:
			state = "HY001";
			break;
		case 1040:
			state = "08004";
			break;

		case 1042:
		case 1043:
		case 1047:
		case 1053:
			state = "08S01";
			break;

		case 1044:
		case 1049:
		case 1055:
		case 1056:
		case 1057:
		case 1059:
		case 1061:
		case 1063:
		case 1064:
		case 1065:
		case 1066:
		case 1067:
		case 1068:
		case 1069:
		case 1070:
		case 1071:
		case 1072:
		case 1073:
		case 1074:
		case 1075:
			state = "42000";
			break;
		case 1045:
			state = "28000";
			break;
		case 1046:
			state = "3D000";
			break;
		case 1048:
		case 1052:
		case 1062:
			state = "23000";
			break;
		case 1050:
			state = "42501";
			break;
		case 1051:
			state = "42S02";
			break;
		case 1054:
			state = "42S22";
			break;
		case 1058:
			state = "21S01";
			break;
		case 1060:
			state = "42S21";
			break;

		case 1264:
			state = "22003";
			break;

		case 1000:
		case 1001:
		case 1002:
		case 1003:
		case 1004:
		case 1005:
		case 1006:
		case 1007:
		case 1008:
		case 1009:
		case 1010:
		case 1011:
		case 1012:
		case 1013:
		case 1014:
		case 1015:
		case 1016:
		case 1017:
		case 1018:
		case 1019:
		case 1020:
		case 1021:
		case 1022:
		case 1023:
		case 1024:
		case 1025:
		case 1026:
		case 1027:
		case 1028:
		case 1029:
		case 1030:
		case 1031:
		case 1032:
		case 1033:
		case 1034:
		case 1035:
		case 1036:
		case 1076:
			state = "HY000";
			break;

		default:
			state = "";
			break;
	}

	return state;
}


MySQL_Warning *
loadMysqlWarnings(sql::Connection * connection)
{
	MySQL_Warning * first = NULL, * current = NULL;
	SQLString state;

	if (connection != NULL) {
		boost::scoped_ptr< sql::Statement > stmt(connection->createStatement());
		boost::scoped_ptr< sql::ResultSet > rset(stmt->executeQuery("SHOW WARNINGS"));

		while (rset->next()) {
			// 1 - Level
			// 2 - Code
			// 3 - Message
			int32_t errCode = rset->getInt(2);

			if (current == NULL) {
				first = current = new MySQL_Warning(sql::SQLString(rset->getString(3)), errCode2SqlState(errCode, state), errCode);
			} else {
				MySQL_Warning * tmp = new MySQL_Warning(sql::SQLString(rset->getString(3)), errCode2SqlState(errCode, state), errCode);
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


