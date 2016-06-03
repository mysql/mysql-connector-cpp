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

#ifndef CDK_API_REPLY_H
#define CDK_API_REPLY_H

#include "mysql/cdk/foundation.h"


namespace cdk {
namespace api {

template <class Traits>
class Reply
    : public Diagnostics
    , public Async_op<void>
{

public:

  typedef typename Traits::row_count_t row_count_t;


  /*
     Method has_results() Returns true if there are result sets included in the
     reply. To access these result sets one has to create a cursor. The exact
     way of creating cursors for result sets of a reply is defined by
     implementation. Once a cursor is created for the first result set, this
     result set can be �consumed� so that it is no longer accessible. In that
     case has_results() informs if there are more result sets left in the reply.
     In other words, has_results() informs about result sets that can be still �consumed� and if it returns false it means that all of them have been processed.
    Returns true if there are result sets included in the reply.
  */
  virtual bool has_results() = 0;


  /*
     Skip a result set (if reply has one) without creating a cursor for it
     (and thus avoiding allocation of cursor resources). If reply has several
     result sets then the next one becomes available.
  */
  virtual void skip_result() = 0;


  /*
     Method has_out_params() informs if this reply contains values for output
     parameters of a stored routine call. The values of the output parameters
     can be processed with a given processor by calling get_out_params() method.

     TODO: Add when implemented
  */
  //virtual bool has_out_params() = 0;


  /*
     Reply::get_out_params use Out_param_processor to inform of the reply.

     TODO: Add when implemented.
  */
  //virtual void get_out_params(Out_param_processor &out) = 0;


  /*
     Inform about how many rows have been affected by the operation.
     This is overall number - more detailed information can be obtained with
     get_statistics() (if supported by the implementation).
  */
  virtual row_count_t affected_rows() = 0;


  /*
     Get detailed statistics about data changes introduced by the operation.
     Returned object is an iterator over entries which describe changes in
     individual tables.

     TODO: Add when implemented.
  */
  //virtual const Op_statistics<Traits>& get_statistics() = 0;


  /*
     Get timing information for the operation (see below).

     TODO: Add when implemented.
  */
  //virtual Op_time_info get_time_info() = 0;


  /*
     Discard the reply freeing all allocated resources before the reply object
     is destroyed. Related objects such as cursors created for this reply are
     also freed. Using Reply instance after calling discard() on it throws an
     error.
  */
  virtual void discard() = 0;

};


}} // cdk::api

#endif
