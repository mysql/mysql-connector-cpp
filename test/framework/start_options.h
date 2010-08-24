/*
  Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

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

  BoolParamsType		          bOptions;
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

        int       getInt     ( const String & param) const;

};

} // namespace testsuite
#endif  // __TEST_FRAMEWORK_START_OPTIONS
