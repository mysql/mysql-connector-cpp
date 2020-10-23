/*
 * Copyright (c) 2016, 2020, Oracle and/or its affiliates.
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

#ifndef MYSQLX_CC_TESTS_TEST_H
#define MYSQLX_CC_TESTS_TEST_H

#include <iostream>
#include <test.h>
#include <mysqlx/xapi.h>

#undef NULL
#define NULL nullptr

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
  cout << endl << "Error!" << mysqlx_error_message(C) << endl; \
  FAIL(); \
}


#define FAIL_COMPARE(A,B) { \
  cout << endl << "Incorrect value: [" << A << "]! Expected: [" << B << "]" << endl; \
  FAIL(); \
}

#define AUTHENTICATE() authenticate(); \
  if (get_session() == NULL) \
    FAIL()

class xapi : public ::mysqlx::test::Xplugin
{
protected:

  mysqlx_session_t *m_sess = NULL;

  virtual void TearDown() override
  {
    cout << endl;
    if (m_sess)
      mysqlx_session_close(m_sess);
    Xplugin::TearDown();
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
    if (!has_xplugin())
      FAIL() << get_status();

    const char *xplugin_usr = usr ? usr : get_user();
    const char *xplugin_pwd = pwd ? pwd : get_password();
    const char *xplugin_host = get_host();
    mysqlx_error_t *error = NULL;

    mysqlx_session_close(m_sess);
    m_sess = NULL;

    m_sess = mysqlx_get_session(
      xplugin_host, get_port(),
      xplugin_usr, xplugin_pwd,
      nullptr,
      &error
    );

    if (!m_sess)
    {
      FAIL() << "Could not connect to xplugin at " << get_port() << std::endl <<
                mysqlx_error_message(error) <<
             " ERROR CODE: " << mysqlx_error_num(error);
      mysqlx_free(error);
    }
    if (!m_auth_silent)
      cout << "Connected to xplugin..." << endl;

    if (db)
    {
      // Drop and re-create schema to clean it up.
      mysqlx_schema_drop(m_sess, db);
      mysqlx_schema_create(m_sess, db);
      string use = string("USE `") + db + "`";
      exec_sql(use.c_str());
    }
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

private:

  bool m_auth_silent = false;

  // SQL execution methods used by Xplugin fixture.

  mysqlx_result_t* sql_exec_throw(string query)
  {
    if (!m_sess)
    {
      m_auth_silent = true;
      authenticate();
      m_auth_silent = false;
    }
    assert(m_sess);

    mysqlx_result_t *res = exec_sql(query.c_str());;

    if (!res)
    {
      throw std::runtime_error("Error when executing SQL: " + query);
    }

    return res;
  }

  void sql_exec(string query) override
  {
    sql_exec_throw(std::move(query));
  }

  result_t sql_query(string query) override
  {
    mysqlx_result_t *res = sql_exec_throw(std::move(query));

    // Return rows as a list of vectors.

    result_t rows;
    size_t col_cnt = mysqlx_column_get_count(res);

    if (col_cnt)
    {
      mysqlx_row_t *row = nullptr;
      size_t row_cnt = 10;  // Note: we return at most 10 rows

      while ((row = mysqlx_row_fetch_one(res)) && (row_cnt-- > 0))
      {
        vector<string> row_data(col_cnt);
        for (size_t col=0; col < col_cnt; ++col)
        {
          string &buf = row_data[col];
          size_t buf_len = 128;
          buf.resize(buf_len);
          if (RESULT_OK == mysqlx_get_bytes(row, col, 0, (void*)buf.data(), &buf_len))
          {
            if (buf_len == 0)
              buf.clear();
            else
              buf.resize(buf_len);
            buf.shrink_to_fit();
          }
          else
            buf.clear();
        }

        rows.push_back(row_data);
      }
    }

    return rows;
  }


public:
  mysqlx_session_t *get_session() { return m_sess; }

  //friend class use_native_pwd;
};


class xapi_bugs : public xapi
{};


#endif
