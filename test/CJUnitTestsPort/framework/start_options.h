/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.

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
#include <stdlib.h>

namespace testsuite
{

struct TestsFilter
{
  List filter;

  TestsFilter()
  {

  }

  TestsFilter(String & filterString)
  {
  }

  bool Admits(String & testName)
  {
    return true;
  }
};

enum UnnamedParameterOrder
{
  poFirst=-1, poUrl, poUser, poPasswd, poSchema, poLast
};

struct StartOptions
{
  bool verbose;
  bool verbose_summary;
  TestsFilter filter;

  bool timer;

  /* TODO - Lawrin, please fix my hack */
  bool use_is;

  String dbUrl;
  String dbUser;
  String dbPasswd;
  String dbSchema;

  StartOptions()
  : verbose(false), verbose_summary(false)

  {
  }

  StartOptions(int paramsNumber, char** paramsValues)
  : verbose(false), verbose_summary(false)
  {
    String * _param[ poLast - poFirst - 1 ]={&dbUrl, &dbUser, &dbPasswd, &dbSchema};
    use_is = true;
    if (paramsNumber > 1)
    {
      UnnamedParameterOrder curParam=poFirst;
      while (--paramsNumber)
      {
        ciString param(*(++paramsValues));

        if (param.substr(0, 2) == "--")
        {
          if (param.substr(0, sizeof("--verbose") - 1) == "--verbose")
          {
            verbose=true;
            verbose_summary=true;

            size_t switch_pos;

            if ((switch_pos=param.find_last_of("=", std::string::npos)) != std::string::npos)
            {
              ++switch_pos;

              if (param.length() > switch_pos)
              {
                verbose_summary=false;
              }
            }
          }
          else if ( param == "--timer"  )
          {
            timer= true;
          }
          else if ( param == "--dont-use-is"  )
          {
            use_is= false;
          }

        }
        else if ((curParam + 1) < poLast)
        {
          curParam=static_cast<UnnamedParameterOrder> (curParam + 1);
          *_param[ curParam ]=String(param.c_str());
        }
      }
    }

  }
};

} // namespace testsuite
#endif  // __TEST_FRAMEWORK_START_OPTIONS
