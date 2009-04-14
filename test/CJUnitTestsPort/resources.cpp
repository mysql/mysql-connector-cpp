/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include <fstream>

#include "resources.h"
#include "../common/stringutils.h"

namespace testsuite
{
namespace resources
{

void CharsetMapping::Init()
{
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("big5"), 2U));

  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("dec8"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("cp850"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("hp8"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("koi8r"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("latin1"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("latin2"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("swe7"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("ascii"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("ujis"), 3U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("sjis"), 2U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("hebrew"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("tis620"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("euckr"), 2U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("koi8u"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("gb2312"), 2U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("greek"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("cp1250"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("gbk"), 2U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("latin5"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("armscii8"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("utf8"), 3U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("ucs2"), 2U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("cp866"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("keybcs2"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("macce"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("macroman"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("cp852"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("latin7"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("cp1251"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("cp1256"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("cp1257"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("binary"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("geostd8"), 1U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("cp932"), 2U));
  STATIC_CHARSET_TO_NUM_BYTES_MAP.insert(
                                         std::map<String, unsigned int>::value_type(_T("eucjpms"), 3U));
}

const CharsetMapping & CharsetMapping::Instance()
{
  static CharsetMapping instance;
  return instance;
}

bool OpenFile(std::ifstream & fileStream, const String & fileName
              , const char * _possibleLocations[])
{
  fileStream.open(fileName.c_str());

  int i=0;

  while (!fileStream.is_open()
         && _possibleLocations != NULL && _possibleLocations[ i ] != NULL)
  {
    fileStream.clear();
    fileStream.open((String(_possibleLocations[ i ]) + "/" + fileName).c_str());
    ++i;
  }

  return fileStream.is_open();
}

int LoadProperties(const String & fileName, Properties & props
                   , const char * _possibleLocations[])
{
  int counter=0;

  std::ifstream propsFile;

  if (OpenFile(propsFile, fileName, _possibleLocations))
  {
    String line;
    while (getline(propsFile, line))
    {
      StringUtils::trim(line);

      // Not empty line or a comment
      if (!propsFile.eof() && line.size() > 0 && line.c_str()[0] != '#')
      {
        String::size_type pos=line.find_first_of("=");

        if (pos != String::npos && pos > 0)
        {
          String key=line.substr(0, pos);
          String val=line.substr(pos + 1);

          StringUtils::trim( key );
          StringUtils::trim( val );

          props.insert(Properties::value_type(key, val));
          ++counter;
        }

      }
    }

    propsFile.close();
  }
  else
  {
    std::cout << "Unable to open file" << std::endl;
    return -1;
  }

  return counter;
}
}
}
