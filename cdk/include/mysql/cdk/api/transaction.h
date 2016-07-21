/*
 * Copyright (c) 2016, Oracle and/or its affiliates. All rights reserved.
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

#ifndef CDK_API_TRANSACTION_H
#define CDK_API_TRANSACTION_H

#include "mysql/cdk/foundation.h"


namespace cdk {
namespace api {

template <class Traits>
class Transaction
    : public Diagnostics
{

public:

  typedef typename Traits::transaction_id_t transaction_id_t;
  //typedef typename Traits::savepoint_id_t   savepoint_id_t;

  virtual transaction_id_t  commit() = 0;
  virtual void rollback() = 0;

  /*
    TODO:
    Returns true if there are any data modification requests collected in
    the transaction.
  */
  //virtual bool has_changes() = 0;

  /*
    TODO:
    Create a savepoint with given id. If a savepoint with the same id was
    created earlier in the same transaction, then it is replaced by the new one.
    It is an error to create savepoint with id 0, which is reserved for
    the beginning of the current transaction.
  */
  //virtual void Transaction::savepoint(savepoint_id_t id) = 0;

  /*
    TODO:
    Rollback transaction to the given savepoint. Savepoint id 0 (the default)
    means beginning of the transaction.
  */
  //virtual void Transaction::rollback(savepoint_id_t sp = 0) = 0;

};


}} // cdk::api

#endif
