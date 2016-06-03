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

#ifndef SDK_FOUNDATION_STATIC_ASSERT_H
#define SDK_FOUNDATION_STATIC_ASSERT_H

/*
  Emulation of some C++11 features used in CDK code.
*/

#include <mysql/cdk/config.h>


namespace cdk {
namespace foundation {

#ifndef HAVE_STATIC_ASSERT

/*
  Idea: Instantiation of static_assertion_test<false> is not defined and compiler
  will choke on sizeof() in static_assert(). But static_assertion_test<true> is
  defined and compiler should go through static_assert().
*/

template <bool test>
struct static_assert_test;

template <>
struct static_assert_test<true>
{
};

// TODO: Fix this implementation or think how to avoid it altogether
// TODO: Better message when assertion fails

#define static_assert(B,Msg)
/*
  enum { static_assert_test ## __LINE_ \
         = sizeof(cdk::foundation::static_assert_test< (bool)(B) >) }
*/

#endif


#ifndef HAVE_IS_SAME

  template <typename T, typename U>
  struct is_same
  {
    static const bool value = false;
  };

  template <typename T>
  struct is_same<T,T>
  {
    static const bool value = true;
  };

#else

  using ::is_same;

#endif


}}  // cdk::foundation


#endif
