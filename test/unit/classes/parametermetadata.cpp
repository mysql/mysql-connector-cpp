
/*
   Copyright 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include <cppconn/prepared_statement.h>


#include <cppconn/connection.h>


#include <cppconn/warning.h>

#include "parametermetadata.h"
#include <stdlib.h>

namespace testsuite
{
namespace classes
{

void parametermetadata::InsertSelectAllTypes()
{
  logMsg("parametermetadata::InsertSelectAllTypes() - MySQL_ParameterMetaData::*");
  ParameterMetaData meta;
  PreparedStatement pstmt;

  try
  {
    pstmt.reset(con->prepareStatement("SELECT 1"));
    meta.reset(pstmt->getParameterMetaData());
  }
  catch (sql::SQLException &e)
  {
    logErr(e.what());
    logErr("SQLState: " + e.getSQLState());
    fail(e.what(), __FILE__, __LINE__);
  }
}


} /* namespace parametermetadata */
} /* namespace testsuite */


