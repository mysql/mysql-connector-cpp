/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#ifndef _TESTSUITE_RESOURCES_H_
#define _TESTSUITE_RESOURCES_H_

#include "../common/ccppTypes.h"

namespace testsuite
{
namespace resources
{

class CharsetMapping
{
public:

  typedef std::map<String, unsigned int> Map;
  typedef Map::const_iterator cit;

private:

  /* Hiding constructor */
  CharsetMapping()
  {
    Init();
  }

  Map STATIC_CHARSET_TO_NUM_BYTES_MAP;

  void Init();

public:

  static const CharsetMapping & Instance();

  const Map & GetMap() const
  {
    return STATIC_CHARSET_TO_NUM_BYTES_MAP;
  }
};

int LoadProperties(const String & fileName, Properties &props
                   , const char * _possibleLocations[]=NULL);
}
}
#endif
