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

/*
  CRUD implementation
  Copy constructor must be disabled for this class
*/

class Schema_ref : public cdk::api::Schema_ref
{
  const cdk::string m_name;
  bool m_null;

public:

  Schema_ref() : m_null(true) {}
  Schema_ref(const cdk::string &name) : m_name(name), m_null(false) {}

  const cdk::string name() const { return m_name; }
  bool is_null() const { return m_null; }
};

/*
  Class for representing Db Object
*/
class Db_obj_ref : public cdk::api::Object_ref,
  public cdk::Any_list
{
  Schema_ref m_schema;
  const cdk::string m_name;

public:

  const cdk::string name() const { return m_name; }
  const cdk::api::Schema_ref* schema() const { return &m_schema; }

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
  
/*
  TODO: add constructor for collections later

  Table_ref(const Collection &coll)
    : m_schema(coll.getSchema().getName())
    , m_name(coll.getName())
  {}
*/

  Db_obj_ref()
  {}

  Db_obj_ref(const cdk::string &schema, const cdk::string &name)
    : m_schema(schema), m_name(name)
  {}
};
