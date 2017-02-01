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

mysqlx_schema_struct::mysqlx_schema_struct(mysqlx_session_t &session, cdk::string name, bool check) :
      m_session(session), m_name(name), m_stmt(NULL)
  {
    if (check && !exists())
      throw Mysqlx_exception("No such schema");
  }

bool mysqlx_schema_struct::exists()
{
  // This SQL query can be enabled for X session
  mysqlx_stmt_t *stmt = m_session.sql_query("SHOW SCHEMAS LIKE ?",
                                            MYSQLX_NULL_TERMINATED, true);
  stmt->sql_bind(m_name);
  mysqlx_result_t *res = stmt->exec();
  return (res->store_result() > 0);
}

mysqlx_collection_t & mysqlx_schema_struct::get_collection(const char *name, bool check)
{
  if (!name || !(*name))
    throw Mysqlx_exception(MYSQLX_ERROR_MISSING_COLLECTION_NAME_MSG);
  cdk::string col_name = name;
  Collection_map::iterator it = m_collection_map.find(col_name);
  // Return existing collection if it was requested before
  if (it != m_collection_map.end())
    return it->second;

  m_collection_map.insert(std::make_pair(col_name,
                      mysqlx_collection_t(*this, col_name, check)));
  return m_collection_map.at(col_name);
}

mysqlx_table_t & mysqlx_schema_struct::get_table(const char *name, bool check)
{
  if (!name || !(*name))
    throw Mysqlx_exception(MYSQLX_ERROR_MISSING_TABLE_NAME_MSG);
  cdk::string tab_name = name;
  Table_map::iterator it = m_table_map.find(tab_name);
  // Return existing table if it was requested before
  if (it != m_table_map.end())
    return it->second;

  m_table_map.insert(std::make_pair(tab_name,
                      mysqlx_table_t(*this, tab_name, check)));
  return m_table_map.at(tab_name);
}

mysqlx_schema_struct::~mysqlx_schema_struct()
{
  if (m_stmt)
    delete m_stmt;
}

mysqlx_stmt_t *mysqlx_schema_struct::stmt_op(const cdk::string obj_name, mysqlx_op_t op_type,
                                             mysqlx_stmt_t *parent)
{
  if (m_stmt)
    delete m_stmt;

  m_stmt = m_session.stmt_op(m_name, obj_name, op_type, false, parent);
  if (!m_stmt)
    throw Mysqlx_exception("Error creating schema operation");
  return m_stmt;
}