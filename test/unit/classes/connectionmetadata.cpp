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

void connectionmetadata::init()
{
  /*
   ResultSet getAttributes(String catalog,
                        String schemaPattern,
                        String typeNamePattern,
                        String attributeNamePattern)
                        throws SQLException

   Retrieves a description of the given attribute of the given type for a user-defined type (UDT) that is available in the given schema and catalog.

Descriptions are returned only for attributes of UDTs matching the catalog, schema, type, and attribute name criteria. They are ordered by TYPE_CAT, TYPE_SCHEM, TYPE_NAME and ORDINAL_POSITION. This description does not contain inherited attributes.

The ResultSet object that is returned has the following columns:

   1. TYPE_CAT String => type catalog (may be null)
   2. TYPE_SCHEM String => type schema (may be null)
   3. TYPE_NAME String => type name
   4. ATTR_NAME String => attribute name
   5. DATA_TYPE int => attribute type SQL type from java.sql.Types
   6. ATTR_TYPE_NAME String => Data source dependent type name. For a UDT, the type name is fully qualified. For a REF, the type name is fully qualified and represents the target type of the reference type.
   7. ATTR_SIZE int => column size. For char or date types this is the maximum number of characters; for numeric or decimal types this is precision.
   8. DECIMAL_DIGITS int => the number of fractional digits. Null is returned for data types where DECIMAL_DIGITS is not applicable.
   9. NUM_PREC_RADIX int => Radix (typically either 10 or 2)
  10. NULLABLE int => whether NULL is allowed
   * attributeNoNulls - might not allow NULL values
   * attributeNullable - definitely allows NULL values
   * attributeNullableUnknown - nullability unknown
  11. REMARKS String => comment describing column (may be null)
  12. ATTR_DEF String => default value (may be null)
  13. SQL_DATA_TYPE int => unused
  14. SQL_DATETIME_SUB int => unused
  15. CHAR_OCTET_LENGTH int => for char types the maximum number of bytes in the column
  16. ORDINAL_POSITION int => index of the attribute in the UDT (starting at 1)
  17. IS_NULLABLE String => ISO rules are used to determine the nullability for a attribute.
   * YES --- if the attribute can include NULLs
   * NO --- if the attribute cannot include NULLs
   * empty string --- if the nullability for the attribute is unknown
  18. SCOPE_CATALOG String => catalog of table that is the scope of a reference attribute (null if DATA_TYPE isn't REF)
  19. SCOPE_SCHEMA String => schema of table that is the scope of a reference attribute (null if DATA_TYPE isn't REF)
  20. SCOPE_TABLE String => table name that is the scope of a reference attribute (null if the DATA_TYPE isn't REF)
  21. SOURCE_DATA_TYPE short => source type of a distinct type or user-generated Ref type,SQL type from java.sql.Types (null if DATA_TYPE isn't DISTINCT or user-generated REF)
   */
  attributes.push_back(udtattribute("TYPE_CAT", 0));
  attributes.push_back(udtattribute("TYPE_SCHEM", 0));
  attributes.push_back(udtattribute("TYPE_NAME", 0));
  attributes.push_back(udtattribute("ATTR_NAME", 0));
  attributes.push_back(udtattribute("DATA_TYPE", 0));
  attributes.push_back(udtattribute("ATTR_TYPE_NAME", 0));
  attributes.push_back(udtattribute("ATTR_SIZE", 0));
  attributes.push_back(udtattribute("DECIMAL_DIGITS", 0));
  attributes.push_back(udtattribute("NUM_PREC_RADIX", 0));
  attributes.push_back(udtattribute("NULLABLE", 0));
  attributes.push_back(udtattribute("REMARKS", 0));
  attributes.push_back(udtattribute("ATTR_DEF", 0));
  attributes.push_back(udtattribute("SQL_DATA_TYPE", 0));
  attributes.push_back(udtattribute("SQL_DATETIME_SUB", 0));
  attributes.push_back(udtattribute("CHAR_OCTET_LENGTH", 0));
  attributes.push_back(udtattribute("ORDINAL_POSITION", 0));
  attributes.push_back(udtattribute("IS_NULLABLE", 0));
  attributes.push_back(udtattribute("SCOPE_CATALOG", 0));
  attributes.push_back(udtattribute("SCOPE_SCHEMA", 0));
  attributes.push_back(udtattribute("SCOPE_TABLE", 0));
  attributes.push_back(udtattribute("SOURCE_DATA_TYPE", 0));
}

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
  std::stringstream msg;
  try
  {
    DatabaseMetaData dbmeta(con->getMetaData());

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id INT PRIMARY KEY NOT NULL)");

    msg.str("");
    msg << "... catalog = '" << con->getCatalog() << "' schema = '" << con->getSchema() << "'";
    msg << " table = 'test', scope = 0, nullable = false";
    logMsg(msg.str());

    ResultSet res(dbmeta->getBestRowIdentifier(con->getCatalog(), con->getSchema(), "test", 0, false));
    ASSERT_EQUALS(true, res->next());
    ASSERT_EQUALS(2, res->getInt(1));
    ASSERT_EQUALS(res->getInt(1), res->getInt("SCOPE"));
    ASSERT_EQUALS("id", res->getString(2));
    ASSERT_EQUALS(res->getInt(2), res->getInt("COLUMN_NAME"));    
    ASSERT_EQUALS(sql::DataType::INTEGER, res->getInt(3));
    ASSERT_EQUALS(res->getInt(3), res->getInt("DATA_TYPE"));     
    ASSERT_EQUALS("INT", res->getString(4));
    ASSERT_EQUALS(res->getString(4), res->getString("TYPE_NAME"));
    ASSERT_EQUALS(10, res->getInt(5));
    ASSERT_EQUALS(res->getInt(5), res->getInt("COLUMN_SIZE"));
    ASSERT_EQUALS(0, res->getInt(6));
    ASSERT_EQUALS(res->getInt(6), res->getInt("BUFFER_LENGTH"));
    ASSERT_EQUALS(0, res->getInt(7));
    ASSERT_EQUALS(res->getInt(7), res->getInt("DECIMAL_DIGITS"));
    ASSERT_EQUALS(sql::DatabaseMetaData::bestRowNotPseudo, res->getInt(8));
    ASSERT_EQUALS(res->getInt(8), res->getInt("PSEUDO_COLUMN"));

    stmt.reset(con->createStatement());
    stmt->execute("DROP TABLE IF EXISTS test");
    stmt->execute("CREATE TABLE test(id1 INT(1), id2 INT(1),  PRIMARY KEY (id1, id2))");

    msg.str("");
    msg << "... catalog = '" << con->getCatalog() << "' schema = '" << con->getSchema() << "'";
    msg << " table = 'test', scope = 0, nullable = false";
    logMsg(msg.str());

    ResultSet res2(dbmeta->getBestRowIdentifier(con->getCatalog(), con->getSchema(), "test", 0, false));
    while (res2->next())
    {
      ASSERT_EQUALS(2, res2->getInt(1));
      ASSERT_EQUALS(res2->getInt(1), res2->getInt("SCOPE"));
      ASSERT_EQUALS(res2->getInt(2), res2->getInt("COLUMN_NAME"));      
      ASSERT_EQUALS(sql::DataType::INTEGER, res2->getInt(3));
      ASSERT_EQUALS(res2->getInt(3), res2->getInt("DATA_TYPE"));
      ASSERT_EQUALS("INT", res2->getString(4));
      ASSERT_EQUALS(res2->getString(4), res2->getString("TYPE_NAME"));
      ASSERT_EQUALS(10, res2->getInt(5));
      ASSERT_EQUALS(res2->getInt(5), res2->getInt("COLUMN_SIZE"));
      ASSERT_EQUALS(0, res2->getInt(6));
      ASSERT_EQUALS(res2->getInt(6), res2->getInt("BUFFER_LENGTH"));
      ASSERT_EQUALS(0, res2->getInt(7));
      ASSERT_EQUALS(res2->getInt(7), res2->getInt("DECIMAL_DIGITS"));
      ASSERT_EQUALS(sql::DatabaseMetaData::bestRowNotPseudo, res2->getInt(8));
      ASSERT_EQUALS(res2->getInt(8), res2->getInt("PSEUDO_COLUMN"));
    }
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
