/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * This code is licensed under the terms of the GPLv2
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

#ifndef CDK_MYSQLX_DELAYED_OP_H
#define CDK_MYSQLX_DELAYED_OP_H

#include <mysql/cdk/common.h>
#include <memory>
#include "converters.h"


namespace cdk {
namespace mysqlx {


/*
  Delayed operations
  ==================

  Delayed operations are created by session an put into a queue for later
  execution. When it is time to execute delayed operation, its start() method
  is called which should start corresponding protocol operation.
*/


class Proto_delayed_op
    : public Proto_op
{

protected:

  Protocol& m_protocol;
  Proto_op* op;

  Proto_delayed_op(Protocol& protocol)
    : m_protocol(protocol)
    , op(NULL)
  {}

public:

  virtual ~Proto_delayed_op()
  {
  }

  bool is_completed() const
  {
    return NULL != op && op->is_completed();
  }

protected:

  virtual Proto_op* start() = 0;

  virtual bool do_cont()
  {
    if (NULL == op)
      op = start();
    return op->cont();
  }

  virtual void do_wait()
  {
    if (op == NULL)
      op = start();

    if (op)
      op->wait();
    else
      THROW("Invalid delayed operation.");
  }

  virtual void do_cancel()
  {
    if (op)
      op->cancel();
  }

  virtual const api::Event_info* get_event_info() const
  {
    if (op)
      return op->waits_for();
    return NULL;
  }

  size_t do_get_result(void)
  { return op ? op->get_result() : 0; }
};


// -------------------------------------------------------------------------


class SndStmt
    : public Proto_delayed_op
{
protected:

  const char *m_ns;
  const string m_stmt;
  Any_list *m_args;

  Proto_op* start()
  {
    Any_list_converter conv;
    if (m_args)
      conv.reset(*m_args);
    return &m_protocol.snd_StmtExecute(m_ns, m_stmt, m_args ? &conv : NULL);
  }

public:

  SndStmt(Protocol& protocol, const char *ns,
          const string& stmt, Any_list *args)
    : Proto_delayed_op(protocol), m_ns(ns)
    , m_stmt(stmt), m_args(args)
  {}
};


// -------------------------------------------------------------------------


class SndInsertDocs
    : public Proto_delayed_op
    , public protocol::mysqlx::Row_source
    , public protocol::mysqlx::api::Db_obj
{
protected:

  const string m_schema;
  const string m_table;
  cdk::Doc_source &m_docs;
  const Param_source *m_param;

  Proto_op* start()
  {
    Param_converter param_conv;

    if (m_param)
        param_conv.reset(*m_param);

    return &m_protocol.snd_Insert(cdk::protocol::mysqlx::DOCUMENT,
                                  *this,
                                  NULL,
                                  *this,
                                  &param_conv);
  }

public:

  SndInsertDocs(Protocol& protocol,
                const string &schema, const string &table,
                cdk::Doc_source &docs,
                const Param_source *param)
    : Proto_delayed_op(protocol)
    , m_schema(schema), m_table(table)
    , m_docs(docs)
    , m_param(param)
  {}

private:

  // Row_source

  void process(Processor &prc) const
  {
    prc.list_begin();
    Processor::Element_prc *ep = prc.list_el();
    if (ep)
    {
      Expr_converter conv(m_docs);
      conv.process(*ep);
    }
    prc.list_end();
  }

  bool next()
  {
    return m_docs.next();
  }

  // Db_obj

  const string& get_name() const { return m_table; }
  const string* get_schema() const { return &m_schema; }
};


// -------------------------------------------------------------------------


class SndInsertRows
  : public Proto_delayed_op
  , public protocol::mysqlx::Row_source
  , public protocol::mysqlx::api::Db_obj
{
protected:

  const string m_schema;
  const string m_table;
  Expr_converter  m_conv;
  cdk::Row_source &m_rows;
  const api::Columns *m_cols;
  const Param_source *m_param;

  Proto_op* start()
  {
    Param_converter param_conv;

    if (m_param)
      param_conv.reset(*m_param);

    return &m_protocol.snd_Insert(cdk::protocol::mysqlx::TABLE,
                                  *this,
                                  m_cols,
                                  *this,
                                  &param_conv);
  }

public:

  // TODO: Life-time of rows instance...

  SndInsertRows(Protocol& protocol,
                const string &schema, const string &table,
                cdk::Row_source &rows,
                const api::Columns *cols,
                const Param_source *param)
    : Proto_delayed_op(protocol)
    , m_schema(schema), m_table(table)
    , m_rows(rows), m_cols(cols), m_param(param)
  {}

private:

  // Row_source

  void process(Processor &prc) const
  {
    Expr_list_converter conv;
    conv.reset(m_rows);
    conv.process(prc);
  }

  bool next()
  {
    return m_rows.next();
  }

  // Db_obj

  const string& get_name() const { return m_table; }
  const string* get_schema() const { return &m_schema; }
};


// -------------------------------------------------------------------------

/*
  Conversion of order by specs which translates CDK sort key expressions
  to protocol expressions.
*/


class Order_prc_converter
  : public Converter<
  Order_prc_converter,
  Order_by::Expression::Processor,
  protocol::mysqlx::api::Order_by::Expression::Processor
  >
{
  Expr_prc_converter  m_conv;

  using Prc_from::Expr_prc;

public:

  virtual ~Order_prc_converter() {}

  Expr_prc* sort_key(Sort_direction::value dir)
  {
    Prc_to::Expr_prc *ep = m_proc->sort_key(dir);
    if (!ep)
      return NULL;
    m_conv.reset(*ep);
    return &m_conv;
  }

};


typedef Expr_conv_base<
          List_prc_converter< Order_prc_converter >,
          Order_by,
          protocol::mysqlx::api::Order_by
        >
        Order_by_converter;


// -------------------------------------------------------------------------


class SndDelete
    : public Proto_delayed_op
    , public Expression
    , public protocol::mysqlx::api::Db_obj
{
protected:

  const string m_schema;
  const string m_table;
  const Expression *m_expr;
  const Limit *m_limit;
  const cdk::mysqlx::Order_by *m_order_by;
  const Param_source *m_param;

  const cdk::protocol::mysqlx::Data_model m_data_model;

  Proto_op* start()
  {
    Expr_converter conv;
    Param_converter param_conv;
    Order_by_converter ord_conv;

    if (m_order_by)
      ord_conv.reset(*m_order_by);

    if (m_expr)
      conv.reset(*m_expr);

    if (m_param)
      param_conv.reset(*m_param);

    return &m_protocol.snd_Delete(m_data_model,
                                  *this,
                                  (m_expr ? &conv : NULL),
                                  (m_order_by ? &ord_conv : NULL),
                                  (cdk::protocol::mysqlx::api::Limit*)m_limit,
                                  (m_param ? &param_conv : NULL));
  }

public:

  SndDelete(Protocol& protocol, cdk::protocol::mysqlx::Data_model data_model,
            const string &schema, const string &table,
            const Expression *expr,
            const Order_by *order_by,
            const Limit *lim = NULL,
            const Param_source *param = NULL)
    : Proto_delayed_op(protocol)
    , m_schema(schema), m_table(table)
    , m_expr(expr), m_limit(lim)
    , m_order_by(order_by), m_param(param)
    , m_data_model(data_model)
  {}

private:

  // Expression

  void process(Expression::Processor &ep) const
  {
    m_expr->process(ep);
  }

  // Db_obj

  const string& get_name() const { return m_table; }
  const string* get_schema() const { return &m_schema; }
};


// -------------------------------------------------------------------------

/*
  Projection converters.

  Projections are specified differently for collections, where they are simply
  document expressions, and for tables, where they are expressed using Projection
  expressions.

  In either case we want to translate each projection specification to protocol
  Projection expression. The converters are built using appropriate processor
  converters.
*/


/*
  Processor coverter for Expression::Document -> protocol::mysqlx::Projection
  conversion. Top-level keys become aliases and their values are projection
  expressions.
*/

class Doc_proj_prc_converter
  : public Converter<
      Doc_proj_prc_converter,
      cdk::Expression::Document::Processor,
      protocol::mysqlx::api::Projection::Processor>
{
  Expr_prc_converter m_conv;

  void doc_begin() { m_proc->list_begin(); }
  void doc_end()   { m_proc->list_end(); }

  Any_prc *key_val(const string &key)
  {
    Prc_to::Element_prc *ep = m_proc->list_el();
    if (!ep)
      return NULL;
    ep->alias(key);
    Prc_to::Element_prc::Expr_prc *expp = ep->expr();
    if (!expp)
      return NULL;
    m_conv.reset(*expp);
    return &m_conv;
  }
};


/*
  Processor converter for cdk::Projection -> protocol::mysqlx::Projection
  conversion. The two interfaces are identical - only expressions must be
  converted from cdk to protocol ones.
*/

class Table_proj_prc_converter
  : public Converter<
      Table_proj_prc_converter,
      Projection::Processor::Element_prc,
      protocol::mysqlx::api::Projection::Processor::Element_prc>
{
  Expr_prc_converter m_conv;

  Expr_prc* expr()
  {
    Prc_to::Expr_prc *prc = m_proc->expr();
    if (!prc)
      return NULL;
    m_conv.reset(*prc);
    return &m_conv;
  }

  void alias(const string &name)
  {
    m_proc->alias(name);
  }

public:

  virtual ~Table_proj_prc_converter() {}
};


/*
  The SndFind delayed operation below has two variants, for documents and
  for tables. Each variant uses different Projection type and different
  projection coverter. Find_traits<> template defines these types for each
  variant.
*/

template <protocol::mysqlx::Data_model DM>
struct Find_traits;

template<>
struct Find_traits<protocol::mysqlx::DOCUMENT>
{
  typedef cdk::Expression::Document Projection;

  typedef Expr_conv_base<
      Doc_proj_prc_converter,
      Expression::Document, protocol::mysqlx::api::Projection >
    Projection_converter;
};

template<>
struct Find_traits<protocol::mysqlx::TABLE>
{
  typedef cdk::Projection Projection;

  typedef Expr_conv_base<
      List_prc_converter<Table_proj_prc_converter>,
      Projection, protocol::mysqlx::api::Projection >
    Projection_converter;
};


template <protocol::mysqlx::Data_model DM>
class SndFind
    : public Proto_delayed_op
    , public protocol::mysqlx::api::Db_obj
{
protected:

  typedef typename Find_traits<DM>::Projection Projection;
  typedef typename Find_traits<DM>::Projection_converter Projection_converter;

  const string m_schema;
  const string m_table;
  const Expression *m_expr;
  const Limit *m_limit;
  const cdk::mysqlx::Order_by *m_order_by;
  const Expr_list  *m_group_by;
  const Expression *m_having;
  const Param_source *m_param;
  const Projection   *m_proj;

  Proto_op* start()
  {
    Expr_converter  expr_conv;
    Param_converter param_conv;
    Order_by_converter ord_conv;
    Projection_converter proj_conv;
    Expr_converter  having_conv;
    Expr_list_converter group_by_conv;

    if (m_expr)
      expr_conv.reset(*m_expr);

    if (m_param)
      param_conv.reset(*m_param);

    if (m_order_by)
      ord_conv.reset(*m_order_by);

    if (m_group_by)
      group_by_conv.reset(*m_group_by);

    if (m_having)
      having_conv.reset(*m_having);

    if (m_proj)
      proj_conv.reset(*m_proj);

    return &m_protocol.snd_Find(DM, *this,
                                (m_expr ? &expr_conv : NULL),
                                (m_proj ? &proj_conv : NULL),
                                (m_order_by ? &ord_conv : NULL),
                                (m_group_by ? &group_by_conv : NULL),
                                (m_having ? &having_conv : NULL),
                                (cdk::protocol::mysqlx::api::Limit*)m_limit,
                                (m_param ? &param_conv : NULL));
  }

public:

  SndFind(Protocol& protocol,
          const string &schema, const string &table,
          const Expression *expr = NULL,
          const Projection *proj = NULL,
          const cdk::mysqlx::Order_by *order_by = NULL,
          const Expr_list *group_by = NULL,
          const Expression *having = NULL,
          const Limit *lim = NULL,
          const Param_source *param = NULL)
    : Proto_delayed_op(protocol)
    , m_schema(schema), m_table(table)
    , m_expr(expr), m_limit(lim), m_order_by(order_by)
    , m_group_by(group_by), m_having(having)
    , m_param(param), m_proj(proj)
  {}

private:

  // Db_obj

  const string& get_name() const { return m_table; }
  const string* get_schema() const { return &m_schema; }
};


// -------------------------------------------------------------------------

/*
   Update_converter
*/

class Update_prc_converter
    : public Converter<
        Update_prc_converter,
        cdk::Update_processor,
        cdk::protocol::mysqlx::Update_processor
      >
{

public:


  cdk::protocol::mysqlx::Data_model m_dm;

//  protocol::mysqlx::update_op::value m_type;

  struct Table : public protocol::mysqlx::api::Db_obj
  {
    string m_table_name;
    bool m_has_schema;
    string m_schema_name;

    virtual ~Table() {}

    //DB_OBJ
    const string& get_name() const
    {
      return m_table_name;
    }

    const string* get_schema() const
    {
      return m_has_schema ? &m_schema_name : NULL;
    }

  } m_table;

public:

  virtual ~Update_prc_converter() {}

  void set_data_model(cdk::protocol::mysqlx::Data_model dm)
  {
    m_dm = dm;
  }


  //Update_processor

  virtual void column(const api::Column_ref &column)
  {
    if (cdk::protocol::mysqlx::DOCUMENT == m_dm)
      cdk::throw_error("Using column() in document mode");

    m_proc->target_name(column.name());

    if (column.table())
    {
      m_table.m_table_name =  column.table()->name();
      if (column.table()->schema())
      {
        m_table.m_has_schema = true;
        m_table.m_schema_name = column.table()->schema()->name();
      }
      else
      {
        m_table.m_has_schema = false;
      }

      m_proc->target_table(m_table);
    }

  }

  virtual void remove(const Doc_path *path)
  {
    if (path)
      m_proc->target_path(*path);

    m_proc->update_op(protocol::mysqlx::update_op::ITEM_REMOVE);
  }


  Expr_prc_converter  m_conv;


  Expr_prc* set(const Doc_path *path, unsigned flags)
  {
    Prc_to::Expr_prc  *prc;

    if (path)
      m_proc->target_path(*path);


    if (flags & Update_processor::NO_INSERT)
      prc = m_proc->update_op(protocol::mysqlx::update_op::ITEM_REPLACE);
    else if(flags & Update_processor::NO_OVERWRITE)
      prc = m_proc->update_op(protocol::mysqlx::update_op::ITEM_MERGE);
    else
      if (cdk::protocol::mysqlx::DOCUMENT == m_dm)
        prc = m_proc->update_op(protocol::mysqlx::update_op::ITEM_SET);
      else
        prc = m_proc->update_op(path ? protocol::mysqlx::update_op::ITEM_SET
                                     : protocol::mysqlx::update_op::SET);


    if (!prc)
      return NULL;

    m_conv.reset(*prc);
    return &m_conv;
  }


  Expr_prc* array_insert(const Doc_path *path)
  {
    if (path)
      m_proc->target_path(*path);

    Prc_to::Expr_prc *prc
      = m_proc->update_op(protocol::mysqlx::update_op::ARRAY_INSERT);

    if (!prc)
      return NULL;

    m_conv.reset(*prc);
    return &m_conv;
  }

  Expr_prc* array_append(const Doc_path *path)
  {
    if (path)
      m_proc->target_path(*path);

    Prc_to::Expr_prc *prc
      = m_proc->update_op(protocol::mysqlx::update_op::ARRAY_APPEND);

    if (!prc)
      return NULL;

    m_conv.reset(*prc);
    return &m_conv;
  }

};


class Update_converter
  : public Expr_conv_base<
      Update_prc_converter,
      cdk::Update_spec,
      protocol::mysqlx::Update_spec
    >
{
public:

  Update_converter(cdk::protocol::mysqlx::Data_model dm,
                   const cdk::Update_spec &us)
  {
    m_conv.set_data_model(dm);
    reset(us);
  }

  bool next()
  {
    assert(m_expr);
    return const_cast<cdk::Update_spec*>(m_expr)->next();
  }
};


template <protocol::mysqlx::Data_model DM>
class SndUpdate
    : public Proto_delayed_op
    , public protocol::mysqlx::api::Db_obj
{
protected:

  const string m_schema;
  const string m_table;
  const Expression *m_expr;
  const Update_spec &m_us;
  const cdk::mysqlx::Order_by *m_order_by;
  const Limit *m_limit;
  const Param_source *m_param;

  Proto_op* start()
  {
    Expr_converter conv;
    Param_converter param_conv;
    Order_by_converter ord_conv;

    if (m_expr)
      conv.reset(*m_expr);

    if (m_param)
      param_conv.reset(*m_param);

    if (m_order_by)
      ord_conv.reset(*m_order_by);

    Update_converter u_conv(DM, m_us);

    return &m_protocol.snd_Update(DM,
                                  *this,
                                  (m_expr ? &conv : NULL),
                                  u_conv,
                                  (m_order_by ? &ord_conv : NULL),
                                  const_cast<Limit*>(m_limit),
                                  (m_param ? &param_conv : NULL));
  }

public:

  SndUpdate(Protocol& protocol,
            const string &schema, const string &table,
            const Expression *expr,
            const Update_spec &us,
            const Order_by *order_by,
            const Limit *lim = NULL,
            const Param_source *param = NULL)
    : Proto_delayed_op(protocol)
    , m_schema(schema), m_table(table)
    , m_expr(expr), m_us(us), m_order_by(order_by), m_limit(lim), m_param(param)
  {}

private:


  // Db_obj

  const string& get_name() const { return m_table; }
  const string* get_schema() const { return &m_schema; }
};


// -------------------------------------------------------------------------


class RcvMetaData
    : public Proto_delayed_op
{
  typedef protocol::mysqlx::Mdata_processor Mdata_processor;

protected:

  Mdata_processor& m_prc;

  Proto_op* start()
  {
    return &m_protocol.rcv_MetaData(m_prc);
  }

public:
  RcvMetaData(Protocol& protocol,
              Mdata_processor& prc)
    : Proto_delayed_op(protocol)
    , m_prc(prc)
  {}

};


class RcvStmtReply
    : public Proto_delayed_op
{
  typedef protocol::mysqlx::Stmt_processor Stmt_processor;

protected:

  Stmt_processor& m_prc;

  Proto_op* start()
  {
    return &m_protocol.rcv_StmtReply(m_prc);
  }

public:
  RcvStmtReply(Protocol& protocol,
               Stmt_processor& prc)
    : Proto_delayed_op(protocol)
    , m_prc(prc)
  {}

};


// -------------------------------------------------------------------------


#if 0
class RcvRows
    : public Proto_delayed_op
{
protected:

  Row_processor& m_prc;

  Op* start()
  {
    return &m_protocol.rcv_Rows(m_prc);
  }

public:
  RcvRows(Protocol& protocol,
          Row_processor& prc)
    : Proto_delayed_op(protocol)
    , m_prc(prc)
  {}

};
#endif


class SndAuthStart
    : public Proto_delayed_op
{

protected:

  const char * m_mechanism;
  bytes m_data;
  bytes m_response;

  Proto_op* start()
  {
    return &m_protocol.snd_AuthenticateStart(m_mechanism,
                                             m_data,
                                             m_response);
  }

public:
  SndAuthStart(Protocol& protocol,
               const char * mechanism,
               bytes data,
               bytes response)
    : Proto_delayed_op(protocol)
    , m_mechanism(mechanism)
    , m_data(data)
    , m_response(response)
  {}

};

class SndAuthContinue
    : public Proto_delayed_op
{

protected:

  bytes m_data;

  Proto_op* start()
  {
    return &m_protocol.snd_AuthenticateContinue(m_data);
  }

public:
  SndAuthContinue(Protocol& protocol,
                  bytes data)
    : Proto_delayed_op(protocol)
    , m_data(data)
  {}

};

class RcvAuthReply
    : public Proto_delayed_op
{
  typedef protocol::mysqlx::Auth_processor Auth_processor;

protected:

  Auth_processor& m_prc;

  Proto_op* start()
  {
    return &m_protocol.rcv_AuthenticateReply(m_prc);
  }

public:
  RcvAuthReply(Protocol& protocol,
               Auth_processor& prc)
    : Proto_delayed_op(protocol)
    , m_prc(prc)
  {}

};


}} // cdk::mysqlx

#endif
