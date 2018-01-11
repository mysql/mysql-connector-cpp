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

#include <mysqlx/xdevapi.h>
#include <uuid_gen.h>

#include <time.h>
#include <forward_list>
#include <list>

#include "impl.h"

using namespace mysqlx;
using namespace uuid;

/*
  Code in this file defines implementations for various CRUD operations used
  by X DevAPI. We use common implementations of these operations.

*/


auto internal::Crud_factory::mk_sql(mysqlx::Session &sess, const string &query)
-> Impl*
{
  return new common::Op_sql(sess.m_impl, query);
}



// --------------------------------------------------------------------

/*
  Collection CRUD operations
  ==========================
*/


auto internal::Crud_factory::mk_add(Collection &coll) -> Impl*
{
  return new common::Op_collection_add(
    coll.get_session(), Object_ref(coll)
  );
}


auto internal::Crud_factory::mk_remove(
  Collection &coll, const mysqlx::string &expr
) -> Impl*
{
  return new common::Op_collection_remove(
    coll.get_session(), Object_ref(coll), expr
  );
}


auto internal::Crud_factory::mk_find(Collection &coll) -> Impl*
{
  return new common::Op_collection_find(
    coll.get_session(), Object_ref(coll)
  );
}


auto internal::Crud_factory::mk_find(
  Collection &coll, const mysqlx::string &expr
) -> Impl*
{
  return new common::Op_collection_find(
    coll.get_session(), Object_ref(coll), expr
  );
}


auto internal::Crud_factory::mk_modify(
  Collection &coll, const mysqlx::string &expr
) -> Impl*
{
  return new common::Op_collection_modify(
    coll.get_session(), Object_ref(coll), expr
  );
}


struct Replace_cmd
  : public Executable<Result, Replace_cmd>
{
  Replace_cmd(
    common::Shared_session_impl sess,
    const cdk::api::Object_ref &coll,
    const std::string &id,
    const cdk::Expression &doc
  )
  {
    reset(new common::Op_collection_replace(
      sess, coll, id, doc
    ));
  }
};


struct Upsert_cmd : public Executable<Result, Upsert_cmd>
{
  Upsert_cmd(
    common::Shared_session_impl sess,
    const cdk::api::Object_ref &coll,
    const std::string &id,
    cdk::Expression &doc
  )
  {
    reset(new common::Op_collection_upsert(
      sess, coll, id, doc
    ));
  }
};


Result
internal::Collection_detail::add_or_replace_one(
  const string &id, Value &&doc, bool replace
)
{
  Object_ref coll(get_schema().m_name, m_name);
  Value_expr expr(doc, parser::Parser_mode::DOCUMENT);

  if (replace)
  {
    Replace_cmd cmd(m_sess, coll, std::string(id), expr);
    return cmd.execute();
  }
  else
  {
    Upsert_cmd cmd(m_sess, coll, std::string(id), expr);
    return cmd.execute();
  }
}



// --------------------------------------------------------------------

/*
  Table CRUD operations
  =====================
*/


auto internal::Crud_factory::mk_insert(Table &table) -> Impl*
{
  return new common::Op_table_insert<Value>(
    table.get_session(), Object_ref(table)
  );
}


auto internal::Crud_factory::mk_select(Table &table) -> Impl*
{
  return new common::Op_table_select(
    table.get_session(), Object_ref(table)
  );
}


auto internal::Crud_factory::mk_update(Table &table) -> Impl*
{
  return new common::Op_table_update(
    table.get_session(), Object_ref(table)
  );
}


auto internal::Crud_factory::mk_remove(Table &table) -> Impl*
{
  return new common::Op_table_remove(
    table.get_session(), Object_ref(table)
  );
}
