/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef __MYSQL_WARNING_H_
#define __MYSQL_WARNING_H_

#include "mysql_connection.h"
#include <cppconn/warning.h>

namespace sql
{
namespace mysql
{
	const std::string & errCode2SqlState(int errCode);

	sql::SQLWarning * loadMysqlWarnings(sql::Connection * connection);

} /* namespace mysql */
} /* namespace sql   */
#endif
