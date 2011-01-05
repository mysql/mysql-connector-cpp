/*
Copyright (c) 2009, 2011, Oracle and/or its affiliates. All rights reserved.

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
