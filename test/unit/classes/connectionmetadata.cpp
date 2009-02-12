/*
Copyright 2008 - 2009 Sun Microsystems, Inc.

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

#include <cppconn/warning.h>

#include "connectionmetadata.h"
#include <sstream>
#include <stdlib.h>
#include <cppconn/resultset.h>
#include <cppconn/datatype.h>

namespace testsuite
{
namespace classes
{

void connectionmetadata::getSchemata()
{
  logMsg("connectionmetadata::getSchemata() - MySQL_ConnectionMetaData::getSchemata");
  bool schema_found=false;
  std::stringstream msg;
  try
  {
    DatabaseMetaData dbmeta(con->getMetaData());
    ResultSet resdbm1(dbmeta->getSchemata());
    ResultSet resdbm2(dbmeta->getSchemaObjects(con->getCatalog(), "", "schema"));
    logMsg("... checking if getSchemata() and getSchemaObjects() report the same schematas");

    resdbm1->beforeFirst();
    while (resdbm1->next())
    {

      schema_found=false;
      resdbm2->beforeFirst();
      while (resdbm2->next())
        if (resdbm2->getString("SCHEMA") == resdbm1->getString(1))
        {
          schema_found=true;
          break;
        }

      if (!schema_found)
        FAIL("Schemata lists differ");

      msg.str("");
      msg << "... OK " << resdbm1->getString(1) << " = " << resdbm2->getString("SCHEMA");
      logMsg(msg.str());
    }
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getAttributes()
{
  logMsg("connectionmetadata::getAttributes() - MySQL_ConnectionMetaData::getAttributes");
  unsigned int i;
  std::vector<udtattribute>::iterator it;
  std::stringstream msg;
  try
  {
    DatabaseMetaData dbmeta(con->getMetaData());
    ResultSet res(dbmeta->getAttributes(con->getCatalog(), con->getSchema(), "", ""));
    ResultSetMetaData resmeta(res->getMetaData());

    it=attributes.begin();
    for (i=1; i <= resmeta->getColumnCount(); i++)
    {
      if (it == attributes.end())
        FAIL("There are more columns than expected");

      ASSERT_EQUALS(it->name, resmeta->getColumnName(i));
      msg.str("");
      msg << "... OK found column " << it->name;
      logMsg(msg.str());

      it++;
    }
    if (it != attributes.end())
      FAIL("There are less columns than expected");

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getBestRowIdentifier()
{
  logMsg("connectionmetadata::getBestRowIdentifier() - MySQL_ConnectionMetaData::getBestRowIdentifier");
  std::vector<columndefinition>::iterator it;
  std::stringstream msg;
  bool got_warning = false;
  try
  {
    DatabaseMetaData dbmeta(con->getMetaData());
    stmt.reset(con->createStatement());

    logMsg("... looping over all kinds of column types");
    for (it=columns.begin(); it != columns.end(); it++)
    {
      stmt->execute("DROP TABLE IF EXISTS test");
      msg.str("");
      msg << "CREATE TABLE test(id " << it->sqldef << ", PRIMARY KEY(id))";
      try
      {
        stmt->execute(msg.str());
      }
      catch (sql::SQLException &e)
      {
        msg.str("");
        msg << "... skipping " << it->sqldef;
        logMsg(msg.str());
        continue;
      }
      res.reset(dbmeta->getBestRowIdentifier(con->getCatalog(), con->getSchema(), "test", 0, false));
      ASSERT_EQUALS(true, res->next());
      ASSERT_EQUALS(sql::DatabaseMetaData::bestRowSession, res->getInt(1));
      ASSERT_EQUALS(res->getInt(1), res->getInt("SCOPE"));
      ASSERT_EQUALS("id", res->getString(2));
      ASSERT_EQUALS(res->getInt(2), res->getInt("COLUMN_NAME"));

      if (it->ctype != res->getInt(3))
      {
        msg.str("");
        msg << "... WARNING - check DATA_TYPE for " << it->sqldef;
        msg << " - expecting type " << it->ctype << " got " << res->getInt(3);
        logMsg(msg.str());
        got_warning = true;
      }
      // TODO - ASSERT_EQUALS(it->ctype, res->getInt(3));
      ASSERT_EQUALS(res->getInt(3), res->getInt("DATA_TYPE"));

      if (it->name != res->getString(4))
      {
        msg.str("");
        msg << "... WARNING - check DATA_TYPE for " << it->sqldef;
        msg << " - expecting precision " << it->name << " got " << res->getString(4);
        logMsg(msg.str());
        got_warning = true;
      }
      // TODO - ASSERT_EQUALS(it->name, res->getString(4));
      ASSERT_EQUALS(res->getString(4), res->getString("TYPE_NAME"));

      if (it->precision != res->getInt(5))
      {
        msg.str("");
        msg << "... WARNING - check COLUMN_SIZE for " << it->sqldef;
        msg << " - expecting precision " << it->precision << " got " << res->getInt(5);
        logMsg(msg.str());
        got_warning = true;
      }
      // TODO - ASSERT_EQUALS(it->precision, res->getInt(5));
      ASSERT_EQUALS(res->getInt(5), res->getInt("COLUMN_SIZE"));

      ASSERT_EQUALS(0, res->getInt(6));
      ASSERT_EQUALS(res->getInt(6), res->getInt("BUFFER_LENGTH"));      
      ASSERT_EQUALS(it->decimal_digits, res->getInt(7));
      ASSERT_EQUALS(res->getInt(7), res->getInt("DECIMAL_DIGITS"));
      ASSERT_EQUALS(sql::DatabaseMetaData::bestRowNotPseudo, res->getInt(8));
      ASSERT_EQUALS(res->getInt(8), res->getInt("PSEUDO_COLUMN"));

      stmt->execute("DROP TABLE IF EXISTS test");
    }
    if (got_warning)
      FAIL("See Warnings!");


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

} /* namespace connectionmetadata */
} /* namespace testsuite */
