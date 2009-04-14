/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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

        virtual ~StorableTest();

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
