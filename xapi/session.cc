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

MYSQLX_SESSION::MYSQLX_SESSION_T(const std::string host, unsigned int port, const string usr,
                  const std::string *pwd, const std::string *db, bool is_node_sess)
                  : m_sess_opt(host, port, usr, pwd, db), m_session(m_sess_opt, m_sess_opt),
                    m_crud(NULL), m_is_node_sess(is_node_sess)
{ }

MYSQLX_SESSION::MYSQLX_SESSION_T(const std::string &conn_str, bool is_node_sess)
  : m_sess_opt(conn_str), m_session(m_sess_opt, m_sess_opt),
    m_crud(NULL), m_is_node_sess(is_node_sess)
{}

MYSQLX_CRUD * MYSQLX_SESSION::sql_query(const char *query, uint32_t length)
{
  if (!m_is_node_sess)
    throw MYSQLX_EXCEPTION(MYSQLX_EXCEPTION::MYSQLX_EXCEPTION_INTERNAL, 0,
                          "Executing SQL is not supported for this session type.");
  // This will be removed after adding multiple CRUD's per session
  if (m_crud)
    delete m_crud;

  if (length == MYSQLX_NULL_TERMINATED)
    length = (uint32_t)strlen(query);

  m_crud = new MYSQLX_CRUD(this, query, length);
  return m_crud;
}

MYSQLX_CRUD * MYSQLX_SESSION::crud_op(const char *schema, const char *obj_name,
                                      MYSQLX_OP op_type)
{
  // This will be removed after adding multiple CRUD's per session
  if (m_crud)
    delete m_crud;

  if (!schema && !m_sess_opt.database())
  {
    m_crud = NULL;
    throw MYSQLX_EXCEPTION("The default schema is not specified");
  }

  m_crud = new MYSQLX_CRUD(this, schema ? schema : *m_sess_opt.database(), obj_name, op_type);
  return m_crud;
}

void MYSQLX_SESSION::reset_crud(MYSQLX_CRUD*)
{
  if (m_crud)
    delete m_crud;
  m_crud = NULL;
}

MYSQLX_ERROR * MYSQLX_SESSION::get_last_error()
{
  // TODO: implement warnings and info using Diagnostic_iterator

  // Return session errors from CDK first
  if (m_session.entry_count())
  {
      m_error.set(&m_session.get_error());
  }
  else if (!m_error.message() && !m_error.error_num())
    return NULL;

  return &m_error;
}

const cdk::Error * MYSQLX_SESSION::get_cdk_error()
{
  if (m_session.entry_count())
    return &m_session.get_error();

  return NULL;
}

void MYSQLX_SESSION::set_diagnostic(const MYSQLX_EXCEPTION &ex)
{
  m_error.set(ex);
}

void MYSQLX_SESSION::set_diagnostic(const char *msg, unsigned int num)
{
  m_error.set(msg, num);
}

void MYSQLX_SESSION::reset_diagnostic()
{
  m_error.reset();
}

void MYSQLX_SESSION::admin_collection(const char *cmd,
                                     const char *schema,
                                     const char *coll_name)
{
  if (!schema && !m_sess_opt.database())
  {
    throw MYSQLX_EXCEPTION("The default schema is not specified");
  }

  Db_obj_ref tab_ref(schema ? schema : *m_sess_opt.database(), coll_name);
  cdk::Reply reply;
  reply = m_session.admin(cmd, tab_ref);
  reply.wait();
  if (reply.entry_count())
  {
    const cdk::Error &err = reply.get_error();
    if (err.code() != cdk::server_error(SKIP_ERROR_COLL_EXISTS))
      throw err;
  }
}

void MYSQLX_SESSION::create_schema(const char *schema)
{
  std::stringstream sstr;
  sstr << "CREATE SCHEMA IF NOT EXISTS `" << schema << "`";

  cdk::Reply reply;
  reply = m_session.sql(sstr.str());
  reply.wait();
  if (reply.entry_count())
    throw reply.get_error();
}

void MYSQLX_SESSION::drop_object(const char *schema, const char *name,
                                 Object_type obj_type)

{
  if (obj_type == COLLECTION)
  {
    admin_collection("drop_collection", schema, name);
    return;
  }

  cdk::Reply reply;
  std::stringstream sstr;

  switch(obj_type)
  {
    case SCHEMA:
      sstr << "DROP SCHEMA "; break;
    case TABLE:
      sstr << "DROP TABLE "; break;
    case VIEW:
      sstr << "DROP VIEW "; break;
    break;
    default:
      throw MYSQLX_EXCEPTION(MYSQLX_EXCEPTION::MYSQLX_EXCEPTION_INTERNAL, 0,
                             "Attempt to drop an object of unknown type.");
  }

  sstr << "IF EXISTS ";

  if (schema && schema[0])
    sstr << " `" << schema << "`";

  if (schema && schema[0] && name && name[0])
    sstr << ".";

  if (name && name[0])
    sstr << " `" << name << "`";

  reply = m_session.sql(sstr.str());
  reply.wait();

  if (reply.entry_count())
    throw reply.get_error();
}

void MYSQLX_SESSION::transaction_begin()
{
  m_session.begin();
}

void MYSQLX_SESSION::transaction_commit()
{
  m_session.commit();
}

void MYSQLX_SESSION::transaction_rollback()
{
  m_session.rollback();
}

MYSQLX_SESSION::~MYSQLX_SESSION_T()
{
  try
  {
    reset_crud(m_crud);
    m_session.close();
  }
  catch(...)
  {
    // Do not do anything
  }
}
