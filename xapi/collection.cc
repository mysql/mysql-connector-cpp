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

#include <mysql_xapi.h>
#include "mysqlx_cc_internal.h"

mysqlx_collection_struct::mysqlx_collection_struct(mysqlx_schema_t &schema,
                                                   cdk::string name,
                                                   bool check) :
  m_schema(schema), m_name(name), m_stmt(NULL)
{
  if (check && !exists())
    throw Mysqlx_exception("No such collection");
}

mysqlx_collection_struct::~mysqlx_collection_struct()
{
  if (m_stmt)
    delete m_stmt;
}

bool mysqlx_collection_struct::exists()
{
  mysqlx_stmt_t *stmt;
  mysqlx_session_t &sess = m_schema.get_session();

  if ((stmt = sess.stmt_op(m_schema.get_name(), m_name, OP_ADMIN_LIST)) == NULL)
    throw Mysqlx_exception("Collection could not be initialized");

  mysqlx_result_t *res = stmt->exec();
  if (res == NULL)
  {
    /*
      Somehow the exception was not thrown in stmt->exec(), but with
      NULL result this code cannot continue
    */
    throw Mysqlx_exception("Error checking the collection");
  }
  res->set_table_list_mask(FILTER_COLLECTION);
  return (res->store_result() > 0);
}

mysqlx_stmt_t *mysqlx_collection_struct::stmt_op(mysqlx_op_t op_type)
{
  if (m_stmt)
    delete m_stmt;

  m_stmt = m_schema.get_session().stmt_op(m_schema.get_name(),
                                          m_name, op_type, false);
  if (!m_stmt)
    throw Mysqlx_exception("Error creating collection operation");
  return m_stmt;
}