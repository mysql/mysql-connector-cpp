/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef __CCPP_SINGLETON_H
#define __CCPP_SINGLETON_H

#include "nocopy.h"

namespace policies
{

template <class T>
class Singleton: public nocopy
{
protected:

  Singleton(){}

public:

  static T & theInstance()
  {
    static T instance;

    return instance;
  }
};


}  // namespace policies

// macros to use in private/protected part of singletoned class
#define CCPP_SINGLETON(classname) classname();\
  friend class policies::Singleton<classname>

#endif
