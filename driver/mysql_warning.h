/*
   Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

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
#include <cppconn/config.h>

namespace sql
{
namespace mysql
{
	const sql::SQLString & errCode2SqlState(int32_t errCode, ::sql::SQLString & state);

	sql::SQLWarning * loadMysqlWarnings(sql::Connection * connection);

} /* namespace mysql */
} /* namespace sql   */
#endif
