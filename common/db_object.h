/*
 * Copyright (c) 2015, 2017, Oracle and/or its affiliates. All rights reserved.
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

#ifndef MYSQLX_COMMON_DB_OBJECT_H
#define MYSQLX_COMMON_DB_OBJECT_H

#include <mysql/cdk.h>


namespace mysqlx {

// --------------------------------------------------------------------

namespace common {

  class Object_ref;

  class Schema_ref : public cdk::api::Schema_ref
  {
    cdk::string m_name;

  public:

    Schema_ref(const cdk::string &name) : m_name(name) {}
    Schema_ref() = default;

    const cdk::string name() const { return m_name; }

    friend Object_ref;
  };


  class Object_ref : public cdk::api::Object_ref
  {
    Schema_ref m_schema;
    const cdk::string m_name;

  public:

    const cdk::string name() const { return m_name; }
    const cdk::api::Schema_ref* schema() const { return &m_schema; }

    Object_ref(const cdk::string &schema, const cdk::string &name)
      : m_schema(schema), m_name(name)
    {}

    Object_ref(const cdk::string &name)
      : m_name(name)
    {}

    Object_ref(const cdk::api::Object_ref &other)
      : Object_ref(other.name())
    {
      if (other.schema())
        m_schema.m_name = other.schema()->name();
    }
  };

} // common
} // mysqlx

#endif
