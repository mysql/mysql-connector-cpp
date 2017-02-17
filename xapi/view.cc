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
#include "mysqlx_cc_internal.h"


View_spec::View_spec(const cdk::string &schema, const cdk::string &name,
                     mysqlx_op_t type) : m_ref(schema, name)
{
  switch (type)
  {
    case OP_VIEW_CREATE:
      m_view_op_type = cdk::api::View_processor<cdk::View_options>::CREATE;
    break;
    case OP_VIEW_UPDATE:
      m_view_op_type = cdk::api::View_processor<cdk::View_options>::UPDATE;
    break;
    case OP_VIEW_REPLACE:
      m_view_op_type = cdk::api::View_processor<cdk::View_options>::REPLACE;
    break;
    default:
      throw Mysqlx_exception("Wrong VIEW operation");
  }
}


void View_spec::set_algorithm(int val)
{
  switch (val)
  {
    case VIEW_ALGORITHM_UNDEFINED:
      m_opts.m_algorithm = cdk::View_algorithm::UNDEFINED;
    break;
    case VIEW_ALGORITHM_MERGE:
      m_opts.m_algorithm = cdk::View_algorithm::MERGE;
    break;
    case VIEW_ALGORITHM_TEMPTABLE:
      m_opts.m_algorithm = cdk::View_algorithm::TEMPTABLE;
    break;
    default:
      throw Mysqlx_exception("Wrong value for VIEW algorithm");
  }
  m_opts.m_algorithm_set = true;
}

void View_spec::set_security(int val)
{
  switch (val)
  {
  case VIEW_SECURITY_DEFINER:
    m_opts.m_security = cdk::View_security::DEFINER;
  break;
  case VIEW_SECURITY_INVOKER:
    m_opts.m_security = cdk::View_security::INVOKER;
  break;
  default:
    throw Mysqlx_exception("Wrong value for VIEW security");
  }
  m_opts.m_security_set = true;
}

void View_spec::set_check(int val)
{
  switch (val)
  {
  case VIEW_CHECK_OPTION_CASCADED:
    m_opts.m_check = cdk::View_check::CASCADED;
  break;
  case VIEW_CHECK_OPTION_LOCAL:
    m_opts.m_check = cdk::View_check::LOCAL;
  break;
  default:
    throw Mysqlx_exception("Wrong value for VIEW check option");
  }
  m_opts.m_check_set = true;
}

void View_spec::set_definer(const char* val)
{
  if (val)
  {
    m_opts.m_definer_set = true;
    m_opts.m_definer = val;
  }
  else
  { // If definer = NULL, we will not report it
    m_opts.m_definer_set = false;
  }
}

void View_spec::set_columns(va_list args)
{
  m_columns.clear();
  const char *col_name = va_arg(args, char*);
  while (col_name)
  {
    m_columns.add_column(col_name);
    col_name = va_arg(args, char*);
  }
}


void View_spec::process(Processor &prc) const
{
  prc.name(m_ref, m_view_op_type);
  m_opts.process_if(prc.options());

  cdk::String_list::Processor *sprc = prc.columns();
  if (sprc)
    m_columns.process(*sprc);
}

void View_spec::View_opts::process(Processor &prc) const
{
  if (m_algorithm_set)
    prc.algorithm(m_algorithm);

  if (m_security_set)
    prc.security(m_security);

  if (m_check_set)
    prc.check(m_check);

  if (m_definer_set)
    prc.definer(m_definer);
}
