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


struct Value_expr_check_id
  : cdk::Expression
    , cdk::Expression::Processor
    , cdk::Expression::Processor::Doc_prc
{
  Value_expr &m_expr;
  bool m_is_expr;

  Processor *m_prc;
  Doc_prc *m_doc_prc;

  struct Any_processor_check
      : cdk::Expression::Processor::Doc_prc::Any_prc
      , cdk::Expression::Processor::Doc_prc::Any_prc::Scalar_prc
      , cdk::Expression::Processor::Doc_prc::Any_prc::Scalar_prc::Value_prc
  {
    Any_prc *m_id_prc;
    Scalar_prc *m_scalar_prc;
    Value_prc *m_value_prc;
    const string& m_id;

    Any_processor_check(const string& id)
      : m_id(id)
    {}

    // Any processor implementation

    Scalar_prc* scalar() override
    {
      m_scalar_prc = m_id_prc->scalar();
      return m_scalar_prc ? this : nullptr;
    }

    List_prc* arr() override
    {
      return m_id_prc->arr();
    }

    Doc_prc* doc() override
    {
      return m_id_prc->doc();
    }

    //Scalar processor implementation

    Value_prc* val() override
    {
      m_value_prc = m_scalar_prc->val();
      return m_value_prc ? this : nullptr;
    }

    Args_prc* op(const char *name) override
    {
      return m_scalar_prc->op(name);
    }
    Args_prc* call(const Object_ref&obj) override
    {
      return m_scalar_prc->call(obj);
    }

    void ref(const Column_ref &col, const Doc_path *path) override
    {
      return m_scalar_prc->ref(col, path);
    }
    void ref(const Doc_path &path) override
    {
      return m_scalar_prc->ref(path);
    }

    void param(const string &val) override
    {
      return m_scalar_prc->param(val);
    }

    void param(uint16_t val) override
    {
      return m_scalar_prc->param(val);
    }

    void var(const string &name) override
    {
      m_scalar_prc->var(name);
    }

    // Value processor implementation
    void null() override { m_value_prc->null();}

    void value(cdk::Type_info type,
                       const cdk::Format_info &format,
                       cdk::foundation::bytes val) override
    {
      m_value_prc->value(type, format, val);
    }

    void str(const string &val) override
    {
      if (m_id != val)
        throw Error(R"(Document "_id" and replace id are different!)");
      m_value_prc->str(val);
    }
    void num(int64_t  val) override { m_value_prc->num(val); }
    void num(uint64_t val) override { m_value_prc->num(val); }
    void num(float    val) override { m_value_prc->num(val); }
    void num(double   val) override { m_value_prc->num(val); }
    void yesno(bool   val) override { m_value_prc->yesno(val); }

  };

  Any_processor_check m_any_prc;

  Value_expr_check_id(Value_expr &expr, bool is_expr, const string& id)
    : m_expr(expr)
    , m_is_expr(is_expr)
    , m_any_prc(id)
  {}

  // Expression implementation

  void process(Processor& prc) const override
  {
    auto self = const_cast<Value_expr_check_id*>(this);
    self->m_prc = &prc;
    m_expr.process(*self);
  }

  // Expression processor implementation

  Scalar_prc* scalar() override
  {
    return m_prc->scalar();
  }

  List_prc* arr() override
  {
    return m_prc->arr();
  }

  Doc_prc* doc() override
  {
    m_doc_prc = m_prc->doc();
    return m_doc_prc ? this : nullptr;
  }

  // Doc_prc implementation

  void doc_begin() override
  {
    m_doc_prc->doc_begin();
  }

  void doc_end() override
  {
    m_doc_prc->doc_end();
  }

  Any_prc* key_val(const string &key) override
  {
    if (string("_id") == key )
    {
      if (m_is_expr)
        throw Error(R"(Document "_id" will be replaced by expression "_id")");

      m_any_prc.m_id_prc = m_doc_prc->key_val(key);
      return m_any_prc.m_id_prc ? &m_any_prc : nullptr;
    }
    return m_doc_prc->key_val(key);
  }

};


Result
internal::Collection_detail::add_or_replace_one(
  const string &id, Value &&doc, bool replace
)
{
  Object_ref coll(get_schema().m_name, m_name);


  if (!Value::Access::is_expr(doc) &&
      doc.getType() == Value::STRING)
  {
    doc = DbDoc(doc.get<string>());
  }

  Value_expr expr(doc, parser::Parser_mode::DOCUMENT);

  if (replace)
  {
    Value_expr_check_id check_id(expr, Value::Access::is_expr(doc), id);

    Replace_cmd cmd(m_sess, coll, std::string(id), check_id);
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
