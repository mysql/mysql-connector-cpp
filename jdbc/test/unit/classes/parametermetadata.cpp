/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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

  try
  {
    pstmt.reset(con->prepareStatement("SELECT 1"));
    pstmt->getParameterMetaData();
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
  catch (sql::InvalidInstanceException&)
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
  catch (sql::InvalidInstanceException&)
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


