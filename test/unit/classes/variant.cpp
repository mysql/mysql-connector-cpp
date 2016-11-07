/*
Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.

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



#include <cppconn/variant.h>
#include <cppconn/connection.h>

#include "variant.h"
#include <stdlib.h>
#include <cppconn/exception.h>


namespace testsuite
{
namespace classes
{

void variant::getValue()
{
  logMsg("variant::getValue()");

  sql::ConnectOptionsMap connection_properties;

  /* Test for integer variant values */
  try
  {
  int val= 1234;
  connection_properties["first"]= val;
  ASSERT_EQUALS(*(connection_properties["first"].get< int >()), 1234);

  val= 4567;
  connection_properties["second"]= val;

  ASSERT_EQUALS(*(connection_properties["first"].get< int >()), 1234);
  ASSERT_EQUALS(*(connection_properties["second"].get< int >()), 4567);
  }
  catch (sql::SQLException &e)
  {
  logErr(e.what());
  logErr("SQLState: " + std::string(e.getSQLState()));
  fail(e.what(), __FILE__, __LINE__);
  }

  connection_properties.erase("first");
  connection_properties.erase("second");

  /* Test for bool variant values */
  try
  {
  bool val= 1;
  connection_properties["first"]= val;
  ASSERT_EQUALS((bool)*(connection_properties["first"].get< bool >()), (bool)1);

  val= 0;
  connection_properties["second"]= val;

  ASSERT_EQUALS((bool)*(connection_properties["first"].get< bool >()), (bool)1);
  ASSERT_EQUALS((bool)*(connection_properties["second"].get< bool >()), (bool)0);
  }
  catch (sql::SQLException &e)
  {
  logErr(e.what());
  logErr("SQLState: " + std::string(e.getSQLState()));
  fail(e.what(), __FILE__, __LINE__);
  }

  connection_properties.erase("first");
  connection_properties.erase("second");

  /* Test for std::string variant values */
  try
  {
  std::string val= "value1";
  connection_properties["first"]= val;
  ASSERT_EQUALS(*(connection_properties["first"].get< std::string >()), "value1");

  val= "another_val";
  connection_properties["second"]= val;

  ASSERT_EQUALS(*(connection_properties["first"].get< std::string >()), "value1");
  ASSERT_EQUALS(*(connection_properties["second"].get< std::string >()), "another_val");
  }
  catch (sql::SQLException &e)
  {
  logErr(e.what());
  logErr("SQLState: " + std::string(e.getSQLState()));
  fail(e.what(), __FILE__, __LINE__);
  }

  connection_properties.erase("first");
  connection_properties.erase("second");

  /* Test for sql::SQLString variant values */
  try
  {
  sql::SQLString val= "value1";
  connection_properties["first"]= val;
  ASSERT_EQUALS(*(connection_properties["first"].get< sql::SQLString >()), "value1");

  val= "another_val";
  connection_properties["second"]= val;

  ASSERT_EQUALS(*(connection_properties["first"].get< sql::SQLString >()), "value1");
  ASSERT_EQUALS(*(connection_properties["second"].get< sql::SQLString >()), "another_val");
  }
  catch (sql::SQLException &e)
  {
  logErr(e.what());
  logErr("SQLState: " + std::string(e.getSQLState()));
  fail(e.what(), __FILE__, __LINE__);
  }

  connection_properties.erase("first");
  connection_properties.erase("second");

  /* Test for std::map< std::string, std::string > variant values */
  try
  {
  std::string value1 ("value1");
  std::string value2 ("value2");
  std::map< std::string, std::string > connectAttrMap;

  connectAttrMap["key1"] = value1;
  connectAttrMap["key2"] = value2;
  connection_properties["first"]= connectAttrMap;

  ASSERT_EQUALS("value1", (*(connection_properties["first"].get< std::map < std::string, std::string > >()))["key1"]);
  ASSERT_EQUALS("value2", (*(connection_properties["first"].get< std::map < std::string, std::string > >()))["key2"]);

  value1 = "value3";
  value2 = "value4";
  connectAttrMap["key1"] = value1;
  connectAttrMap["key2"] = value2;
  connection_properties["second"]= connectAttrMap;

  ASSERT_EQUALS("value1", (*(connection_properties["first"].get< std::map < std::string, std::string > >()))["key1"]);
  ASSERT_EQUALS("value2", (*(connection_properties["first"].get< std::map < std::string, std::string > >()))["key2"]);
  ASSERT_EQUALS("value3", (*(connection_properties["second"].get< std::map < std::string, std::string > >()))["key1"]);
  ASSERT_EQUALS("value4", (*(connection_properties["second"].get< std::map < std::string, std::string > >()))["key2"]);
  }
  catch (sql::SQLException &e)
  {
  logErr(e.what());
  logErr("SQLState: " + std::string(e.getSQLState()));
  fail(e.what(), __FILE__, __LINE__);
  }

  connection_properties.erase("first");
  connection_properties.erase("second");

  /* Test for std::map< sql::SQLString, sql::SQLString > variant values */
  try
  {
  sql::SQLString value1 ("value1");
  sql::SQLString value2 ("value2");
  std::map< sql::SQLString, sql::SQLString > connectAttrMap;

  connectAttrMap["key1"] = value1;
  connectAttrMap["key2"] = value2;
  connection_properties["first"]= connectAttrMap;

  ASSERT_EQUALS("value1", (*(connection_properties["first"].get< std::map < sql::SQLString, sql::SQLString > >()))["key1"]);
  ASSERT_EQUALS("value2", (*(connection_properties["first"].get< std::map < sql::SQLString, sql::SQLString > >()))["key2"]);

  value1 = "value3";
  value2 = "value4";
  connectAttrMap["key1"] = value1;
  connectAttrMap["key2"] = value2;
  connection_properties["second"]= connectAttrMap;

  ASSERT_EQUALS("value1", (*(connection_properties["first"].get< std::map < sql::SQLString, sql::SQLString > >()))["key1"]);
  ASSERT_EQUALS("value2", (*(connection_properties["first"].get< std::map < sql::SQLString, sql::SQLString > >()))["key2"]);
  ASSERT_EQUALS("value3", (*(connection_properties["second"].get< std::map < sql::SQLString, sql::SQLString > >()))["key1"]);
  ASSERT_EQUALS("value4", (*(connection_properties["second"].get< std::map < sql::SQLString, sql::SQLString > >()))["key2"]);
  }
  catch (sql::SQLException &e)
  {
  logErr(e.what());
  logErr("SQLState: " + std::string(e.getSQLState()));
  fail(e.what(), __FILE__, __LINE__);
  }
  connection_properties.erase("first");
  connection_properties.erase("second");

}


void variant::getUsingWrongStringType()
{
  logMsg("variant::getValue()");

  sql::ConnectOptionsMap connection_properties;

  /* Set std::string value and get sql::SQLString value */
  try
  {
  std::string val= "value1";
  connection_properties["first"]= val;
  ASSERT_EQUALS(*(connection_properties["first"].get< sql::SQLString >()), "value1");
  }
  catch (sql::SQLException &e)
  {
  logErr(e.what());
  logErr("SQLState: " + std::string(e.getSQLState()));
  fail(e.what(), __FILE__, __LINE__);
  }
  connection_properties.erase("first");

  /* Set sql::SQLString value and get using std::string value */
  try
  {
  sql::SQLString val= "value1";
  connection_properties["first"]= val;
  ASSERT_EQUALS(*(connection_properties["first"].get< std::string >()), "value1");
  }
  catch (sql::SQLException &e)
  {
  logErr(e.what());
  logErr("SQLState: " + std::string(e.getSQLState()));
  fail(e.what(), __FILE__, __LINE__);
  }
  connection_properties.erase("first");

  /*
  Set std::map< std::string, std::string > value
  and get std::map < sql::SQLString, sql::SQLString > value
  */
  try
  {
  std::string value1 ("value1");
  std::string value2 ("value2");
  std::map< std::string, std::string > connectAttrMap;

  connectAttrMap["key1"] = value1;
  connectAttrMap["key2"] = value2;
  connection_properties["first"]= connectAttrMap;

  ASSERT_EQUALS("value1", (*(connection_properties["first"].get< std::map < sql::SQLString, sql::SQLString > >()))["key1"]);
  ASSERT_EQUALS("value2", (*(connection_properties["first"].get< std::map < sql::SQLString, sql::SQLString > >()))["key2"]);
  }
  catch (sql::SQLException &e)
  {
  logErr(e.what());
  logErr("SQLState: " + std::string(e.getSQLState()));
  fail(e.what(), __FILE__, __LINE__);
  }
  connection_properties.erase("first");

  /*
  Set std::map < sql::SQLString, sql::SQLString > value
  and get std::map< std::string, std::string > value
  */
  try
  {
  sql::SQLString value1 ("value1");
  sql::SQLString value2 ("value2");
  std::map< sql::SQLString, sql::SQLString > connectAttrMap;

  connectAttrMap["key1"] = value1;
  connectAttrMap["key2"] = value2;
  connection_properties["first"]= connectAttrMap;

  ASSERT_EQUALS("value1", (*(connection_properties["first"].get< std::map < std::string, std::string > >()))["key1"]);
  ASSERT_EQUALS("value2", (*(connection_properties["first"].get< std::map < std::string, std::string > >()))["key2"]);
  }
  catch (sql::SQLException &e)
  {
  logErr(e.what());
  logErr("SQLState: " + std::string(e.getSQLState()));
  fail(e.what(), __FILE__, __LINE__);
  }
  connection_properties.erase("first");

}

void variant::getUsingWrongDatatype()
{
  logMsg("variant::getUsingWrongDatatype()");

  sql::ConnectOptionsMap connection_properties;

  try
  {
  try
  {
    std::string user ("mysql_user");
    connection_properties["user"]= user;

    const int *val= connection_properties["user"].get< int >();
    val = NULL;

    FAIL("No exception II");
  }
  catch (sql::InvalidArgumentException)
  {
  /* expected */
  }
  }
  catch (sql::SQLException &e)
  {
  logErr(e.what());
  logErr("SQLState: " + std::string(e.getSQLState()));
  fail(e.what(), __FILE__, __LINE__);
  }

}

} /* namespace classes   */
} /* namespace testsuite */
