/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
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

  if (doc.getType() == Value::STRING)
  {
    doc = DbDoc(doc.get<string>());
  }

  Value_expr expr(doc, parser::Parser_mode::DOCUMENT);

  if (replace)
  {
    string doc_id;
    bool has_id = true;

    try {
      doc_id = doc.get<DbDoc>()["_id"];
    } catch (std::out_of_range)
    {
      has_id = false;
    }

    if (has_id && (doc_id != id))
    {
      throw Error(R"(Document "_id" and replace id are different!)");
    }

    Replace_cmd cmd(m_sess, coll, std::string(id), expr);
    return cmd.execute();
  }
  else
  {
    Upsert_cmd cmd(m_sess, coll, std::string(id), expr);
    return cmd.execute();
  }
}


void internal::Collection_detail::index_drop(const string &name)
{
  Object_ref coll(get_schema().m_name, m_name);
  common::Op_idx_drop cmd(m_sess, coll, name);
  cmd.execute();
}

void
internal::Collection_detail::index_create(const string &name, Value &&spec)
{
  switch (spec.getType())
  {
  case Value::STRING:
    break;
  default:
    // TODO: support other forms: DbDoc, expr("{...}")?
    throw_error("Index specification must be a string.");
  }

  Object_ref coll(get_schema().m_name, m_name);
  common::Op_idx_create cmd(m_sess, coll, name, spec);
  cmd.execute();

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
