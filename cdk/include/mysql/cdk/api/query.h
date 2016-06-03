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
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#ifndef CDK_API_QUERY_H
#define CDK_API_QUERY_H

#include "expression.h"
#include "../foundation/types.h"

namespace cdk {
namespace api {

using foundation::string;

/*
  Classes for representing different parts of a query such as
  LIMIT, ORDER BY etc.
*/

struct Sort_direction
{
  enum value { ASC = 1, DESC = 2 };
};


template <typename row_count_type>
class Limit
{
public:

  typedef row_count_type row_count_t;

  virtual row_count_t get_row_count() const =0;
  virtual const row_count_t* get_offset() const
  { return NULL; }
};


/*
  Order_by specification is a list of items of type Order_expr. Each item
  is an expression over Order_expr_processor which describes a sorting
  key.
*/

template <class EXPR>
class Order_expr_processor
{
public:

  typedef typename EXPR::Processor  Expr_prc;

  /*
    Report expression used as the sort key. This callback should return
    a processor for processing the expression. The dir parameter defines
    the sorting order.
  */

  virtual Expr_prc* sort_key(Sort_direction::value dir) =0;

};

template <class EXPR>
class Order_expr : public Expr_base< Order_expr_processor<EXPR> >
{};

template <class EXPR>
class Order_by
  : public Expr_list< Order_expr<EXPR> >
{
};


/*
  Projection specification is a list of items. Each item is an expression over
  Projection_processor which reports an expression and optional alias.
*/

template <class EXPR>
class Projection_processor
{
public:

  typedef typename EXPR::Processor Expr_prc;

  virtual Expr_prc* expr() = 0;
  virtual void      alias(const string&) = 0;
};

template <class EXPR>
class Projection_expr : public Expr_base< Projection_processor<EXPR> >
{};

template <class EXPR>
class Projection
  : public Expr_list< Projection_expr<EXPR> >
{};



/*
  Document path specification is a list of items, each to be processed
  with Doc_path_processor to describe one element of the path.
*/

class Doc_path_processor
{
public:

  typedef cdk::api::string  string;
  typedef uint32_t          index_t;

  // Path element is name of document field.

  virtual void member(const string &name) =0;

  // Path element "*".

  virtual void any_member() =0;

  // Path element is at given position within an array.

  virtual void index(index_t) =0;

  // Path element "[*]".

  virtual void any_index() =0;

  // Path element "**".

  virtual void any_path() =0;
};

typedef Expr_list< Expr_base<Doc_path_processor> > Doc_path;


/*
  Columns specification specifies table columns into which
  table insert operation should insert values. It is a list
  of items, each to be processed with Column_processor to
  describe:

  - name of the table column into which to insert,
  - optional document path if this column holds documents
    - the value will be inserted into specified element
    within the document,
  - optional alias for the column (TODO: how is it used).

  TODO: If alias is not used, consider removing alias()
  from Column_processor.
*/

class Column_processor
{
public:

  typedef cdk::api::string    string;
  typedef Doc_path::Processor Path_prc;

  virtual void name(const string&) =0;
  virtual void alias(const string&) =0;
  virtual Path_prc* path() =0;
};

typedef Expr_list< Expr_base<Column_processor> > Columns;

}}  // cdk::api


namespace cdk {

template<>
struct Safe_prc<api::Doc_path_processor>
  : Safe_prc_base<api::Doc_path_processor>
{
  typedef Safe_prc_base<api::Doc_path_processor> Base;
  using Base::Processor;
  typedef Processor::string  string;
  typedef Processor::index_t index_t;

  Safe_prc(Processor *prc) : Base(prc)
  {}

  Safe_prc(Processor &prc) : Base(&prc)
  {}

  using Base::m_prc;

  void member(const string &name)
  { return m_prc ? m_prc->member(name) : (void)NULL; }

  void any_member()
  { return m_prc ? m_prc->any_member() : (void)NULL; }

  void index(index_t ind)
  { return m_prc ? m_prc->index(ind) : (void)NULL; }

  void any_index()
  { return m_prc ? m_prc->any_index() : (void)NULL; }

  void any_path()
  { return m_prc ? m_prc->any_path() : (void)NULL; }
};



template<>
struct Safe_prc<api::Column_processor>
  : Safe_prc_base<api::Column_processor>
{
  typedef Safe_prc_base<api::Column_processor> Base;
  using Base::Processor;
  typedef Processor::string   string;
  typedef Processor::Path_prc Path_prc;

  Safe_prc(Processor *prc) : Base(prc)
  {}

  Safe_prc(Processor &prc) : Base(&prc)
  {}

  using Base::m_prc;

  void name(const string &n)
  { return m_prc ? m_prc->name(n) : (void)NULL; }

  void alias(const string &a)
  { return m_prc ? m_prc->alias(a) : (void)NULL; }

  Safe_prc<Path_prc> path()
  { return m_prc ? m_prc->path() : NULL; }

};

}  // cdk

#endif
