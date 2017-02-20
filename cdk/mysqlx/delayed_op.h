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
#include <mysql/cdk/protocol/mysqlx.h>
#include <memory>
#include "converters.h"

#include <list>


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


class Crud_op_base
  : public Proto_delayed_op
  , public protocol::mysqlx::api::Db_obj
{
protected:

  string m_name;
  string m_schema;
  bool  m_has_schema;

  Crud_op_base(Protocol &proto)
    : Proto_delayed_op(proto)
    , m_has_schema(false)
  {}

  Crud_op_base(Protocol &proto, const api::Object_ref &obj)
    : Proto_delayed_op(proto)
  {
    set(obj);
  }

  void set(const api::Object_ref &obj)
  {
    m_name = obj.name();
    m_has_schema = (NULL != obj.schema());
    if (m_has_schema)
      m_schema = obj.schema()->name();
  }

    // Db_obj

  const string& get_name() const { return m_name; }
  const string* get_schema() const { return m_has_schema ? &m_schema : NULL; }

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
    : public Crud_op_base
    , public protocol::mysqlx::Row_source
{
protected:

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

  SndInsertDocs(Protocol& protocol, const api::Table_ref &coll,
                cdk::Doc_source &docs,
                const Param_source *param)
    : Crud_op_base(protocol, coll)
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

};


// -------------------------------------------------------------------------


class SndInsertRows
  : public Crud_op_base
  , public protocol::mysqlx::Row_source
{
protected:

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
                const api::Table_ref &coll,
                cdk::Row_source &rows,
                const api::Columns *cols,
                const Param_source *param)
    : Crud_op_base(protocol, coll)
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

/*
  Helper base class which implements protocol's Select_spec
  (or Find_spec) interface. This is used by CRUD operations
  which involve selecting a subset of rows/documents in the
  table/colleciton.

  A CRUD operation class which derives from this Select_op_base
  can be used as selection criteria specification as required
  by protocol object methods.

  Note: This class uses converters to convert selection
  parameters from generic cdk types to types required by
  the protocol layer.
*/

template <class IF = protocol::mysqlx::Select_spec>
class Select_op_base
  : public Crud_op_base
  , public IF
{
protected:

  Expr_converter     m_expr_conv;
  Param_converter    m_param_conv;
  Order_by_converter m_ord_conv;
  const Limit       *m_limit;


  Select_op_base(
    Protocol &protocol,
    const api::Object_ref &obj,
    const cdk::Expression *expr,
    const cdk::Order_by *order_by,
    const cdk::Limit *lim = NULL,
    const cdk::Param_source *param = NULL
  )
    : Crud_op_base(protocol, obj)
    , m_expr_conv(expr), m_param_conv(param), m_ord_conv(order_by)
    , m_limit(lim)
  {}


  virtual ~Select_op_base()
  {}


  // Select_spec

  const protocol::mysqlx::api::Db_obj& obj() const { return *this; }

  const protocol::mysqlx::api::Expression* select() const
  {
    return m_expr_conv.get();
  }

  const protocol::mysqlx::api::Order_by* order() const
  {
    return m_ord_conv.get();
  }

  const protocol::mysqlx::api::Limit* limit() const
  {
    return m_limit;
  }

};


// -------------------------------------------------------------------------


template <protocol::mysqlx::Data_model DM>
class SndDelete
    : public Select_op_base<>
{
protected:

  Proto_op* start()
  {
    return &m_protocol.snd_Delete(DM, *this, m_param_conv.get());
  }

public:

  SndDelete(Protocol& protocol, const api::Object_ref &obj,
            const cdk::Expression *expr,
            const cdk::Order_by *order_by,
            const cdk::Limit *lim = NULL,
            const cdk::Param_source *param = NULL)
    : Select_op_base(protocol, obj, expr, order_by, lim, param)
  {}

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


template <protocol::mysqlx::Data_model DM> class SndViewCrud;


template <protocol::mysqlx::Data_model DM>
class SndFind
    : public Select_op_base<protocol::mysqlx::Find_spec>
{
protected:

  typedef typename Find_traits<DM>::Projection Projection;
  typedef typename Find_traits<DM>::Projection_converter Projection_converter;

  Projection_converter m_proj_conv;
  Expr_list_converter  m_group_by_conv;
  Expr_converter       m_having_conv;

  Proto_op* start()
  {
    return &m_protocol.snd_Find(DM, *this, m_param_conv.get());
  }

public:

  SndFind(
    Protocol& protocol, const api::Table_ref &coll,
    const cdk::Expression *expr = NULL,
    const Projection      *proj = NULL,
    const cdk::Order_by   *order_by = NULL,
    const cdk::Expr_list  *group_by = NULL,
    const cdk::Expression *having = NULL,
    const cdk::Limit *lim = NULL,
    const cdk::Param_source *param = NULL
  )
    : Select_op_base(protocol, coll, expr, order_by, lim, param)
    , m_proj_conv(proj)
    , m_group_by_conv(group_by), m_having_conv(having)
  {}

private:

  const protocol::mysqlx::api::Projection* project() const
  {
    return m_proj_conv.get();
  }

  const protocol::mysqlx::api::Expr_list*  group_by() const
  {
    return m_group_by_conv.get();
  }

  const protocol::mysqlx::api::Expression* having() const
  {
    return m_having_conv.get();
  }

  friend class SndViewCrud<DM>;
};


// -------------------------------------------------------------------------

/*
  Conversion from string processor used to process a list of view column names
  to callbacks expected by protocol's column info processor.
  Basically, each string in a list is reported as column name. Other column
  specification parameters, such as alias, are not reported.
*/

struct String_to_col_prc_converter
  : public Converter<
    String_to_col_prc_converter,
    cdk::api::String_processor,
    cdk::protocol::mysqlx::api::Columns::Processor::Element_prc
  >
{
  void val(const string &col)
  {
    m_proc->name(col);
  }

  virtual ~String_to_col_prc_converter()
  {}
};

typedef List_prc_converter<String_to_col_prc_converter> Columns_prc_converter;


/*
  Delayed operation which sends view create or update request. These request
  can include a find message. Whether update or create request should be sent
  is determined by the view specification passed when creating this delayed
  operation.
*/

template <protocol::mysqlx::Data_model DM>
class SndViewCrud
  : public Crud_op_base
  , public View_spec::Processor
  , public cdk::protocol::mysqlx::api::Columns
  , public protocol::mysqlx::api::View_options
{
  const View_spec *m_view;
  SndFind<DM> *m_find;
  View_spec::op_type  m_type;
  bool   m_has_cols;
  bool   m_has_opts;

  // Columns

  void process(cdk::protocol::mysqlx::api::Columns::Processor &prc) const
  {
    assert(m_view);

    /*
      Column names are reported to the protocol layer as column specification
      (as used by snd_Insert() for example). We use processor converter to convert
      string list processor callbacks to these of Columns specification
      processor.
    */

    Columns_prc_converter conv;
    conv.reset(prc);

    /*
      Process view specification extracting columns information and passing
      it to the converter.
    */

    struct : public cdk::View_spec::Processor
    {
      String_list::Processor *m_prc;

      void name(const Table_ref&, op_type) {}

      Options::Processor* options()
      {
        return NULL;
      }

      List_processor* columns()
      {
        return m_prc;
      }

    }
    vprc;

    vprc.m_prc = &conv;
    m_view->process(vprc);
  }

  protocol::mysqlx::api::Columns*
  get_cols()
  {
    return m_has_cols ? this : NULL;
  }

  // View_options

  void process(protocol::mysqlx::api::View_options::Processor &prc) const
  {
    assert(m_view);

    /*
      Process view specification extracting options information and passing
      it to the processor.
    */

    struct Opts : public cdk::View_spec::Processor
    {
      Options::Processor *m_prc;

      void name(const Table_ref&, op_type)
      {}

      Options::Processor* options()
      {
        return m_prc;
      }

      List_processor* columns()
      {
        return NULL;
      }
    }
    vprc;

    vprc.m_prc = &prc;
    m_view->process(vprc);
  }

  protocol::mysqlx::api::View_options*
  get_opts()
  {
    return m_has_opts ? this : NULL;
  }

  const protocol::mysqlx::api::Args_map*
  get_args()
  {
    return m_find->m_param_conv.get();
  }


  Proto_op* start()
  {
    switch (m_type)
    {
    case CREATE:
    case REPLACE:
      return &m_protocol.snd_CreateView(DM, *this, *m_find,
                                        get_cols(), REPLACE == m_type,
                                        get_opts(), get_args());

    case UPDATE:
      return &m_protocol.snd_ModifyView(DM, *this, *m_find,
                                        get_cols(), get_opts(),
                                        m_find->m_param_conv.get());
    default:
      assert(false);
      return NULL;  // quiet compile warnings
    }
  }

public:

  SndViewCrud(const View_spec &view, SndFind<DM> *find = NULL)
    : Crud_op_base(find->m_protocol)
    , m_view(&view), m_find(find), m_type(CREATE)
    , m_has_cols(false), m_has_opts(false)
  {
    /*
      Process view specification to extract view name and information which
      type of view operation should be sent (m_update member). This also
      determines whether columns and options information is present in the
      specification.
    */
    view.process(*this);
  }

  ~SndViewCrud()
  {
    delete m_find;
  }

private:

  // View_spec::Processor

  void name(const Table_ref &view, View_spec::op_type type)
  {
    Crud_op_base::set(view);
    m_type = type;
  }

  List_processor* columns()
  {
    m_has_cols = true;
    /*
      Note: we do not process columns here, it is done above when this
      object acts as protocol Columns specification.
    */
    return NULL;
  }

  Options::Processor* options()
  {
    m_has_opts = true;
    return NULL;
  }

};


class SndDropView
  : public Crud_op_base
{
  bool m_check_exists;

  Proto_op* start()
  {
    return &m_protocol.snd_DropView(*this, m_check_exists);
  }

public:

  SndDropView(
    Protocol &protocol,
    const api::Object_ref &view,
    bool check_exists
  )
    : Crud_op_base(protocol, view)
    , m_check_exists(check_exists)
  {}

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
    report_path(path);

    m_proc->update_op(protocol::mysqlx::update_op::ITEM_REMOVE);
  }


  Expr_prc_converter  m_conv;


  Expr_prc* set(const Doc_path *path, unsigned flags)
  {
    Prc_to::Expr_prc  *prc;

    report_path(path);

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
    report_path(path);

    Prc_to::Expr_prc *prc
      = m_proc->update_op(protocol::mysqlx::update_op::ARRAY_INSERT);

    if (!prc)
      return NULL;

    m_conv.reset(*prc);
    return &m_conv;
  }

  Expr_prc* array_append(const Doc_path *path)
  {
    report_path(path);

    Prc_to::Expr_prc *prc
      = m_proc->update_op(protocol::mysqlx::update_op::ARRAY_APPEND);

    if (!prc)
      return NULL;

    m_conv.reset(*prc);
    return &m_conv;
  }

  void report_path(const Doc_path *path)
  {
    if (path)
    {
      Doc_path_storage dp;
      path->process(dp);
      if (!dp.is_empty())
        m_proc->target_path(dp);
    }
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
    : public Select_op_base<>
{
protected:

  Update_converter    m_upd_conv;

  Proto_op* start()
  {
    return &m_protocol.snd_Update(DM, *this, m_upd_conv, m_param_conv.get());
  }

public:

  SndUpdate(Protocol& protocol,
            const api::Table_ref &table,
            const cdk::Expression *expr,
            const cdk::Update_spec &us,
            const cdk::Order_by *order_by,
            const cdk::Limit *lim = NULL,
            const cdk::Param_source *param = NULL)
    : Select_op_base(protocol, table, expr, order_by, lim, param)
    , m_upd_conv(DM, us)
  {}

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
