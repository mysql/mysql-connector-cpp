/*
 * Copyright (c) 2020, Oracle and/or its affiliates. All rights reserved.
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

#ifndef MYSQLX_DEVAPI_TEST_H
#define MYSQLX_DEVAPI_TEST_H

#include <test.h>
#include <mysqlx/xdevapi.h>
#include <iostream>

namespace mysqlx {
namespace test {

/*
  Fixture for tests that speak to xplugin. The xplugin port should be set
  with XPLUGIN_PORT env variable.
*/

class DevAPI : public Xplugin
{
public:

  using string = mysqlx::string;

  class Client;
  class Session;

protected:


  mysqlx::Client *m_client = nullptr;
  mysqlx::Session *m_sess = nullptr;


  virtual void SetUp() override
  {
    using namespace mysqlx;

    Xplugin::SetUp();

    create_session();
  }

  virtual void TearDown() override
  {
    delete m_sess;
    delete m_client;
    Xplugin::TearDown();
  }


  Schema getSchema(const string &name)
  {
    return get_sess().getSchema(name);
  }

  SqlResult sql(const string &query)
  {
    return get_sess().sql(query).execute();
  }

  mysqlx::Client& get_client() const
  {
    // TODO: better error.
    if (!m_client)
      throw std::runtime_error(get_status());
    return *m_client;
  }

  mysqlx::Session& get_sess() const
  {
    // TODO: better error.
    if (!m_sess)
      throw std::runtime_error(get_status());
    return *m_sess;
  }

  void create_session()
  {
    if (!has_xplugin())
      return;

    try {
      if (!m_client)
      {
        m_client = new mysqlx::Client(
                     SessionOption::HOST, get_host(),
                     SessionOption::PORT, get_port(),
                     SessionOption::USER, get_user(),
                     SessionOption::PWD, get_password()
        );
      }
      delete m_sess;
      m_sess = nullptr;
      m_sess = new mysqlx::Session(*m_client);
    } catch (const Error &e)
    {
      delete m_client;
      delete m_sess;
      m_client = nullptr;
      m_sess = nullptr;
      set_status(e.what());
      FAIL() << "Could not connect to xplugin at " << get_port()
        << " (" << get_host() << ")" << ": " << e;
    }
  }

  void output_id_list(Result& res)
  {
    std::vector<std::string> ids = res.getGeneratedIds();
    for (auto id : ids)
    {
      std::cout << "- added doc with id: " << id << std::endl;
    }
  }

private:

  result_t sql_query(std::string query) override
  {
    assert(has_xplugin());
    if (!m_sess)
      create_session();
    assert(m_sess);

    SqlResult res = sql(query);
    size_t col_cnt = res.getColumnCount();
    result_t rows;

    if (res.hasData())
    {
      Row row;
      size_t row_cnt = 10;  // Note: we return at most 10 rows

      while ((row = res.fetchOne()) && (row_cnt-- > 0))
      {
        std::vector<std::string> row_data(col_cnt);
        for (size_t col = 0; col < col_cnt; ++col)
        {
          // Note: this assumes that all values are strings...
          row_data[col] = row[col].get<std::string>();
        }
        rows.push_back(row_data);
      }
    }

    return rows;
  }

  void sql_exec(std::string query) override
  {
    assert(has_xplugin());
    if (!m_sess)
      create_session();
    assert(m_sess);
    sql(query);
  }

};


class DevAPI::Client : public mysqlx::Client
{
public:

  Client(const Xplugin *test)
    : mysqlx::Client(
        SessionOption::HOST, test->get_host(),
        SessionOption::PORT, test->get_port(),
        SessionOption::USER, test->get_user(),
        SessionOption::PWD, test->get_password()
      )
  {}
};

class DevAPI::Session : public mysqlx::Session
{
public:

  Session(const DevAPI *test)
    : mysqlx::Session(test->get_client())
  {}
};


}
} // mysql::test

#endif
