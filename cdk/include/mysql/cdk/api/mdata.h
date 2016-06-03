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

#ifndef CDK_API_MDATA_H
#define CDK_API_MDATA_H

#include "mysql/cdk/foundation.h"


namespace cdk {
namespace api {

template <class Traits>
class Meta_data
{
public:

  typedef typename Traits::col_count_t   col_count_t;
  typedef typename Traits::Type_info     Type_info;
  typedef typename Traits::Format_info   Format_info;
  typedef typename Traits::Column_info   Column_info;

  virtual col_count_t col_count() const =0;

  virtual Type_info   type(col_count_t) =0;
  virtual Format_info format(col_count_t) =0;
  virtual Column_info col_info(col_count_t) =0;

};

}} // cdk::api

#endif
