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

#ifndef CDK_API_PROCESSORS_H
#define CDK_API_PROCESSORS_H

#include <mysql/cdk/foundation.h>

namespace cdk {
namespace api {


template <class Traits>
class Row_processor
{
public:

  typedef cdk::byte  byte;
  typedef cdk::bytes bytes;
  typedef typename Traits::row_count_t row_count_t;
  typedef typename Traits::col_count_t col_count_t;

  /*
     An object implementing Row_processor interface is used to examine data from
     a result set (via Cursor::get_rows() method).
  */


  /*
     Methods called before and after processing single row. The pos parameter
     starts from 0 and is increased by 1 for each row processed in a single call
     to Cursor::get_rows() (note: it is not position within the cursor).
     If row_begin() returns false then given row is skipped (no field data will
     be passed to the processor). If row is skipped then row_end() is not called
     for that row.
  */
  virtual bool row_begin(row_count_t pos) = 0;
  virtual void row_end(row_count_t pos) = 0;


  /*
     Called before and after processing one field within a row. The pos
     parameter indicates 0-based position of the field within the row.
     Method field_begin() returns the amount of space available for storing
     field data - following field_data() calls should respect this limit.
     If 0 is returned then given field is skipped without calling field_end()
     for it. The amount of available space can be adjusted by filed_data()
     method (see below).
  */
  virtual size_t field_begin(col_count_t pos, size_t data_len) = 0;
  virtual void field_end(col_count_t pos) = 0;


  /*
     Called if given field is NULL. Methods field_begin() and field_end() are
     not called in that case.
  */
  virtual void field_null(col_count_t pos) = 0;


  /*
     Called to pass data stored in a given field. This data can be sent in
     several chunks using several calls to field_data() with the same field
     position. End of data is indicated by field_end() call. Method field_data()
     returns the currently available space for storing the data. The chunks of
     data passed via the following field_data() calls should not exceed this
     space limit. If field_data() returns 0 then it means that processor is not
     interested in seeing any more data for this field and remaining data
     (if any) will be discarded (followed by field_end() call)
  */
  virtual size_t field_data(col_count_t pos, bytes data) = 0;


  /*
     Called when there are no more rows in the result set. Note that if a
     requested number of rows has been passed to row processor then this method
     is not called - it is called only if end of data is detected before passing
     the last of requested rows.
  */
  virtual void end_of_data() = 0;


};


}}  // cdk::api

#endif
