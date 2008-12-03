/* Copyright (C) 2007 - 2008 MySQL AB, 2008 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   There are special exceptions to the terms and conditions of the GPL
   as it is applied to this software. View the full text of the
   exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this
   software distribution.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <time.h>

#include "BaseTestCase.h"
#include "../common/stringutils.h"

namespace testsuite
{
  /* Static properties initialization */
  const String TestFixtureCommon::ADMIN_CONNECTION_PROPERTY_NAME =
    _T("com.mysql.jdbc.testsuite.admin-url");

  const String TestFixtureCommon::NO_MULTI_HOST_PROPERTY_NAME =
    _T("com.mysql.jdbc.testsuite.no-multi-hosts-tests");

  //String TestFixtureCommon::dbUrl = _T("tcp://127.0.0.1/");

  int TestFixtureCommon::instanceCount = 1;

  Properties TestFixtureCommon::sqlProps;

  static const char * possiblePropertiesLocations[] = { ".."
                                                      , "test/CJUnitTestsPort"
                                                      , NULL //last should be NULL
                                                      };

  int TestFixtureCommon::propsLoaded = resources::LoadProperties( "sql.properties"
                                                                , sqlProps
                                                                , possiblePropertiesLocations);

  Driver *  TestFixtureCommon::driver = NULL;


  TestFixtureCommon::TestFixtureCommon (  const String & dbUrl
                                        , const String & dbUser
                                        , const String & dbPasswd
                                        , const String & dbSchema)
        :   host  ( dbUrl   )
         ,  login ( dbUser  )
         ,  passwd( dbPasswd)
         ,  db    ( dbSchema)
      
  {
  }

  String TestFixtureCommon::extractVal( const String  & sTableName
                                      , int             count
                                      , Properties    & sqlProps
                                      , Connection    & conn) 
  {
    String  sKeyName;
    String  insertString;
    String  retStr;
    String  parameters;
    List    sToken;

    try
    {
      sKeyName = sTableName;
      sKeyName.append("_Insert");

      insertString = sqlProps[ sKeyName ];

      String::size_type openPar= insertString.find_first_of("(") + 1;

      parameters = insertString.substr(
        openPar, insertString.find_first_of(")", 1) - openPar );

      StringUtils::split(sToken, parameters, ",");

      retStr = sToken[ count - 1];

      if ( sTableName == "Bit_Tab"
        || sTableName == "Boolean_Tab" )
      {
        if ( retStr == "1" )
        {
          retStr = "true";
        }
        else if (retStr == "0")
        {
          retStr = "false";
        }
      }
    }
    catch (std::exception & e)
    {
      logErr( String( "Exception " ) + e.what() );
      FAIL("Call to extractVal is Failed!");
    }

    return retStr;
  }


  void TestFixtureCommon::logMsg(String message)
  {
    TestsListener::theInstance().messagesLog() << message << std::endl;
  }


  void TestFixtureCommon::logErr(String message)
  {
    TestsListener::theInstance().errorsLog() << message << std::endl;
    /*new Throwable().printStackTrace();*/
  }


  String TestFixtureCommon::randomString()
  {
    srand( (unsigned)time( NULL ) );

    int     length= (rand()%32);
    String  buf, ch("a");

    buf.resize(length);

    for (int i = 0; i < length; i++)
    {
      ch.assign( static_cast<String::size_type>(1), 'a' + rand()%26 );
      buf.append( ch );
    }

    return buf;
  }


  /** value_object methods */

  value_object::value_object( const sql::ResultSet * rs, int colNum )
          : asString    ( rs->getString( colNum ) )
  {
    wasNull = rs->wasNull();
  }


  int value_object::intValue() const
  {
    return StringUtils::toInt( asString, wasNull );
  }


  float value_object::floatValue() const
  {
    return StringUtils::toFloat( asString, wasNull );
  }


  long long value_object::longValue() const
  {
    return StringUtils::toLong( asString, wasNull );
  }


  value_object * getObject( sql::ResultSet * rs, int colNum )
  {
    return new value_object( rs, colNum );
  }


  /* few temporary hacks*/
  bool value_object::instanceof( value_type type )
  {
    if ( type < vtLast )
    {
      switch ( type )
      {
      case vtByte   : return true;
      case vtFloat  :  
      case vtDouble :
        {
          long long llValue = longValue();

          if (llValue != 0 )
          {
            double dValue = doubleValue();
            return ( dValue > llValue ? dValue - llValue : llValue - dValue ) > 0.001;
          }
        }

      default: return false;
      }

    }

    return false;
  }
}

