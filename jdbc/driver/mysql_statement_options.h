/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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



#ifndef _MYSQL_STATEMENT_OPTIONS_H_
#define _MYSQL_STATEMENT_OPTIONS_H_

namespace sql
{
namespace mysql
{

enum MySQL_Statement_Options
{
  /*
  When doing mysql_stmt_store_result calculate max_length attribute
  of statement metadata. This is to be consistent with the old API,
  where this was done automatically.
  In the new API we do that only by request because it slows down
  mysql_stmt_store_result sufficiently.
  */
  STMT_ATTR_UPDATE_MAX_LENGTH,
  /*
  unsigned long with combination of cursor flags (read only, for update,
  etc)
  */
  STMT_ATTR_CURSOR_TYPE,
  /*
  Amount of rows to retrieve from server per one fetch if using cursors.
  Accepts unsigned long attribute in the range 1 - ulong_max
  */
  STMT_ATTR_PREFETCH_ROWS
};

} /* namespace mysql */
} /* namespace sql */

#endif
