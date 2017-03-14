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

#ifndef MYSQLX_COLLECTION_CRUD_H
#define MYSQLX_COLLECTION_CRUD_H

/**
  @file
  Declarations for CRUD operations on document collections.

  Classes declared here represent CRUD operations on a given document
  collection. An Object of a class such as CollectionAdd represents
  a "yet-to-be-executed" operation and stores all the parameters for the
  operation. The operation is sent to server for execution only when
  `execute()` method is called.

  The following classes for collection CRUD operations are defined:
  - CollectionAdd
  - CollectionRemove
  - CollectionFind
  - CollectionModify

  CRUD operation objects can be created directly, or assigned from
  result of DevAPI methods that create such operations:
  ~~~~~~
     CollectionAdd  add_op(coll);
     CollectionFind find_op = coll.find(...).sort(...);
  ~~~~~~

  CRUD operation objects have methods which can modify the operation
  before it gets executed. For example `CollectionAdd::add()`
  appends a document to the list of documents that should be added
  by given CollectionAdd operation. These methods can be chained
  as allowed by the fluent API grammar. In case of collection CRUD
  operations the grammar can be described by the following diagram:

    CollectionAddBase     -> CollectionAdd         : add()
    CollectionRemoveBase  -> CollectionRemove      : remove()
    CollectionFindBase    -> CollectionFind        : find()
    CollectionModifyBase  -> CollectionModifyFirst : modify()

    CollectionAdd = Executable<Result>
    CollectionAdd -> CollectionAdd : add()

    CollectionRemove = CollectionSort<Result,false>

    CollectionFind = CollectionSort<DocResult,true>

    CollectionModify = CollectionSort<Result,false>
    CollectionModify -> CollectionModify : set()
    CollectionModify -> CollectionModify : unset()
    CollectionModify -> CollectionModify : arrayXXX()

    CollectionSort<R,F> = Limit<R,F>
    CollectionSort<R,F> -> Limit<R,F> : sort()

    Collection = CollectionAddBase
    Collection = CollectionRemoveBase
    Collection = CollectionFindBase
    Collection = CollectionModifyBase

  In this diagram notation X -> Y : foo() means that class X defines
  public method foo() with return type Y (roughly). Notation X = Y means
  that X inherits public methods from Y. See crud.h for grammar
  rules for Limit<> and other common classes.

  The top-level classes CollectionXXXBase correspond to four main kinds
  of CRUD operations. Collection class, which supports all four kinds,
  is derived from all the CollecitonXXXBase classes using multiple
  inheritance.

  CRUD operation objects do not have copy semantics. If CRUD object a is
  constructed from b, like in "a = b", then the operation moves from b to
  a and any attempt to execute or modify b will trigger error.
*/


#include "common.h"
#include "result.h"
#include "statement.h"
#include "crud.h"

#include <utility>
#include <sstream>

namespace cdk {

class Session;

}  // cdk


namespace mysqlx {

class XSession;
class Collection;


namespace internal {

  /*
    Virtual base class for CollectionXXXBase classes defined below.

    It defines members that can be shared between the different
    CollectionXXXBase classes which all are used as a base for
    the Collection class. Currently the only common member is
    `m_coll` which points to the collection on which the operation
    is performed.
  */

  class PUBLIC_API CollectionOpBase
  {
  protected:

    Collection *m_coll;

    CollectionOpBase(Collection &coll) : m_coll(&coll)
    {}

    CollectionOpBase(const CollectionOpBase&) = default;

    /*
      This constructor is here only to allow defining
      CollectionXXXBase classes without a need to explicitly
      invoke CollectionOpBase constructor. But in the end,
      only the constructor called from the Collection class
      should be used to initialize m_coll member, not this one.
      Thus we add assertion to verify that it is not used.
    */

    CollectionOpBase() : m_coll(NULL)
    {
      assert(false);
    }
  };

}  // internal

// ----------------------------------------------------------------------

/*
  Adding documents to a collection
  ================================

  Various forms of add() clause are defined by CollectionAddInterface
  template. It is inherited by CollectionAdd and CollectionAddBase.
  They differ by return type of the add() method. In case of
  CollectionAddBase the add() method returns new CollectionAdd object
  by value, while CollectionAdd::add() method returns reference to
  itself.

  Thus, once CRUD add operation is created by CollectionAddBase::add(),
  this operation is then modified in-place by subsequent calls to
  the add() method.
*/

class CollectionAdd;


namespace internal {

  /*
    Type trait which is used to distinguish types which can describe a single
    document from other types such as document collections. This is needed
    to correctly define CollectionAddInterface below.

    Currently we have DbDoc type and also support documents described by JSON
    strings.
  */

  template <typename D> struct is_doc_type
  {
    static const bool value
      = std::is_convertible<D, DbDoc>::value
      || std::is_convertible<D, string>::value;
  };


  /**
    Class which defines various variants of `add()` method.

    This class defines `add()` methods that append new documents to the
    list of documents that should be inserted by a `CollectionAdd` operation.
    Documents can be specified as JSON strings or DbDoc instances. Several
    documents can be added in a single call to `add()`.

    @cond IGNORE
    Template is parametrized by return type of the `add()` method which
    is different in case of `CollectionAddBase` and `CollcetionAdd`.
    This return type is either `CollectionAdd` or reference to
    `CollectionAdd`, respectively. In either case `CollectionAdd::do_add()`
    method is used to append documents to the list.
    @endcond
  */

  template<class AddOp>
  class CollectionAddInterface
  {
    /*
      This method must be overridden by derived class to return an
      empty add operation.
    */

    virtual AddOp get_op() = 0;

    static void add_docs(AddOp&) {}

    template<typename D, typename... Types>
    static void add_docs(AddOp &op, const D &first, Types... rest)
    {
      op.do_add(first);
      add_docs(op, rest...);
    }

    /*
      Method which extends given add operation with a range of
      documents given by two iterators.
    */

    template<typename It>
    void add_range(AddOp &op, const It &begin, const It &end)
    {
      for (It it = begin; it != end; ++it)
      {
        op.do_add(*it);
      }
    }

  public:

    /**
      Add all documents from a range defined by two iterators. These
      iterators should return a document object of one of accepted types
      (as given by the is_doc_type<> trait).

      Note: We use enable_if to remove ambiguity between this overload
      and the one which adds 2 documents: add(doc1,doc2). Thus this
      overload is enabled only if type It is not a document type.
    */

    template <
      typename It,
      typename = enable_if_t<!is_doc_type<It>::value>
    >
    AddOp add(const It &begin, const It &end)
    {
      try {
        AddOp add = get_op();
        add_range(add, begin, end);
        return add;
      }
      CATCH_AND_WRAP
    }

    /**
      Add all documents within given container.

      Any container type for which `std::begin()`/`std::end()` are defined
      should work.

      Note: we use `enable_if<>` to remove ambiguity between this overload
      and the one which adds a single document: `add(doc)`. Thus this
      overload is enabled only if type Container is not a document type.
    */

    template <
      class Container,
      typename = enable_if_t<!is_doc_type<Container>::value>
    >
    AddOp add(const Container &c)
    {
      try {
        AddOp add = get_op();
        add_range(add, std::begin(c), std::end(c));
        return add;
      }
      CATCH_AND_WRAP
  }

    /**
      Add document(s) to a collection.

      Documents can be described by JSON strings or DbDoc objects.
    */

    template<typename... Types>
    AddOp add(Types... docs)
    {
      try {
        AddOp op = get_op();
        add_docs(op, docs...);
        return op;
      }
      CATCH_AND_WRAP
    }

    friend CollectionAdd;
  };


  /*
    Interface to be implemented by internal implementations
    of CRUD add operation.
  */

  struct CollectionAdd_impl : public Executable_impl
  {
    /*
      Note: Current implementation only supports sending
      documents in form of JSON strings.
    */

    virtual void add_json(const string&) = 0;
  };

  /// TODO
  class CollectionAddBase;

}  // internal


/**
  Operation which adds documents to a collection.

  Documents to be added by this operation are specified with various variants
  of `add()` method.

  Each document must have a unique identifier which is stored in `_id`
  field of the document. Document identifiers are character strings no longer
  than 32 characters. If added document does not have `_id` field, a unique
  identifier will be generated for it. Document identifier generated by given
  collection add operation can be examined using `Result::getDocumentIds()`
  method. Generated document identifiers are string of 32 hexadecimal digits,
  like this one `0512020981044082E6119DFA0E4C0584`.

  @note Generated document identifiers are based on UUIDs but they are not
  valid UUIDs (fields are reversed).

  @cond IGNORE
  The various `add()` methods defined by `CollectionAddInterface`
  call `do_add()` to append documents to the list one by one. This
  method, in turn, passes these documents to the internal
  implementation object.
  @endcond

  @ingroup devapi_op
*/

DLL_WARNINGS_PUSH

class PUBLIC_API CollectionAdd
  : public virtual Executable<Result, CollectionAdd>
  , public internal::CollectionAddInterface<CollectionAdd&>
{

DLL_WARNINGS_POP

public:

  /**
    Create empty add operation for a given collection.
  */

  CollectionAdd(Collection &coll);

  CollectionAdd(CollectionAdd &other) : Executable(other) {}
  CollectionAdd(CollectionAdd &&other) : Executable(std::move(other)) {}

private:

  typedef internal::CollectionAdd_impl Impl;

  Impl* get_impl()
  {
    check_if_valid();
    return static_cast<Impl*>(m_impl.get());
  }

  virtual CollectionAdd& get_op()
  {
    return *this;
  }

  void do_add(const string &json)
  {
    get_impl()->add_json(json);
  }

  void do_add(const DbDoc &doc)
  {
    // TODO: Do it better when we support sending structured
    // document descriptions to the server.

    std::ostringstream buf;
    buf << doc;
    get_impl()->add_json(buf.str());
  }

  ///@cond IGNORED
  friend internal::CollectionAddBase;
  friend internal::CollectionAddInterface<CollectionAdd>;
  friend internal::CollectionAddInterface<CollectionAdd&>;
  ///@endcond
};


namespace internal {


  class PUBLIC_API CollectionAddBase
    : public CollectionAddInterface<mysqlx::CollectionAdd>
    , public virtual CollectionOpBase
  {
    mysqlx::CollectionAdd get_op()
    {
      return mysqlx::CollectionAdd(*m_coll);
    }
  };

}  // internal


// ----------------------------------------------------------------------

namespace internal {

  /**
    Base class defining various forms of CRUD .sort() clause.

    Note: the actual job of adding sort specification to the underlying
    implementation is done by calling `add_sort` method of `Sort_impl`
    interface.
  */

  template <class Res, class Op, bool limit_with_offset>
  class CollectionSort
    : public Limit<Res, Op, limit_with_offset>
  {
  protected:

    typedef internal::Sort_impl Impl;

    using Limit<Res, Op, limit_with_offset>::check_if_valid;
    using Limit<Res, Op, limit_with_offset>::m_impl;

    Impl* get_impl()
    {
      check_if_valid();
      return static_cast<Impl*>(m_impl.get());
    }

  public:

    Limit<Res, Op, limit_with_offset>& sort(const string& ord_spec)
    {
      get_impl()->add_sort(ord_spec);
      return *this;
    }

    Limit<Res, Op, limit_with_offset>& sort(const char* ord_spec)
    {
      get_impl()->add_sort(ord_spec);
      return *this;
    }

    /**
      Specify ordering of documents in the operation.

      This form accepts a vector, list or other container with strings, each
      string defining sorting direction and the value to sort on.
    */

    template <typename Ord>
    Limit<Res, Op, limit_with_offset>& sort(Ord ord)
    {
      for (auto el : ord)
      {
        get_impl()->add_sort(ord);
      }
      return *this;
    }

    /**
      Specify ordering of documents in the operation.

      Arguments are one or more strings, each defining sorting direction and the
      value to sort on.
    */

    template <typename Ord, typename...Type>
    Limit<Res, Op, limit_with_offset>& sort(Ord ord, const Type...rest)
    {
      get_impl()->add_sort(ord);
      return sort(rest...);
    }

  };

}  // internal

// ----------------------------------------------------------------------

namespace internal {

  /**
    Class defining .having() clause.
  */

  template <class Op>
  class CollectionHaving
      : public internal::CollectionSort<DocResult, Op, true>
  {

    typedef internal::Having_impl Impl;
    typedef internal::CollectionSort<DocResult, Op, true> CollectionSort;

  protected:

    using internal::CollectionSort<DocResult, Op, true>::check_if_valid;
    using internal::CollectionSort<DocResult, Op, true>::m_impl;

    Impl* get_impl()
    {
      check_if_valid();
      return static_cast<Impl*>(m_impl.get());
    }

  public:

    /**
      Specify having filter in the operation.

      Arguments are a string defining filter to be used.
    */

    CollectionSort& having(const string& having_spec)
    {
      get_impl()->set_having(having_spec);
      return *this;
    }
  };
}

// ----------------------------------------------------------------------

namespace internal {

  /**
    Class defining .groupBy() clause.
  */

  template <class Op>
  class CollectionGroupBy
      : public internal::CollectionHaving<Op>
  {

    typedef internal::Group_by_impl Impl;
    typedef internal::CollectionHaving<Op> CollectionHaving;

  protected:

    using internal::CollectionHaving<Op>::check_if_valid;
    using internal::CollectionHaving<Op>::m_impl;

    Impl* get_impl()
    {
      check_if_valid();
      return static_cast<Impl*>(m_impl.get());
    }


  public:

    /**
      Specify groupBy fields in the operation.

      Arguments are a one or more strings defining fields to group.
    */

    template <typename GroupBy,
              typename std::enable_if<std::is_convertible<GroupBy, string>::value>::type* = nullptr
              >
    CollectionHaving& groupBy(GroupBy group_by_spec)
    {
      get_impl()->add_group_by(group_by_spec);
      return *this;
    }

    template <typename GroupBy,
              typename std::enable_if<!std::is_convertible<GroupBy, string>::value>::type* = nullptr
              >
    CollectionHaving& groupBy(GroupBy group_by_spec)
    {
      for (auto el : group_by_spec)
      {
        get_impl()->add_group_by(el);
      }
      return *this;
    }

    template<typename GroupBy, typename...T>
    CollectionHaving& groupBy(GroupBy group_by_spec,
                              T...rest)
    {
      groupBy(group_by_spec);
      return groupBy(rest...);
    }
  };
}


// ----------------------------------------------------------------------


/*
  Removing documents from a collection
  ====================================

  Class CollectionRemoveBase defines the remove() methods which return
  CollectionRemove operation.

  Note: CollectionRemove does not have any own methods except these
  derived from CollectionSort<>.
*/


/**
  Class representing operation which removes documents from a collection.

  @ingroup devapi_op
*/

class PUBLIC_API CollectionRemove
  : public internal::CollectionSort<Result, CollectionRemove,false>
{
public:

  /**
    Create empty remove operation for a given collection.
  */

  CollectionRemove(Collection &coll);

  /**
    Create remove operation for given collection and documents
    selected by an expression.
  */

  CollectionRemove(Collection &coll, const string&);

DIAGNOSTIC_PUSH

#if _MSC_VER && _MSC_VER < 1900
  /*
    MSVC 2013 has problems with delegating constructors for classes which
    use virtual inheritance.
    See: https://www.daniweb.com/programming/software-development/threads/494204/visual-c-compiler-warning-initvbases
  */
  DISABLE_WARNING(4100)
#endif

  CollectionRemove(CollectionRemove &other) : Executable(other) {}
  CollectionRemove(CollectionRemove &&other) : CollectionRemove(other) {}

DIAGNOSTIC_POP

};


namespace internal {

  /**
    Base class defining methods which create remove statements.
  */

  class PUBLIC_API CollectionRemoveBase
    : public virtual CollectionOpBase
  {

  public:

    /**
      Return operation which removes all documents from the collection.
    */

    virtual CollectionRemove remove()
    {
      return CollectionRemove(*m_coll);
    }

    /**
      Return operation which removes documents satisfying given expression.
    */

    virtual CollectionRemove remove(const string &cond)
    {
      return CollectionRemove(*m_coll, cond);
    }


    friend Collection;
  };

}  // internal


// ----------------------------------------------------------------------

/*
  Searching for documents in a collection
  =======================================

  Class CollectionFindBase defines the find() methods which return
  CollectionFind operation.

  @todo Grouping/aggregation of returned documents.
*/


/**
  Operation which returns all or selected documents from a collection.

  Apart from all the methods inherited from `CollectionSort` it defines
  .fields() clauses which optionally specify final transformation of
  the returned documents.

  @ingroup devapi_op
*/

DLL_WARNINGS_PUSH

class PUBLIC_API CollectionFind
  : public internal::CollectionGroupBy<CollectionFind>
{

DLL_WARNINGS_POP

protected:

  typedef internal::Proj_impl Impl;
  typedef internal::CollectionGroupBy<CollectionFind>  CollectionGroupBy;

  using internal::CollectionGroupBy<CollectionFind>::get_impl;
  using internal::CollectionGroupBy<CollectionFind>::m_impl;

  Impl* get_impl()
  {
    check_if_valid();
    return static_cast<Impl*>(m_impl.get());
  }

public:

  /**
    Create operation which returns all documents from a collection.
  */

  CollectionFind(Collection &coll);

  /**
    Create operation which returns all documents from a collection
    which satisfy given criteria.
  */

  CollectionFind(Collection &coll, const string&);

DIAGNOSTIC_PUSH

#if _MSC_VER && _MSC_VER < 1900
    DISABLE_WARNING(4100)
#endif

  CollectionFind(CollectionFind &other) : Executable<DocResult,CollectionFind>(other) {}
  CollectionFind(CollectionFind &&other) : CollectionFind(other) {}

DIAGNOSTIC_POP

public:

  /**
    Specify projection for documents found in the collection.

    The projection should be an expression given by `expr(<string>)`
    which evaluates to a document. Each document found will be transformed by
    evaluating the expression an the result will be returned by the operation.
  */

  CollectionGroupBy& fields(internal::ExprValue proj)
  {
    if (!proj.isExpression())
      throw_error("Invalid projection");
    get_impl()->set_proj(proj);
    return *this;
  }


  CollectionGroupBy& fields(const string& ord)
  {
    get_impl()->add_proj(ord);
    return *this;
  }

  CollectionGroupBy& fields(const char* ord)
  {
    get_impl()->add_proj(ord);
    return *this;
  }

  /**
    Specify projection for documents found in the collection.

    In this form the projection is given as list of strings of the form
    `"<expression> AS <path>"`. Each expression is evaluated and `<path>`
    specifies where to put the value of the expression in the resulting
    document.

    The strings which define the projection are passed as a vector, list
    or other container with strings.
  */

  template <typename Ord>
  CollectionGroupBy& fields(const Ord& ord)
  {
    for (auto el : ord)
    {
      get_impl()->add_proj(el);
    }
    return *this;
  }

  /**
    Specify projection for documents found in the collection.

    In this form the projection is given as list of strings of the form
    `"<expression> AS <path>"`. Each expression is evaluated and `<path>`
    specifies where to put the value of the expression in the resulting
    document.
  */

  /*
    Note: If e is an expression (of type ExprValue) then only
    .fields(e) is valid - the multi-argument variant .fields(e,...)
    should be disabled.
  */

  template <
    typename Proj, typename...Type,
    typename = typename std::enable_if<
      !std::is_same<Proj, internal::ExprValue>::value
    >::type
  >
  CollectionGroupBy& fields(const Proj& ord, const Type&...rest)
  {
    fields(ord);
    return fields(rest...);
  }

  struct INTERNAL Access;
  friend Access;
};


namespace internal {

  /**
    Base class defining methods which create document queries.
  */

  class PUBLIC_API CollectionFindBase
    : public virtual CollectionOpBase
  {
  public:

    /**
      Return operation which fetches all the documents in the collection.
    */

    CollectionFind find()
    {
      return CollectionFind(*m_coll);
    }

    /**
      Return operation which finds documents that satisfy given expression.
    */

    CollectionFind find(const string &cond)
    {
      return CollectionFind(*m_coll, cond);
    }


    friend Collection;
  };

}  // internal


// ----------------------------------------------------------------------

/*
  Modifying documents in a collection
  ===================================

  Class CollectionModifyBase defines the modify() methods which return
  CollectionModify operation.

*/

class CollectionModify;

namespace internal {

  class CollectionModifyBase;

  /*
    Interface to be implemented by internal implementations of
    CRUD modify operation. Methods `add_operation` are used to
    pass to the implementation object the modifications requested
    by the user.
  */


  struct CollectionModify_impl : public Sort_impl
  {
    enum Operation
    {
      SET,
      UNSET,
      ARRAY_INSERT,
      ARRAY_APPEND,
      ARRAY_DELETE
    };

    virtual void add_operation(Operation, const Field&, ExprValue&&) = 0;
    virtual void add_operation(Operation, const Field&) = 0;
  };

}  // internal


/**
  Operation which modifies all or selected documents in a collection.

   Apart from all the methods inherited from `CollectionSort` it defines
   various clauses which specify modifications to be performed on each
   document.

   @ingroup devapi_op
*/

class PUBLIC_API CollectionModify
  : public internal::CollectionSort<Result, CollectionModify,false>
{
private:

  typedef internal::CollectionModify_impl Impl;

  CollectionModify() = default;

  Impl* get_impl()
  {
    return static_cast<Impl*>(m_impl.get());
  }

public:

  /// Create modify operation for all documents in a collection.

  CollectionModify(Collection &coll);

  /// Create operation which modifies selected documents in a collection.

  CollectionModify(Collection &base, const string &expr);

DIAGNOSTIC_PUSH

#if _MSC_VER && _MSC_VER < 1900
    /*
    MSVC 2013 has problems with delegating constructors for classes which
    use virtual inheritance.
    See: https://www.daniweb.com/programming/software-development/threads/494204/visual-c-compiler-warning-initvbases
    */
    DISABLE_WARNING(4100)
#endif

  CollectionModify(CollectionModify &other) : Executable<Result,CollectionModify>(other) {}
  CollectionModify(CollectionModify &&other) : CollectionModify(other) {}

DIAGNOSTIC_POP

  /**
    Set a given field in a document to the given value.

    Field is given by a document path. The value can be either a direct literal
    or an expression given by `expr(<string>)`, evaluated in the server.
  */

  CollectionModify& set(const Field &field, internal::ExprValue &&val)
  {
    try {
      get_impl()->add_operation(Impl::SET, field, std::move(val));
      return *this;
    }
    CATCH_AND_WRAP
  }

  /**
    Unset a given field in a document.

    The field is given by a document path.
  */

  CollectionModify& unset(const Field &field)
  {
    try {
      get_impl()->add_operation(Impl::UNSET, field);
      return *this;
    }
    CATCH_AND_WRAP
  }

  /**
    Insert value into an array field of a document.

    The `field` parameter should be a document path pointing at a location
    inside an array field. The given value is inserted at this position.
  */

  CollectionModify& arrayInsert(const Field &field, internal::ExprValue &&val)
  {
    try {
      get_impl()->add_operation(Impl::ARRAY_INSERT, field, std::move(val));
      return *this;
    }
    CATCH_AND_WRAP
  }

  /**
    Append value to an array field of a document.

    The `field` parameter should be a document path pointing at an array
    field inside the document. The given value is appended at the end of the
    array.
  */

  CollectionModify& arrayAppend(const Field &field, internal::ExprValue &&val)
  {
    try {
      get_impl()->add_operation(Impl::ARRAY_APPEND, field, std::move(val));
      return *this;
    }
    CATCH_AND_WRAP
  }

  /**
    Delete element from an array field of a document.

    The `field` parameter should be a document path pointing at a location
    inside an array field. The element at indicated location is removed from
    the array.
  */

  CollectionModify& arrayDelete(const Field &field)
  {
    try {
      get_impl()->add_operation(Impl::ARRAY_DELETE, field);
      return *this;
    }
    CATCH_AND_WRAP
  }

  ///@cond IGNORED
  friend internal::CollectionModifyBase;
  ///@endcond
};


namespace internal {

  /**
    Base class defining methods which create document modifying operations.
  */

  class PUBLIC_API CollectionModifyBase
    : public virtual CollectionOpBase
  {
  public:

    /**
      Return operation which modifies all documents in the collection.
    */

    CollectionModify modify()
    {
      try {
        return CollectionModify(*m_coll);
      }
      CATCH_AND_WRAP;
    }

    /**
      Return operation which modifies documents that satisfy given expression.
    */

    CollectionModify modify(const string &expr)
    {
      try {
        return CollectionModify(*m_coll, expr);
      }
      CATCH_AND_WRAP;
    }


    friend Collection;
  };

}  // internal

}  // mysqlx

#endif
