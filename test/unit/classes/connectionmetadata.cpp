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
  bool got_warning=false;
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
      catch (sql::SQLException &)
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
        got_warning=true;
      }
      // TODO - ASSERT_EQUALS(it->ctype, res->getInt(3));
      ASSERT_EQUALS(res->getInt(3), res->getInt("DATA_TYPE"));

      if (it->name != res->getString(4))
      {
        msg.str("");
        msg << "... WARNING - check DATA_TYPE for " << it->sqldef;
        msg << " - expecting type name " << it->name << " got " << res->getString(4);
        logMsg(msg.str());
        got_warning=true;
      }
      // TODO - ASSERT_EQUALS(it->name, res->getString(4));
      ASSERT_EQUALS(res->getString(4), res->getString("TYPE_NAME"));

      if (it->precision != res->getUInt64(5))
      {
        msg.str("");
        msg << "... WARNING - check COLUMN_SIZE for " << it->sqldef;
        msg << " - expecting pecision " << it->precision << " got " << res->getInt(5);
        logMsg(msg.str());
        got_warning=true;
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

    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(col1 INT NOT NULL, col2 INT NOT NULL, PRIMARY KEY(col1, col2))");


  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getColumnPrivileges()
{
  logMsg("connectionmetadata::getColumnPrivileges() - MySQL_ConnectionMetaData::getColumnPrivileges");
  int rows=0;
  try
  {

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(col1 INT, col2 INT)");
    DatabaseMetaData dbmeta(con->getMetaData());

    res.reset(dbmeta->getColumnPrivileges(con->getCatalog(), con->getSchema(), "test", "id"));
    ASSERT_EQUALS(false, res->next());

    res.reset(dbmeta->getColumnPrivileges(con->getCatalog(), con->getSchema(), "test", "col%"));
    rows=0;
    while (res->next())
    {
      rows++;

      ASSERT_EQUALS(con->getCatalog(), res->getString(1));
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

    res.reset(dbmeta->getColumnPrivileges(con->getCatalog(), con->getSchema(), "test", "col2"));
    ASSERT_EQUALS(true, res->next());
    ASSERT_EQUALS("col2", res->getString("COLUMN_NAME"));
    ASSERT_EQUALS(res->getString(4), res->getString("COLUMN_NAME"));

    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getColumns()
{
  logMsg("connectionmetadata::getColumn() - MySQL_ConnectionMetaData::getColumns");
  std::vector<columndefinition>::iterator it;
  std::stringstream msg;
  bool got_warning=false;
  try
  {
    DatabaseMetaData dbmeta(con->getMetaData());
    stmt.reset(con->createStatement());

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
      ASSERT_EQUALS(true, res->next());
      ASSERT_EQUALS(con->getCatalog(), res->getString("TABLE_CAT"));
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
        msg << "... WARNING - check DATA_TYPE for " << it->sqldef;
        msg << " - expecting type " << it->ctype << " got " << res->getInt("DATA_TYPE");
        logMsg(msg.str());
        got_warning=true;
      }
      // ASSERT_EQUALS(it->ctype, res->getInt("DATA_TYPE"));
      ASSERT_EQUALS(res->getInt(5), res->getInt("DATA_TYPE"));

      if (it->name != res->getString("TYPE_NAME"))
      {
        msg.str("");
        msg << "... WARNING - check TYPE_NAME for " << it->sqldef;
        msg << " - expecting type " << it->name << " got " << res->getString("TYPE_NAME");
        logMsg(msg.str());
        got_warning=true;
      }
      // ASSERT_EQUALS(it->name, res->getString("TYPE_NAME"));
      ASSERT_EQUALS(res->getString(6), res->getString("TYPE_NAME"));

      if (it->precision != res->getUInt64(7))
      {
        msg.str("");
        msg << "... WARNING - check COLUMN_SIZE for " << it->sqldef;
        msg << " - expecting pecision " << it->precision << " got " << res->getUInt64(7);
        logMsg(msg.str());
        got_warning=true;
      }
      // TODO - ASSERT_EQUALS(it->precision, res->getInt(7));
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
        msg << "... WARNING - check NULLABLE for " << it->sqldef;
        msg << " - expecting nullable = " << it->nullable << " got " << res->getInt(11);
        msg << " columnNoNull = " << sql::DatabaseMetaData::columnNoNulls << ", ";
        msg << " columnNullable = " << sql::DatabaseMetaData::columnNullable << ", ";
        msg << " columnNullableUnknown = " << sql::DatabaseMetaData::columnNullableUnknown;
        logMsg(msg.str());
        got_warning=true;
      }
      ASSERT_EQUALS(it->nullable, res->getInt(11));
      ASSERT_EQUALS(res->getInt(11), res->getInt("NULLABLE"));
      ASSERT_EQUALS(it->remarks, res->getString(12));
      ASSERT_EQUALS(res->getString(12), res->getString("REMARKS"));
      ASSERT_EQUALS(it->column_def, res->getString(13));
      ASSERT_EQUALS(res->getString(13), res->getString("COLUMN_DEF"));
      ASSERT_EQUALS(res->getInt(14), res->getInt("SQL_DATA_TYPE"));
      ASSERT_EQUALS(res->getInt(15), res->getInt("SQL_DATETIME_SUB"));
      if (it->char_octet_length != 0 && (it->char_octet_length != res->getUInt64(16)))
      {
        msg.str("");
        msg << "... WARNING - check CHAR_OCTET_LENGTH for " << it->sqldef;
        msg << " - expecting char_octet_length " << it->char_octet_length << " got " << res->getUInt64(16);
        logMsg(msg.str());
        got_warning=true;
      }
      ASSERT_EQUALS(res->getUInt64(16), res->getUInt64("CHAR_OCTET_LENGTH"));

      ASSERT_EQUALS(2, res->getInt(17));
      ASSERT_EQUALS(res->getInt(17), res->getInt("ORDINAL_POSITION"));

      if (((it->nullable == sql::DatabaseMetaData::columnNoNulls) && (res->getString(18) != "NO")) ||
          ((it->nullable == sql::DatabaseMetaData::columnNullable) && (res->getString(18) != "YES")) ||
          ((it->nullable == sql::DatabaseMetaData::columnNullableUnknown) && (res->getString(18) != "")))
      {
        msg.str("");
        msg << "... WARNING - check IS_NULLABLE for " << it->sqldef;
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
      FAIL("See Warnings!");

    stmt->execute("DROP TABLE IF EXISTS test");
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getConnection()
{
  logMsg("connectionmetadata::getConnection() - MySQL_ConnectionMetaData::getConnection");
  sql::Connection* same_con;
  try
  {
    stmt.reset(con->createStatement());
    stmt->execute("SET @this_is_my_connection_id=101");
    DatabaseMetaData dbmeta(con->getMetaData());
    same_con=dbmeta->getConnection();
    stmt.reset(same_con->createStatement());
    res.reset(stmt->executeQuery("SELECT @this_is_my_connection_id AS _connection_id"));
    ASSERT(res->next());
    ASSERT_EQUALS(101, res->getInt("_connection_id"));
    ASSERT_EQUALS(res->getInt(1), res->getInt("_connection_id"));
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getDatabaseVersions()
{
  logMsg("connectionmetadata::getDatabaseVersions() - MySQL_ConnectionMetaData::getDatabase[Minor|Major|Patch]Version()");
  std::stringstream prodversion;
  try
  {
    DatabaseMetaData dbmeta(con->getMetaData());
    ASSERT_GT(2, dbmeta->getDatabaseMajorVersion());
    ASSERT_LT(7, dbmeta->getDatabaseMajorVersion());
    ASSERT_GT(-1, dbmeta->getDatabaseMinorVersion());
    ASSERT_LT(100, dbmeta->getDatabaseMinorVersion());
    ASSERT_GT(-1, dbmeta->getDatabasePatchVersion());
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
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getDriverVersions()
{
  logMsg("connectionmetadata::getDriverVersions() - MySQL_ConnectionMetaData::getDriver[Minor|Major|Patch]Version()");
  std::stringstream prodversion;
  try
  {
    DatabaseMetaData dbmeta(con->getMetaData());
    ASSERT_GT(0, dbmeta->getDriverMajorVersion());
    ASSERT_LT(2, dbmeta->getDriverMajorVersion());
    ASSERT_GT(-1, dbmeta->getDriverMinorVersion());
    ASSERT_LT(100, dbmeta->getDriverMinorVersion());
    ASSERT_GT(-1, dbmeta->getDriverPatchVersion());
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
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getDefaultTransactionIsolation()
{
  logMsg("connectionmetadata::getDefaultTransactionIsolation() - MySQL_ConnectionMetaData::getDefaultTransactionIsolation()");
  int server_version;
  try
  {
    DatabaseMetaData dbmeta(con->getMetaData());

    server_version=(10000 * dbmeta->getDatabaseMajorVersion())
            + (100 * dbmeta->getDriverMinorVersion())
            + dbmeta->getDriverPatchVersion();

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
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getExtraNameCharacters()
{
  logMsg("connectionmetadata::getExtraNameCharacters() - MySQL_ConnectionMetaData::getExtraNameCharacters()");
  try
  {
    DatabaseMetaData dbmeta(con->getMetaData());
    ASSERT_EQUALS("#@", dbmeta->getExtraNameCharacters());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}

void connectionmetadata::getIdentifierQuoteString()
{
  logMsg("connectionmetadata::getIdentifierQuoteString() - MySQL_ConnectionMetaData::getIdentifierQuoteString()");
  bool can_set_sqlmode=false;
  try
  {
    DatabaseMetaData dbmeta(con->getMetaData());
    stmt.reset(con->createStatement());
    try
    {
      stmt->execute("SET @@sql_mode = ''");
      res.reset(stmt->executeQuery("SELECT @@sql_mode AS _sql_mode"));
      ASSERT(res->next());
      ASSERT_EQUALS("", res->getString("_sql_mode"));
      can_set_sqlmode=true;
    }
    catch (sql::SQLException &e)
    {
      logMsg("Cannot set SQL_MODE, skipping test");
    }
    if (can_set_sqlmode)
    {
      ASSERT_EQUALS("`", dbmeta->getIdentifierQuoteString());
      stmt->execute("SET @@sql_mode = 'ANSI_QUOTES,ALLOW_INVALID_DATES'");
      res.reset(stmt->executeQuery("SELECT @@sql_mode AS _sql_mode"));
      ASSERT(res->next());
      ASSERT_EQUALS("ANSI_QUOTES,ALLOW_INVALID_DATES", res->getString("_sql_mode"));
      ASSERT_EQUALS("\"", dbmeta->getIdentifierQuoteString());
    }
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }

}

void connectionmetadata::getImportedKeys()
{
  logMsg("connectionmetadata::getImportedKeys() - MySQL_ConnectionMetaData::getImportedKeys()");
  bool can_create_pk=false;
  try
  {
    DatabaseMetaData dbmeta(con->getMetaData());
    stmt.reset(con->createStatement());

    stmt->execute("DROP TABLE IF EXISTS child");
    stmt->execute("DROP TABLE IF EXISTS parent");
    try
    {
      stmt->execute("CREATE TABLE parent(pid INT NOT NULL, PRIMARY KEY(pid)) ENGINE=INNODB;");
      stmt->execute("CREATE TABLE child(cid INT NOT NULL, cpid INT, "
                    "INDEX idx_parent_id(cpid), FOREIGN KEY idx_parent_id(cpid) "
                    "REFERENCES parent(pid) ON DELETE CASCADE ON UPDATE CASCADE, PRIMARY KEY(cid)) ENGINE=INNODB;");
      can_create_pk=true;
    }
    catch (sql::SQLException &)
    {
      logMsg("... cannot create necessary FK tables");
    }

    if (can_create_pk)
    {
      int num_res = 0;

      res.reset(dbmeta->getImportedKeys(con->getCatalog(), con->getSchema(), "parent"));
      ASSERT(!res->next());

      res.reset(dbmeta->getImportedKeys(con->getCatalog(), con->getSchema(), "child"));
      ASSERT(res->next());
      ASSERT_EQUALS(con->getCatalog(), res->getString(1));
      ASSERT_EQUALS(res->getString(1), res->getString("PKTABLE_CAT"));

      ASSERT_EQUALS(con->getSchema(), res->getString(2));
      ASSERT_EQUALS(res->getString(2), res->getString("PKTABLE_SCHEM"));

      ASSERT_EQUALS("parent", res->getString(3));
      ASSERT_EQUALS(res->getString(3), res->getString("PKTABLE_NAME"));

      ASSERT_EQUALS("pid", res->getString(4));
      ASSERT_EQUALS(res->getString(4), res->getString("PKCOLUMN_NAME"));

      ASSERT_EQUALS(con->getCatalog(), res->getString(5));
      ASSERT_EQUALS(res->getString(5), res->getString("FKTABLE_CAT"));

      ASSERT_EQUALS(con->getSchema(), res->getString(6));
      ASSERT_EQUALS(res->getString(6), res->getString("FKTABLE_SCHEM"));

      ASSERT_EQUALS("child", res->getString(7));
      ASSERT_EQUALS(res->getString(7), res->getString("FKTABLE_NAME"));

      ASSERT_EQUALS("cpid", res->getString(8));
      ASSERT_EQUALS(res->getString(8), res->getString("FKCOLUMN_NAME"));

      ASSERT_EQUALS(1, res->getInt(9));
      ASSERT_EQUALS(res->getInt(9), res->getInt("KEY_SEQ"));

      ASSERT_EQUALS((int64_t) sql::DatabaseMetaData::importedKeyCascade, res->getInt64(10));
      ASSERT_EQUALS(res->getInt64(10), res->getInt64("UPDATE_RULE"));

      ASSERT(sql::DatabaseMetaData::importedKeyNoAction != res->getInt64(10));
      ASSERT(sql::DatabaseMetaData::importedKeySetNull != res->getInt64(10));
      ASSERT(sql::DatabaseMetaData::importedKeySetDefault != res->getInt64(10));
      ASSERT(sql::DatabaseMetaData::importedKeyRestrict != res->getInt64(10));

      ASSERT_EQUALS((int64_t) sql::DatabaseMetaData::importedKeyCascade, res->getInt64(11));
      ASSERT_EQUALS(res->getInt64(11), res->getInt64("DELETE_RULE"));

      ASSERT(sql::DatabaseMetaData::importedKeyNoAction != res->getInt64(11));
      ASSERT(sql::DatabaseMetaData::importedKeySetNull != res->getInt64(11));
      ASSERT(sql::DatabaseMetaData::importedKeySetDefault != res->getInt64(11));
      ASSERT(sql::DatabaseMetaData::importedKeyRestrict != res->getInt64(11));

      // InnoDB should give the FK a name
      ASSERT("" != res->getString("FK_NAME"));
      ASSERT_EQUALS(res->getString(12), res->getString("FK_NAME"));

      // TODO - not sure what value to expect
//      ASSERT_EQUALS("", res->getString("PK_NAME"));
//      ASSERT_EQUALS(res->getString(13), res->getString("PK_NAME"));

      ASSERT_EQUALS((int64_t)sql::DatabaseMetaData::importedKeyNotDeferrable, res->getInt64(14));
      ASSERT(sql::DatabaseMetaData::importedKeyInitiallyDeferred != res->getInt64(10));
      ASSERT(sql::DatabaseMetaData::importedKeyInitiallyImmediate != res->getInt64(10));

      ASSERT(!res->next());

      stmt->execute("DROP TABLE IF EXISTS child");
      stmt->execute("DROP TABLE IF EXISTS parent");
      stmt->execute("CREATE TABLE parent(pid1 INT NOT NULL, pid2 INT NOT NULL, PRIMARY KEY(pid1, pid2)) ENGINE=INNODB;");
      stmt->execute("CREATE TABLE child(cid INT NOT NULL, cpid2 INT, cpid1 INT, "
                    "INDEX idx_parent_id(cpid1, cpid2), FOREIGN KEY idx_parent_id(cpid1, cpid2) "
                    "REFERENCES parent(pid1, pid2) ON DELETE CASCADE ON UPDATE CASCADE, PRIMARY KEY(cid)) ENGINE=INNODB;");

      res.reset(dbmeta->getImportedKeys(con->getCatalog(), con->getSchema(), "child"));
      num_res = 0;
      while (res->next()) {
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
    }
    stmt->execute("DROP TABLE IF EXISTS child");
    stmt->execute("DROP TABLE IF EXISTS parent");
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
