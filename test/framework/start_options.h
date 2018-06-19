/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is also distributed with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms,
 * as designated in a particular file or component or in included license
 * documentation.  The authors of MySQL hereby grant you an
 * additional permission to link the program and your derivative works
 * with the separately licensed software that they have included with
 * MySQL.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of MySQL Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * http://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
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
