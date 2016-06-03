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

/**
  Unit tests for opaque implementation infrastructure.
*/

#include "test.h"
#include "opaque_t.h"
#include <iostream>


using namespace ::std;


/*
  Class deriving from Y with hidden implementation
  (defined in separate translation uint opaque_t_impl.cc)
*/

class U : public Y
{
public:

  U() : Y(5)
  {}

  void foo()
  {
    cout <<"U: ";
    Y::foo();
  }
};



/*
  Implementation of class Z (to have it in a different
  translation unit). Copy constructor for Z is generated
  by compiler. Default constructor is defined explicitly.
*/


#include <mysql/cdk/foundation/opaque_impl.i>

using namespace cdk::foundation;

struct Z_impl
{
  void foo()
  {
    cout <<"Z: foo()" <<endl;
  }
};

IMPL_TYPE(Z, Z_impl);
IMPL_DEFAULTCOPY(Z);


void Z::foo() { get_impl().foo(); }

Z::Z()
{
  cout <<"Z: default constructor" <<endl;
}



TEST(Opaque, basic)
{
  X x(7);
  x.foo();
  Y y(x);
  y.foo();
  Z z;
  Z zz(z);
  zz.foo();
  U u;
  u.foo();
  cout <<"Done!" <<endl;
}



