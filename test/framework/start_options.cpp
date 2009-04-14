/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */


#include "start_options.h"


namespace testsuite
{
  /*** TODO figure out will i need it
   * static const String::value_type * defaultUnnamedParam[]= { "dbUrl"
                                                           , "dbUser"
                                                           , "dbPasswd"
                                                           , "dbSchema" }; */

  static const String::value_type * trueStrings[]= {"y", "yes", "true", "t", "1"
                                                , NULL};
  static const String::value_type * falseStrings[]= {"n", "no", "false", "f", "0"
                                                , NULL};

  bool equal2any(const ciString & val, const String::value_type * arr[])
  {
    while( *arr != NULL )
    {
      if ( val == *arr )
        return true;

      ++arr;
    }

    return false;
  }


  StartOptions::StartOptions()
  {
  }


  /* Last orderedParams array member must be NULL */
  StartOptions::StartOptions( const String::value_type    * orderedParams[]
                            , const Properties            * defStrVals
                            , const std::map<String, bool>* defBoolVals )
  {
    while ( *orderedParams != NULL )
    {
      unnamedParams.push_back( *orderedParams );
      ++orderedParams;
    }

    if ( defStrVals != NULL )
      defStringValues= *defStrVals;

    if ( defBoolVals != NULL )
      defBoolValues= *defBoolVals;
  }


  StartOptions::StartOptions( const List                  & orderedParams
                            , const Properties            * defStrVals
                            , const std::map<String, bool>* defBoolVals )
  {
    for ( List::const_iterator cit= orderedParams.begin();
          cit != orderedParams.end();
          ++cit )
    {
      unnamedParams.push_back( *cit );
    }

    if ( defStrVals != NULL )
      defStringValues= *defStrVals;

    if ( defBoolVals != NULL )
      defBoolValues= *defBoolVals;
  }


  bool StartOptions::paramAsBool( const ciString & param ) const
  {
    // not actually needed. since true is default
    if ( equal2any( param, trueStrings ) )
      return true;

    if ( equal2any( param, falseStrings ) )
      return false;

    return true;
  }


	bool StartOptions::parseParams(int paramsNumber, char** paramsValues)
  {
    List paramPair;

    if (paramsNumber > 1)
    {
      List::const_iterator curParam= unnamedParams.begin();

      while (--paramsNumber)
      {
        ciString param(*(++paramsValues));

        if (param.substr(0, 2) == "--")
        {
          paramPair.clear();

          StringUtils::split( paramPair, param.substr(2).c_str(), "=" );

           // Latter shouldn't really ever happen
          if ( paramPair.size() > 2 || paramPair.size() == 0 ) 
          {
            std::cerr << "Can't parse. Wrong start parameter: " << *paramsValues
              << std::endl;
            return false;
          }

          if ( paramPair.size() == 2 )
          {
            sOptions[paramPair[0]]= paramPair[1];

            bOptions[paramPair[0]]= paramAsBool( ciString( paramPair[1].c_str() ) );
          }
          else
          {
            BoolParamsType::const_iterator cit= defBoolValues.find( paramPair[0] );

            if ( cit != defBoolValues.end() )
              bOptions[paramPair[0]]= cit->second;
            else
              bOptions[paramPair[0]]= true;
          }
        }
        else if ( curParam != unnamedParams.end() )
        {
          sOptions[ *curParam ]= String(param.c_str());
          ++curParam;
        }
      }
    }

    return true;
  }


  bool StartOptions::defaultBoolValue( const String & name ) const
  {
    BoolParamsType::const_iterator cit= defBoolValues.find( name );

    if ( (cit) != defBoolValues.end() )
      return cit->second;

    return false;
  }

  // false is default bool value
  bool StartOptions::getBool( const String & name ) const
  {
    BoolParamsType::const_iterator cit= bOptions.find( name );

    if ( cit != bOptions.end() )
      return cit->second;

    return defaultBoolValue( name );
  }


  const String & StartOptions::defaultStringValue( const String & name  ) const
  {
    static String empty;

    Properties::const_iterator cit= defStringValues.find( name );

    if ( cit != defStringValues.end() )
      return cit->second;

    return empty;
  }


  const String &  StartOptions::getString( const String & param) const
  {
    Properties::const_iterator cit= sOptions.find( param );

    if ( cit != sOptions.end() )
      return cit->second;

    return defaultStringValue( param );
  }

} // namespace testsuite

