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

#ifndef MYSQLX_CC_TESTS_TEST_H
#define MYSQLX_CC_TESTS_TEST_H

#include <iostream>
#include <gtest/gtest.h>
#include <mysql_xapi.h>

using namespace ::std;

#define SESS_CHECK(A) if((A) == NULL) { \
  cout << endl << mysqlx_error_message(mysqlx_error(get_session())) << endl; \
  FAIL(); \
}

#define RESULT_CHECK(A) if((A) == NULL) { \
  cout << endl << "Invalid NULL result" << endl; \
  FAIL(); \
}

#define CRUD_CHECK(A, C) if((A) == NULL) { \
  cout << endl << "Error!" << mysqlx_error_message(C) << endl; \
  FAIL(); \
}

#define ERR_CHECK(A, C) if((A) == RESULT_ERROR) { \
  cout << endl << "Error!" << mysqlx_stmt_error_message(C) << endl; \
  FAIL(); \
}


#define FAIL_COMPARE(A,B) { \
  cout << endl << "Incorrect value: [" << A << "]! Expected: [" << B << "]" << endl; \
  FAIL(); \
}

#define AUTHENTICATE() authenticate(); \
  if (get_session() == NULL) \
    FAIL()

class xapi : public ::testing::Test
{
protected:

  unsigned short m_port;
  const char *m_status;
  mysqlx_session_t *m_sess;

  const char *m_xplugin_usr;
  const char *m_xplugin_pwd;
  const char *m_xplugin_host;
  const char *m_xplugin_port;
  const char *m_xplugin_socket;

  xapi() : m_port(0), m_status(NULL), m_sess(NULL)
  {
    m_xplugin_port = getenv("XPLUGIN_PORT");
    if (!m_xplugin_port)
    {
      m_status = "XPLUGIN_PORT not set";
      return;
    }

    m_port = atoi(m_xplugin_port);
    if (!m_port)
      m_status = "invalid port number in XPLUGIN_PORT";

    m_xplugin_socket = getenv("MYSQLX_SOCKET");

    m_xplugin_usr = getenv("XPLUGIN_USER");
    m_xplugin_pwd = getenv("XPLUGIN_PASSWORD");
    m_xplugin_host = getenv("XPLUGIN_HOST");

    m_xplugin_usr = (m_xplugin_usr && strlen(m_xplugin_usr) ? m_xplugin_usr : "root");
    m_xplugin_pwd = (m_xplugin_pwd && strlen(m_xplugin_pwd) ? m_xplugin_pwd : NULL);
    m_xplugin_host = (m_xplugin_host && strlen(m_xplugin_host) ? m_xplugin_host : "127.0.0.1");
  }


  virtual void SetUp()
  {}

  virtual void TearDown()
  {
    cout << endl;
    if (m_sess)
      mysqlx_session_close(m_sess);
  }


  mysqlx_result_t* exec_sql(const char *query)
  {
    return exec_sql(get_session(), query);
  }

  mysqlx_result_t* exec_sql(mysqlx_session_t *sess, const char *query)
  {
    mysqlx_result_t *res = NULL;
    mysqlx_stmt_t *stmt = NULL;
    stmt = mysqlx_sql_new(sess, query, strlen(query));
    if (!stmt)
    {
      cout << "Could not create statement for SQL query: " << query << endl;
      cout << mysqlx_error_message(sess) << endl;
      return NULL;
    }

    res = mysqlx_execute(stmt);

    if (!res)
    {
      cout << "Error when executing SQL: " << query << endl;
      cout << mysqlx_error_message(stmt) << endl;
      return NULL;
    }

    return res;
  }

  void exec_sql_error(const char *query)
  {
    mysqlx_stmt_t *stmt = NULL;
    RESULT_CHECK(stmt = mysqlx_sql_new(get_session(), query, strlen(query)));
    if (mysqlx_execute(stmt))
    {
      FAIL() << " The error is expected. No error is reported!";
    }
    else
    {
      cout << "Expected Error:" << mysqlx_error_message(stmt) << endl;
    }
  }

  void authenticate(const char *usr = NULL, const char *pwd = NULL,
                    const char *db = NULL)
  {
    if (m_status)
      FAIL() << m_status;

    char conn_error[MYSQLX_MAX_ERROR_LEN] = { 0 };
    int conn_err_code = 0;

    const char *xplugin_usr = usr ? usr : m_xplugin_usr;
    const char *xplugin_pwd = pwd ? pwd : m_xplugin_pwd;
    const char *xplugin_host = m_xplugin_host;

    m_sess = mysqlx_get_session(xplugin_host, m_port, xplugin_usr, xplugin_pwd, db,
                                conn_error, &conn_err_code);

    if (!m_sess)
    {
      FAIL() << "Could not connect to xplugin at " << m_port << std::endl << conn_error <<
             " ERROR CODE: " << conn_err_code;
    }
    cout << "Connected to xplugin..." << endl;
  }


  std::string get_ca_file()
  {
    char buf[1024];

    mysqlx_result_t *res = exec_sql(
      "select if("
      "@@ssl_ca REGEXP '^([^:]+:)?[/\\\\\\\\]'"
      ", @@ssl_ca"
      ", concat(ifnull(@@ssl_capath,@@datadir), @@ssl_ca))"
    );

    if (!res)
      return std::string();

    mysqlx_row_t *row = mysqlx_row_fetch_one(res);

    if (!row)
      return std::string();

    size_t buf_len = sizeof(buf);
    if (RESULT_OK != mysqlx_get_bytes(row, 0, 0, buf, &buf_len))
      return std::string();

    if (buf_len < 2)
      return std::string();

    // Note: buf_len includes terminating '\0'
    return std::string(buf, buf + buf_len - 1);
  }


  std::string get_ssl_cipher(mysqlx_session_t *sess)
  {
    mysqlx_result_t *res = exec_sql(sess, "SHOW STATUS LIKE 'mysqlx_ssl_cipher'");

    if (!res)
      throw "Failed to query mysqlx_ssl_cipher status variable from server";

    mysqlx_row_t *row = mysqlx_row_fetch_one(res);

    if (!row)
      throw "Failed to get value of mysqlx_ssl_cipher status variable";

    char data[128] = { 0 };
    size_t data_len = sizeof(data);
    if (RESULT_OK !=  mysqlx_get_bytes(row, 1, 0, data, &data_len))
      throw "Failed to get value of mysqlx_ssl_cipher status variable";

    return (data_len > 1 ? std::string(data, data + data_len - 1)
                         : std::string() );
  }


public:
  mysqlx_session_t *get_session() { return m_sess; }

};


#define SKIP_IF_NO_XPLUGIN  \
  if (m_status) { std::cerr <<"SKIPPED: " <<m_status <<std::endl; return; }

#define SKIP_IF_NO_UNIX_SOCKET  \
  if (!m_xplugin_socket) { std::cerr << "SKIPPED: No Unix Socket" <<std::endl; return; }


class xapi_bugs : public xapi
{};


#endif
