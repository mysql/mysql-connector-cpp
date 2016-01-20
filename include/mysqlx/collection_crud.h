/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
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

  An object representing such an operation, has methods which modify
  parameters of the operation, such as `CollectionAdd::add()` method.
  Such method returns modified operation object whose methods can be
  used again to modify it further until the operation is described
  completely.

  For example, a call to `CollectionAdd::add()` adds a document to
  the list of documents that this operation should insert  into the
  collection. Chaining several calls to add() like this:
  ~~~~~~
    coll.add(doc1).add(doc2)...add(docN)
  ~~~~~~
  will produce an operation which has all documents `doc1`, ..., `docN`
  in the list and will insert them all at once.
*/


#include "mysqlx/common.h"
#include "mysqlx/result.h"
#include "mysqlx/task.h"


namespace cdk {

class Session;

}  // cdk


namespace mysqlx {

class XSession;
class Collection;
class CollectionAdd;
class CollectionAddExec;


/*
  Adding documents to a collection
  ================================

  We have two variants of this operation: `CollectionAdd` and
  `CollectionAddExec`. The first variant has only `add()` methods but
  not `execute()`. Second variant has both `add()` and execute `add()`.

  The distinction is because insert operation can be executed only
  when at least one document has been specified. Thus, initially operation
  of type `CollectionAdd` has only `add()` methods. These methods return
  new operation object of type `CollectionAddExec` which now can be either
  extended with another call to `add()` or executed with `exec()`.
*/


/**
  Base class for document adding operations.

  This class defines `add()` methods that append new documents to the
  list of documents that shuld be inserted by the operation. Documents
  can be specified as JSON strings or DbDoc instances. Several documents
  can be added in a single call to `add()`.

  @note We have two variants of document adding operation each having
  a different return type of the `add()` method. For that reason the base
  class is a template parametrised with return type of the `add()` method.

  @see `CollectionAdd`, `CollectionAddExec`
*/

template <typename R>
class CollectionAddBase
{
protected:

  Collection &m_coll;

  CollectionAddBase(Collection &coll)
    : m_coll(coll)
  {}

  /*
    These methods are overriden by derived operation classes.
    They append given document to the list of documents to be
    inserted and return resulting modified operation.
  */

  virtual R do_add(const string &json) =0;
  virtual R do_add(const DbDoc &doc) =0;

public:

  /**
    Add document(s) to a collection.

    Documents can be described by JSON strings or DbDoc objects.
  */

  template <typename D>
  R add(const D &doc)
  {
    return do_add(doc);
  }

  /**
    @copydoc add(const DbDoc&)
    Several documents can be passed to single `add()` call.
  */

  template<typename D, typename... Types>
    R add(const D &first, Types... rest)
  {
    try {

      /*
        Note: When do_add() returns CollectionAddExec object then
        the following add() will return reference to the same object.
        Here we return CollectionAddExec by value, so a new instance
        will be created from the one returned by add() using
        move-constructor.
      */

      return std::move(do_add(first).add(rest...));
    }
    CATCH_AND_WRAP
  }

  friend class CollectionAddExec;
};


/**
  Operation which adds documents to a collection.

  Operation stores a list of documents that will be added
  to a given collection when this operation is executed.
  New documents can be appended to the list with `add()`
  method.
*/

class CollectionAddExec
: public Executable
, public CollectionAddBase<CollectionAddExec&>
{
  /*
    Note: We derive from CollectionAddBase<CollectionAddExec&>
    which means that `add()` methods will return references to
    CollectionAddExec object. This way, after adding document
    to the list, we can return reference to `*this` and avoid
    unnecessary copy/move-constructor invocations.
  */

  using Base = CollectionAddBase < CollectionAddExec& > ;

  CollectionAddExec(CollectionAddExec &&other)
    : Executable(std::move(other)), Base(other.m_coll)
  {}

  /*
    Note: This constructor is called from `CollectionAdd::add()`
    to create CollectionAddExec instance with the first document
    put on the list.
  */

  template <typename D>
  CollectionAddExec(Collection &coll, const D &doc)
    : Base(coll)
  {
    initialize();
    do_add(doc);
  }

  void initialize();
  CollectionAddExec& do_add(const string&);
  CollectionAddExec& do_add(const DbDoc&);

  friend class CollectionAdd;
  friend class CollectionAddBase<CollectionAddExec>;
};


/**
  Operation which adds documents to a collection.

  After adding the first document, a `CollectionAddExec` object
  is returned which can be used to add further documents or
  execute the operation.
*/

class CollectionAdd
  : public CollectionAddBase<CollectionAddExec>
{
  using Base = CollectionAddBase < CollectionAddExec >;

  CollectionAdd(Collection &coll)
    : Base(coll)
  {}

  /*
    Note: `do_add()` methods create CollectionAddExec
    instance which is then responsible for adding further
    documents or executing the operation.
  */

  CollectionAddExec do_add(const string &json)
  {
   return CollectionAddExec(m_coll, json);
  }

  CollectionAddExec do_add(const DbDoc &doc)
  {
   return CollectionAddExec(m_coll, doc);
  }

  friend class Collection;
};

/*
  Binding expression parameters
  ================================

*/


/**
  Base class for arguments binding operations.

  This class defines `bind()` methods that bind values to its parameters.

  @note This class is going to be used by different classes, each having  a
  different return type of the `bind()` method. For that reason the base  class
  is a template parameterized with return type of the `bind()` method.

  @see `CollectionFindBind`, `CollectionRemoveBind`
*/

template <typename R>
class CollectionBindBase
{
protected:

  Collection &m_coll;

  CollectionBindBase(Collection &coll)
    : m_coll(coll)
  {}

  /*
    These methods are overriden by derived operation classes.
    They append given document to the list of documents to be
    inserted and return resulting modified operation.
  */

  virtual R& do_bind(const string &parameter, bool val) =0;
  virtual R& do_bind(const string &parameter, int val) =0;
  virtual R& do_bind(const string &parameter, unsigned val) =0;
  virtual R& do_bind(const string &parameter, float val) =0;
  virtual R& do_bind(const string &parameter, double val) =0;
  virtual R& do_bind(const string &parameter, const string &val) =0;

public:

  /**
    Add document(s) to a collection.

    Documents can be described by JSON strings or DbDoc objects.
  */

  template <typename V>
  R& bind(const string& parameter, const V &value)
  {
    return do_bind(parameter, value);
  }

  /**
    @copydoc add(const DbDoc&)
    Several documents can be passed to single `add()` call.
  */

  template<typename D, typename... Types>
  R& add(const D &first, Types... rest)
  {
    try {

      /*
        Note: When do_add() returns CollectionAddExec object then
        the following add() will return reference to the same object.
        Here we return CollectionAddExec by value, so a new instance
        will be created from the one returned by add() using
        move-constructor.
      */

      return std::move(do_add(first).add(rest...));
    }
    CATCH_AND_WRAP
  }

};



/*
  Removing documents from a collection
  ====================================
*/

class CollectionRemoveBind
  : public CollectionBindBase<CollectionRemoveBind>
  , public Executable
{
  using Base = CollectionBindBase<CollectionRemoveBind>;

    public:

      CollectionRemoveBind(Collection &coll)
        : Base(coll)
      {

      }

    protected:

      virtual CollectionRemoveBind& do_bind(const string &parameter, bool val)
      {
        //TODO
        return *this;
      }

      virtual CollectionRemoveBind& do_bind(const string &parameter, int val)
      {
        //TODO
        return *this;
      }

      virtual CollectionRemoveBind& do_bind(const string &parameter, unsigned val)
      {
        //TODO
        return *this;
      }

      virtual CollectionRemoveBind& do_bind(const string &parameter, float val)
      {
        //TODO
        return *this;
      }

      virtual CollectionRemoveBind& do_bind(const string &parameter, double val)
      {
        //TODO
        return *this;
      }

      virtual CollectionRemoveBind& do_bind(const string &parameter,
                                          const string &val)
      {
        //TODO
        return *this;
      }

    };


/**
  Operation which removes documents from a collection.

  @todo Sorting and limiting the range of deleted documents.
  @todo Binding values for operation parameters.
*/

class CollectionRemove
{
  Collection &m_coll;
  CollectionRemoveBind m_exec;

  CollectionRemove(Collection &coll)
    : m_coll(coll)
    , m_exec(coll)
  {}

public:

  /// Remove all documents from the collection.
  virtual Executable& remove();

  /// Remove documents satisfying given expression.
  virtual CollectionRemoveBind& remove(const string&);

  friend class Collection;
};


/*
  Searching for documents in a collection
  =======================================
*/

class CollectionFindBind
    : public CollectionBindBase<CollectionFindBind>
    , public Executable
{
  using Base = CollectionBindBase<CollectionFindBind>;

public:

  CollectionFindBind(Collection &coll)
    : Base(coll)
  {

  }

protected:

  virtual CollectionFindBind& do_bind(const string &parameter, bool val)
  {
    //TODO
    return *this;
  }

  virtual CollectionFindBind& do_bind(const string &parameter, int val)
  {
    //TODO
    return *this;
  }

  virtual CollectionFindBind& do_bind(const string &parameter, unsigned val)
  {
    //TODO
    return *this;
  }

  virtual CollectionFindBind& do_bind(const string &parameter, float val)
  {
    //TODO
    return *this;
  }

  virtual CollectionFindBind& do_bind(const string &parameter, double val)
  {
    //TODO
    return *this;
  }

  virtual CollectionFindBind& do_bind(const string &parameter,
                                      const string &val)
  {
    //TODO
    return *this;
  }

};

/**
  Operation which finds documents satisfying given criteria.

  @todo Sorting and limiting the result.
  @todo Binding values for operation parameters.
  @todo Grouping of returned documents.
*/

class CollectionFind
{
  Collection &m_coll;
  CollectionFindBind m_exec;

  CollectionFind(Collection &coll)
    : m_coll(coll)
    , m_exec(coll)
  {

  }

public:

  /// Return all the documents in the collection.
  virtual Executable& find();

  /// Find documents that satisfy given expression.
  virtual CollectionFindBind& find(const string&);

  friend class Collection;
};


}  // mysqlx

#endif