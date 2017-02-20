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

#ifndef CDK_SESSION_H
#define CDK_SESSION_H

#include "api/session.h"
#include "api/transaction.h"
#include "data_source.h"
#include "reply.h"
#include "common.h"

namespace cdk {


/*
  Session class
  =============
*/


class Session
    : public api::Session
    , public api::Transaction<Traits>
{

protected:
  mysqlx::Session *m_session;
  api::Connection *m_connection;
  bool             m_trans;

  typedef Reply::Initializer Reply_init;

public:

  typedef api::Session::Diagnostics Diagnostics;

  /// Create session to a data store represented by `ds` object.

  Session(ds::TCPIP &ds,
          const ds::TCPIP::Options &options = ds::TCPIP::Options());

  ~Session();

  // Core Session operations.

  option_t is_valid() { return m_session->is_valid(); }
  option_t check_valid() { return m_session->check_valid(); }

  void close() {
    if (m_trans)
      rollback();
    m_session->close();
    m_connection->close();
  }

  /*
    Transactions
    ------------
  */

  /*
    Start new transaction.

    There can be only one open transaction in the session. If
    a transaction is open then begin() throws error.
  */

  void begin() {
    if (m_trans)
      throw_error(cdkerrc::in_transaction, "While starting new transaction");
    m_trans = true;
    m_session->begin();
  }

  /*
    Commit open transaction.

    After commiting transaction is closed and another one can
    be started with begin(). Does nothing if no transaction
    is open.
  */

  void commit() {
    if (m_trans)
      m_session->commit();
    m_trans = false;
  }

  /*
    Rollback open transaction.

    After rolling back, the transaction is closed and another one
    can be started with begin(). Does nothing if no transaction is
    open.
  */

  void rollback() {
    if (m_trans)
      m_session->rollback();
    m_trans = false;
  }


  /*
    Diagnostics
    -----------
    Interface for accessing error/warning/info entries stored in
    a given session object. Note that errors realeated to query
    execution are normally accessed via Reply object. These methods
    access diagnostics which is related to the session itself, not
    the individual queries.
  */

  unsigned int entry_count(Severity::value level=Severity::ERROR)
  {
    return m_session->entry_count(level);
  }

  Iterator& get_entries(Severity::value level=Severity::ERROR)
  {
    return m_session->get_entries(level);
  }

  const Error& get_error()
  {
    return m_session->get_error();
  }

  void clear_errors() { return m_session->clear_errors(); }

  /*
    Data manipulation
    -----------------
    Methods which manipulate data in the data store return vlaue
    of type Reply_init that is used to initialize a Reply instance.
    A Reply object is then used to examine server's reply to a given
    operation, including retreiving and processing result sets
    if any.
  */

  // Statements

  /**
    Execute an SQL query.

    If query contins "?" placeholders, values of these are given by
    `args` list.
  */

  Reply_init sql(const string &query, Any_list *args =NULL)
  {
    return m_session->sql(query, args);
  }

  /**
    Execute xplugin admin command.

    Execute admin command `cmd` with arguments `args`. For example,
    xplugin implements admin command "create_collection" whit 2 arguments:
    name of the collection and name of the schema in which to create.

    TODO: Find documentation for supported admin commands.
  */

  Reply_init admin(const char *cmd, Any_list &args)
  {
    return m_session->admin(cmd, args);
  }

  /*
    CRUD operations
    ---------------
    CRUD operations come in two flavours: operations on collections of
    documents and operations on tables. If an operation returns data,
    this data is in the form of one or more sets of rows (which can be
    processed using a Cursor object).

    Different operations use these common arguments:

    Order_by *order_by -- optional specification of how to order results,
    Limit    *limit    -- optional specification of how to limit the number
                          of returned rows.
    Param_source *param -- optional object which specifies values for named
                          parameters used in expressions that are passed to
                          the operation (such as selection criteria).
  */

  // CRUD for Collections
  // --------------------

  /**
    Add documents to a collection.

    Documents to be inserted are given by a Doc_source object which is
    a sequence of expressions, each expression describing a single document.
    Note that a document can be represented as a JSON blob or as a structured
    document expression.

    Note: Server requires that inserted documents contain "_id" field with
    unique document id.
  */

  Reply_init coll_add(const api::Object_ref &coll, Doc_source &docs, const Param_source *param)
  {
    return m_session->coll_add(coll, docs, param);
  }

  /**
    Remove documents from a collection.

    Remove documents which match condition given by expression `expr` (all
    documents if `expr` is NULL). The range of removed documents can be
    limited by Limit/Order_by specifications.
  */

  Reply_init coll_remove(const api::Object_ref &coll,
                         const Expression *expr = NULL,
                         const Order_by *order_by = NULL,
                         const Limit *lim = NULL,
                         const Param_source *param = NULL)
  {
    return m_session->coll_remove(coll, expr, order_by, lim, param);
  }

  /**
    Find documents in a collection.

    Return all documents which match selection criteria given by expression
    `expr` (all documents if `expr` is NULL). Documents are returned as rows
    with 2 columns

    - column `doc` of type JSON containing the document,
    - column `_id` of type CHAR(N) containing document's id.

    Note: Docuemnt id is also included in the document as a field with
    name "_id".

    Selected documents can be projected to a different document given by
    document expression `proj`. This document expression describes a document
    in which values of fields are given by expressions that can use fields
    extracted from the source document. This way the source doucment can be
    transformed into a document with different structure. If `proj` is NULL
    then documents are returned as-is.
  */

  Reply_init coll_find(const api::Object_ref &coll,
                       const View_spec *view = NULL,
                       const Expression *expr = NULL,
                       const Expression::Document *proj = NULL,
                       const Order_by *order_by = NULL,
                       const Expr_list *group_by = NULL,
                       const Expression *having = NULL,
                       const Limit *lim = NULL,
                       const Param_source *param = NULL)
  {
    return m_session->coll_find(coll, view, expr, proj, order_by,
                                group_by, having, lim, param);
  }

  /**
    Update documents in a collection.

    Update documents that match given expression (all documents if `expr` is
    NULL) according to specification given by `us`. The range of updated
    documents can be limited by Limit/Order_by specifications.

    @see `Update_processor` for information how to specify updates that should
    be applied to each document in the collection.
  */

  Reply_init coll_update(const api::Object_ref &table,
                         const Expression *expr,
                         const Update_spec &us,
                         const Order_by *order_by = NULL,
                         const Limit *lim = NULL,
                         const Param_source *param = NULL)
  {
    return m_session->coll_update(table, expr, us, order_by, lim, param);
  }

  // Table CRUD
  // ----------

  /**
    Select rows from a table.

    Select rows which satisfy criteria given by expression `expr` (or all rows
    if `expr` is NULL).

    Returned rows can be transformed as specified by `proj` argument.
    Projection specification is a list of expressions, each possibly with an
    alias. Expressions give the values of columns in the resulting row. These
    values can depend on values of fields in the source row.

    @see `api::Projection_processor`
  */

  Reply_init table_select(const api::Table_ref &tab,
                          const View_spec *view = NULL,
                          const Expression *expr = NULL,
                          const Projection *proj = NULL,
                          const Order_by *order_by = NULL,
                          const Expr_list *group_by = NULL,
                          const Expression *having = NULL,
                          const Limit* lim = NULL,
                          const Param_source *param = NULL)
  {
    return m_session->table_select(tab, view, expr, proj, order_by,
                                   group_by, having, lim, param);
  }

  /**
    Insert rows into a table.

    Insert rows given by a Row_source object. A Row_source object is a sequence
    of rows where each row is described by a list of expressions, one
    expression per one column in the row.
  */

  Reply_init table_insert(const api::Table_ref &tab,
                          Row_source &rows,
                          const api::Columns *cols,
                          const Param_source *param)
  {
    return m_session->table_insert(tab, rows, cols, param);
  }

  /**
    Delete rows from a table.

    Delete rows which match condition given by expression `expr`. If `expr`
    is NULL, deletes all rows in the table. The range of removed rows
    can be limited by Limit/Order_by specifications.
  */

  Reply_init table_delete(const api::Table_ref &tab,
                          const Expression *expr,
                          const Order_by *order_by,
                          const Limit* lim = NULL,
                          const Param_source *param = NULL)
  {
    return m_session->table_delete(tab, expr, order_by, lim, param);
  }


  /**
    Update rows in a table.

    Update rows that match given expression (all rows if `expr` is NULL)
    according to specification given by `us`. The range of updated rows
    can be limited by Limit/Order_by specifications.

    @see `Update_processor` for information how to specify updates that should
    be applied to each row.
  */

  Reply_init table_update(const api::Table_ref &tab,
                          const Expression *expr,
                          const Update_spec &us,
                          const Order_by *order_by,
                          const Limit *lim = NULL,
                          const Param_source *param = NULL)
  {
    return m_session->table_update(tab, expr, us, order_by, lim, param);
  }


  // Views
  // -----

  Reply_init view_drop(const api::Table_ref &view, bool check_existence = false)
  {
    return m_session->view_drop(view, check_existence);
  }


  // Async_op interface

public:

  bool is_completed() const { return m_session->is_completed(); }

  /*
    Note: This does not work correctly yet, because xplugin is not
    correctly reporting current schema changes.
  */

  const string& current_schema() const
  {
    return m_session->get_current_schema();
  }

private:

  bool do_cont() { return m_session->cont(); }
  void do_wait() { m_session->wait(); }
  void do_cancel() { THROW("not supported"); }
  const api::Event_info* get_event_info() const { return m_session->get_event_info(); }

};


} // cdk

#endif // CDK_SESSION_H
