/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */


#ifndef __TEST_FRAMEWORK_START_OPTIONS
#define __TEST_FRAMEWORK_START_OPTIONS

#include "../common/ccppTypes.h"
#include "../common/stringutils.h"


namespace testsuite
{

class StartOptions
{

private:

  typedef std::map<String, bool>  BoolParamsType;

  List                            unnamedParams;

  BoolParamsType                  defBoolValues;
  Properties                      defStringValues;

	BoolParamsType		              bOptions;
	Properties                      sOptions;

protected:

          bool    paramAsBool       ( const ciString  & param ) const;
          bool    defaultBoolValue  ( const String    & name  ) const;
  const String &  defaultStringValue( const String    & name  ) const;

public:

  StartOptions();

  StartOptions( const List                    & orderedParams
              , const Properties              * defStrVals  = NULL
              , const std::map<String, bool>  * defBoolVals = NULL );

  /* Last array member must be NULL */
  StartOptions( const String::value_type      * orderedParams[]
              , const Properties              * defStrVals  = NULL
              , const std::map<String, bool>  * defBoolVals = NULL );

        bool      parseParams(int paramsNumber, char** paramsValues);

        bool      getBool    ( const String & name ) const;
  const String &  getString  ( const String & param) const;

};

} // namespace testsuite
#endif  // __TEST_FRAMEWORK_START_OPTIONS
