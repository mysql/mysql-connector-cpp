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

#ifndef _MYSQL_CONNECTION_DATA_H_
#define _MYSQL_CONNECTION_DATA_H_

#include <list>
#include "mysql_util.h"
struct st_mysql;

namespace sql
{
namespace mysql
{


class MySQL_DebugLogger;

class MySQL_ConnectionData
{
public:
	bool closed;
	bool autocommit;
	enum_transaction_isolation txIsolationLevel;

	/* disable compile warnings on Windows */
#if defined(_WIN32) || defined(_WIN64)
#pragma warning(push)
#pragma warning (disable : 4251)
#endif

  std::auto_ptr<const sql::SQLWarning> warnings;

#if defined(_WIN32) || defined(_WIN64)
#pragma warning (pop)
#endif

	bool is_valid;

	std::string sql_mode;

	sql::mysql::util::my_shared_ptr< MySQL_DebugLogger > * logger;

	struct ::st_mysql * mysql; /* let it be last . If wrong dll is used we will get valgrind error or runtime error !*/
};

}; /* namespace mysql */
}; /* namespace sql */

#endif // _MYSQL_CONNECTION_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
