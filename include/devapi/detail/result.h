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

#ifndef MYSQLX_DETAIL_RESULT_H
#define MYSQLX_DETAIL_RESULT_H

/**
  @file
  Classes used to access query and command execution results.
*/


#include "../common.h"
#include "../error.h"
#include "../document.h"
#include "../row.h"

#include <memory>


namespace cdk {

  class Reply;

}  // cdk


namespace mysqlx {

class RowResult;

namespace internal {

  struct Session_detail;
  class Result_base;

/*
  List_initializer object can be used to initialize a container of
  arbitrary type U with list of items taken from source object.

  It is assumed that the source object type Source defines iterator
  type and that std::begin/end() return iterators to the beginning
  and end of the sequence. The container type U is assumed to have
  a constructor from begin/end iterator.

  List_iterator defines begin/end() methods, so it is possible to
  iterate over the sequence without storing it in any container.

  TODO: common.h contains List_init<> template - check if code can be
  merged.
*/

template <class Source>
class List_initializer
{
protected:

  Source &m_src;

  List_initializer(Source &src)
    : m_src(src)
  {}

  friend Source;

public:

  typedef typename Source::iterator iterator;

  /*
    Narrow the set of types for which this template is instantiated
    to avoid ambiguous conversion errors. It is important to disallow
    conversion to std::initializer_list<> because this conversion path
    is considered when assigning to STL containers.
  */

  template <
    typename U
    , typename
      = typename std::is_constructible<
          U, const iterator&, const iterator&
        >::type
    , typename
      = typename std::enable_if<
          !std::is_same<
            U,
            std::initializer_list<typename U::value_type>
          >::value
        >::type
  >
  operator U()
  {
    try {
      return U(std::begin(m_src), std::end(m_src));
    }
    CATCH_AND_WRAP
  }

  iterator begin()
  {
    try {
      return std::begin(m_src);
    }
    CATCH_AND_WRAP
  }

  iterator end() const
  {
    try {
      return std::end(m_src);
    }
    CATCH_AND_WRAP
  }

};


/*
  Iterator template.

  It defines an STL input iterator which is implemented using an
  implementation object of some type Impl. It is assumed that Impl
  has the following methods:

   void iterator_start() - puts iterator in "before begin" position;
   bool iterator_next() - moves iterator to next position, returns
                          false if it was not possible;
   Value_type iterator_get() - gets current value.
*/

template<typename Value_type, typename Impl>
struct iterator
  : std::iterator < std::input_iterator_tag, Value_type>
{
protected:

  Impl *m_impl = NULL;
  bool m_at_end = false;

  iterator(Impl& impl)
    : m_impl(&impl)
  {
    m_impl->iterator_start();
    m_at_end = !m_impl->iterator_next();
  }

  iterator()
    : m_at_end(true)
  {}

public:

  bool operator !=(const iterator &other) const
  {
    /*
      Compares only if both iterators are at the end
      of the sequence.
    */
    return !(m_at_end && other.m_at_end);
  }

  iterator<Value_type, Impl>& operator++()
  {
    try {
      if (m_impl && !m_at_end)
        m_at_end = !m_impl->iterator_next();
      return *this;
    }
    CATCH_AND_WRAP
  }

  Value_type operator*() const
  {
    if (!m_impl || m_at_end)
      THROW("Attempt to dereference null iterator");

    try {
      return m_impl->iterator_get();
    }
    CATCH_AND_WRAP
  }

  friend Impl;
};

} // internal


class Session;


namespace internal {

class Result_base;
class Row_result_detail;
class Doc_result_detail;


class PUBLIC_API Result_detail : nocopy
{

public:

  Result_detail() = default;
  virtual ~Result_detail();

protected:

  struct INTERNAL Impl;
  using WarningList = internal::List_initializer<Result_detail>;
  using iterator = internal::iterator<Warning, Result_detail>;

  Impl  *m_impl = NULL;
  bool m_owns_impl = false;


  Result_detail(Session*, cdk::Reply*);

  Result_detail(Session*, cdk::Reply*, const std::vector<GUID>&);


  void init(Result_detail&&);

  Impl& get_impl();

  const Impl& get_impl() const
  {
    return const_cast<Result_detail*>(this)->get_impl();
  }

  void check_result() const;


  // warning iterator implementation

  unsigned m_wpos;
  bool   m_at_begin;

  void iterator_start();
  bool iterator_next();
  Warning iterator_get();

  unsigned    get_warning_count() const;
  Warning     get_warning(unsigned pos);
  WarningList get_warnings();

public:

  iterator begin()
  {
    return iterator(*this);
  }

  iterator end()
  {
    return iterator();
  }

  friend iterator;
  friend Result_base;
  friend Row_result_detail;
  friend Doc_result_detail;
  friend Session_detail;
  friend List_initializer<Result_detail>;

  struct Access;
  friend Access;
};


class PUBLIC_API Column_detail
{
protected:

  class INTERNAL Impl;
  DLL_WARNINGS_PUSH
  std::shared_ptr<Impl> m_impl;
  DLL_WARNINGS_POP

  Column_detail(const std::shared_ptr<Impl> &impl)
    : m_impl(impl)
  {}

public:

  friend Impl;
  friend Row_result_detail;
  friend RowResult;

  struct INTERNAL Access;
  friend Access;
};


class PUBLIC_API Row_result_detail
 : virtual Result_detail
{
  using Impl = Result_detail::Impl;

protected:

  // Column meta-data access

  struct PUBLIC_API Columns_src
  {
    const Row_result_detail::Impl &m_res_impl;

    Columns_src(const Row_result_detail::Impl &res)
      : m_res_impl(res)
    {}

    struct iterator
      : public internal::iterator<Column_detail, Columns_src>
    {
      // Some valid C++11 constructs do not work in MSVC 2013.

#if defined(_MSC_VER) && _MSC_VER > 1800

      using internal::iterator<Column_detail, Columns_src>::iterator;

#else

      iterator() {}
      iterator(Columns_src &src)
        : internal::iterator<Column_detail, Columns_src>::iterator(src)
      {}

#endif

      // Note: define reference type so that Column instance can be constructe
      // from it.
      using reference = Column_detail&&;

      friend Columns_src;
    };

    iterator begin()
    {
      return iterator(*this);
    }

    iterator end()
    {
      return iterator();
    }

    // iterator implementation

    col_count_t m_pos;
    bool m_at_begin;

    void iterator_start();
    bool iterator_next();
    Column_detail iterator_get();
  };

  struct PUBLIC_API Columns
    : public internal::List_initializer<Columns_src>
  {
    Columns_src m_src;

    Columns(const Row_result_detail &res)
      : List_initializer(m_src)
      , m_src(res.get_impl())
    {}

    /*
      Note: Without this empty destructor code crashes on
      Solaris but works fine on all other platforms. The
      crash is like if the m_src object gets destroyed too
      early.
    */

    ~Columns()
    {}

    Column_detail operator[](col_count_t pos) const;
  };

  Row get_row();

private:

  // Row iterator implementation

  Row m_cur_row;

  void iterator_start() {}

  bool iterator_next()
  {
    m_cur_row = get_row();
    return !m_cur_row.isNull();
  }

  Row iterator_get()
  {
    return m_cur_row;
  }

public:

  using iterator = internal::iterator<Row, Row_result_detail>;

  iterator begin()
  {
    try {
      return iterator(*this);
    }
    CATCH_AND_WRAP
  }

  iterator end() const
  {
    try {
      return iterator();
    }
    CATCH_AND_WRAP
  }

  friend iterator;
  friend RowResult;
};


// Document based results
// ----------------------

class PUBLIC_API Doc_result_detail
  : virtual Result_detail
{
protected:

  void init(Doc_result_detail&&)
  {}

  DbDoc get_doc();
  uint64_t count();

public:

  using iterator = internal::iterator<DbDoc, Doc_result_detail>;

  iterator begin()
  {
    try {
      return iterator(*this);
    }
    CATCH_AND_WRAP
  }

  iterator end() const
  {
    try {
      return iterator();
    }
    CATCH_AND_WRAP
  }

private:

  // iterator implementation

  DbDoc m_cur_doc;

  void iterator_start() {}

  bool iterator_next()
  {
    m_cur_doc = get_doc();
    return !m_cur_doc.isNull();
  }

  DbDoc iterator_get()
  {
    return m_cur_doc;
  }

  friend Impl;
  friend iterator;
};

}  // internal namespace
}  // mysqlx

#endif
