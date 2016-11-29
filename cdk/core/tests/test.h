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

#ifndef MYSQL_CDK_CORE_TESTS_TEST_H
#define MYSQL_CDK_CORE_TESTS_TEST_H

#include "xplugin_test.h"
#include <mysql/cdk.h>

namespace cdk {
namespace test {


class Core_test
  : public cdk::test::Xplugin
{
  scoped_ptr<ds::TCPIP> m_ds;

public:

  ds::TCPIP& get_ds()
  {
    if (!m_ds)
      m_ds.reset(new ds::TCPIP(m_host, m_port));
    return *m_ds;
  }

  ds::TCPIP::Options& get_opts()
  {
    // TODO: make it configurable through env. variables
    static ds::TCPIP::Options opts("root", NULL);
    opts.set_database("test");
    return opts;
  }

  class Session : public cdk::Session
  {
  public:

    Session(Core_test *fixture)
      : cdk::Session(fixture->get_ds(), fixture->get_opts())
    {}
  };

};

template <class X>
struct Helper
{
  unsigned m_pos;

  void set_pos(unsigned pos) const
  {
    const_cast<Helper*>(this)->m_pos = pos;
  }

  X& get_ref() const
  {
    return *const_cast<X*>(static_cast<const X*>(this));
  }
};


class Schema_ref : public cdk::api::Schema_ref
{
  const cdk::string m_name;
  bool  m_null;

public:

  const cdk::string name() const { return m_name; }
  bool is_null() const { return m_null; }

  Schema_ref(const cdk::string &name) : m_name(name), m_null(false) {}
  Schema_ref() : m_null(true) {}
};


class Table_ref
  : public cdk::api::Table_ref
  // to be able to pass schema.name as admin command arguments
  , public cdk::Any_list
{
  Schema_ref m_schema;
  const cdk::string m_name;

public:

  const cdk::string name() const { return m_name; }
  const cdk::api::Schema_ref* schema() const { return m_schema.is_null() ? NULL : &m_schema; }

  Table_ref(const cdk::string &name)
    : m_name(name)
  {}

  Table_ref(const cdk::string &name, const cdk::string &schema)
    : m_schema(schema), m_name(name)
  {}

  friend std::ostream& operator<<(std::ostream &out, const Table_ref &tbl);

private:

  /*
    Any_list: pass schema.name as a list of two strings, starting with
    schema name (null if not present) followed by table name.
  */

  void process(Processor &prc) const
  {
    prc.list_begin();
    if (m_schema.is_null())
      prc.list_el()->scalar()->null();
    else
      prc.list_el()->scalar()->str(m_schema.name());
    prc.list_el()->scalar()->str(m_name);
    prc.list_end();
  }

};


inline
std::ostream& operator<<(std::ostream &out, const Table_ref &tbl)
{
  if (tbl.schema())
    out <<"`" <<tbl.schema()->name() <<"`.";
  out <<"`" <<tbl.name() <<"`";
  return out;
}


}} // cdk::test

#endif
