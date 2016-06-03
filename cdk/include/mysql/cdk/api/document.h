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

#ifndef CDK_API_DOCUMENT_H
#define CDK_API_DOCUMENT_H

#include "../foundation.h"
#include "expression.h"

namespace cdk {
namespace api {

/**
  Documents over processor PRC
  ============================

  Such documents are key-value maps where value of a key can be either an
  expression over PRC (a "scalar" value), list of such expressions, or
  another document over PRC.

  Object that describes a document over PRC should implement the
  Doc_base<PRC> interface. This is Expr_base<> interface over
  special Doc_processor<PRC> processor type that defines callbacks used
  to describe the key-value mapping (see below).

  Given document object d implementing Doc_base<PRC> and a corresponding
  processor dp, executing d.process(dp) will first call dp.doc_begin(),
  then dp.key_val() callbacks to present keys and their values and finally
  dp.doc_end().

  An example document class is cdk::JSON defined in common.h. It is a document
  over processor of type JSON_processor.

  TODO: Example.
*/

template <class PRC>
class Doc_processor;

template <class PRC>
class Doc_base;

template <class PRC>
class Any;


/*
  "Any" Expression is an expression which can be a base expression over
  some processor PRC, an array of such expressions or a document over PRC
  (that is, a key-value map where key values can be again such "any"
  expressions).

  Given expression processor type PRC, Any_processor<PRC> is a processor
  of any values over PRC. The processor callbacks for different kind of
  values return an appropriate processor to process the value or NULL if
  the value should be ignored.
*/

template <class PRC>
class Any_processor
{
public:

  typedef PRC                            Scalar_prc;
  typedef Doc_processor<PRC>             Doc_prc;
  typedef List_processor<Any_processor>  List_prc;


  /*
    Report that any value is a "scalar", that is, a base expression over
    PRC.
  */

  virtual Scalar_prc* scalar() =0;

  // Report that any value is an array, that is, a list of any expressions.

  virtual List_prc*   arr() =0;

  // Report that any value is a document.

  virtual Doc_prc*    doc() =0;

  virtual ~Any_processor() {}
};


template <class PRC>
class
    Any: public Expr_base< Any_processor<PRC> >
{
public:

  typedef Any_processor<PRC>    APRC;

  typedef Expr_base<PRC>   Scalar;
  typedef Doc_base<PRC>    Document;
  typedef Expr_list<Any>   List;

};


/**
  Documents over PRC are expression over Doc_processor<PRC>. If d is a
  document object and dp is a ducument processor then d.process(dp) will
  describe key-value pairs to the processor using Doc_processor<PRC>
  callbacks.
*/

template <class PRC>
class Doc_processor
{
public:

  typedef cdk::foundation::string  string;
  typedef Any_processor<PRC>       Any_prc;

  /// This is called before any key_val() or key_doc() callback.
  virtual void doc_begin() {}

  /// This is called after last key_val() or key_doc() callback.
  virtual void doc_end() {}

  /**
    Called to report key value which can be either simple expression,
    array or document. The callback should return a processor to process
    this key value or NULL to ignore it.
  */
  virtual Any_prc* key_val(const string &key) =0;

  virtual ~Doc_processor() {}
};


template <class PRC>
class Doc_base : public Expr_base< Doc_processor<PRC> >
{
public:

  typedef typename Doc_processor<PRC>::string    string;
  typedef typename Any<PRC>::Scalar    Scalar;
  typedef typename Any<PRC>::Document  Document;
  typedef typename Any<PRC>::List      List;
};


}}  // cdk::api


namespace cdk {


template <class PRC>
struct Safe_prc< cdk::api::Any_processor<PRC> >
  : Safe_prc_base< cdk::api::Any_processor<PRC> >
{
  typedef Safe_prc_base< cdk::api::Any_processor<PRC> >  Base;
  using typename Base::Processor;

  typedef typename Base::Processor::Scalar_prc  Scalar_prc;
  typedef typename Base::Processor::Doc_prc     Doc_prc;
  typedef typename Base::Processor::List_prc    List_prc;

  Safe_prc(Processor *prc) : Base(prc)
  {}

  Safe_prc(Processor &prc) : Base(&prc)
  {}

  using Base::m_prc;

  Safe_prc<Scalar_prc> scalar()
  { return m_prc ? m_prc->scalar() : NULL; }

  Safe_prc<List_prc> arr()
  { return m_prc ? m_prc->arr() : NULL; }

  Safe_prc<Doc_prc> doc()
  { return m_prc ? m_prc->doc() : NULL; }

};


template <class PRC>
struct Safe_prc< cdk::api::Doc_processor<PRC> >
  : Safe_prc_base< cdk::api::Doc_processor<PRC> >
{
  typedef Safe_prc_base< cdk::api::Doc_processor<PRC> >  Base;
  using typename Base::Processor;

  typedef typename Base::Processor::string  string;
  typedef typename Base::Processor::Any_prc Any_prc;

  Safe_prc(Processor *prc) : Base(prc)
  {}

  Safe_prc(Processor &prc) : Base(&prc)
  {}

  using Base::m_prc;

  void doc_begin()
  { return m_prc ? m_prc->doc_begin() : (void)NULL; }

  void doc_end()
  { return m_prc ? m_prc->doc_end() : (void)NULL; }

  Safe_prc<Any_prc> key_val(const string &key)
  { return m_prc ? m_prc->key_val(key) : NULL; }

};

}

#endif
