/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates.
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



#include <cppconn/warning.h>

#include "connectionmetadata.h"
#include <sstream>
#include <stdlib.h>
#include <cppconn/resultset.h>
#include <cppconn/datatype.h>
#include <cppconn/connection.h>
#include <cppconn/metadata.h>

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
    DatabaseMetaData * dbmeta=con->getMetaData();
    ResultSet resdbm1(dbmeta->getSchemata());
    checkResultSetScrolling(resdbm1);
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
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getSchemaObjects()
{
  logMsg("connectionmetadata::getSchemaObject() - MySQL_ConnectionMetaData::getSchemaObjects");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    ResultSet resdbm1(dbmeta->getSchemaObjects());
    checkResultSetScrolling(resdbm1);
    ResultSet resdbm2;
    while (resdbm1->next())
    {
      resdbm2.reset(dbmeta->getSchemaObjects(con->getCatalog(), "", resdbm1->getString(1)));
      checkResultSetScrolling(resdbm2);
    }

    //And now sys
    resdbm1.reset(dbmeta->getSchemaObjects("", "sys"));
    checkResultSetScrolling(resdbm1);
    while (resdbm1->next())
    {
      resdbm2.reset(dbmeta->getSchemaObjects(con->getCatalog(), "", resdbm1->getString(1)));
      checkResultSetScrolling(resdbm2);
    }
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
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
    DatabaseMetaData * dbmeta=con->getMetaData();
    res.reset(dbmeta->getAttributes(con->getCatalog(), con->getSchema(), "", ""));
    checkResultSetScrolling(res);
    ResultSetMetaData * resmeta=res->getMetaData();
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

    res.reset(dbmeta->getAttributes("ABC", "DEF", "GHI", "JKL"));
    ASSERT(!res->next());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getBestRowIdentifier()
{
  logMsg("connectionmetadata::getBestRowIdentifier() - MySQL_ConnectionMetaData::getBestRowIdentifier");
  std::vector<columndefinition>::iterator it;
  std::stringstream msg;
  bool got_warning=false;
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
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
      catch (sql::SQLException &)
      {
        msg.str("");
        msg << "... skipping " << it->sqldef;
        logMsg(msg.str());
        continue;
      }
      res.reset(dbmeta->getBestRowIdentifier(con->getCatalog(), con->getSchema(), "test", 0, false));
      checkResultSetScrolling(res);
      ASSERT_EQUALS(true, res->next());
      ASSERT_EQUALS(DatabaseMetaData::bestRowSession, res->getInt(1));
      ASSERT_EQUALS(res->getInt(1), res->getInt("SCOPE"));
      ASSERT_EQUALS("id", res->getString(2));
      ASSERT_EQUALS(res->getInt(2), res->getInt("COLUMN_NAME"));

      if (it->ctype != res->getInt(3))
      {
        msg.str("");
        msg << "... \t\tWARNING - check DATA_TYPE for " << it->sqldef;
        msg << " - expecting type " << it->ctype << " got " << res->getInt(3);
        logMsg(msg.str());

        ResultSet cres(stmt->executeQuery("SHOW CREATE TABLE test"));
        cres->next();
        logMsg(cres->getString(2).c_str());

        got_warning=true;
      }
      // TODO - ASSERT_EQUALS(it->ctype, res->getInt(3));
      ASSERT_EQUALS(res->getInt(3), res->getInt("DATA_TYPE"));

      if (it->name != res->getString(4))
      {
        msg.str("");
        msg << "... \t\tWARNING - check DATA_TYPE for " << it->sqldef;
        msg << " - expecting type name " << it->name << " got " << res->getString(4);
        logMsg(msg.str());
        got_warning=true;
      }
      // TODO - ASSERT_EQUALS(it->name, res->getString(4));
      ASSERT_EQUALS(res->getString(4), res->getString("TYPE_NAME"));

      if (it->precision != res->getUInt64(5))
      {
        msg.str("");
        msg << "... \t\tWARNING - check COLUMN_SIZE for " << it->sqldef;
        msg << " - expecting pecision " << it->precision << " got " << res->getInt(5);
        logMsg(msg.str());
        got_warning=true;
      }
      // TODO - ASSERT_EQUALS(it->precision, res->getInt(5));
      ASSERT_EQUALS(res->getInt(5), res->getInt("COLUMN_SIZE"));

      ASSERT_EQUALS(0, res->getInt(6));
      ASSERT_EQUALS(res->getInt(6), res->getInt("BUFFER_LENGTH"));

      if (it->decimal_digits != res->getInt(7))
      {
        msg.str("");
        msg << "... \t\tWARNING - check DECIMAL_DIGITS for " << it->sqldef;
        msg << " - expecting decimal digits = " << it->decimal_digits << " got " << res->getInt(7);
        logMsg(msg.str());
        got_warning=true;
      }
      ASSERT_EQUALS(res->getInt(7), res->getInt("DECIMAL_DIGITS"));

      ASSERT_EQUALS(DatabaseMetaData::bestRowNotPseudo, res->getInt(8));
      ASSERT_EQUALS(res->getInt(8), res->getInt("PSEUDO_COLUMN"));

      stmt->execute("DROP TABLE IF EXISTS test");
    }
    if (got_warning)
    {
      FAIL("See Warnings!");
    }

    stmt->execute("DROP TABLE IF EXISTS test");
    // TODO - stmt->execute("CREATE TABLE test(col1 INT NOT NULL, col2 INT NOT NULL, PRIMARY KEY(col1, col2))");

    res.reset(dbmeta->getBestRowIdentifier(con->getCatalog(), con->getSchema(), "test", 0, false));
    ASSERT(!res->next());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getColumnPrivileges()
{
  logMsg("connectionmetadata::getColumnPrivileges() - MySQL_ConnectionMetaData::getColumnPrivileges");

  int rows=0;
  bool got_warning=false;
  std::stringstream msg;
  try
  {

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(col1 INT, col2 INT)");
    DatabaseMetaData * dbmeta=con->getMetaData();

    res.reset(dbmeta->getColumnPrivileges(con->getCatalog(), con->getSchema(), "test", "id"));
    ASSERT_EQUALS(false, res->next());

    res.reset(dbmeta->getColumnPrivileges(con->getCatalog(), con->getSchema(), "test", "col%"));
    checkResultSetScrolling(res);
    rows=0;
    while (res->next())
    {
      rows++;

      if (con->getCatalog() != "" && res->getString(1) != "" && con->getCatalog() != res->getString(1))
      {
        got_warning=true;
        msg.str("");
        msg << "... TABLE_CAT: expecting '" << con->getCatalog() << "' got ";
        msg << "'" << res->getString(1) << "'";
        logMsg(msg.str());
      }
      ASSERT_EQUALS(res->getString(1), res->getString("TABLE_CAT"));

      ASSERT_EQUALS(con->getSchema(), res->getString(2));
      ASSERT_EQUALS(res->getString(2), res->getString("TABLE_SCHEM"));
      ASSERT_EQUALS("test", res->getString(3));

      ASSERT_EQUALS(res->getString(3), res->getString("TABLE_NAME"));
      ASSERT_EQUALS(res->getString(4), res->getString("COLUMN_NAME"));
      ASSERT_EQUALS("", res->getString(5));
      ASSERT_EQUALS(res->getString(5), res->getString("GRANTOR"));
      ASSERT_EQUALS(res->getString(6), res->getString("GRANTEE"));
      ASSERT_EQUALS(res->getString(7), res->getString("PRIVILEGE"));
      ASSERT_EQUALS(res->getString(8), res->getString("IS_GRANTABLE"));
      if (("NO" != res->getString(8)) && ("YES" != res->getString(8)) && ("" != res->getString(8)))
      {
        // Let's be optimistic that  the column does not hold this exact value...
        ASSERT_EQUALS("Any of 'YES', 'NO' and empty string ''", res->getString(8));
      }

    }
    ASSERT_GT(2, rows);
    if (got_warning)
    {
      TODO("See --verbose warnings");
    }

    res.reset(dbmeta->getColumnPrivileges(con->getCatalog(), con->getSchema(), "test", "col2"));
    ASSERT_EQUALS(true, res->next());
    ASSERT_EQUALS("col2", res->getString("COLUMN_NAME"));
    ASSERT_EQUALS(res->getString(4), res->getString("COLUMN_NAME"));

    stmt->execute("DROP TABLE IF EXISTS test");

    res.reset(dbmeta->getColumnPrivileges(con->getCatalog(), con->getSchema(), "test", "col2"));
    ASSERT(!res->next());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  if (got_warning)
  {

    FAIL("TODO - See --verbose warnings");
  }
}

void connectionmetadata::getColumns()
{
  logMsg("connectionmetadata::getColumn() - MySQL_ConnectionMetaData::getColumns");

  if (getMySQLVersion(con) < 80000)
  {
    SKIP("Due to changes on the VARBINARY, this test is disabled.");
    return;
  }

  std::vector<columndefinition>::iterator it;
  std::stringstream msg;
  bool got_warning=false;
  bool got_todo_warning=false;
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    stmt.reset(con->createStatement());
    bool isVer6=dbmeta->getDatabaseMajorVersion() == 6;

    logMsg("... looping over all kinds of column types");
    for (it=columns.begin(); it != columns.end(); it++)
    {
      stmt->execute("DROP TABLE IF EXISTS test");
      msg.str("");
      msg << "CREATE TABLE test(dummy TIMESTAMP, id " << it->sqldef << ")";
      try
      {
        stmt->execute(msg.str());

        msg.str("");
        msg << "... testing " << it->sqldef;
        logMsg(msg.str());
      }
      catch (sql::SQLException &)
      {
        msg.str("");
        msg << "... skipping " << it->sqldef;
        logMsg(msg.str());
        continue;
      }
      res.reset(dbmeta->getColumns(con->getCatalog(), con->getSchema(), "test", "id"));
      checkResultSetScrolling(res);
      ASSERT_EQUALS(true, res->next());
      if (con->getCatalog() != "" && res->getString(1) != "" && con->getCatalog() != res->getString("TABLE_CAT"))
      {
        got_todo_warning=true;
        (void)msg.str();  // FIXME: is it needed for anything?
        msg << "...\t\tWARNING - expecting TABLE_CAT = '" << con->getCatalog() << "'";
        msg << " got '" << res->getString("TABLE_CAT") << "'";
        logMsg(msg.str());
      }
      ASSERT_EQUALS(res->getString(1), res->getString("TABLE_CAT"));
      ASSERT_EQUALS(con->getSchema(), res->getString("TABLE_SCHEM"));
      ASSERT_EQUALS(res->getString(2), res->getString("TABLE_SCHEM"));
      ASSERT_EQUALS("test", res->getString("TABLE_NAME"));
      ASSERT_EQUALS(res->getString(3), res->getString("TABLE_NAME"));
      ASSERT_EQUALS("id", res->getString("COLUMN_NAME"));
      ASSERT_EQUALS(res->getString(4), res->getString("COLUMN_NAME"));
      if (it->ctype != res->getInt("DATA_TYPE"))
      {
        msg.str("");
        msg << "... \t\tWARNING - check DATA_TYPE for " << it->sqldef;
        msg << " - expecting type " << it->ctype << " got " << res->getInt("DATA_TYPE");
        logMsg(msg.str());
        got_warning=true;
      }
      // ASSERT_EQUALS(it->ctype, res->getInt("DATA_TYPE"));
      ASSERT_EQUALS(res->getInt(5), res->getInt("DATA_TYPE"));

      if (it->name != res->getString("TYPE_NAME"))
      {
        msg.str("");
        msg << "... \t\tWARNING - check TYPE_NAME for " << it->sqldef;
        msg << " - expecting type " << it->name << " got " << res->getString("TYPE_NAME");
        logMsg(msg.str());
        got_warning=true;
      }
      // ASSERT_EQUALS(it->name, res->getString("TYPE_NAME"));
      ASSERT_EQUALS(res->getString(6), res->getString("TYPE_NAME"));

      if (it->precision != res->getUInt64(7))
      {
        msg.str("");
        msg << "... \t\tWARNING - check COLUMN_SIZE for " << it->sqldef;
        msg << " - expecting pecision " << it->precision << " got " << res->getUInt64(7);
        logMsg(msg.str());
        got_warning=true;
      }
      ASSERT_EQUALS(res->getInt(7), res->getInt("COLUMN_SIZE"));

      ASSERT_EQUALS(0, res->getInt(8));
      ASSERT_EQUALS(res->getInt(8), res->getInt("BUFFER_LENGTH"));
      ASSERT_EQUALS(it->decimal_digits, res->getInt(9));
      ASSERT_EQUALS(res->getInt(9), res->getInt("DECIMAL_DIGITS"));
      ASSERT_EQUALS(it->num_prec_radix, res->getInt(10));
      ASSERT_EQUALS(res->getInt(10), res->getInt("NUM_PREC_RADIX"));

      if (it->nullable != res->getInt(11))
      {
        msg.str("");
        msg << "... \t\tWARNING - check NULLABLE for " << it->sqldef;
        msg << " - expecting nullable = " << it->nullable << " got " << res->getInt(11);
        msg << " columnNoNull = " << DatabaseMetaData::columnNoNulls << ", ";
        msg << " columnNullable = " << DatabaseMetaData::columnNullable << ", ";
        msg << " columnNullableUnknown = " << DatabaseMetaData::columnNullableUnknown;
        logMsg(msg.str());
        got_warning=true;
      }
      ASSERT_EQUALS(it->nullable, res->getInt(11));
      ASSERT_EQUALS(res->getInt(11), res->getInt("NULLABLE"));
      ASSERT_EQUALS(it->remarks, res->getString(12));
      ASSERT_EQUALS(res->getString(12), res->getString("REMARKS"));
      if(it->column_def != res->getString(13))
      {
        msg.str("");
        msg << "... \t\tWARNING - check COLUMN_def for " << it->sqldef;
        msg << " - expecting COLUMN_def = " << it->column_def << " got " << res->getString(13);
        logMsg(msg.str());
        got_warning=true;
      }
      ASSERT_EQUALS(it->column_def, res->getString(13));
      ASSERT_EQUALS(res->getString(13), res->getString("COLUMN_DEF"));
      ASSERT_EQUALS(res->getInt(14), res->getInt("SQL_DATA_TYPE"));
      ASSERT_EQUALS(res->getInt(15), res->getInt("SQL_DATETIME_SUB"));
      if (it->char_octet_length != 0 && (it->ctype == sql::DataType::CHAR || it->ctype == sql::DataType::VARCHAR))
      {
        size_t expected_len=it->char_octet_length;
        if ((it->ctype == sql::DataType::CHAR || it->ctype == sql::DataType::VARCHAR) &&
            isVer6 &&
            it->sqldef.find("TINYTEXT") == std::string::npos &&
            (it->sqldef.find("utf8") != std::string::npos || it->sqldef.find("NATIONAL") != std::string::npos))
        {
          expected_len=(expected_len / 3)*4;
        }
        if (res->getUInt64(16) != expected_len)
        {
          msg.str("");
          msg << "... \t\tWARNING - check CHAR_OCTET_LENGTH for " << it->sqldef;
          msg << " - expecting char_octet_length " << it->char_octet_length << " got " << res->getUInt64(16);
          logMsg(msg.str());
          got_warning=true;
        }
      }
      ASSERT_EQUALS(res->getUInt64(16), res->getUInt64("CHAR_OCTET_LENGTH"));

      ASSERT_EQUALS(2, res->getInt(17));
      ASSERT_EQUALS(res->getInt(17), res->getInt("ORDINAL_POSITION"));

      if (((it->nullable == DatabaseMetaData::columnNoNulls) && (res->getString(18) != "NO")) ||
          ((it->nullable == DatabaseMetaData::columnNullable) && (res->getString(18) != "YES")) ||
          ((it->nullable == DatabaseMetaData::columnNullableUnknown) && (res->getString(18) != "")))
      {
        msg.str("");
        msg << "... \t\tWARNING - check IS_NULLABLE for " << it->sqldef;
        msg << " - expecting nullable = " << it->nullable << " got is_nullable = '" << res->getInt(18) << "'";
        logMsg(msg.str());
        got_warning=true;
      }
      ASSERT_EQUALS(res->getString(18), res->getString("IS_NULLABLE"));
      ASSERT_EQUALS("", res->getString(19));
      ASSERT_EQUALS(res->getString(19), res->getString("SCOPE_CATALOG"));
      ASSERT_EQUALS("", res->getString(20));
      ASSERT_EQUALS(res->getString(20), res->getString("SCOPE_SCHEMA"));
      ASSERT_EQUALS("", res->getString(21));
      ASSERT_EQUALS(res->getString(21), res->getString("SCOPE_TABLE"));
      ASSERT_EQUALS("", res->getString(22));
      ASSERT_EQUALS(res->getString(22), res->getString("SOURCE_DATA_TYPE"));
      ASSERT_EQUALS(it->is_autoincrement, res->getString(23));
      ASSERT_EQUALS(res->getString(23), res->getString("IS_AUTOINCREMENT"));
      stmt->execute("DROP TABLE IF EXISTS test");
    }
    if (got_warning)
      FAIL("See --verbose warnings!");

    if (got_todo_warning)
    {
      TODO("See --verbose warnings!");
      FAIL("TODO - see --verbose warnings");
    }

    try {
      bool input_value=true;
      bool output_value=false;
      void * input;
      void * output;

      stmt->execute("CREATE TABLE test(id INT,val VARCHAR(20))");

      input=(static_cast<bool *> (&input_value));
      output=(static_cast<bool *> (&output_value));

      con->setClientOption("metadataUseInfoSchema", input);
      con->getClientOption("metadataUseInfoSchema", output);
      ASSERT_EQUALS(input_value, output_value);

      dbmeta=con->getMetaData();
      res.reset(dbmeta->getColumns(con->getCatalog(), "", "test", "%"));
      ASSERT(res->rowsCount() == 2);

      input_value=false;
      output_value=true;

      con->setClientOption("metadataUseInfoSchema", input);
      con->getClientOption("metadataUseInfoSchema", output);
      ASSERT_EQUALS(input_value, output_value);

      dbmeta=con->getMetaData();
      res.reset(dbmeta->getColumns(con->getCatalog(), "", "test", "%"));
      ASSERT(res->rowsCount() == 2);
    }
    catch (sql::SQLException &)
    {
      FAIL("getColumns() does not work properly for metadataUseInfoSchema");
    }

    stmt->execute("DROP TABLE IF EXISTS test");
    res.reset(dbmeta->getColumns(con->getCatalog(), con->getSchema(), "test", "id"));
    ASSERT(!res->next());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getConnection()
{
  logMsg("connectionmetadata::getConnection() - MySQL_ConnectionMetaData::getConnection");
  sql::Connection *same_con;
  try
  {
    stmt.reset(con->createStatement());
    stmt->execute("SET @this_is_my_connection_id=101");
    DatabaseMetaData * dbmeta=con->getMetaData();
    same_con= dbmeta->getConnection();
    stmt.reset(same_con->createStatement());
    res.reset(stmt->executeQuery("SELECT @this_is_my_connection_id AS _connection_id"));
    ASSERT(res->next());
    ASSERT_EQUALS(101, res->getInt("_connection_id"));
    ASSERT_EQUALS(res->getInt(1), res->getInt("_connection_id"));
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getDatabaseVersions()
{
  logMsg("connectionmetadata::getDatabaseVersions() - MySQL_ConnectionMetaData::getDatabase[Minor|Major|Patch]Version()");
  std::stringstream prodversion;
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    ASSERT_GT(2, dbmeta->getDatabaseMajorVersion());
    ASSERT_LT(8, dbmeta->getDatabaseMajorVersion());
    ASSERT_LT(100, dbmeta->getDatabaseMinorVersion());
    ASSERT_LT(100, dbmeta->getDatabasePatchVersion());

    ASSERT_EQUALS("MySQL", dbmeta->getDatabaseProductName());

    prodversion.str("");
    prodversion << dbmeta->getDatabaseMajorVersion() << "." << dbmeta->getDatabaseMinorVersion();
    prodversion << "." << dbmeta->getDatabasePatchVersion();
    if (prodversion.str().length() < dbmeta->getDatabaseProductVersion().length())
    {
      // Check only left prefix, database could have "-alpha" or something in its product versin
      ASSERT_EQUALS(prodversion.str(), dbmeta->getDatabaseProductVersion().substr(0, prodversion.str().length()));
    }
    else
    {
      ASSERT_EQUALS(prodversion.str(), dbmeta->getDatabaseProductVersion());
    }
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getDriverVersions()
{
  logMsg("connectionmetadata::getDriverVersions() - MySQL_ConnectionMetaData::getDriver[Minor|Major|Patch]Version()");
  std::stringstream prodversion;
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    ASSERT_GT(0, dbmeta->getDriverMajorVersion());
    ASSERT_LT(8, dbmeta->getDriverMajorVersion());
    ASSERT_LT(100, dbmeta->getDriverMinorVersion());
    ASSERT_LT(100, dbmeta->getDriverPatchVersion());

    ASSERT_EQUALS("MySQL Connector/C++", dbmeta->getDriverName());

    prodversion.str("");
    prodversion << dbmeta->getDriverMajorVersion() << "." << dbmeta->getDriverMinorVersion();
    prodversion << "." << dbmeta->getDriverPatchVersion();
    if (prodversion.str().length() < dbmeta->getDriverVersion().length())
    {
      // Check only left prefix, Driver could have "-alpha" or something in its product versin
      ASSERT_EQUALS(prodversion.str(), dbmeta->getDriverVersion().substr(0, prodversion.str().length()));
    }
    else
    {
      ASSERT_EQUALS(prodversion.str(), dbmeta->getDriverVersion());
    }
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getDefaultTransactionIsolation()
{
  logMsg("connectionmetadata::getDefaultTransactionIsolation() - MySQL_ConnectionMetaData::getDefaultTransactionIsolation()");
  int server_version;
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();

    server_version=(10000 * dbmeta->getDatabaseMajorVersion())
            + (100 * dbmeta->getDatabaseMinorVersion())
            + dbmeta->getDatabasePatchVersion();

    if (server_version < 32336)
      FAIL("Sorry guys - we do not support MySQL <5.1. This test will not handle this case.");

    ASSERT_EQUALS(sql::TRANSACTION_READ_COMMITTED, dbmeta->getDefaultTransactionIsolation());
    ASSERT(sql::TRANSACTION_NONE != dbmeta->getDefaultTransactionIsolation());
    ASSERT(sql::TRANSACTION_READ_UNCOMMITTED != dbmeta->getDefaultTransactionIsolation());
    ASSERT(sql::TRANSACTION_REPEATABLE_READ != dbmeta->getDefaultTransactionIsolation());
    ASSERT(sql::TRANSACTION_SERIALIZABLE != dbmeta->getDefaultTransactionIsolation());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getExtraNameCharacters()
{
  logMsg("connectionmetadata::getExtraNameCharacters() - MySQL_ConnectionMetaData::getExtraNameCharacters()");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    ASSERT_EQUALS("#@", dbmeta->getExtraNameCharacters());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getIdentifierQuoteString()
{
  logMsg("connectionmetadata::getIdentifierQuoteString() - MySQL_ConnectionMetaData::getIdentifierQuoteString()");

  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    stmt.reset(con->createStatement());
    try
    {
      stmt->execute("SET @@sql_mode = ''");
      res.reset(stmt->executeQuery("SELECT @@sql_mode AS _sql_mode"));
      ASSERT(res->next());
      ASSERT_EQUALS("", res->getString("_sql_mode"));
    }
    catch (sql::SQLException &)
    {
      SKIP("Cannot set SQL_MODE, skipping test");
    }

    ASSERT_EQUALS("`", dbmeta->getIdentifierQuoteString());
    stmt->execute("SET @@sql_mode = 'ANSI_QUOTES,ALLOW_INVALID_DATES'");
    res.reset(stmt->executeQuery("SELECT @@sql_mode AS _sql_mode"));
    ASSERT(res->next());
    ASSERT_EQUALS("ANSI_QUOTES,ALLOW_INVALID_DATES", res->getString("_sql_mode"));
    ASSERT_EQUALS("\"", dbmeta->getIdentifierQuoteString());

  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

}

void connectionmetadata::getImportedKeys()
{
  logMsg("connectionmetadata::getImportedKeys() - MySQL_ConnectionMetaData::getImportedKeys()");

  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    stmt.reset(con->createStatement());

    stmt->execute("DROP TABLE IF EXISTS child");
    stmt->execute("DROP TABLE IF EXISTS parent");
    try
    {
      stmt->execute("CREATE TABLE parent(pid INT NOT NULL, PRIMARY KEY(pid)) ENGINE=INNODB;");
      stmt->execute("CREATE TABLE child(cid INT NOT NULL, cpid INT, "
                    "INDEX idx_parent_id(cpid), FOREIGN KEY idx_parent_id(cpid) "
                    "REFERENCES parent(pid) ON DELETE CASCADE ON UPDATE CASCADE, PRIMARY KEY(cid)) ENGINE=INNODB;");
    }
    catch (sql::SQLException &)
    {
      SKIP("Cannot create necessary FK tables");
    }

    int num_res=0;

    res.reset(dbmeta->getImportedKeys(con->getCatalog(), con->getSchema(), "parent"));
    ASSERT(!res->next());

    res.reset(dbmeta->getImportedKeys(con->getCatalog(), con->getSchema(), "child"));
    checkResultSetScrolling(res);
    ASSERT(res->next());
    logMsg("... calling checkForeignKey for child");
    checkForeignKey(con, res);

    ASSERT(!res->next());

    stmt->execute("DROP TABLE IF EXISTS child");
    stmt->execute("DROP TABLE IF EXISTS parent");
    stmt->execute("CREATE TABLE parent(pid1 INT NOT NULL, pid2 INT NOT NULL, PRIMARY KEY(pid1, pid2)) ENGINE=INNODB;");
    stmt->execute("CREATE TABLE child(cid INT NOT NULL, cpid2 INT, cpid1 INT, "
                  "INDEX idx_parent_id(cpid1, cpid2), FOREIGN KEY idx_parent_id(cpid1, cpid2) "
                  "REFERENCES parent(pid1, pid2) ON DELETE CASCADE ON UPDATE CASCADE, PRIMARY KEY(cid)) ENGINE=INNODB;");

    res.reset(dbmeta->getImportedKeys(con->getCatalog(), con->getSchema(), "child"));
    num_res=0;
    while (res->next())
    {
      num_res++;
      switch (num_res) {
      case 1:
        ASSERT_EQUALS("cpid1", res->getString("FKCOLUMN_NAME"));
        break;
      case 2:
        ASSERT_EQUALS("cpid2", res->getString("FKCOLUMN_NAME"));
        break;
      default:
        FAIL("Expecting only two rows");
        break;
      }
    }
    ASSERT_EQUALS(2, num_res);

    stmt->execute("DROP TABLE IF EXISTS child");
    stmt->execute("DROP TABLE IF EXISTS parent");
    res.reset(dbmeta->getImportedKeys(con->getCatalog(), con->getSchema(), "child"));
    ASSERT(!res->next());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getExportedKeys()
{
  logMsg("connectionmetadata::getExportedKeys() - MySQL_ConnectionMetaData::getExportedKeys()");

  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    stmt.reset(con->createStatement());

    stmt->execute("DROP TABLE IF EXISTS child");
    stmt->execute("DROP TABLE IF EXISTS parent");
    try
    {
      stmt->execute("CREATE TABLE parent(pid INT NOT NULL, PRIMARY KEY(pid)) ENGINE=INNODB;");
      stmt->execute("CREATE TABLE child(cid INT NOT NULL, cpid INT, "
                    "INDEX idx_parent_id(cpid), FOREIGN KEY idx_parent_id(cpid) "
                    "REFERENCES parent(pid) ON DELETE CASCADE ON UPDATE CASCADE, PRIMARY KEY(cid)) ENGINE=INNODB;");
    }
    catch (sql::SQLException &)
    {
      SKIP("Cannot create necessary FK tables");
    }

    int num_res=0;

    res.reset(dbmeta->getExportedKeys(con->getCatalog(), con->getSchema(), "child"));
    ASSERT(!res->next());

    res.reset(dbmeta->getExportedKeys(con->getCatalog(), con->getSchema(), "parent"));
    ASSERT(res->next());
    logMsg("... calling checkForeignKey for parent");
    checkForeignKey(con, res);

    ASSERT(!res->next());

    stmt->execute("DROP TABLE IF EXISTS child");
    stmt->execute("DROP TABLE IF EXISTS parent");
    stmt->execute("CREATE TABLE parent(pid1 INT NOT NULL, pid2 INT NOT NULL, PRIMARY KEY(pid1, pid2)) ENGINE=INNODB;");
    stmt->execute("CREATE TABLE child(cid INT NOT NULL, cpid2 INT, cpid1 INT, "
                  "INDEX idx_parent_id(cpid1, cpid2), FOREIGN KEY idx_parent_id(cpid1, cpid2) "
                  "REFERENCES parent(pid1, pid2) ON DELETE CASCADE ON UPDATE CASCADE, PRIMARY KEY(cid)) ENGINE=INNODB;");

    res.reset(dbmeta->getExportedKeys(con->getCatalog(), con->getSchema(), "parent"));
    checkResultSetScrolling(res);
    num_res=0;
    while (res->next())
    {
      num_res++;
      switch (num_res) {
      case 1:
        ASSERT_EQUALS("cpid1", res->getString("FKCOLUMN_NAME"));
        break;
      case 2:
        ASSERT_EQUALS("cpid2", res->getString("FKCOLUMN_NAME"));
        break;
      default:
        FAIL("Expecting only two rows");
        break;
      }
    }
    ASSERT_EQUALS(2, num_res);

    stmt->execute("DROP TABLE IF EXISTS child");
    stmt->execute("DROP TABLE IF EXISTS parent");
    res.reset(dbmeta->getExportedKeys(con->getCatalog(), con->getSchema(), "child"));
    ASSERT(!res->next());
  }
  catch (sql::MethodNotImplementedException &e)
  {
    logMsg(e.what());
    SKIP("MySQL is too old, MethodNotImplementedException!");
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::checkForeignKey(Connection &mycon, ResultSet &myres)
{
  bool got_warning=false;
  std::stringstream msg;

  if (mycon->getCatalog() != "" && mycon->getCatalog() != myres->getString(1))
  {
    got_warning=true;
    msg.str("");
    msg << "... WARNING expecting PKTABLE_CAT = '" << mycon->getCatalog() << "'";
    msg << " got '" << myres->getString(1) << "'";
    logMsg(msg.str());
  }
  ASSERT_EQUALS(myres->getString(1), myres->getString("PKTABLE_CAT"));

  ASSERT_EQUALS(mycon->getSchema(), myres->getString(2));
  ASSERT_EQUALS(myres->getString(2), myres->getString("PKTABLE_SCHEM"));

  ASSERT_EQUALS("parent", myres->getString(3));
  ASSERT_EQUALS(myres->getString(3), myres->getString("PKTABLE_NAME"));

  ASSERT_EQUALS("pid", myres->getString(4));
  ASSERT_EQUALS(myres->getString(4), myres->getString("PKCOLUMN_NAME"));

  if (mycon->getCatalog() != "" && mycon->getCatalog() != myres->getString(5))
  {
    got_warning=true;
    msg.str("");
    msg << "... WARNING expecting FKTABLE_CAT = '" << mycon->getCatalog() << "'";
    msg << " got '" << myres->getString(1) << "'";
    logMsg(msg.str());
  }
  ASSERT_EQUALS(myres->getString(5), myres->getString("FKTABLE_CAT"));

  ASSERT_EQUALS(mycon->getSchema(), myres->getString(6));
  ASSERT_EQUALS(myres->getString(6), myres->getString("FKTABLE_SCHEM"));

  ASSERT_EQUALS("child", myres->getString(7));
  ASSERT_EQUALS(myres->getString(7), myres->getString("FKTABLE_NAME"));

  ASSERT_EQUALS("cpid", myres->getString(8));
  ASSERT_EQUALS(myres->getString(8), myres->getString("FKCOLUMN_NAME"));

  ASSERT_EQUALS(1, myres->getInt(9));
  ASSERT_EQUALS(myres->getInt(9), myres->getInt("KEY_SEQ"));

  ASSERT_EQUALS((int64_t) DatabaseMetaData::importedKeyCascade, myres->getInt64(10));
  ASSERT_EQUALS(myres->getInt64(10), myres->getInt64("UPDATE_RULE"));

  ASSERT(DatabaseMetaData::importedKeyNoAction != myres->getInt64(10));
  ASSERT(DatabaseMetaData::importedKeySetNull != myres->getInt64(10));
  ASSERT(DatabaseMetaData::importedKeySetDefault != myres->getInt64(10));
  ASSERT(DatabaseMetaData::importedKeyRestrict != myres->getInt64(10));

  ASSERT_EQUALS((int64_t) DatabaseMetaData::importedKeyCascade, myres->getInt64(11));
  ASSERT_EQUALS(myres->getInt64(11), myres->getInt64("DELETE_RULE"));

  ASSERT(DatabaseMetaData::importedKeyNoAction != myres->getInt64(11));
  ASSERT(DatabaseMetaData::importedKeySetNull != myres->getInt64(11));
  ASSERT(DatabaseMetaData::importedKeySetDefault != myres->getInt64(11));
  ASSERT(DatabaseMetaData::importedKeyRestrict != myres->getInt64(11));

  // InnoDB should give the FK a name
  ASSERT("" != myres->getString("FK_NAME"));
  ASSERT_EQUALS(myres->getString(12), myres->getString("FK_NAME"));

  // TODO - not sure what value to expect
  ASSERT_EQUALS("PRIMARY", myres->getString("PK_NAME"));
  ASSERT_EQUALS(myres->getString(13), myres->getString("PK_NAME"));

  ASSERT_EQUALS((int64_t) DatabaseMetaData::importedKeyNotDeferrable, myres->getInt64(14));
  ASSERT(DatabaseMetaData::importedKeyInitiallyDeferred != myres->getInt64(10));
  ASSERT(DatabaseMetaData::importedKeyInitiallyImmediate != myres->getInt64(10));

  if (got_warning)
  {

    TODO("See --verbose warnings!");
    FAIL("TODO - See --verbose warnings!");
  }

}

void connectionmetadata::getIndexInfo()
{
  logMsg("connectionmetadata::getIndexInfo() - MySQL_ConnectionMetaData::getIndexInfo()");
  std::stringstream msg;
  bool got_warning=false;
  bool got_todo_warning=false;
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(col1 INT NOT NULL, col2 INT NOT NULL, col3 INT NOT NULL, col4 INT, col5 INT, PRIMARY KEY(col1))");
    stmt->execute("INSERT INTO test(col1, col2, col3) VALUES (1, 1, 1)");
    res.reset(dbmeta->getIndexInfo(con->getCatalog(), con->getSchema(), "test", false, false));
    ASSERT(res->next());
    if (con->getCatalog() != "" && res->getString(1) != "" && con->getCatalog() != res->getString(1))
    {
      got_todo_warning=true;
      msg.str("");
      msg << "...\t\tWARNING expecting TABLE_CAT = '" << con->getCatalog() << "'";
      msg << " got '" << res->getString(1) << "'";
      logMsg(msg.str());
    }
    ASSERT_EQUALS(res->getString(1), res->getString("TABLE_CAT"));
    ASSERT_EQUALS(con->getSchema(), res->getString(2));
    ASSERT_EQUALS(res->getString(2), res->getString("TABLE_SCHEM"));
    ASSERT_EQUALS("test", res->getString(3));
    ASSERT_EQUALS(res->getString(3), res->getString("TABLE_NAME"));
    ASSERT_EQUALS(false, res->getBoolean("NON_UNIQUE"));
    ASSERT_EQUALS(res->getBoolean(4), res->getBoolean("NON_UNIQUE"));
    ASSERT_EQUALS(res->getString("TABLE_SCHEM"), res->getString(5));
    ASSERT_EQUALS(res->getString(5), res->getString("INDEX_QUALIFIER"));
    ASSERT_EQUALS("PRIMARY", res->getString(6));
    ASSERT_EQUALS(res->getString(6), res->getString("INDEX_NAME"));
    ASSERT_EQUALS(DatabaseMetaData::tableIndexOther, res->getInt(7));
    ASSERT_EQUALS(res->getInt(7), res->getInt("TYPE"));
    ASSERT(DatabaseMetaData::tableIndexStatistic != res->getInt(7));
    ASSERT(DatabaseMetaData::tableIndexClustered != res->getInt(7));
    ASSERT(DatabaseMetaData::tableIndexHashed != res->getInt(7));
    ASSERT_EQUALS(1, res->getInt(8));
    ASSERT_EQUALS(res->getInt(8), res->getInt("ORDINAL_POSITION"));
    ASSERT_EQUALS("col1", res->getString(9));
    ASSERT_EQUALS(res->getString(9), res->getString("COLUMN_NAME"));
    ASSERT_EQUALS("A", res->getString(10));
    ASSERT_EQUALS(res->getString(10), res->getString("ASC_OR_DESC"));
    if (res->getInt(11) != 1)
    {
      got_warning=true;
      msg.str("");
      msg << "... \t\tWARNING: There is one row in the table and PK should have a ";
      msg << "cardinality of 1, got " << res->getInt(11);
      logMsg(msg.str());
    }

    ASSERT_EQUALS(res->getInt(11), res->getInt("CARDINALITY"));
    ASSERT_EQUALS(0, res->getInt(12));
    ASSERT_EQUALS(res->getInt(12), res->getInt("PAGES"));
    ASSERT_EQUALS("", res->getString(13));
    ASSERT_EQUALS(res->getString(13), res->getString("FILTER_CONDITION"));
    ASSERT(!res->next());

    // New unique index
    stmt->execute("CREATE UNIQUE INDEX an_idx_col3 ON test(col3 ASC)");
    res.reset(dbmeta->getIndexInfo(con->getCatalog(), con->getSchema(), "test", false, false));
    checkResultSetScrolling(res);
    ASSERT(res->next());
    ASSERT_EQUALS("an_idx_col3", res->getString("INDEX_NAME"));
    ASSERT_EQUALS(false, res->getBoolean("NON_UNIQUE"));
    ASSERT(res->next());
    ASSERT_EQUALS("PRIMARY", res->getString("INDEX_NAME"));
    ASSERT_EQUALS(false, res->getBoolean("NON_UNIQUE"));
    ASSERT(!res->next());

    // Now we have three indexes, unique PK, unique an_idx_col3 and non-unique idx_col2
    stmt->execute("CREATE INDEX idx_col2 ON test(col2 ASC)");
    // Show only the unique ones...
    res.reset(dbmeta->getIndexInfo(con->getCatalog(), con->getSchema(), "test", true, false));
    ASSERT(res->next());
    ASSERT_EQUALS("an_idx_col3", res->getString("INDEX_NAME"));
    ASSERT_EQUALS(false, res->getBoolean("NON_UNIQUE"));
    ASSERT(res->next());
    ASSERT_EQUALS("PRIMARY", res->getString("INDEX_NAME"));
    ASSERT_EQUALS(false, res->getBoolean("NON_UNIQUE"));
    if (res->next())
    {
      got_warning=true;
      msg.str("");
      msg << "... \t\tWARNING: requesting only unique keys but got also non-unique key ";
      msg << "'" << res->getString("INDEX_NAME") << "', UNIQUE = " << std::boolalpha;
      msg << !res->getBoolean("NON_UNIQUE");
      logMsg(msg.str());
    }
    ASSERT(!res->next());

    // Another index. Should appear in the sort order prior to the idx_col2 one...
    // Sort order is: NON_UNIQUE, TYPE, INDEX_NAME
    stmt->execute("CREATE INDEX an_a_idx_col4 ON test(col4 DESC)");
    res.reset(dbmeta->getIndexInfo(con->getCatalog(), con->getSchema(), "test", false, false));
    ASSERT(res->next());
    ASSERT_EQUALS(DatabaseMetaData::tableIndexOther, res->getInt(7));
    ASSERT_EQUALS("an_idx_col3", res->getString("INDEX_NAME"));
    ASSERT_EQUALS(false, res->getBoolean("NON_UNIQUE"));
    ASSERT(res->next());
    ASSERT_EQUALS("PRIMARY", res->getString("INDEX_NAME"));
    ASSERT_EQUALS(false, res->getBoolean("NON_UNIQUE"));
    ASSERT_EQUALS(DatabaseMetaData::tableIndexOther, res->getInt(7));
    ASSERT(res->next());
    ASSERT_EQUALS("an_a_idx_col4", res->getString("INDEX_NAME"));
    ASSERT_EQUALS(true, res->getBoolean("NON_UNIQUE"));
    ASSERT_EQUALS(DatabaseMetaData::tableIndexOther, res->getInt(7));
    ASSERT(res->next());
    ASSERT_EQUALS("idx_col2", res->getString("INDEX_NAME"));
    ASSERT_EQUALS(true, res->getBoolean("NON_UNIQUE"));
    ASSERT_EQUALS(DatabaseMetaData::tableIndexOther, res->getInt(7));
    ASSERT(!res->next());

    //Was wrong on previous versions....
    if (getMySQLVersion(con) >= 80000)
    {
      stmt->execute("DROP TABLE IF EXISTS test");
      stmt->execute("CREATE TABLE test(col1 INT NOT NULL, col2 INT NOT NULL, col3 INT NOT NULL, col4 INT, col5 INT, PRIMARY KEY(col1))");
      stmt->execute("CREATE INDEX idx_col4_col5 ON test(col5 DESC, col4 ASC)");
      res.reset(dbmeta->getIndexInfo(con->getCatalog(), con->getSchema(), "test", false, false));
      ASSERT(res->next());
      ASSERT_EQUALS("PRIMARY", res->getString("INDEX_NAME"));
      ASSERT_EQUALS(false, res->getBoolean("NON_UNIQUE"));
      ASSERT(res->next());
      ASSERT_EQUALS("idx_col4_col5", res->getString("INDEX_NAME"));
      ASSERT_EQUALS("D", res->getString("ASC_OR_DESC"));
      ASSERT_EQUALS("col5", res->getString("COLUMN_NAME"));
      ASSERT_EQUALS(true, res->getBoolean("NON_UNIQUE"));
      ASSERT(res->next());
      ASSERT_EQUALS("idx_col4_col5", res->getString("INDEX_NAME"));
      ASSERT_EQUALS("A", res->getString("ASC_OR_DESC"));
      ASSERT_EQUALS("col4", res->getString("COLUMN_NAME"));
      ASSERT_EQUALS(true, res->getBoolean("NON_UNIQUE"));
      ASSERT(!res->next());
    }

    try
    {
      stmt->execute("DROP TABLE IF EXISTS test");
      stmt->execute("CREATE TABLE test(col1 INT NOT NULL, col2 INT NOT NULL, PRIMARY KEY(col1)) ENGINE=MEMORY");
      stmt->execute("CREATE INDEX idx_col2 USING HASH ON test(col2 DESC)");
      res.reset(dbmeta->getIndexInfo(con->getCatalog(), con->getSchema(), "test", false, false));
      ASSERT(res->next());
      ASSERT_EQUALS("PRIMARY", res->getString("INDEX_NAME"));
      ASSERT_EQUALS(false, res->getBoolean("NON_UNIQUE"));
      ASSERT(res->next());
      ASSERT_EQUALS("idx_col2", res->getString("INDEX_NAME"));
      // There is no order when using HASH
      ASSERT_EQUALS("", res->getString("ASC_OR_DESC"));
      ASSERT_EQUALS("col2", res->getString("COLUMN_NAME"));
      ASSERT_EQUALS(DatabaseMetaData::tableIndexHashed, res->getInt("TYPE"));
      ASSERT_EQUALS(true, res->getBoolean("NON_UNIQUE"));
      ASSERT(!res->next());
    }
    catch (sql::SQLException &)
    {
    }

    stmt->execute("DROP TABLE IF EXISTS test");
    res.reset(dbmeta->getIndexInfo(con->getCatalog(), con->getSchema(), "test", false, false));
    ASSERT(!res->next());

    if (got_warning)
    {
      FAIL("See above warnings!");
    }
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  if (got_todo_warning)
  {

    TODO("See --verbose warnings!");
    FAIL("TODO - see --verbose warnings");
  }
}

void connectionmetadata::getLimitsAndStuff()
{
  logMsg("connectionmetadata::getLimitsAndStuff() - MySQL_ConnectionMetaData::getLimitsAndStuff()");

  std::string funcs("ASCII,BIN,BIT_LENGTH,CHAR,CHARACTER_LENGTH,CHAR_LENGTH,CONCAT,"\
                          "CONCAT_WS,CONV,ELT,EXPORT_SET,FIELD,FIND_IN_SET,HEX,INSERT,"\
                          "INSTR,LCASE,LEFT,LENGTH,LOAD_FILE,LOCATE,LOCATE,LOWER,LPAD,"\
                          "LTRIM,MAKE_SET,MATCH,MID,OCT,OCTET_LENGTH,ORD,POSITION,"\
                          "QUOTE,REPEAT,REPLACE,REVERSE,RIGHT,RPAD,RTRIM,SOUNDEX,"\
                          "SPACE,STRCMP,SUBSTRING,SUBSTRING,SUBSTRING,SUBSTRING,SUBSTRING_INDEX,TRIM,UCASE,UPPER");

  std::string sys_funcs("DATABASE,USER,SYSTEM_USER,SESSION_USER,PASSWORD,ENCRYPT,LAST_INSERT_ID,VERSION");

  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    ASSERT_EQUALS(3, dbmeta->getCDBCMajorVersion());
    ASSERT_EQUALS(0, dbmeta->getCDBCMinorVersion());
    ASSERT_EQUALS(16777208, dbmeta->getMaxBinaryLiteralLength());
    ASSERT_EQUALS(32, dbmeta->getMaxCatalogNameLength());
    ASSERT_EQUALS(16777208, dbmeta->getMaxCharLiteralLength());
    ASSERT_EQUALS(64, dbmeta->getMaxColumnNameLength());
    ASSERT_EQUALS(64, dbmeta->getMaxColumnsInGroupBy());
    ASSERT_EQUALS(16, dbmeta->getMaxColumnsInIndex());
    ASSERT_EQUALS(64, dbmeta->getMaxColumnsInOrderBy());
    ASSERT_EQUALS(256, dbmeta->getMaxColumnsInSelect());
    ASSERT_EQUALS(512, dbmeta->getMaxColumnsInTable());

    stmt.reset(con->createStatement());
    res.reset(stmt->executeQuery("SELECT @@max_connections AS _max"));
    ASSERT(res->next());
    ASSERT_EQUALS(res->getInt("_max"), dbmeta->getMaxConnections());

    ASSERT_EQUALS(64, dbmeta->getMaxCursorNameLength());
    ASSERT_EQUALS(256, dbmeta->getMaxIndexLength());
    ASSERT_EQUALS(64, dbmeta->getMaxProcedureNameLength());
    ASSERT_EQUALS(2147483639, dbmeta->getMaxRowSize());
    ASSERT_EQUALS(64, dbmeta->getMaxSchemaNameLength());

    stmt.reset(con->createStatement());
    res.reset(stmt->executeQuery("SHOW VARIABLES LIKE 'max_allowed_packet'"));
    ASSERT(res->next());
    ASSERT_EQUALS(res->getInt(2) - 4, dbmeta->getMaxStatementLength());

    ASSERT_EQUALS(0, dbmeta->getMaxStatements());
    ASSERT_EQUALS(64, dbmeta->getMaxTableNameLength());
    ASSERT_EQUALS(256, dbmeta->getMaxTablesInSelect());
    ASSERT_EQUALS(16, dbmeta->getMaxUserNameLength());
    ASSERT_EQUALS("ABS,ACOS,ASIN,ATAN,ATAN2,BIT_COUNT,CEILING,COS,"
                  "COT,DEGREES,EXP,FLOOR,LOG,LOG10,MAX,MIN,MOD,PI,POW,"
                  "POWER,RADIANS,RAND,ROUND,SIN,SQRT,TAN,TRUNCATE"
                  , dbmeta->getNumericFunctions());

    ASSERT_EQUALS(false, dbmeta->allProceduresAreCallable());
    ASSERT_EQUALS(false, dbmeta->allTablesAreSelectable());
    ASSERT_EQUALS(true, dbmeta->dataDefinitionCausesTransactionCommit());
    ASSERT_EQUALS(false, dbmeta->dataDefinitionIgnoredInTransactions());
    ASSERT_EQUALS(false, dbmeta->deletesAreDetected(-1));
    ASSERT_EQUALS(false, dbmeta->deletesAreDetected(0));
    ASSERT_EQUALS(false, dbmeta->deletesAreDetected(1));
    ASSERT_EQUALS(true, dbmeta->dataDefinitionCausesTransactionCommit());
    ASSERT_EQUALS(true, dbmeta->doesMaxRowSizeIncludeBlobs());

    ASSERT_EQUALS(DatabaseMetaData::sqlStateSQL99, dbmeta->getSQLStateType());
    ASSERT(DatabaseMetaData::sqlStateXOpen != dbmeta->getSQLStateType());

    ASSERT_EQUALS(funcs, dbmeta->getStringFunctions());
    ASSERT_EQUALS(sys_funcs, dbmeta->getSystemFunctions());

    ASSERT_EQUALS(false, dbmeta->insertsAreDetected(-1));
    ASSERT_EQUALS(false, dbmeta->insertsAreDetected(0));
    ASSERT_EQUALS(false, dbmeta->insertsAreDetected(1));

    ASSERT_EQUALS(true, dbmeta->isCatalogAtStart());
    ASSERT_EQUALS(false, dbmeta->isReadOnly());

    ASSERT_EQUALS(true, dbmeta->nullPlusNonNullIsNull());
    ASSERT_EQUALS(false, dbmeta->nullsAreSortedAtEnd());
    ASSERT_EQUALS(false, dbmeta->nullsAreSortedAtStart());
    // KLUDGE - the code takes care of some exotic MySQL 4.x, however, we don't support 4.x
    ASSERT_EQUALS(false, dbmeta->nullsAreSortedHigh());
    ASSERT_EQUALS(!dbmeta->nullsAreSortedLow(), dbmeta->nullsAreSortedHigh());

    ASSERT_EQUALS(false, dbmeta->othersDeletesAreVisible(-1));
    ASSERT_EQUALS(false, dbmeta->othersDeletesAreVisible(0));
    ASSERT_EQUALS(false, dbmeta->othersDeletesAreVisible(1));
    ASSERT_EQUALS(false, dbmeta->othersInsertsAreVisible(-1));
    ASSERT_EQUALS(false, dbmeta->othersInsertsAreVisible(0));
    ASSERT_EQUALS(false, dbmeta->othersInsertsAreVisible(1));
    ASSERT_EQUALS(false, dbmeta->othersUpdatesAreVisible(-1));
    ASSERT_EQUALS(false, dbmeta->othersUpdatesAreVisible(0));
    ASSERT_EQUALS(false, dbmeta->othersUpdatesAreVisible(1));

    ASSERT_EQUALS(false, dbmeta->ownDeletesAreVisible(-1));
    ASSERT_EQUALS(false, dbmeta->ownDeletesAreVisible(0));
    ASSERT_EQUALS(false, dbmeta->ownDeletesAreVisible(1));
    ASSERT_EQUALS(false, dbmeta->ownInsertsAreVisible(-1));
    ASSERT_EQUALS(false, dbmeta->ownInsertsAreVisible(0));
    ASSERT_EQUALS(false, dbmeta->ownInsertsAreVisible(1));
    ASSERT_EQUALS(false, dbmeta->ownUpdatesAreVisible(-1));
    ASSERT_EQUALS(false, dbmeta->ownUpdatesAreVisible(0));
    ASSERT_EQUALS(false, dbmeta->ownUpdatesAreVisible(1));

    ASSERT_EQUALS(false, dbmeta->storesUpperCaseIdentifiers());
    ASSERT_EQUALS(true, dbmeta->storesUpperCaseQuotedIdentifiers());

    ASSERT_EQUALS(true, dbmeta->supportsAlterTableWithAddColumn());
    ASSERT_EQUALS(true, dbmeta->supportsAlterTableWithDropColumn());

    ASSERT_EQUALS(true, dbmeta->supportsANSI92EntryLevelSQL());
    ASSERT_EQUALS(false, dbmeta->supportsANSI92FullSQL());
    ASSERT_EQUALS(false, dbmeta->supportsANSI92IntermediateSQL());

    ASSERT_EQUALS(true, dbmeta->supportsBatchUpdates());

    ASSERT_EQUALS(false, dbmeta->supportsCatalogsInDataManipulation());
    ASSERT_EQUALS(false, dbmeta->supportsCatalogsInIndexDefinitions());
    ASSERT_EQUALS(false, dbmeta->supportsCatalogsInPrivilegeDefinitions());
    ASSERT_EQUALS(false, dbmeta->supportsCatalogsInProcedureCalls());
    ASSERT_EQUALS(false, dbmeta->supportsCatalogsInTableDefinitions());

    ASSERT_EQUALS(true, dbmeta->supportsColumnAliasing());
    ASSERT_EQUALS(false, dbmeta->supportsConvert());
    ASSERT_EQUALS(true, dbmeta->supportsCoreSQLGrammar());
    /* We support MySQL 5.1+ . It must be true */
    ASSERT_EQUALS(true, dbmeta->supportsCorrelatedSubqueries());
    ASSERT_EQUALS(false, dbmeta->supportsDataDefinitionAndDataManipulationTransactions());
    ASSERT_EQUALS(false, dbmeta->supportsDataManipulationTransactionsOnly());
    ASSERT_EQUALS(true, dbmeta->supportsDifferentTableCorrelationNames());
    ASSERT_EQUALS(true, dbmeta->supportsExpressionsInOrderBy());
    ASSERT_EQUALS(false, dbmeta->supportsExtendedSQLGrammar());
    ASSERT_EQUALS(false, dbmeta->supportsFullOuterJoins());
    ASSERT_EQUALS(true, dbmeta->supportsGetGeneratedKeys());
    ASSERT_EQUALS(true, dbmeta->supportsGroupBy());
    ASSERT_EQUALS(true, dbmeta->supportsGroupByBeyondSelect());
    ASSERT_EQUALS(true, dbmeta->supportsGroupByUnrelated());
    ASSERT_EQUALS(true, dbmeta->supportsLikeEscapeClause());
    ASSERT_EQUALS(true, dbmeta->supportsLimitedOuterJoins());
    ASSERT_EQUALS(true, dbmeta->supportsMinimumSQLGrammar());
    ASSERT_EQUALS(true, dbmeta->supportsMultipleOpenResults());
    ASSERT_EQUALS(false, dbmeta->supportsMultipleResultSets());
    ASSERT_EQUALS(true, dbmeta->supportsMultipleTransactions());
    ASSERT_EQUALS(false, dbmeta->supportsNamedParameters());
    ASSERT_EQUALS(true, dbmeta->supportsNonNullableColumns());
    ASSERT_EQUALS(false, dbmeta->supportsOpenCursorsAcrossCommit());
    ASSERT_EQUALS(false, dbmeta->supportsOpenCursorsAcrossRollback());
    ASSERT_EQUALS(false, dbmeta->supportsOpenStatementsAcrossCommit());
    ASSERT_EQUALS(false, dbmeta->supportsOpenStatementsAcrossRollback());
    ASSERT_EQUALS(false, dbmeta->supportsOrderByUnrelated());
    ASSERT_EQUALS(true, dbmeta->supportsOuterJoins());
    ASSERT_EQUALS(false, dbmeta->supportsPositionedDelete());
    ASSERT_EQUALS(false, dbmeta->supportsPositionedUpdate());

    ASSERT_EQUALS(true, dbmeta->supportsResultSetHoldability(sql::ResultSet::HOLD_CURSORS_OVER_COMMIT));
    ASSERT_EQUALS(false, dbmeta->supportsResultSetHoldability(sql::ResultSet::CLOSE_CURSORS_AT_COMMIT));

    ASSERT_EQUALS(true, dbmeta->supportsResultSetType(sql::ResultSet::TYPE_SCROLL_INSENSITIVE));
    ASSERT_EQUALS(false, dbmeta->supportsResultSetType(sql::ResultSet::TYPE_SCROLL_SENSITIVE));
    ASSERT_EQUALS(false, dbmeta->supportsResultSetType(sql::ResultSet::TYPE_FORWARD_ONLY));

    /* We support MySQL 5.1+ . It must be true */
    ASSERT_EQUALS(true, dbmeta->supportsSavepoints());
    ASSERT_EQUALS(true, dbmeta->supportsSchemasInDataManipulation());

    ASSERT_EQUALS(true, dbmeta->supportsSchemasInDataManipulation());
    ASSERT_EQUALS(true, dbmeta->supportsSchemasInIndexDefinitions());
    ASSERT_EQUALS(true, dbmeta->supportsSchemasInPrivilegeDefinitions());
    ASSERT_EQUALS(true, dbmeta->supportsSchemasInProcedureCalls());
    ASSERT_EQUALS(true, dbmeta->supportsSchemasInTableDefinitions());

    /* We support MySQL 5.1+ . It must be true */
    ASSERT_EQUALS(true, dbmeta->supportsSelectForUpdate());
    ASSERT_EQUALS(false, dbmeta->supportsStatementPooling());
    ASSERT_EQUALS(true, dbmeta->supportsStoredProcedures());
    ASSERT_EQUALS(true, dbmeta->supportsSubqueriesInComparisons());
    ASSERT_EQUALS(true, dbmeta->supportsSubqueriesInExists());
    ASSERT_EQUALS(true, dbmeta->supportsSubqueriesInIns());
    ASSERT_EQUALS(true, dbmeta->supportsSubqueriesInQuantifieds());

    ASSERT_EQUALS(true, dbmeta->supportsTableCorrelationNames());

    /* We support MySQL 5.1+ . It must be true */
    ASSERT_EQUALS(true, dbmeta->supportsTransactionIsolationLevel(sql::TRANSACTION_NONE));
    ASSERT_EQUALS(true, dbmeta->supportsTransactionIsolationLevel(sql::TRANSACTION_READ_COMMITTED));
    ASSERT_EQUALS(true, dbmeta->supportsTransactionIsolationLevel(sql::TRANSACTION_READ_UNCOMMITTED));
    ASSERT_EQUALS(true, dbmeta->supportsTransactionIsolationLevel(sql::TRANSACTION_REPEATABLE_READ));
    ASSERT_EQUALS(true, dbmeta->supportsTransactionIsolationLevel(sql::TRANSACTION_SERIALIZABLE));
    ASSERT_EQUALS(true, dbmeta->supportsTransactions());

    ASSERT_EQUALS(true, dbmeta->supportsTypeConversion());

    /* We support MySQL 5.1+ . It must be true */
    ASSERT_EQUALS(true, dbmeta->supportsUnion());
    ASSERT_EQUALS(true, dbmeta->supportsUnionAll());

    ASSERT_EQUALS(false, dbmeta->updatesAreDetected(sql::ResultSet::TYPE_FORWARD_ONLY));
    ASSERT_EQUALS(false, dbmeta->updatesAreDetected(sql::ResultSet::TYPE_SCROLL_INSENSITIVE));
    ASSERT_EQUALS(false, dbmeta->updatesAreDetected(sql::ResultSet::TYPE_SCROLL_SENSITIVE));

    ASSERT_EQUALS(false, dbmeta->usesLocalFilePerTable());
    ASSERT_EQUALS(false, dbmeta->usesLocalFiles());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getPrimaryKeys()
{
  logMsg("connectionmetadata::getPrimaryKeys() - MySQL_ConnectionMetaData::getPrimaryKeys");
  int row_num;
  std::string catalog;
  std::string schema;
  std::stringstream msg;
  bool got_warning=false;

  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(col2 INT NOT NULL, col1 INT NOT NULL, PRIMARY KEY(col2, col1))");
    // The descriptions are ordered by the column COLUMN_NAME, will they?
    res.reset(dbmeta->getPrimaryKeys(con->getCatalog(), con->getSchema(), "test"));
    checkResultSetScrolling(res);
    row_num=0;
    while (res->next())
    {
      row_num++;
      if (con->getCatalog() != "" && res->getString("TABLE_CAT") != "" && con->getCatalog() != res->getString("TABLE_CAT"))
      {
        got_warning=true;
        msg.str("");
        msg << "...\t\tWARNING expecting TABLE_CAT = '" << con->getCatalog() << "'";
        msg << " got '" << res->getString("TABLE_CAT") << "'";
        logMsg(msg.str());
      }

      ASSERT_EQUALS(con->getSchema(), res->getString("TABLE_SCHEM"));
      ASSERT_EQUALS("test", res->getString("TABLE_NAME"));
      switch (row_num) {
      case 1:
        // No, ordered by KEY_SEQ
        ASSERT_EQUALS("col2", res->getString("COLUMN_NAME"));
        ASSERT_EQUALS(row_num, res->getInt("KEY_SEQ"));
        break;
      case 2:
        ASSERT_EQUALS("col1", res->getString("COLUMN_NAME"));
        ASSERT_EQUALS(row_num, res->getInt("KEY_SEQ"));
        break;
      default:
        FAIL("Too many PK columns reported");
        break;
      }
      ASSERT_EQUALS("PRIMARY", res->getString("PK_NAME"));
    }
    ASSERT_EQUALS(2, row_num);

    // catalog - a string ... "" retrieves pk from tables wo catalog, NULL = catalog should not be used to narrow the search
    res.reset(dbmeta->getPrimaryKeys(catalog, con->getSchema(), "test"));
    ASSERT_EQUALS(true, res->next());
    ASSERT_EQUALS("test", res->getString("TABLE_NAME"));

    res.reset(dbmeta->getPrimaryKeys(catalog, schema, "test"));
    ASSERT(!res->next());

    stmt->execute("DROP TABLE IF EXISTS test");
    res.reset(dbmeta->getPrimaryKeys(con->getCatalog(), con->getSchema(), "test"));
    ASSERT_EQUALS(false, res->next());

  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  if (got_warning)
  {

    TODO("See --verbose warnings!");
    FAIL("TODO - see --verbose warnings!");
  }
}

void connectionmetadata::getProcedures()
{
  logMsg("connectionmetadata::getProcedures() - MySQL_ConnectionMetaData::getProcedures");

  sql::ConnectOptionsMap options;

  for(int i=0; i < 2; ++i)
  {
    options[OPT_METADATA_INFO_SCHEMA] = i == 0;

    con.reset(unit_fixture::getConnection(&options));
    con->setSchema(db);


    bool got_warning=false;
    std::stringstream msg;
    try
    {
      DatabaseMetaData * dbmeta=con->getMetaData();
      stmt.reset(con->createStatement());
      try
      {
        stmt->execute("DROP PROCEDURE IF EXISTS p1");
        stmt->execute("CREATE PROCEDURE p1(OUT param1 INT) COMMENT 'Sample Procedure' BEGIN SELECT 1 INTO param1; END");
        stmt->execute("DROP FUNCTION IF EXISTS f1");
        stmt->execute("CREATE FUNCTION f1(s CHAR(20)) RETURNS CHAR(50) COMMENT 'Sample Function'  DETERMINISTIC RETURN CONCAT('Hello, ',s,'!')");
      }
      catch (sql::SQLException &)
      {
        SKIP("Cannot create procedure");
      }

      // Verify if the procedure creally has been created...
      stmt->execute("SET @myvar = -1");
      stmt->execute("CALL p1(@myvar)");

      res.reset(stmt->executeQuery("SELECT @myvar AS _myvar"));
      ASSERT(res->next());
      ASSERT_EQUALS(1, res->getInt("_myvar"));
      logMsg("...who is the bad guy?");

      res.reset(dbmeta->getProcedures(con->getCatalog(), con->getSchema(), "p1"));
      checkResultSetScrolling(res);
      logMsg("...is it you, getProcedures()?");
      ASSERT(res->next());

      if (con->getCatalog() != "" && res->getString("PROCEDURE_CAT") != "" && con->getCatalog() != res->getString("PROCEDURE_CAT"))
      {
        got_warning=true;
        msg.str("");
        msg << "\t\tWARNING expecting PROCEDURE_CAT = '" << con->getCatalog() << "'";
        msg << " got '" << res->getString("PROCEDURE_CAT") << "'";
        logMsg(msg.str());
      }

      ASSERT_EQUALS(res->getString(1), res->getString("PROCEDURE_CAT"));
      ASSERT_EQUALS(con->getSchema(), res->getString("PROCEDURE_SCHEM"));
      ASSERT_EQUALS(res->getString(2), res->getString("PROCEDURE_SCHEM"));
      ASSERT_EQUALS("p1", res->getString(3));
      ASSERT_EQUALS(res->getString(3), res->getString("PROCEDURE_NAME"));
      ASSERT_EQUALS("", res->getString(4));
      ASSERT_EQUALS("", res->getString(5));
      ASSERT_EQUALS("", res->getString(6));
      ASSERT_EQUALS("Sample Procedure", res->getString(7));
      ASSERT_EQUALS(res->getString("REMARKS"), res->getString(7));
      ASSERT_EQUALS(DatabaseMetaData::procedureNoResult, res->getInt("PROCEDURE_TYPE"));
      ASSERT(DatabaseMetaData::procedureReturnsResult != res->getInt(8));
      ASSERT(DatabaseMetaData::procedureResultUnknown != res->getInt(8));
      ASSERT(!res->next());

      res.reset(dbmeta->getProcedures(con->getCatalog(), con->getSchema(), "f1"));
      checkResultSetScrolling(res);

      ASSERT(res->next());

      if (con->getCatalog() != "" && res->getString("PROCEDURE_CAT") != "" && con->getCatalog() != res->getString("PROCEDURE_CAT"))
      {
        got_warning=true;
        msg.str("");
        msg << "\t\tWARNING expecting PROCEDURE_CAT = '" << con->getCatalog() << "'";
        msg << " got '" << res->getString("PROCEDURE_CAT") << "'";
        logMsg(msg.str());
      }

      ASSERT_EQUALS(res->getString(1), res->getString("PROCEDURE_CAT"));
      ASSERT_EQUALS(con->getSchema(), res->getString("PROCEDURE_SCHEM"));
      ASSERT_EQUALS(res->getString(2), res->getString("PROCEDURE_SCHEM"));
      ASSERT_EQUALS("f1", res->getString(3));
      ASSERT_EQUALS(res->getString(3), res->getString("PROCEDURE_NAME"));
      ASSERT_EQUALS("", res->getString(4));
      ASSERT_EQUALS("", res->getString(5));
      ASSERT_EQUALS("", res->getString(6));
      ASSERT_EQUALS("Sample Function", res->getString(7));
      ASSERT_EQUALS(res->getString("REMARKS"), res->getString(7));
      ASSERT_EQUALS(DatabaseMetaData::procedureReturnsResult, res->getInt("PROCEDURE_TYPE"));
      ASSERT(DatabaseMetaData::procedureNoResult != res->getInt(8));
      ASSERT(DatabaseMetaData::procedureResultUnknown != res->getInt(8));
      ASSERT(!res->next());
    }
    catch (sql::SQLException &e)
    {
      logErr(e.what());
      logErr("SQLState: " + std::string(e.getSQLState()));
      fail(e.what(), __FILE__, __LINE__);
    }

    if (got_warning)
    {
      if(i==0){
        TODO("Using Information Schema")
      } else {
        TODO("Not Using Information Schema");
      }
      TODO("See --verbose warnings!");
      FAIL("TODO - see --verbose warnings!");
    }
  }
  con.reset(unit_fixture::getConnection());
  con->setSchema(db);
}


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS

void connectionmetadata::getProcedureColumns()
{
  logMsg("connectionmetadata::getProcedureColumns() - MySQL_ConnectionMetaData::getProcedureColumns()");
  int server_version;

  SKIP("method not implemented");

  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    stmt.reset(con->createStatement());
    try
    {
      stmt->execute("DROP PROCEDURE IF EXISTS p1");
      stmt->execute("CREATE PROCEDURE p1(OUT param1 INT) BEGIN SELECT 1 INTO param1; END");
    }
    catch (sql::SQLException &)
    {
      SKIP("Cannot create procedure");
    }

    // Verify if the procedure creally has been created...
    stmt->execute("SET @myvar = -1");
    stmt->execute("CALL p1(@myvar)");
    res.reset(stmt->executeQuery("SELECT @myvar AS _myvar"));
    checkResultSetScrolling(res);
    ASSERT(res->next());
    ASSERT_EQUALS(1, res->getInt("_myvar"));

    res.reset(dbmeta->getProcedureColumns(con->getCatalog(), con->getSchema(), "p1", "%"));
    server_version=(10000 * dbmeta->getDatabaseMajorVersion())
            + (100 * dbmeta->getDatabaseMinorVersion())
            + dbmeta->getDatabasePatchVersion();

    if (server_version < 50206)
      ASSERT(!res->next());
    else
    {
      ASSERT(res->next());
      FAIL("Theres a new I_S table PARAMETERS. The test should use it");
    }

  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}
#endif

void connectionmetadata::getCatalogs()
{
  logMsg("connectionmetadata::getCatalogs() - MySQL_ConnectionMetaData::getCatalogs()");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    res.reset(dbmeta->getCatalogs());
    ASSERT(res->next());
    ASSERT(!res->next());
    ResultSetMetaData * resmeta=res->getMetaData();
    /* http://java.sun.com/j2se/1.4.2/docs/api/java/sql/DatabaseMetaData.html#getCatalogs() */
    ASSERT_EQUALS((unsigned int) 1, resmeta->getColumnCount());
    ASSERT_EQUALS("TABLE_CAT", resmeta->getColumnLabel(1));
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getCatalogSeparator()
{
  logMsg("connectionmetadata::getCatalogSeparator() - MySQL_ConnectionMetaData::getCatalogSeparator()");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    ASSERT_EQUALS("", dbmeta->getCatalogSeparator());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getCatalogTerm()
{
  logMsg("connectionmetadata::getCatalogTerm() - MySQL_ConnectionMetaData::getCatalogTerm()");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    ASSERT_EQUALS("n/a", dbmeta->getCatalogTerm());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getCrossReference()
{
  logMsg("connectionmetadata::getCrossReference() - MySQL_ConnectionMetaData::getCrossReference()");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    stmt.reset(con->createStatement());

    stmt->execute("DROP TABLE IF EXISTS child");
    stmt->execute("DROP TABLE IF EXISTS parent");
    try
    {
      stmt->execute("CREATE TABLE parent(pid INT NOT NULL, PRIMARY KEY(pid)) ENGINE=INNODB;");
      stmt->execute("CREATE TABLE child(cid INT NOT NULL, cpid INT, "
                    "INDEX idx_parent_id(cpid), FOREIGN KEY idx_parent_id(cpid) "
                    "REFERENCES parent(pid) ON DELETE CASCADE ON UPDATE CASCADE, PRIMARY KEY(cid)) ENGINE=INNODB;");
    }
    catch (sql::SQLException &)
    {
      SKIP("Cannot create necessary FK tables");
    }
    logMsg("... checking parent->child");
    res.reset(dbmeta->getCrossReference(con->getCatalog(), con->getSchema(), "parent", con->getCatalog(), con->getSchema(), "child"));
    checkResultSetScrolling(res);
    ASSERT(res->next());
    checkForeignKey(con, res);

    logMsg("... checking child->parent");
    stmt->execute("DROP TABLE IF EXISTS child");
    stmt->execute("DROP TABLE IF EXISTS parent");
    res.reset(dbmeta->getCrossReference(con->getCatalog(), con->getSchema(), "child", con->getCatalog(), con->getSchema(), "parent"));
    ASSERT(!res->next());
  }
  catch (sql::MethodNotImplementedException &e)
  {
    logMsg(e.what());
    SKIP("MySQL is too old, MethodNotImplementedException!");
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getProcedureTerm()
{
  logMsg("connectionmetadata::getProcedureTerm() - MySQL_ConnectionMetaData::getProcedureTerm");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    ASSERT_EQUALS("procedure", dbmeta->getProcedureTerm());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getResultSetHoldability()
{
  logMsg("connectionmetadata::getResultSetHoldability() - MySQL_ConnectionMetaData::getResultSetHoldability()");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    ASSERT_EQUALS(sql::ResultSet::HOLD_CURSORS_OVER_COMMIT, dbmeta->getResultSetHoldability());
    ASSERT(sql::ResultSet::CLOSE_CURSORS_AT_COMMIT != dbmeta->getResultSetHoldability());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getSchemaTerm()
{
  logMsg("connectionmetadata::getSchemaTerm() - MySQL_ConnectionMetaData::getSchemaTerm()");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    ASSERT_EQUALS("database", dbmeta->getSchemaTerm());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getSearchStringEscape()
{
  logMsg("connectionmetadata::getSearchStringEscape - MySQL_ConnectionMetaData::getSearchStringEscape()");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    ASSERT_EQUALS("\\", dbmeta->getSearchStringEscape());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getSQLKeywords()
{
  logMsg("connectionmetadata::getSQLKeywords - MySQL_ConnectionMetaData::getSQLKeywords()");
  int server_version = 0;
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();

    server_version=(10000 * dbmeta->getDatabaseMajorVersion())
            + (100 * dbmeta->getDatabaseMinorVersion())
            + dbmeta->getDatabasePatchVersion();
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  std::string keywords;

  if (server_version < 80022)
    keywords =  "ACCESSIBLE, ADD, ALL,"\
    "ALTER, ANALYZE, AND, AS, ASC, ASENSITIVE, BEFORE,"\
    "BETWEEN, BIGINT, BINARY, BLOB, BOTH, BY, CALL,"\
    "CASCADE, CASE, CHANGE, CHAR, CHARACTER, CHECK,"\
    "COLLATE, COLUMN, CONDITION, CONNECTION, CONSTRAINT,"\
    "CONTINUE, CONVERT, CREATE, CROSS, CURRENT_DATE,"\
    "CURRENT_TIME, CURRENT_TIMESTAMP, CURRENT_USER, CURSOR,"\
    "DATABASE, DATABASES, DAY_HOUR, DAY_MICROSECOND,"\
    "DAY_MINUTE, DAY_SECOND, DEC, DECIMAL, DECLARE,"\
    "DEFAULT, DELAYED, DELETE, DESC, DESCRIBE,"\
    "DETERMINISTIC, DISTINCT, DISTINCTROW, DIV, DOUBLE,"\
    "DROP, DUAL, EACH, ELSE, ELSEIF, ENCLOSED,"\
    "ESCAPED, EXISTS, EXIT, EXPLAIN, FALSE, FETCH,"\
    "FLOAT, FLOAT4, FLOAT8, FOR, FORCE, FOREIGN, FROM,"\
    "FULLTEXT, GRANT, GROUP, HAVING, HIGH_PRIORITY,"\
    "HOUR_MICROSECOND, HOUR_MINUTE, HOUR_SECOND, IF,"\
    "IGNORE, IN, INDEX, INFILE, INNER, INOUT,"\
    "INSENSITIVE, INSERT, INT, INT1, INT2, INT3, INT4,"\
    "INT8, INTEGER, INTERVAL, INTO, IS, ITERATE, JOIN,"\
    "KEY, KEYS, KILL, LEADING, LEAVE, LEFT, LIKE,"\
    "LOCALTIMESTAMP, LOCK, LONG, LONGBLOB, LONGTEXT,"\
    "LOOP, LOW_PRIORITY, MATCH, MEDIUMBLOB, MEDIUMINT,"\
    "MEDIUMTEXT, MIDDLEINT, MINUTE_MICROSECOND,"\
    "MINUTE_SECOND, MOD, MODIFIES, NATURAL, NOT,"\
    "NO_WRITE_TO_BINLOG, NULL, NUMERIC, ON, OPTIMIZE,"\
    "OPTION, OPTIONALLY, OR, ORDER, OUT, OUTER,"\
    "OUTFILE, PRECISION, PRIMARY, PROCEDURE, PURGE,"\
    "RANGE, READ, READS, READ_ONLY, READ_WRITE, REAL,"\
    "REFERENCES, REGEXP, RELEASE, RENAME, REPEAT,"\
    "REPLACE, REQUIRE, RESTRICT, RETURN, REVOKE, RIGHT,"\
    "RLIKE, SCHEMA, SCHEMAS, SECOND_MICROSECOND, SELECT,"\
    "SENSITIVE, SEPARATOR, SET, SHOW, SMALLINT, SPATIAL,"\
    "SPECIFIC, SQL, SQLEXCEPTION, SQLSTATE, SQLWARNING,"\
    "SQL_BIG_RESULT, SQL_CALC_FOUND_ROWS, SQL_SMALL_RESULT,"\
    "SSL, STARTING, STRAIGHT_JOIN, TABLE, TERMINATED,"\
    "THEN, TINYBLOB, TINYINT, TINYTEXT, TO, TRAILING,"\
    "TRIGGER, TRUE, UNDO, UNION, UNIQUE, UNLOCK,"\
    "UNSIGNED, UPDATE, USAGE, USE, USING, UTC_DATE,"\
    "UTC_TIME, UTC_TIMESTAMP, VALUES, VARBINARY, VARCHAR,"\
    "VARCHARACTER, VARYING, WHEN, WHERE, WHILE, WITH,"\
    "WRITE, X509, XOR, YEAR_MONTH, ZEROFILL," \
    "GENERAL, IGNORE_SERVER_IDS,"\
    "\x4D\x41\x53\x54\x45\x52_HEARTBEAT_PERIOD," \
    "MAXVALUE, RESIGNAL, SIGNAL, SLOW";
  else
    keywords =  "ACCESSIBLE, ADD, ALL,"\
    "ALTER, ANALYZE, AND, AS, ASC, ASENSITIVE, BEFORE,"\
    "BETWEEN, BIGINT, BINARY, BLOB, BOTH, BY, CALL,"\
    "CASCADE, CASE, CHANGE, CHAR, CHARACTER, CHECK,"\
    "COLLATE, COLUMN, CONDITION, CONNECTION, CONSTRAINT,"\
    "CONTINUE, CONVERT, CREATE, CROSS, CURRENT_DATE,"\
    "CURRENT_TIME, CURRENT_TIMESTAMP, CURRENT_USER, CURSOR,"\
    "DATABASE, DATABASES, DAY_HOUR, DAY_MICROSECOND,"\
    "DAY_MINUTE, DAY_SECOND, DEC, DECIMAL, DECLARE,"\
    "DEFAULT, DELAYED, DELETE, DESC, DESCRIBE,"\
    "DETERMINISTIC, DISTINCT, DISTINCTROW, DIV, DOUBLE,"\
    "DROP, DUAL, EACH, ELSE, ELSEIF, ENCLOSED,"\
    "ESCAPED, EXISTS, EXIT, EXPLAIN, FALSE, FETCH,"\
    "FLOAT, FLOAT4, FLOAT8, FOR, FORCE, FOREIGN, FROM,"\
    "FULLTEXT, GRANT, GROUP, HAVING, HIGH_PRIORITY,"\
    "HOUR_MICROSECOND, HOUR_MINUTE, HOUR_SECOND, IF,"\
    "IGNORE, IN, INDEX, INFILE, INNER, INOUT,"\
    "INSENSITIVE, INSERT, INT, INT1, INT2, INT3, INT4,"\
    "INT8, INTEGER, INTERVAL, INTO, IS, ITERATE, JOIN,"\
    "KEY, KEYS, KILL, LEADING, LEAVE, LEFT, LIKE,"\
    "LOCALTIMESTAMP, LOCK, LONG, LONGBLOB, LONGTEXT,"\
    "LOOP, LOW_PRIORITY, MATCH, MEDIUMBLOB, MEDIUMINT,"\
    "MEDIUMTEXT, MIDDLEINT, MINUTE_MICROSECOND,"\
    "MINUTE_SECOND, MOD, MODIFIES, NATURAL, NOT,"\
    "NO_WRITE_TO_BINLOG, NULL, NUMERIC, ON, OPTIMIZE,"\
    "OPTION, OPTIONALLY, OR, ORDER, OUT, OUTER,"\
    "OUTFILE, PRECISION, PRIMARY, PROCEDURE, PURGE,"\
    "RANGE, READ, READS, READ_ONLY, READ_WRITE, REAL,"\
    "REFERENCES, REGEXP, RELEASE, RENAME, REPEAT,"\
    "REPLACE, REQUIRE, RESTRICT, RETURN, REVOKE, RIGHT,"\
    "RLIKE, SCHEMA, SCHEMAS, SECOND_MICROSECOND, SELECT,"\
    "SENSITIVE, SEPARATOR, SET, SHOW, SMALLINT, SPATIAL,"\
    "SPECIFIC, SQL, SQLEXCEPTION, SQLSTATE, SQLWARNING,"\
    "SQL_BIG_RESULT, SQL_CALC_FOUND_ROWS, SQL_SMALL_RESULT,"\
    "SSL, STARTING, STRAIGHT_JOIN, TABLE, TERMINATED,"\
    "THEN, TINYBLOB, TINYINT, TINYTEXT, TO, TRAILING,"\
    "TRIGGER, TRUE, UNDO, UNION, UNIQUE, UNLOCK,"\
    "UNSIGNED, UPDATE, USAGE, USE, USING, UTC_DATE,"\
    "UTC_TIME, UTC_TIMESTAMP, VALUES, VARBINARY, VARCHAR,"\
    "VARCHARACTER, VARYING, WHEN, WHERE, WHILE, WITH,"\
    "WRITE, X509, XOR, YEAR_MONTH, ZEROFILL," \
    "GENERAL, IGNORE_SERVER_IDS, SOURCE_HEARTBEAT_PERIOD," \
    "MAXVALUE, RESIGNAL, SIGNAL, SLOW";

  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    ASSERT_EQUALS(keywords, dbmeta->getSQLKeywords());
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getSuperTables()
{
  logMsg("connectionmetadata::getSuperTables - MySQL_ConnectionMetaData::getSuperTables()");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT)");
    res.reset(dbmeta->getSuperTables(con->getCatalog(), con->getSchema(), "test"));
    checkResultSetScrolling(res);
    ASSERT(!res->next());
    ResultSetMetaData * resmeta=res->getMetaData();
    ASSERT_EQUALS((unsigned int) 4, resmeta->getColumnCount());
    ASSERT_EQUALS("TABLE_CAT", resmeta->getColumnLabel(1));
    ASSERT_EQUALS("TABLE_SCHEM", resmeta->getColumnLabel(2));
    ASSERT_EQUALS("TABLE_NAME", resmeta->getColumnLabel(3));
    ASSERT_EQUALS("SUPERTABLE_NAME", resmeta->getColumnLabel(4));

    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getSuperTypes()
{
  logMsg("connectionmetadata::getSuperTypes - MySQL_ConnectionMetaData::getSuperTypes()");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT)");
    res.reset(dbmeta->getSuperTypes(con->getCatalog(), con->getSchema(), "test"));
    checkResultSetScrolling(res);
    ASSERT(!res->next());
    ResultSetMetaData * resmeta=res->getMetaData();
    ASSERT_EQUALS((unsigned int) 6, resmeta->getColumnCount());
    ASSERT_EQUALS("TYPE_CAT", resmeta->getColumnLabel(1));
    ASSERT_EQUALS("TYPE_SCHEM", resmeta->getColumnLabel(2));
    ASSERT_EQUALS("TYPE_NAME", resmeta->getColumnLabel(3));
    ASSERT_EQUALS("SUPERTYPE_CAT", resmeta->getColumnLabel(4));
    ASSERT_EQUALS("SUPERTYPE_SCHEM", resmeta->getColumnLabel(5));
    ASSERT_EQUALS("SUPERTYPE_NAME", resmeta->getColumnLabel(6));

    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::classAttributes()
{
  logMsg("connectionmetadata::classAttributes - MySQL_ConnectionMetaData class attributes");
  TODO("Check if JDBC compliance requires certain values");
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();

    ASSERT_EQUALS(0, dbmeta->attributeNoNulls);
    ASSERT_EQUALS(1, dbmeta->attributeNullable);
    ASSERT_EQUALS(2, dbmeta->attributeNullableUnknown);

    ASSERT_EQUALS(1, dbmeta->bestRowNotPseudo);
    // BUG - ASSERT_EQUALS(2, dbmeta->bestRowPseudo);
    ASSERT_EQUALS(2, dbmeta->bestRowSession);

    ASSERT_EQUALS(0, dbmeta->bestRowTemporary);
    ASSERT_EQUALS(1, dbmeta->bestRowTransaction);
    ASSERT_EQUALS(0, dbmeta->bestRowUnknown);

    ASSERT_EQUALS(0, dbmeta->columnNoNulls);
    ASSERT_EQUALS(1, dbmeta->columnNullable);
    ASSERT_EQUALS(2, dbmeta->columnNullableUnknown);

    ASSERT_EQUALS(0, dbmeta->importedKeyCascade);
    // BUG - got 1 - ASSERT_EQUALS(5, dbmeta->importedKeyInitiallyDeferred);
    // BUG - got 2 - ASSERT_EQUALS(6, dbmeta->importedKeyInitiallyImmediate);
    ASSERT_EQUALS(3, dbmeta->importedKeyNoAction);
    // BUG - got 4 - ASSERT_EQUALS(7, dbmeta->importedKeyNotDeferrable);
    // BUG - got 5 - ASSERT_EQUALS(1, dbmeta->importedKeyRestrict);
    // BUG - got 6 - ASSERT_EQUALS(4, dbmeta->importedKeySetDefault);
    // BUG - got 7 - ASSERT_EQUALS(2, dbmeta->importedKeySetNull);

    // BUG - got 0 - ASSERT_EQUALS(1, dbmeta->procedureColumnIn);
    // BUG - got 1 - ASSERT_EQUALS(2, dbmeta->procedureColumnInOut);
    // BUG - got 2 - ASSERT_EQUALS(4, dbmeta->procedureColumnOut);
    ASSERT_EQUALS(3, dbmeta->procedureColumnResult);
    // BUG - got 4 - ASSERT_EQUALS(5, dbmeta->procedureColumnReturn);

    // BUG - got 5 - ASSERT_EQUALS(1, dbmeta->importedKeyRestrict);
    // BUG - got 6 - ASSERT_EQUALS(4, dbmeta->importedKeySetDefault);
    // BUG - got 7 - ASSERT_EQUALS(2, dbmeta->importedKeySetNull);

    // BUG - got 0 - ASSERT_EQUALS(1, dbmeta->procedureColumnIn);
    ASSERT_EQUALS(1, dbmeta->procedureColumnInOut);
    // BUG - got 2 - ASSERT_EQUALS(4, dbmeta->procedureColumnOut);
    ASSERT_EQUALS(3, dbmeta->procedureColumnResult);
    // BUG - got 4 - ASSERT_EQUALS(5, dbmeta->procedureColumnReturn);
    // BUG - got 5 - ASSERT_EQUALS(0, dbmeta->procedureColumnUnknown);
    // BUG - got 6 - ASSERT_EQUALS(0, dbmeta->procedureNoNulls);
    // BUG - got 7 - ASSERT_EQUALS(1, dbmeta->procedureNoResult);
    // BUG - got 8 - ASSERT_EQUALS(1, dbmeta->procedureNullable);
    // BUG - got 9 - ASSERT_EQUALS(0, dbmeta->procedureNullableUnknown);
    // BUG - got 10 - ASSERT_EQUALS(2, dbmeta->procedureResultUnknown);
    // BUG - got 11 - ASSERT_EQUALS(2, dbmeta->procedureReturnsResult);

    // BUG - got 0 - ASSERT_EQUALS(2, dbmeta->sqlStateSQL99);
    ASSERT_EQUALS(1, dbmeta->sqlStateXOpen);

    // BUG - got 0 - ASSERT_EQUALS(1, dbmeta->tableIndexClustered);
    // BUG - got 1 - ASSERT_EQUALS(2, dbmeta->tableIndexHashed);
    // BUG - got 2 - ASSERT_EQUALS(3, dbmeta->tableIndexOther);
    // BUG - got 3 - ASSERT_EQUALS(0, dbmeta->tableIndexStatistic);

    ASSERT_EQUALS(0, dbmeta->typeNoNulls);
    ASSERT_EQUALS(1, dbmeta->typeNullable);
    ASSERT_EQUALS(2, dbmeta->typeNullableUnknown);

    ASSERT_EQUALS(2, dbmeta->typePredBasic);
    ASSERT_EQUALS(1, dbmeta->typePredChar);
    ASSERT_EQUALS(0, dbmeta->typePredNone);

    ASSERT_EQUALS(3, dbmeta->typeSearchable);
    ASSERT_EQUALS(1, dbmeta->versionColumnNotPseudo);
    ASSERT_EQUALS(2, dbmeta->versionColumnPseudo);
    ASSERT_EQUALS(0, dbmeta->versionColumnUnknown);

  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getColumnsTypeConversions()
{
  logMsg("connectionmetadata::getColumnsTypeConversions() - MySQL_ConnectionMetaData::getColumns");
  std::vector<columndefinition>::iterator it;
  std::stringstream msg;
  int i;
  bool got_warning;
  try
  {
    DatabaseMetaData * dbmeta=con->getMetaData();
    stmt.reset(con->createStatement());

    got_warning=false;
    logMsg("... looping over all kinds of column types");
    for (it=columns.begin(), i=0; it != columns.end(); i++, it++)
    {
      stmt->execute("DROP TABLE IF EXISTS test");
      msg.str("");
      msg << "CREATE TABLE test(dummy TIMESTAMP, id " << it->sqldef << ")";
      try
      {
        stmt->execute(msg.str());
        msg.str("");
        msg << "... testing " << it->sqldef;
        logMsg(msg.str());
      }
      catch (sql::SQLException &)
      {
        msg.str("");
        msg << "... skipping " << it->sqldef;
        logMsg(msg.str());
        continue;
      }
      res.reset(dbmeta->getColumns(con->getCatalog(), con->getSchema(), "test", "id"));
      checkResultSetScrolling(res);
      ASSERT_EQUALS(true, res->next());

      // string -> xyz
      ASSERT_EQUALS("test", res->getString("TABLE_NAME"));
      ASSERT_EQUALS(false, res->wasNull());
      ASSERT_EQUALS(res->getString(3), res->getString("TABLE_NAME"));

      ASSERT_EQUALS(false, res->getBoolean("TABLE_NAME"));
      ASSERT_EQUALS(false, res->wasNull());
      ASSERT_EQUALS(res->getBoolean(3), res->getBoolean("TABLE_NAME"));

      ASSERT_EQUALS((int64_t) 0, res->getInt64("TABLE_NAME"));
      ASSERT_EQUALS(false, res->wasNull());
      ASSERT_EQUALS(res->getInt64(3), res->getInt64("TABLE_NAME"));

      ASSERT_EQUALS((uint64_t) 0, res->getUInt64("TABLE_NAME"));
      ASSERT_EQUALS(false, res->wasNull());
      ASSERT_EQUALS(res->getUInt64(3), res->getUInt64("TABLE_NAME"));

      ASSERT_EQUALS((double) 0, res->getDouble("TABLE_NAME"));
      ASSERT_EQUALS(false, res->wasNull());
      ASSERT_EQUALS(res->getDouble(3), res->getDouble("TABLE_NAME"));

      ASSERT_EQUALS((int) 0, res->getInt("TABLE_NAME"));
      ASSERT_EQUALS(false, res->wasNull());
      ASSERT_EQUALS(res->getInt(3), res->getInt("TABLE_NAME"));

      ASSERT_EQUALS((unsigned int) 0, res->getUInt("TABLE_NAME"));
      ASSERT_EQUALS(false, res->wasNull());
      ASSERT_EQUALS(res->getUInt(3), res->getUInt("TABLE_NAME"));

      ASSERT_EQUALS(false, res->isNull("TABLE_NAME"));
      ASSERT_EQUALS(false, res->wasNull());
      ASSERT_EQUALS(res->isNull(3), res->isNull("TABLE_NAME"));

      // integer -> xyz
      if (it->decimal_digits != res->getInt("DECIMAL_DIGITS"))
      {
        msg.str("");
        msg << "...\t\tWARNING: expecting DECIMAL_DIGITS = (int)'" << it->decimal_digits << "'";
        msg << " got (int)'" << res->getString("DECIMAL_DIGITS") << "'";
        logMsg(msg.str());
        got_warning=true;
      }
      ASSERT_EQUALS(false, res->wasNull());
      ASSERT_EQUALS(res->getInt(9), res->getInt("DECIMAL_DIGITS"));

      msg.str("");
      if (0 == it->decimal_digits)
      {
        msg << "0";
      }
      else
      {
        msg << it->decimal_digits;
      }
      if (msg.str() != res->getString("DECIMAL_DIGITS"))
      {
        msg.str("");
        msg << "...\t\tWARNING: expecting DECIMAL_DIGITS = '" << it->decimal_digits << "'";
        msg << " length() is '" << msg.str().length() << "'";
        msg << " got '" << res->getString("DECIMAL_DIGITS") << "'";
        msg << " length() is '" << res->getString("DECIMAL_DIGITS").length() << "'";
        logMsg(msg.str());
        got_warning=true;
      }
      else
      {
        // If string and int are broken, the rest is broken as well - you can bet!
        ASSERT_EQUALS(false, res->wasNull());
        ASSERT_EQUALS(res->getString(9), res->getString("DECIMAL_DIGITS"));

        ASSERT_EQUALS(it->decimal_digits != 0, res->getBoolean("DECIMAL_DIGITS"));
        ASSERT_EQUALS(false, res->wasNull());
        ASSERT_EQUALS(res->getBoolean(9), res->getBoolean("DECIMAL_DIGITS"));

        ASSERT_EQUALS((int64_t) it->decimal_digits, res->getInt64("DECIMAL_DIGITS"));
        ASSERT_EQUALS(false, res->wasNull());
        ASSERT_EQUALS(res->getInt64(9), res->getInt64("DECIMAL_DIGITS"));

        ASSERT_EQUALS((uint64_t) it->decimal_digits, res->getUInt64("DECIMAL_DIGITS"));
        ASSERT_EQUALS(false, res->wasNull());
        ASSERT_EQUALS(res->getUInt64(9), res->getUInt64("DECIMAL_DIGITS"));

        ASSERT_EQUALS((double) it->decimal_digits, res->getDouble("DECIMAL_DIGITS"));
        ASSERT_EQUALS(false, res->wasNull());
        ASSERT_EQUALS(res->getDouble(9), res->getDouble("DECIMAL_DIGITS"));

        ASSERT_EQUALS((int32_t) it->decimal_digits, res->getInt("DECIMAL_DIGITS"));
        ASSERT_EQUALS(false, res->wasNull());
        ASSERT_EQUALS(res->getInt(9), res->getInt("DECIMAL_DIGITS"));

        ASSERT_EQUALS((uint32_t) it->decimal_digits, res->getUInt("DECIMAL_DIGITS"));
        ASSERT_EQUALS(false, res->wasNull());
        ASSERT_EQUALS(res->getUInt(9), res->getUInt("DECIMAL_DIGITS"));

        ASSERT_EQUALS(false, res->isNull(9));
        ASSERT_EQUALS(res->isNull(9), res->isNull("DECIMAL_DIGITS"));
        ASSERT_EQUALS(false, res->wasNull());
      }

      try
      {
        res->isNull(0);
        FAIL("Invalid column index");
      }
      catch (sql::SQLException &)
      {
      }

      try
      {
        res->isNull("invalid column index");
        FAIL("Invalid column index");
      }
      catch (sql::SQLException &)
      {
      }

      stmt->execute("DROP TABLE IF EXISTS test");
    }

    stmt->execute("DROP TABLE IF EXISTS test");
    res.reset(dbmeta->getColumns(con->getCatalog(), con->getSchema(), "test", "id"));
    ASSERT(!res->next());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  try
  {
    res->isNull(1);
    FAIL("Not on resultset, should fail");
  }
  catch (sql::SQLException &)
  {
  }

  try
  {
    res->isNull("not on resultset and unknown column");
    FAIL("Not on resultset, should fail");
  }
  catch (sql::SQLException &)
  {
  }

  if (got_warning)
    FAIL("See --verbose warnings!");
}


/* Simple testcase of getBestRowIdentifier returns columns making UNIQUE not Null filters
   in case of primary key is not present
 */
void connectionmetadata::bestIdUniqueNotNull()
{
  createSchemaObject("TABLE", "bestIdUniqueNotNull", "(id int not null, value varchar(25),"
                                                     "UNIQUE INDEX(id))");
  createSchemaObject("TABLE", "bestIdUniqueNull", "(id int, value varchar(25),"
                                                     "UNIQUE INDEX(id))");

  DatabaseMetaData *dbmeta= con->getMetaData();
  res.reset(dbmeta->getBestRowIdentifier(con->getCatalog(), con->getSchema(), "bestIdUniqueNotNull", 0, false));

  ASSERT(res->next());
  ASSERT_EQUALS("id", res->getString(2));
  ASSERT(!res->next());

  res.reset(dbmeta->getBestRowIdentifier(con->getCatalog(), con->getSchema(), "bestIdUniqueNull", 0, false));
  ASSERT(!res->next());
}


void connectionmetadata::getSchemaCollation()
{
  logMsg("connectionmetadata::getSchemaCollation - MySQL_ConnectionMetaData::getSchemaCollation()");
  try
  {
  ResultSetMetaData * resmeta;
  DatabaseMetaData * dbmeta=con->getMetaData();

  stmt.reset(con->createStatement());
  stmt->execute("DROP DATABASE IF EXISTS collationTestDatabase");
  stmt->execute("CREATE DATABASE collationTestDatabase CHARACTER SET utf8 COLLATE utf8_bin");

  /* SchemaCollation */
  res.reset(dbmeta->getSchemaCollation(con->getCatalog(), "collationTestDatabase"));
  ASSERT(res->next());
  resmeta=res->getMetaData();
  ASSERT_EQUALS((unsigned int) 3, resmeta->getColumnCount());
  ASSERT_EQUALS("SCHEMA_CAT", resmeta->getColumnLabel(1));
  ASSERT_EQUALS("SCHEMA_NAME", resmeta->getColumnLabel(2));
  ASSERT_EQUALS("SCHEMA_COLLATION", resmeta->getColumnLabel(3));

  ASSERT(res->getString("SCHEMA_NAME").caseCompare("collationTestDatabase") == 0);
  ASSERT_EQUALS("utf8_bin", res->getString("SCHEMA_COLLATION"));

  stmt->execute("DROP DATABASE IF EXISTS collationTestDatabase");
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void connectionmetadata::getSchemaCharset()
{
  logMsg("connectionmetadata::getSchemaCharset - MySQL_ConnectionMetaData::getSchemaCharset()");
  try
  {
  ResultSetMetaData * resmeta;
  DatabaseMetaData * dbmeta=con->getMetaData();

  stmt.reset(con->createStatement());
  stmt->execute("DROP DATABASE IF EXISTS charsetTestDatabase");
  stmt->execute("CREATE DATABASE charsetTestDatabase CHARACTER SET utf8 COLLATE utf8_bin");

  /* SchemaCharset */
  res.reset(dbmeta->getSchemaCharset(con->getCatalog(), "charsetTestDatabase"));
  ASSERT(res->next());
  resmeta=res->getMetaData();
  ASSERT_EQUALS((unsigned int) 3, resmeta->getColumnCount());
  ASSERT_EQUALS("SCHEMA_CAT", resmeta->getColumnLabel(1));
  ASSERT_EQUALS("SCHEMA_NAME", resmeta->getColumnLabel(2));
  ASSERT_EQUALS("SCHEMA_CHARSET", resmeta->getColumnLabel(3));

  ASSERT(res->getString("SCHEMA_NAME").caseCompare("charsetTestDatabase") == 0);
  ASSERT_EQUALS("utf8", res->getString("SCHEMA_CHARSET"));

  stmt->execute("DROP DATABASE IF EXISTS charsetTestDatabase");
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void connectionmetadata::getTableCollation()
{
  logMsg("connectionmetadata::getTableCollation - MySQL_ConnectionMetaData::getTableCollation()");
  try
  {
  ResultSetMetaData * resmeta;
  DatabaseMetaData * dbmeta=con->getMetaData();

  stmt.reset(con->createStatement());
  stmt->execute("DROP DATABASE IF EXISTS collationTestDatabase");
  stmt->execute("CREATE DATABASE collationTestDatabase CHARACTER SET utf8 COLLATE utf8_bin");
  stmt->execute("DROP TABLE IF EXISTS collationTestDatabase.collationTestTable");
  stmt->execute("CREATE TABLE collationTestDatabase.collationTestTable(id INT) CHARACTER SET latin1 COLLATE latin1_general_ci");
  stmt->execute("DROP TABLE IF EXISTS collationTestDatabase.collationTestTableAnother");
  stmt->execute("CREATE TABLE collationTestDatabase.collationTestTableAnother(id INT) CHARACTER SET utf8 COLLATE utf8_bin");

  /* TableCollation */
  res.reset(dbmeta->getTableCollation(con->getCatalog(), "collationTestDatabase", "%collationTestTable%"));
  ASSERT(res->next());
  resmeta=res->getMetaData();
  ASSERT_EQUALS((unsigned int) 4, resmeta->getColumnCount());
  ASSERT_EQUALS("TABLE_CAT", resmeta->getColumnLabel(1));
  ASSERT_EQUALS("TABLE_SCHEMA", resmeta->getColumnLabel(2));
  ASSERT_EQUALS("TABLE_NAME", resmeta->getColumnLabel(3));
  ASSERT_EQUALS("TABLE_COLLATION", resmeta->getColumnLabel(4));

  ASSERT(res->getString("TABLE_SCHEMA").caseCompare("collationTestDatabase") == 0);
  ASSERT(res->getString("TABLE_NAME").caseCompare("collationTestTable") == 0);
  ASSERT_EQUALS("latin1_general_ci", res->getString("TABLE_COLLATION"));

  ASSERT(res->next());
  ASSERT(res->getString("TABLE_SCHEMA").caseCompare("collationTestDatabase") == 0);
  ASSERT(res->getString("TABLE_NAME").caseCompare("collationTestTableAnother") == 0);
  ASSERT_EQUALS("utf8_bin", res->getString("TABLE_COLLATION"));

  stmt->execute("DROP DATABASE IF EXISTS collationTestDatabase");
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void connectionmetadata::getTableCharset()
{
  logMsg("connectionmetadata::getTableCharset - MySQL_ConnectionMetaData::getTableCharset()");
  try
  {
  ResultSetMetaData * resmeta;
  DatabaseMetaData * dbmeta=con->getMetaData();

  stmt.reset(con->createStatement());
  stmt->execute("DROP DATABASE IF EXISTS charsetTestDatabase");
  stmt->execute("CREATE DATABASE charsetTestDatabase CHARACTER SET utf8 COLLATE utf8_bin");
  stmt->execute("DROP TABLE IF EXISTS charsetTestDatabase.charsetTestTable");
  stmt->execute("CREATE TABLE charsetTestDatabase.charsetTestTable(id INT) CHARACTER SET latin1 COLLATE latin1_general_ci");
  stmt->execute("DROP TABLE IF EXISTS charsetTestDatabase.charsetTestTableAnother");
  stmt->execute("CREATE TABLE charsetTestDatabase.charsetTestTableAnother(id INT) CHARACTER SET utf8 COLLATE utf8_bin");

  /* TableCharset */
  res.reset(dbmeta->getTableCharset(con->getCatalog(), "charsetTestDatabase", "%charsetTestTable%"));
  ASSERT(res->next());
  resmeta=res->getMetaData();
  ASSERT_EQUALS((unsigned int) 4, resmeta->getColumnCount());
  ASSERT_EQUALS("TABLE_CAT", resmeta->getColumnLabel(1));
  ASSERT_EQUALS("TABLE_SCHEMA", resmeta->getColumnLabel(2));
  ASSERT_EQUALS("TABLE_NAME", resmeta->getColumnLabel(3));
  ASSERT_EQUALS("TABLE_CHARSET", resmeta->getColumnLabel(4));

  ASSERT(res->getString("TABLE_SCHEMA").caseCompare("charsetTestDatabase") == 0);
  ASSERT(res->getString("TABLE_NAME").caseCompare("charsetTestTable") == 0);
  ASSERT_EQUALS("latin1", res->getString("TABLE_CHARSET"));

  ASSERT(res->next());
  ASSERT(res->getString("TABLE_SCHEMA").caseCompare("charsetTestDatabase") == 0);
  ASSERT(res->getString("TABLE_NAME").caseCompare("charsetTestTableAnother") == 0);
  ASSERT_EQUALS("utf8", res->getString("TABLE_CHARSET"));

  stmt->execute("DROP DATABASE IF EXISTS charsetTestDatabase");
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


void connectionmetadata::getTables()
{
  logMsg("connectionmetadata::getTables - MySQL_ConnectionMetaData::getTables()");

  //TODO: Enable it after fixing
  SKIP("Removed untill fixed");

  try
  {
  DatabaseMetaData * dbmeta=con->getMetaData();
  std::list< sql::SQLString > tableTypes;

  stmt.reset(con->createStatement());
  stmt->execute("DROP TABLE IF EXISTS testTable1");
  stmt->execute("CREATE TABLE testTable1(id INT)");
  stmt->execute("DROP VIEW IF EXISTS testView1");
  stmt->execute("CREATE VIEW testView1 AS SELECT * FROM testTable1");

  /* for tableType = TABLE */
  tableTypes.clear();
  tableTypes.push_back(sql::SQLString("TABLE"));
  res.reset(dbmeta->getTables("", "%", "testTable%", tableTypes));
  ASSERT(res->next());

  ASSERT_EQUALS(res->getString(3), "testTable1");
  ASSERT_EQUALS(res->getString(4), "TABLE");

  /* for tableType = VIEW */
  tableTypes.clear();
  tableTypes.push_back(sql::SQLString("VIEW"));
  res.reset(dbmeta->getTables("", "%", "testView%", tableTypes));
  ASSERT(res->next());

  ASSERT_EQUALS(res->getString(3), "testView1");
  ASSERT_EQUALS(res->getString(4), "VIEW");

    stmt->execute("DROP TABLE IF EXISTS testTable1");
  stmt->execute("DROP VIEW IF EXISTS testView1");
  }
  catch (sql::SQLException &e)
  {

    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }
}


} /* namespace connectionmetadata */
} /* namespace testsuite */
