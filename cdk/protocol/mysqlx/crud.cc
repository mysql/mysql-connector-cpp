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

/*
  Implementation of mysqlx protocol API: CRUD Implementation
  ==============================================================
*/


#include "protocol.h"
#include "builders.h"
#include <boost/format.hpp>

PUSH_PB_WARNINGS
#include "protobuf/mysqlx_sql.pb.h"
POP_PB_WARNINGS


using namespace cdk::foundation;
using namespace google::protobuf;
using namespace cdk::protocol::mysqlx;


/*
  Implementation of Protocol methods using the internal implementation.
*/

namespace cdk {
namespace protocol {
namespace mysqlx {


/*
  Helper function template to set a db object for a given message of MSG class
*/

template <class MSG> void set_db_obj(api::Db_obj &db_obj, MSG &msg)
{
  Mysqlx::Crud::Collection *proto_collect = msg.mutable_collection();

  proto_collect->set_name(db_obj.get_name());

  const string *collect_schema = db_obj.get_schema();
  if (collect_schema)
    proto_collect->set_schema(*collect_schema);
}


/*
  Helper function template to set a data model for a given message of MSG class
*/

template <class MSG> void set_data_model(Data_model dm, MSG &msg)
{
  if (dm != DEFAULT)
    msg.set_data_model(static_cast<Mysqlx::Crud::DataModel>(dm));
}


/*
  Helper function template to set `limit` field inside message of type MSG.
*/

template <class MSG> void set_limit(api::Limit &lim, MSG &msg)
{

  Mysqlx::Crud::Limit *proto_lim = msg.mutable_limit();

  proto_lim->set_row_count(lim.get_row_count());
  const row_count_t *lim_offset = lim.get_offset();

  if (lim_offset)
    proto_lim->set_offset(*lim_offset);
}


/*
  Helper function template to set `criteria` field within a message of type
  MSG.
*/

template <class MSG> void set_criteria(api::Expression &api_expr, MSG &msg,
                                       Args_conv &conv /*= NULL*/)
{
  Mysqlx::Expr::Expr *pb_expr = msg.mutable_criteria();

  Expr_builder eb(*pb_expr, &conv);
  api_expr.process(eb);
}


// -------------------------------------------------------------------------

/*
  Store single projection information in a Crud::Projection message.

  This builder for a single projection is then used to construct full projection
  builder for Crud::Find message using Array_builder<> template.
*/

struct Projection_builder
  : public Builder_base<Mysqlx::Crud::Projection,
                        api::Projection::Processor::Element_prc>
{
  Expr_builder m_expr_builder;

  void reset(Message &msg, Args_conv *conv = NULL)
  {
    Builder_base::reset(msg, conv);
    m_expr_builder.reset(*msg.mutable_source(), conv);
  }

  Expr_prc* expr()
  {
    return &m_expr_builder;
  }

  void alias(const string &a)
  {
    m_msg->set_alias(a);
  }
};


/*
  Alternative to Arr_msg_traits<Crud::Find> which is used with Array_builder<>
  tempalte to create a builder which stores single projection specifications
  in repeated `projection` field in the Find message.
*/

struct Proj_msg_traits
{
  typedef Mysqlx::Crud::Find       Array;
  typedef Mysqlx::Crud::Projection Msg;

  static Msg& add_element(Array &arr)
  {
    return *arr.add_projection();
  }
};


// -------------------------------------------------------------------------


/*
  Storing Order_by information inside protocol commands. Messages that
  store Order_by information have a repeated `order` field of type
  Mysqlx::Crud::Order. The Order_builder class defines a builder that can
  fill a single Mysqlx::Crud::Order sub-message given sort key information
  form order expression. From this builder an array builder is created
  which can process a list of order expressions and store order information
  in repeated `order` fields of a command message.
*/

struct Order_builder
  : public Builder_base<Mysqlx::Crud::Order, api::Order_expr::Processor>
{
  Expr_builder m_expr_builder;

  void reset(Message &msg, Args_conv *conv = NULL)
  {
    Builder_base::reset(msg, conv);
    m_expr_builder.reset(*msg.mutable_expr(), conv);
  }

  Expr_prc* sort_key(api::Sort_direction::value dir)
  {
    m_msg->set_direction(dir == api::Sort_direction::ASC ?
                         Message::ASC : Message::DESC);
    return &m_expr_builder;
  }
};


/*
  Alternative to Arr_msg_traits<MSG> which is used with Array_builder<MSG>
  to crate a builder which stores array elements in a repeated `order` field
  within the message.
*/

template<class MSG>
struct Ord_msg_traits
{
  typedef MSG                 Array;
  typedef Mysqlx::Crud::Order Msg;

  static Msg& add_element(Array &arr)
  {
    return *arr.add_order();
  }
};


/*
  Helper function template to store order by information within message of
  type MSG. It uses Ord_msg_traits<> which assume that order by information
  is stored in a repeated `order` field within the message.
*/

template <class MSG> void set_order_by(api::Order_by &order_by,
                                       MSG &msg,
                                       Args_conv &conv)
{
  Array_builder<Order_builder, MSG, Ord_msg_traits<MSG> > ord_builder;
  ord_builder.reset(msg, &conv);
  order_by.process(ord_builder);
}


// -------------------------------------------------------------------------



class Any_to_Scalar_builder
  : public cdk::api::Any_processor<api::Scalar_processor>
{
  Scalar_builder m_builder;

public:

  void reset(Mysqlx::Datatypes::Scalar &msg)
  {
    m_builder.reset(msg);
  }

  virtual Scalar_prc* scalar()
  {
    return &m_builder;
  }

  // Report that any value is an array, that is, a list of any expressions.

  virtual List_prc*   arr()
  {
    throw Generic_error("Array not supported on parameters.");
  }

  // Report that any value is a document.

  virtual Doc_prc*    doc()
  {
    throw Generic_error("Document not supported on parameters.");
  }

};


class Placeholder_conv_imp
    : public Args_conv
{
  map<string, unsigned> m_map;

public:

  virtual ~Placeholder_conv_imp() {}

  unsigned conv_placeholder(const string &name)
  {
    map<string, unsigned>::const_iterator it = m_map.find(name);
    if (it == m_map.end())
      throw Generic_error((boost::format("Placeholder %s was not defined on args.")
                           % name).str());

    return it->second;
  }

  void add_placeholder(const string &name)
  {
    map<string, unsigned>::const_iterator it = m_map.find(name);
    if (it != m_map.end())
      throw Generic_error((boost::format("Redifined placeholder %s.")
                           % name).str());
    assert(m_map.size() < std::numeric_limits<unsigned>::max());
    unsigned pos = static_cast<unsigned>(m_map.size());
    m_map[name] = pos;
  }

};

template <class MSG>
class Param_builder
    : public api::Args_map::Processor
{

  MSG &m_msg;
  Placeholder_conv_imp &m_conv;
  Any_to_Scalar_builder m_builder;

public:
  Param_builder(MSG &msg, Placeholder_conv_imp &conv)
    : m_msg(msg)
    , m_conv(conv)
  {}

  virtual Any_prc* key_val(const string &key)
  {
    m_builder.reset(*m_msg.add_args());

    m_conv.add_placeholder(key);

    return &m_builder;
  }

};



/*
  Helper function template to add parameter (arguments) values to
  a given message of MSG class. At the same time it builds and
  stores name->position map in the map argument.
*/

template <class MSG> void set_args(const api::Args_map &args, MSG &msg,
                                   Placeholder_conv_imp &map)
{
  Param_builder<MSG> param_builder(msg, map);
  args.process(param_builder);
}

// -------------------------------------------------------------------------

/*
  Helper function to fill ColumnIdentifier sub-message from information
  given by api::Doc_path object.
 */

void set_doc_path(Mysqlx::Expr::ColumnIdentifier *p_col_id,
                  const api::Doc_path &doc)
{
  for (unsigned pos = 0; pos < doc.length(); ++pos)
  {
    Mysqlx::Expr::DocumentPathItem *dpi = p_col_id->add_document_path();

    switch (doc.get_type(pos))
    {
    case api::Doc_path::MEMBER:
      dpi->set_type(Mysqlx::Expr::DocumentPathItem::MEMBER);
      if (doc.get_name(pos))
        dpi->set_value(*doc.get_name(pos));
      break;

    case api::Doc_path::MEMBER_ASTERISK:
      dpi->set_type(Mysqlx::Expr::DocumentPathItem::MEMBER_ASTERISK);
      break;

    case api::Doc_path::ARRAY_INDEX:
      dpi->set_type(Mysqlx::Expr::DocumentPathItem::ARRAY_INDEX);
      if (doc.get_index(pos))
        dpi->set_index(*doc.get_index(pos));
      break;

    case api::Doc_path::ARRAY_INDEX_ASTERISK:
      dpi->set_type(Mysqlx::Expr::DocumentPathItem::ARRAY_INDEX_ASTERISK);
      break;

    case api::Doc_path::DOUBLE_ASTERISK:
      dpi->set_type(Mysqlx::Expr::DocumentPathItem::DOUBLE_ASTERISK);
      break;

    default: break;
    }
  }
}


// -------------------------------------------------------------------------


/*
  Storing Group_by information inside Find protocol command. 
  This command has a repeated `grouping` field of type
  Mysqlx::Expr::Expr. Below we fill it using a builder created
  from Array_builer<> template. Such builder process list of
  expressions and stores each expression inside Find message
  in a submessage appended with Group_by_traits::add_element()
  method (the Group_by_traits structure is passed to the Array_builder<>
  template when the builder is created).
*/

struct Group_by_traits
{
  typedef Mysqlx::Crud::Find    Array;
  typedef Mysqlx::Expr::Expr    Msg;

  static Msg& add_element(Array &arr)
  {
    return *arr.add_grouping();
  }
};


Protocol::Op&
Protocol::snd_Find(
    Data_model dm,
    api::Db_obj &db_obj,
    api::Expression *api_expr,
    api::Projection *proj,
    api::Order_by   *order,
    api::Expr_list  *group_by,
    api::Expression *having,
    api::Limit *lim,
    api::Args_map *args)
{
  Mysqlx::Crud::Find find;

  Placeholder_conv_imp conv;

  set_db_obj(db_obj, find);
  set_data_model(dm, find);

  if (args)
    set_args(*args, find, conv);

  if (api_expr)
    set_criteria(*api_expr, find, conv);

  if (lim)
    set_limit(*lim, find);

  if (order)
    set_order_by(*order, find, conv);

  if (proj)
  {
    Array_builder<Projection_builder, Mysqlx::Crud::Find, Proj_msg_traits> 
                 proj_builder;
    proj_builder.reset(find, &conv);
    proj->process(proj_builder);
  }

  if (group_by)
  {
    Array_builder<Expr_builder, Mysqlx::Crud::Find, Group_by_traits>
      group_by_builder;
    group_by_builder.reset(find, &conv);
    group_by->process(group_by_builder);
  }

  if (having)
  {
    Expr_builder expr_builder;
    expr_builder.reset(*find.mutable_grouping_criteria());
    having->process(expr_builder);
  }

  return get_impl().snd_start(find, msg_type::cli_CrudFind);
}


// -------------------------------------------------------------------------


template<>
struct Arr_msg_traits<Mysqlx::Crud::Insert_TypedRow>
{
  typedef Mysqlx::Crud::Insert_TypedRow Array;
  typedef Mysqlx::Expr::Expr            Msg;

  static Msg& add_element(Array &arr)
  {
    return *arr.add_field();
  }
};


/*
  Filling projection information inside Insert message.

  Class Proj_builder fills single Crud::Column sub-message. It is used to
  create full projection builder with Array_builder<> template. This full
  builder processes an api::Columns list and for each element in the list,
  adds new Crud::Column sub-message to the Insert message using
  Proj_traits::add_element(), then fills it with data from the list element
  using Proj_builder.
*/

struct Proj_builder
  : Builder_base<Mysqlx::Crud::Column, Columns::Processor::Element_prc>
{
  using Builder_base<Mysqlx::Crud::Column, Columns::Processor::Element_prc>::m_msg;

  void name(const string &n) { m_msg->set_name(n); }
  void alias(const string &a)  { m_msg->set_alias(a); }

  Path_prc* path()
  {
    // TODO
    THROW("Paths in column projections not implemented");
  }
};

struct Proj_traits
{
  typedef Mysqlx::Crud::Insert  Array;
  typedef Mysqlx::Crud::Column  Msg;

  static Msg& add_element(Array &arr)
  {
    return *arr.add_projection();
  }
};



Protocol::Op&
Protocol::snd_Insert(
    Data_model dm,
    api::Db_obj &db_obj,
    const api::Columns *columns,
    Row_source &rs,
    api::Args_map *args)
{
  Mysqlx::Crud::Insert insert;

  Placeholder_conv_imp conv;

  set_db_obj(db_obj, insert);
  set_data_model(dm, insert);

  if (args)
    set_args(*args, insert, conv);

  if (columns)
  {
    Array_builder<Proj_builder, Mysqlx::Crud::Insert, Proj_traits> proj_builder;
    proj_builder.reset(insert);
    columns->process(proj_builder);
  }

  while (rs.next())
  {
    Mysqlx::Crud::Insert_TypedRow *msg = insert.add_row();

    Array_builder<Expr_builder, Mysqlx::Crud::Insert_TypedRow> row_builder;

    row_builder.reset(*msg, &conv);
    rs.process(row_builder);
  }

  return get_impl().snd_start(insert, msg_type::cli_CrudInsert);
}


// -------------------------------------------------------------------------


class Update_builder
    : public Update_processor
{
private:

  Mysqlx::Crud::UpdateOperation  &m_upd_op;
  Mysqlx::Expr::ColumnIdentifier &m_source;
  Args_conv                      &m_conv;

  scoped_ptr<Expr_builder> m_expr_builder;

public:

  Update_builder(::Mysqlx::Crud::UpdateOperation &upd_op, Args_conv &conv)
    : m_upd_op(upd_op)
    , m_source (*m_upd_op.mutable_source())
    , m_conv(conv)
  {}

  ~Update_builder()
  {
  }

  virtual void target_name(const string &name)
  {
    m_source.set_name(name);
  }

  virtual void target_table(const api::Db_obj &table)
  {
    m_source.set_table_name(table.get_name());
    const string* schema = table.get_schema();
    if (schema)
      m_source.set_schema_name(*schema);
  }

  virtual void target_path(const api::Doc_path &path)
  {
    set_doc_path(&m_source, path);
  }

  Expr_prc* update_op(update_op::value type)
  {
    m_upd_op.set_operation((Mysqlx::Crud::UpdateOperation::UpdateType) type);

    switch(type)
    {
    case update_op::ITEM_REMOVE:
      return NULL; //Doesn't have value;

    case update_op::SET:
    case update_op::ITEM_SET:
    case update_op::ITEM_MERGE:
    case update_op::ITEM_REPLACE:
    case update_op::ARRAY_INSERT:
    case update_op::ARRAY_APPEND:
    default:
      {
        m_expr_builder.reset(new Expr_builder(*m_upd_op.mutable_value(), &m_conv));
        return m_expr_builder.get();
      }
    };
  }

};



Protocol::Op& Protocol::snd_Update(
    Data_model dm,
    api::Db_obj &db_obj,
    api::Expression *api_expr,
    Update_spec &us,
    api::Order_by *order,
    api::Limit *lim,
    api::Args_map *args)
{
  Mysqlx::Crud::Update update;
  Placeholder_conv_imp conv;

  set_db_obj(db_obj, update);
  set_data_model(dm, update);

  if (args)
    set_args(*args, update, conv);

  if (api_expr)
    set_criteria(*api_expr, update, conv);

  if (order)
    set_order_by(*order, update, conv);

  if (lim)
    set_limit(*lim, update);

  while (us.next())
  {
    Update_builder prc(*update.add_operation(), conv);
    us.process(prc);
  }

  return get_impl().snd_start(update, msg_type::cli_CrudUpdate);
}


// -------------------------------------------------------------------------


Protocol::Op&
Protocol::snd_Delete(
    Data_model dm,
    api::Db_obj &db_obj,
    api::Expression *api_expr,
    api::Order_by *order_by,
    api::Limit *lim,
    api::Args_map *args)
{
  Mysqlx::Crud::Delete del;
  Placeholder_conv_imp conv;

  set_db_obj(db_obj, del);
  set_data_model(dm, del);

  if (args)
    set_args(*args, del, conv);

  if (api_expr)
    set_criteria(*api_expr, del, conv);

  if (lim)
    set_limit(*lim, del);

  if (order_by)
    set_order_by(*order_by, del, conv);

  return get_impl().snd_start(del, msg_type::cli_CrudDelete);
}


}}}  // cdk::protocol::mysqlx
