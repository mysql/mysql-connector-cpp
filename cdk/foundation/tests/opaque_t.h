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

#ifndef MYSQL_CDK_FOUNDATION_TESTS_OPAQUE_T_H
#define MYSQL_CDK_FOUNDATION_TESTS_OPAQUE_T_H

/*
  Declare classes X, Y and Z whose implementation is hidden
  using opaque_impl<> template. Implementation of X and Y is
  defined in opaque_t_impl.cc, class Z is implemented in
  opaque_t.cc.
*/

#include <mysql/cdk/foundation/opaque_impl.h>

using cdk::foundation::opaque_impl;

class Y;

class X : public opaque_impl<X>
{
public:

  X(int x);

  void foo();
  int  bar() const;

  friend class Y;
};


class Y : opaque_impl<Y>
{
public:

  Y(int x);
  Y(const X&);

  void foo();
};


class Z : opaque_impl<Z>
{
public:
  Z();
  void foo();
};

#endif

