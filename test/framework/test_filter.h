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




#ifndef __TEST_FILTRE_H_
#define __TEST_FILTRE_H_

#include "../common/ccppTypes.h"
#include "../common/stringutils.h"
#include <stdlib.h>

namespace testsuite
{

_ABSTRACT class Filter
{
protected:
  typedef std::vector<Filter *> FiltersList;

public:

  virtual ~Filter(){}

  virtual bool Admits( const String & name2test ) const _PURE;

};


class SingleFilter : public Filter
{
private:

  List  staticPart;
  bool  negative;

public:

        SingleFilter( const String & filterStr
                    , const String & NOTsymbol= "!" );

  bool  Admits      ( const String & testName ) const;
};


// Sequence of "&&" filters
class SerialFilter : public Filter
{
  FiltersList filter;

public:

        SerialFilter  ( const String & filterString
                      , const String & ANDsymbol= "&&"
                      , const String & NOTsymbol= "!" );

        ~SerialFilter ();

  bool  Admits        ( const String & testName ) const;


};


// Sequence of "||" filters
class FiltersSuperposition : public Filter
{
  FiltersList filter;

public:

        FiltersSuperposition  ( const String & filterString
                              , const String & ORsymbol=  "||"
                              , const String & ANDsymbol= "&&"
                              , const String & NOTsymbol= "!" );

        ~FiltersSuperposition ();

  bool  Admits                ( const String & testName ) const;
};


} // namespace testsuite
#endif  // __TEST_FILTRE_H_
