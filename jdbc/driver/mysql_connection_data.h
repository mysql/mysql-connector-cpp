/*
 * Copyright (c) 2008, 2023, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */



#ifndef _MYSQL_CONNECTION_DATA_H_
#define _MYSQL_CONNECTION_DATA_H_

#include <list>
#include <memory>
#include <cppconn/resultset.h>
#include "mysql_util.h"
#include "mysql_metadata.h"
#include "mysql_warning.h"
#include "mysql_telemetry.h"

namespace sql
{
namespace mysql
{

class MySQL_DebugLogger;
class MySQL_ConnectionMetaData;

struct MySQL_ConnectionData
{
  MySQL_ConnectionData(std::shared_ptr<MySQL_DebugLogger> &l)
      : closed(false),
        autocommit(false),
        txIsolationLevel(TRANSACTION_READ_COMMITTED),
        is_valid(false),
        sql_mode_set(false),
        cache_sql_mode(false),
        reconnect(false),
        defaultStatementResultType(sql::ResultSet::TYPE_SCROLL_INSENSITIVE),
        defaultPreparedStatementResultType(
            sql::ResultSet::TYPE_SCROLL_INSENSITIVE),
        logger(l)
  {}

  ~MySQL_ConnectionData()
  {
  }

  bool closed;
  bool autocommit;
  enum_transaction_isolation txIsolationLevel;

  std::unique_ptr<const MySQL_Warning> warnings;

  bool is_valid;

  sql::SQLString sql_mode;
  bool sql_mode_set;
  bool cache_sql_mode;
  bool reconnect;

  sql::ResultSet::enum_type defaultStatementResultType;
  sql::ResultSet::enum_type defaultPreparedStatementResultType;

  std::shared_ptr<MySQL_DebugLogger> logger;

  std::unique_ptr<MySQL_ConnectionMetaData> meta;

  telemetry::Telemetry<MySQL_Connection> telemetry;
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
