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

#ifndef CDK_API_CURSOR_H
#define CDK_API_CURSOR_H

#include "processors.h"


namespace cdk {
namespace api {

template <class Traits>
class Cursor : public Async_op<void>
{
public:

  typedef typename Traits::row_count_t   row_count_t;
  typedef cdk::api::Row_processor<Traits> Row_processor;


  /*
     Fetch given amount of rows from the cursor and pass them to a row processor,
     one-by-one. This method returns immediately after starting an asynchronous
     operation that is controlled using methods from Async_op interface.
  */
  virtual void get_rows(Row_processor& rp) = 0;
  virtual void get_rows(Row_processor& rp, row_count_t limit) = 0;


  /*
     Convenience method that calls get_rows(rp, 1) to fetch a single row, then
     waits for this operation to complete and then returns true if a row was
     fetched or false if there are no more rows in the cursor.
  */
  virtual bool get_row(Row_processor& rp) = 0;


  /*
     Close cursor and free all resources before it is destroyed. Using the
     cursor after close() throws an error.
  */
  virtual void close() = 0;


  //  TODO: Add seek()/rewind() methods when implemented.

#if 0
  /*
     Method seek() changes current position within the cursor. Convenience
     method rewind() is equivalent to seek(BEGIN). If current position of the
     cursor can not be changed then these methods should throw error. Possible
     starting positions for seek() are: BEGIN, END and CURRENT. Possible
     directions are: BACK and FORWARD.
  */

  enum from
  {
    BEGIN,
    END,
    CURRENT
  };

  enum direction
  {
    BACK,
    FORWARD
  };


  virtual void rewind() = 0;
  virtual void seek(enum from, row_count_t count=0,
                    enum direction=FORWARD) = 0;
#endif

};

}} // cdk::api

#endif
