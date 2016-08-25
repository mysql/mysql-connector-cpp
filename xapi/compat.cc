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
#include <iostream>
#include "mysqlx_cc_internal.h"
#include "compat_internal.h"

MYSQL * STDCALL mysql_init(MYSQL *mysql)
{
  if (mysql)
  {
    /* Statically allocated MYSQL handler */
  }
  else
  {
    /* Dynamically allocated MYSQL handler */
    mysql = new MYSQL(false);
  }

  return mysql;
}

MYSQL *	STDCALL mysql_real_connect(MYSQL *mysql, const char *host,
					   const char *user,
					   const char *passwd,
					   const char *db,
					   unsigned int port,
					   const char *, // unix_socket
					   unsigned long // client_flag
             )
{
  if (mysql)
  {
    mysql->m_session.reset(mysqlx_get_node_session_s(host, port, user, passwd, db,
                                                mysql->m_connect_error,
                                                &mysql->m_connect_error_code));
  }
  else
  {
    // TODO: set the error
    return NULL;
  }
  return mysql;
}

int STDCALL mysql_real_query(MYSQL *mysql, const char *q, unsigned long length)
{
  // TODO: Error processing, correct error codes, etc.
  if (!mysql->set_crud(mysqlx_sql_query(mysql->m_session.get(), q, length)))
  {
    mysql->m_error.set("Error initializing CRUD request", CR_UNKNOWN_ERROR);
    return CR_UNKNOWN_ERROR;
  }

  if (!mysql->set_result(mysqlx_crud_execute(mysql->m_crud.get())))
  {
    mysql->m_error.set("Error getting the result", CR_UNKNOWN_ERROR);
    return CR_UNKNOWN_ERROR;
  }
  return 0;
}


unsigned int STDCALL mysql_num_fields(MYSQL_RES *res)
{
  return res->column_get_count();
}

MYSQL_RES * STDCALL mysql_store_result(MYSQL *mysql)
{
  MYSQL_RES * res = new MYSQL_RES(mysql->get_result());
  res->store_result();
  return res;
}

MYSQL_RES * STDCALL mysql_use_result(MYSQL *mysql)
{
  MYSQL_RES * res = new MYSQL_RES(mysql->get_result());
  return res;
}

const char * STDCALL mysql_error(MYSQL *mysql)
{
  if (mysql->m_connect_error[0])
    return mysql->m_connect_error;

  return mysql->m_error.message();
}

void STDCALL mysql_close(MYSQL *sock)
{
  mysqlx_session_close(sock->m_session.get());
  // Delete the MYSQL object only if it was dynamically allocated
  if (!sock->m_static_alloc)
    delete sock;
}

MYSQL_ROW	STDCALL mysql_fetch_row(MYSQL_RES *result)
{
  return result->fetch_row();
}

MYSQL_FIELD * STDCALL mysql_fetch_field(MYSQL_RES *result)
{
  return result->fetch_next_field();
}

unsigned long * STDCALL mysql_fetch_lengths(MYSQL_RES *result)
{
  return result->lenghts();
}

void STDCALL mysql_free_result(MYSQL_RES *result)
{
  result->get()->get_crud().set_result(NULL);
}