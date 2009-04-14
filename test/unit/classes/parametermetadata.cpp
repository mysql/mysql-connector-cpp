
/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include <cppconn/prepared_statement.h>
#include <cppconn/parameter_metadata.h>

#include <cppconn/connection.h>


#include <cppconn/warning.h>

#include "parametermetadata.h"
#include <stdlib.h>

namespace testsuite
{
namespace classes
{

void parametermetadata::getMeta()
{
  logMsg("parametermetadata::InsertSelectAllTypes() - MySQL_ParameterMetaData::*");
  ParameterMetaData * parameta;

  try
  {
    pstmt.reset(con->prepareStatement("SELECT 1"));
    parameta = pstmt->getParameterMetaData();
    pstmt->close();
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  try
  {
    pstmt->getParameterMetaData();
    FAIL("Closed connection not detected");
  }
  catch (sql::InvalidInstanceException)
  {
  }

}

void parametermetadata::getParameterCount()
{
  logMsg("parametermetadata::getParameterCount() - MySQL_ParameterMetaData::getParameterCount");
  ParameterMetaData * parameta;

  try
  {
    pstmt.reset(con->prepareStatement("SELECT 1"));
    parameta = (pstmt->getParameterMetaData());
    parameta->getParameterCount();
    pstmt->close();
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  try
  {

    pstmt->getParameterMetaData();
    FAIL("Closed connection not detected");
  }
  catch (sql::InvalidInstanceException)
  {
  }

}


#ifdef INCLUDE_NOT_IMPLEMENTED_METHODS
void parametermetadata::notImplemented()
{
  logMsg("parametermetadata::notImplemented() - MySQL_ParameterMetaData::*");
  ParameterMetaData * parameta;

  try
  {
    pstmt.reset(con->prepareStatement("SELECT 1"));
    parameta = (pstmt->getParameterMetaData());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + std::string(e.getSQLState()));
    fail(e.what(), __FILE__, __LINE__);
  }

  try
  {
    parameta->getParameterClassName(1);
    FAIL("API change");
  }
  catch (sql::MethodNotImplementedException)
  {
  }

  try
  {
    parameta->getParameterMode(1);
    FAIL("API change");
  }
  catch (sql::MethodNotImplementedException)
  {
  }

  try
  {
    parameta->getParameterType(1);
    FAIL("API change");
  }
  catch (sql::MethodNotImplementedException)
  {
  }

  try
  {
    parameta->getParameterTypeName(1);
    FAIL("API change");
  }
  catch (sql::MethodNotImplementedException)
  {
  }

  try
  {
    parameta->getPrecision(1);
    FAIL("API change");
  }
  catch (sql::MethodNotImplementedException)
  {
  }

  try
  {
    parameta->getScale(1);
    FAIL("API change");
  }
  catch (sql::MethodNotImplementedException)
  {
  }

  try
  {
    parameta->isNullable(1);
    FAIL("API change");
  }
  catch (sql::MethodNotImplementedException)
  {
  }

  try
  {
    parameta->isSigned(1);
    FAIL("API change");
  }
  catch (sql::MethodNotImplementedException)
  {
  }

}
#endif

} /* namespace parametermetadata */
} /* namespace testsuite */


