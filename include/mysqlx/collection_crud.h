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
  before it gets executed. For example `CollectionAdd:add()`
  appends a document to the list of documents that should be added
  by given operation. These methods can be chained as allowed by
  the fluent API grammar. In case of collection CRUD operations the
  grammar can be described by the following diagram:

    Collection -> CollectionAdd         : add()
    Collection -> CollectionRemove      : remove()
    Collection -> CollectionFind        : find()
    Collection -> CollectionModifyFirst : modify()

    CollectionAdd = Executable
    CollectionAdd -> CollectionAdd : add()

    CollectionRemove = CollectionSort<false>

    CollectionFind = CollectionSort<true>

    CollectionModifyFirst = Executable
    CollectionModifyFirst -> CollectionModify : set()
    CollectionModifyFirst -> CollectionModify : unset()
    CollectionModifyFirst -> CollectionModify : arrayXXX()

    CollectionModify = CollectionModifyFirst
    CollectionModify = CollectionSort<false>

    CollectionSort<F> = Limit<F>
    CollectionSort<F> -> Limit<F> : sort()

  In this diagram notation X -> Y : foo() means that class X defines
  public method foo() with return type Y (roughly). Notation X = Y means
  that X inherits public methods from Y. See crud.h for grammar
  rules for Limit<> and other commoon classes.

  CRUD operation objects do not have copy semantics. If CRUD object a is
  constructed from b, like in "a = b", then the operation moves from b to
  a and any attempt to execute or modify b will trigger error.
*/


#include "common.h"
#include "result.h"
#include "task.h"
#include "crud.h"

#include <utility>

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
    the Collection class.
  */

  class CollectionOpBase
  {
  protected:

    Collection *m_coll;

    CollectionOpBase(Collection &coll) : m_coll(&coll)
    {}

    CollectionOpBase(const CollectionOpBase&) = default;

    /*
      This constructor is here only to alow defining
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

  Base class CollectionAddBase defines the various forms of add() method
  which produce CollectionAdd operation. The same methods are defined
  by CollectionAdd class and can be used to append further documents
  to the operation.

*/

class CollectionAdd;


namespace internal {


  /*
    Type trait which is used to distinguish types which can describe a single
    document from other types such as document collections. This is needed
    to correctly defined CollectionAddInterface below.

    Currently we have DbDoc type and also support documents described by JSON
    strings.
  */

  template <typename D> struct is_doc_type
  {
    static const bool value
      = std::is_convertible<D, DbDoc>::value
      || std::is_convertible<D, string>::value;
  };


  /*
    Class which defines various variants of `add()` method.

    This class defines `add()` methods that append new documents to the
    list of documents that shuld be inserted by a CollectionAdd operation.
    Documents can be specified as JSON strings or DbDoc instances. Several
    documents can be added in a single call to `add()`.

    @note We have two variants of add() operations which differ by return type.
    CollectionAddBase::add() returns CollectionAdd object by value while
    CollectionAdd::add() returns reference to itself. For that reason
    CollectionAddInterface<> is a template parametrised with return type of
    the `add()` method. But in either case the AddOp type refers to
    a CollectionAdd instance which implemnts do_add() method which appends a
    document to the list.
  */

  template<class AddOp>
  class CollectionAddInterface
  {
    /*
      This method must be overriden by derived class to return an
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

      Any container type for which std::begin()/std::end() are defined
      should work.

      Note: we use enable_if to remove ambiguity between this overload
      and the one which adds a single document: add(doc). Thus this
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

    friend class CollectionAdd;
  };


  class CollectionAddBase;

}  // internal


/**
  Operation which adds documents to a collection.

  Operation stores a list of documents that will be added
  to a given collection when this operation is executed.
  New documents can be appended to the list with various variants
  of `add()` method, as defined by CollectionAddInterface<>.
*/

class CollectionAdd
  : public Statement
  , public internal::CollectionAddInterface<CollectionAdd&>
{
public:

  /**
    Create empty add operation for a given collection.
  */

  CollectionAdd(Collection &coll);

  CollectionAdd(CollectionAdd &other) : Statement(other) {}
  CollectionAdd(CollectionAdd &&other) : CollectionAdd(other) {}

private:

  virtual CollectionAdd& get_op()
  {
    check_if_valid();
    return *this;
  }

  void do_add(const string&);
  void do_add(const DbDoc&);

  friend class internal::CollectionAddBase;
  friend class internal::CollectionAddInterface<CollectionAdd>;
  friend class internal::CollectionAddInterface<CollectionAdd&>;
};


namespace internal {

  class CollectionAddBase
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

  /*
    Sort interface for collection CRUD operations.

    Note: the actual job of adding sort specification to the underlying
    task is done by do_sort() method defined by SortBase<> (see crud.h)
  */

  template <bool limit_with_offset>
  class CollectionSort
    : public SortBase<limit_with_offset>
  {
  public:

    Limit<limit_with_offset>& sort(const string& ord_spec)
    {
      this->do_sort(ord_spec);
      return *this;
    }

    Limit<limit_with_offset>& sort(const char* ord_spec)
    {
      this->do_sort(ord_spec);
      return *this;
    }

    template <typename Ord>
    Limit<limit_with_offset>& sort(Ord ord)
    {
      for (auto el : ord)
      {
        this->do_sort(ord);
      }
      return *this;
    }

    template <typename Ord, typename...Type>
    Limit<limit_with_offset>& sort(Ord ord, const Type...rest)
    {
      this->do_sort(ord);
      return sort(rest...);
    }

  };

}  // internal


// ----------------------------------------------------------------------

namespace internal {


class CollectionFields
  : public CollectionSort<true>
{

  void do_fields(const string&);

public:

  CollectionSort<true>& fields(const string& ord)
  {
    do_fields(ord);
    return *this;
  }

  CollectionSort<true>& fields(const char* ord)
  {
    do_fields(ord);
    return *this;
  }

  template <typename Ord>
  CollectionSort<true>& fields(const Ord& ord)
  {
    for(auto el : ord)
    {
      do_fields(el);
    }
    return *this;
  }

  template <typename Ord, typename...Type>
  CollectionSort<true>& fields(const Ord& ord, const Type&...rest)
  {
    fields(ord);
    return fields(rest...);
  }

};

}  // internal



/*
  Removing documents from a collection
  ====================================

  Class CollectionRemoveBase defines the remove() methods which return
  CollectionRemove operation.

  Note: CollectionRemove does not have any own methods except these
  derived from CollectionSort<>.
*/


class CollectionRemove
  : public internal::CollectionSort<false>
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

  CollectionRemove(CollectionRemove &other) : Statement(other) {}
  CollectionRemove(CollectionRemove &&other) : CollectionRemove(other) {}

DIAGNOSTIC_POP

};


namespace internal {

  class CollectionRemoveBase
    : public virtual CollectionOpBase
  {

  public:

    /**
      Remove all documents from the collection.
    */

    virtual CollectionRemove remove()
    {
      return CollectionRemove(*m_coll);
    }

    /**
      Remove documents satisfying given expression.
    */

    virtual CollectionRemove remove(const string &cond)
    {
      return CollectionRemove(*m_coll, cond);
    }


    friend class Collection;
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


class CollectionFind
  : public internal::CollectionFields
{
public:

  /**
    Create operation which returns all documents from a collection.
  */

  CollectionFind(Collection &coll);

  /**
    Create opeartion which returns all documents from a collection
    which satisfy given criteria.
  */

  CollectionFind(Collection &coll, const string&);

DIAGNOSTIC_PUSH

#if _MSC_VER && _MSC_VER < 1900
    DISABLE_WARNING(4100)
#endif

  CollectionFind(CollectionFind &other) : Statement(other) {}
  CollectionFind(CollectionFind &&other) : CollectionFind(other) {}

DIAGNOSTIC_POP


};


namespace internal {

  class CollectionFindBase
    : public virtual CollectionOpBase
  {
  public:

    /**
      Return all the documents in the collection.
    */

    CollectionFind find()
    {
      return CollectionFind(*m_coll);
    }

    /**
      Find documents that satisfy given expression.
    */

    CollectionFind find(const string &cond)
    {
      return CollectionFind(*m_coll, cond);
    }


    friend class Collection;
  };

}  // internal


// ----------------------------------------------------------------------

/*
  Modifying documents in a collection
  ===================================

  Class CollectionModifyBase defines the modify() methods which return
  CollectionModify operation.

  To be precise, modify() methods return CollectionModifyFirst, which
  is like CollectionModify, but it does not have sorting/limiting
  metods. Only after specifying the first modification, one gets
  CollectionModify object which inherits from CollectionSort<>.
*/

class CollectionModify;

namespace internal {

  /*
    Common implementaiton of modification clauses used by CollectionModify
    and CollectionModifyFirst.
  */

  class CollectionModifyInterface
  {
  protected:

    /*
      This method must be overriden by derived class to return reference
      to the operation that should be manipulated by the modify methods.
    */

    virtual CollectionModify& get_op() = 0;

  public:

    CollectionModify& set(const Field &field, ExprValue &&val);

    CollectionModify& unset(const Field &field);

    CollectionModify& arrayInsert(const Field &field, ExprValue &&val);

    CollectionModify& arrayAppend(const Field &field, ExprValue &&val);

    CollectionModify& arrayDelete(const Field &field);

  };

  class CollectionModifyBase;
  class CollectionModifyFirst;

}  // internal


class CollectionModify
: public virtual internal::CollectionModifyInterface
, public internal::CollectionSort<false>
{
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

  CollectionModify(CollectionModify &other) : Statement(other) {}
  CollectionModify(CollectionModify &&other) : CollectionModify(other) {}
  CollectionModify(internal::CollectionModifyFirst &&other);

DIAGNOSTIC_POP

private:

  CollectionModify& get_op() { return *this; }

  struct Access;
  friend struct Access;
  friend class internal::CollectionModifyBase;
};


namespace internal {


  /*
    Class CollectionModifyFirst contains CollectionModify
    but it does not expose sorting/limiting methods.
  */

  class CollectionModifyFirst
    : public virtual CollectionModifyInterface
    , public virtual Statement
    , CollectionModify
  {
  public:

    /*
      Note: this constructor is needed for the following code to work:

        auto op = coll.modify(...);

      Method coll.modify() returns CollectionModifyFirst object by value
      and it needs to be moved to the local variable.
    */

    CollectionModifyFirst(CollectionModifyFirst &&other)
      : Statement(other)
      , CollectionModify(std::move(other))
    {}

    CollectionModifyFirst(const CollectionModifyFirst&) = delete;

  private:

    CollectionModifyFirst(CollectionModify &&other)
      : Statement(other)
      , CollectionModify(other)
    {}

    CollectionModify& get_op()
    {
      return *static_cast<CollectionModify*>(this);
    }


    friend class CollectionModifyBase;
    friend class mysqlx::CollectionModify;
  };


  class CollectionModifyBase
    : public virtual CollectionOpBase
  {
  public:

    /**
      Modify all documents.
    */

    CollectionModifyFirst modify()
    {
      try {
        return CollectionModify(*m_coll);
      }
      CATCH_AND_WRAP;
    }

    /**
      Modify documents that satisfy given expression.
    */

    CollectionModifyFirst modify(const string &expr)
    {
      try {
        return CollectionModify(*m_coll, expr);
      }
      CATCH_AND_WRAP;
    }


    friend class Collection;
  };

}  // internal


DIAGNOSTIC_PUSH

#if _MSC_VER && _MSC_VER < 1900
  DISABLE_WARNING(4100)
#endif

inline
CollectionModify::CollectionModify(internal::CollectionModifyFirst &&other)
  : CollectionModify(other.get_op())
{}

DIAGNOSTIC_POP

}  // mysqlx

#endif
