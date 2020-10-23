/*
 * Copyright (c) 2015, 2020, Oracle and/or its affiliates.
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

#ifndef MYSQLX_TESTING_TEST_H
#define MYSQLX_TESTING_TEST_H

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>

#undef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

namespace mysqlx {
namespace test {

/*
  Fixture for tests that speak to xplugin. The xplugin port should be set
  with XPLUGIN_PORT env variable.
*/

class Xplugin : public ::testing::Test
{
public:

  using string = std::string;
  using result_t = std::list< std::vector<string> >;

private:

  virtual void sql_exec(string query) = 0;
  virtual result_t sql_query(string query) = 0;

  // Note: These are set from environment (see SetUpTestCase) and
  // shared by all tests.

  static string m_status;
  static const char *m_host;
  static unsigned short m_port;
  static string m_user;
  static string m_password;
  static const char *m_socket;
  static const char *m_srv;
  static string m_ca;
  static string m_version;

protected:

  // Per-test-case set-up.
  // Called before the first test in this test case.
  // Can be omitted if not needed.

  static void SetUpTestCase()
  {
    // Note: XPLUGIN_PORT must be defined.

    const char *xplugin_port = getenv("XPLUGIN_PORT");
    if (!xplugin_port)
    {
      m_status = "XPLUGIN_PORT not set";
      return;
    }
    m_port = (short)atoi(xplugin_port);
    if (!m_port)
      m_status = "invalid port number in XPLUGIN_PORT";

    m_socket              = getenv("MYSQLX_SOCKET");
    m_host                = getenv("XPLUGIN_HOST");
    const char *user      = getenv("XPLUGIN_USER");
    const char *password  = getenv("XPLUGIN_PASSWORD");
    m_srv                 = getenv("MYSQLX_SRV");  // FIXME: What it is?
    const char *ca        = getenv("MYSQLX_CA");

    if (user)
      m_user = user;
    if (password)
      m_password = password;
    if (ca)
      m_ca = ca;

    // Default values.

    if (!m_host || !strlen(m_host))
      m_host = "localhost";

    // By default use "root" user without any password.
    if (m_user.empty())
      m_user = "root";
  }


  // Per-test-case tear-down.
  // Called after the last test in this test case.
  // Can be omitted if not needed.

  static void TearDownTestCase()
  {}

  string m_save_user;
  string m_save_pwd;

  // You can define per-test set-up and tear-down logic as usual.
  virtual void SetUp()
  {
    // Note: We save and restore user and password to be on the safe side,
    // because it can be changed by Use_native_pwd (see below).

    m_save_user = m_user;
    m_save_pwd  = m_password;

    if (!has_xplugin())
      return;

    // Drop and re-create test schema to clear up after previous tests.

    sql_exec("DROP SCHEMA IF EXISTS test");
    sql_exec("CREATE SCHEMA test");

    // Get server version if not already done.

    if (m_version.empty())
    {
      // Note: version number is in 2-nd column.

      auto version = sql_query("SHOW VARIABLES LIKE 'version'");
      if (!version.empty())
        m_version = version.front()[1];
    }

    // Try to get server CA location, if not set.

    if (m_ca.empty())
    {
      auto res = sql_query(
        "select if("
        "@@ssl_ca REGEXP '^([^:]+:)?[/\\\\\\\\]'"
        ", @@ssl_ca"
        ", concat(ifnull(@@ssl_capath,@@datadir), @@ssl_ca))"
      );

      if (res.empty())
        return;

      m_ca = res.front()[0];
    }
  }

  virtual void TearDown()
  {
    m_user = m_save_user;
    m_password = m_save_pwd;
  }

  void set_status(std::string val)
  {
    m_status = std::move(val);
  }

public:

  bool has_xplugin() const
  {
    return m_status.empty();
  }

  const char* get_status() const
  {
    return m_status.c_str();
  }

  const char* get_host() const
  {
    return m_host;
  }

  const char* get_socket() const
  {
    return m_socket;
  }

  const char* get_srv() const
  {
    return m_srv;
  }

  unsigned short get_port() const
  {
    return m_port;
  }

  const char* get_user() const
  {
    return m_user.c_str();
  }

  const char* get_password() const
  {
    return m_password.empty() ? nullptr : m_password.c_str();
  }

  const char* get_ca() const
  {
    return m_ca.empty() ? nullptr : m_ca.c_str();
  }

  std::string get_uri() const
  {
    std::stringstream uri;
    uri << "mysqlx://" << get_user();
    if (get_password() && *get_password())
      uri << ":" << get_password();
    uri << "@" << get_host() << ":" << get_port();
    return uri.str();
  }

  bool is_server_version_less(int test_upper_version,
                              int test_lower_version,
                              int test_release_version)
  {
    if (m_version.empty())
      throw std::logic_error("Unknown server version");

    std::stringstream version;
    version << m_version;

    int upper_version, minor_version, release_version;
    char sep;
    version >> upper_version;
    version >> sep;
    version >> minor_version;
    version >> sep;
    version >> release_version;

    if ((upper_version < test_upper_version) ||
      (upper_version == test_upper_version &&
        minor_version < test_lower_version) ||
        (upper_version == test_upper_version &&
          minor_version == test_lower_version &&
          release_version < test_release_version))
    {
      return true;
    }
    return false;
  }

  friend class Use_native_pwd;
};


class Use_native_pwd
{
  Xplugin& m_xplugin;
  std::string m_user;
  std::string m_password;

public:
  Use_native_pwd(Xplugin &xplugin)
    : m_xplugin(xplugin)
  {
    m_xplugin.sql_exec("DROP USER If EXISTS unsecure_root ");
    m_xplugin.sql_exec("CREATE USER unsecure_root IDENTIFIED WITH 'mysql_native_password';");
    m_xplugin.sql_exec("grant all on *.* to unsecure_root;");
    m_user = m_xplugin.m_user;
    m_password = m_xplugin.m_password;
    m_xplugin.m_user = "unsecure_root";
    m_xplugin.m_password.clear();
  }

  ~Use_native_pwd()
  {
    m_xplugin.sql_exec("DROP USER unsecure_root");
    m_xplugin.m_user = m_user;
    m_xplugin.m_password = m_password;
  }
};

}} // mysql::test


#define SKIP_IF_NO_XPLUGIN  \
  if (!has_xplugin()) { std::cerr <<"SKIPPED: No server: " <<get_status() <<std::endl; return; }

#define SKIP_IF_NO_SOCKET  \
  if (!get_socket()) { std::cerr <<"SKIPPED: No unix socket" <<std::endl; return; }

#define SKIP_IF_NO_SRV_SERVICE  \
  if (!get_srv()) { std::cerr <<"SKIPPED: No MYSQLX_SRV defined." <<std::endl; return; }

#define SKIP_IF_SERVER_VERSION_LESS(x,y,z)\
  if (is_server_version_less(x, y, z)) \
  {\
    std::cerr <<"SKIPPED: " << \
    "Server version not supported (" \
    << x << "." << y <<"." << ")" << z <<std::endl; \
    return; \
  }

#define SKIP_TEST(A) \
  do { std::cerr << "SKIPPED: " << A << std::endl; return; } while (0)


#define EXPECT_ERR(Code) \
  do { \
    try { Code; FAIL() << "Expected an error"; } \
    catch (const std::exception &e) \
    { cout << "Expected error: " << e.what() << endl; } \
    catch (const char *e) { FAIL() << "Bad exception: " << e; } \
    catch (...) { FAIL() << "Bad exception"; } \
  } while(false)

#define USE_NATIVE_PWD  \
  mysqlx::test::Use_native_pwd __dummy_user__(*this)


#endif
