/*
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
