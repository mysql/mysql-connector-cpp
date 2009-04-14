/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef __CCPP_NOCOPY_H
#define __CCPP_NOCOPY_H

namespace policies
{

class nocopy
{
private:
  nocopy (const nocopy & other){}
  const nocopy & operator = (const nocopy & other);

protected:

  // don't need anybody could create(and delete) instance of class.
  nocopy(){}
  ~nocopy(){}

};


}  // namespace policies

#endif
