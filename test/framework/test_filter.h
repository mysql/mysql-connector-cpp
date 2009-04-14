/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
