/*
Copyright (c) 2008, 2011, Oracle and/or its affiliates. All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPLv2
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPLv2 as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
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
