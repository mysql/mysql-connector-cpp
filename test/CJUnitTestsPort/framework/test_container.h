/* Copyright 2009 Sun Microsystems, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

There are special exceptions to the terms and conditions of the GPL
as it is applied to this software. View the full text of the
exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
software distribution.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __TESTCONTAINER_H_
#define __TESTCONTAINER_H_

#include "../common/ccppTypes.h"
#include "../common/nocopy.h"


namespace testsuite
{

  class Test;

  namespace Private
  {
    class TestContainer
    {
    protected:
      class StorableTest : public policies::nocopy
      {
        StorableTest(){}

        Test *        test;

      public:

        StorableTest( Test & test2decorate );

        void            runTest ();

        const String &  name    () const;

        /*Test & operator * ();
        Test * operator ->();*/

        Test * get();

      };
    };
  }
}


#endif  // ifndef __TESTCONTAINER_H_
