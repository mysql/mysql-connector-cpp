/*
* Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
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

class View_spec : public cdk::View_spec
{

  struct View_opts : cdk::View_spec::Options
  {
    cdk::View_algorithm::value m_algorithm;
    cdk::View_security::value m_security;
    cdk::View_check::value m_check;
    std::string m_definer;

    /*
      Set flags must be introduced in order not to overwrite
      previously set values by the default ones
    */
    bool m_algorithm_set, m_security_set, m_check_set, m_definer_set;

    View_opts() : m_algorithm_set(false), m_security_set(false),
                  m_check_set(false), m_definer_set(false)
    {}

    void process(Processor &prc) const;
  };

  View_opts m_opts;
  Db_obj_ref m_ref;
  cdk::string m_schema;
  op_type m_view_op_type;
  Column_source m_columns;

public:

  View_spec()
  {};

  View_spec(const cdk::string &schema, const cdk::string &name,
                       mysqlx_op_t op_type);

  void set_algorithm(int val);
  void set_security(int val);
  void set_check(int val);
  void set_definer(const char *val);
  void set_columns(va_list arg);

  void process(Processor &prc) const;

};