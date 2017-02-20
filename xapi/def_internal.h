/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

typedef parser::Expression_parser Expression_parser;
typedef uint64_t row_count_t;
typedef cdk::api::Sort_direction Sort_direction;

typedef enum mysqlx_op_enum
{
/*
  Table operation codes
*/
  OP_SELECT = 1, OP_INSERT = 2, OP_UPDATE = 3, OP_DELETE = 4,
/*
  Document operation codes
*/
  OP_FIND = 5, OP_ADD = 6, OP_MODIFY = 7, OP_REMOVE = 8,
/*
  Plain SQL operation
*/
  OP_SQL = 9,

/*
  View operation codes
*/
  OP_VIEW_CREATE = 10, OP_VIEW_UPDATE = 11, OP_VIEW_REPLACE = 12,

  OP_ADMIN_LIST = 13
} mysqlx_op_t;

typedef enum mysqlx_modify_op_enum
{
  MODIFY_SET = 1,
  MODIFY_UNSET = 2,
  MODIFY_ARRAY_INSERT = 3,
  MODIFY_ARRAY_APPEND = 4,
  MODIFY_ARRAY_DELETE = 5
} mysqlx_modify_op;