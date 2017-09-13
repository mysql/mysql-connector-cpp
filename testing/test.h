/*
* Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
*
* The MySQL Connector/C++ is licensed under the terms of the GPLv2
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

#ifndef MYSQLX_TESTING_TEST_H
#define MYSQLX_TESTING_TEST_H

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

//#include "cdk_test.h"
#include <mysql_devapi.h>
#include <gtest/gtest.h>

namespace mysqlx {
  namespace test {

    /*
    Fixture for tests that speak to xplugin. The xplugin port should be set
    with XPLUGIN_PORT env variable.
    */

    class Xplugin : public ::testing::Test
    {
    public:

      class Session;

    protected:
      // Per-test-case set-up.
      // Called before the first test in this test case.
      // Can be omitted if not needed.
      static void SetUpTestCase()
      {

      }

      // Per-test-case tear-down.
      // Called after the last test in this test case.
      // Can be omitted if not needed.
      static void TearDownTestCase()
      {

      }

      const char *m_status;
      mysqlx::Session *m_sess;
      unsigned short m_port;
      const char *m_user;
      const char *m_password;
      const char *m_socket;

      // You can define per-test set-up and tear-down logic as usual.
      virtual void SetUp()
      {
        using namespace mysqlx;

        m_status = NULL;
        m_port = 0;
        m_socket = NULL;
        m_user = NULL;
        m_password = NULL;
        m_sess = NULL;

        const char *xplugin_port = getenv("XPLUGIN_PORT");
        if (!xplugin_port)
        {
          m_status = "XPLUGIN_PORT not set";
          return;
        }
        m_port = (short)atoi(xplugin_port);

        m_socket = getenv("MYSQLX_SOCKET");

        // By default use "root" user without any password.
        m_user = getenv("XPLUGIN_USER");
        if(!m_user)
          m_user = "root";

        m_password = getenv("XPLUGIN_PASSWORD");

        try {
          m_sess = new mysqlx::Session(
            SessionOption::PORT, m_port,
            SessionOption::USER, m_user,
            SessionOption::PWD, m_password,
            SessionOption::SSL_MODE, SSLMode::DISABLED
          );
        }
        catch (const Error &e)
        {
          m_sess = NULL;
          m_status = e.what();
          FAIL() << "Could not connect to xplugin at " << m_port << ": " << e;
        }

        // Drop and re-create test schema to clear up after previous tests.

        try {
          get_sess().dropSchema("test");
        }
        catch (const Error&)
        {}

        get_sess().createSchema("test");
      }

      virtual void TearDown()
      {
        delete m_sess;
      }

      Schema getSchema(const string &name)
      {
        return get_sess().getSchema(name);
      }

      SqlResult sql(const string &query)
      {
        return get_sess().sql(query).execute();
      }

      mysqlx::Session& get_sess() const
      {
        // TODO: better error.
        if (!m_sess)
         throw m_status;
        return *m_sess;
      }

      const char* get_socket() const
      {
        return m_socket;
      }

      unsigned short get_port() const
      {
        return m_port;
      }

      const char* get_user() const
      {
        return m_user;
      }

      const char* get_password() const
      {
        return m_password;
      }

      bool has_xplugin() const
      {
        return NULL == m_status;
      }

      bool is_server_version_less(int test_upper_version,
                                  int test_lower_version,
                                  int test_release_version)
      {
        SqlResult res_version= sql("SHOW VARIABLES LIKE 'version'");

        std::stringstream version;
        version << res_version.fetchOne()[1].get<string>();

        int upper_version, minor_version, release_version;
        char sep;
        version >> upper_version;
        version >> sep;
        version >> minor_version;
        version >> sep;
        version >> release_version;

        if ( (upper_version < test_upper_version) ||
             (upper_version == test_upper_version &&
              minor_version << test_lower_version) ||
             (upper_version == test_upper_version &&
              minor_version == test_lower_version &&
              release_version < test_release_version))
        {
          return true;
        }
        return false;
      }
    };

    class Xplugin::Session : public mysqlx::Session
    {
    public:

      Session(const Xplugin *test)
      : mysqlx::Session(test->get_port(), test->get_user(), test->get_password())
      {}
    };

  }
} // cdk::test

#define SKIP_IF_NO_XPLUGIN  \
  if (!has_xplugin()) { std::cerr <<"SKIPPED: " <<m_status <<std::endl; return; }

#define SKIP_IF_NO_SOCKET  \
  if (!get_socket()) { std::cerr <<"SKIPPED: No unix socket" <<std::endl; return; }

#define SKIP_IF_SERVER_VERSION_LESS(x,y,z)\
  if (is_server_version_less(x, y, z)) \
  {\
    std::cerr <<"SKIPPED: " << \
    "Server version not supported (" \
    << x << "." << y <<"." << ")" << z <<std::endl; \
    return; \
  }

// TODO: remove this when prepare is ok again
#define SKIP_TEST(A) std::cerr << "SKIPPED: " << A << std::endl; return;

#endif
