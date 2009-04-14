/*
   Copyright 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "UnbufferedRsStmtTest.h"


namespace testsuite
{
namespace compliance
{

  UnbufferedRsStmtTest::UnbufferedRsStmtTest( const String & name ) : StatementTest( name )
  {
    logMsg( "UnbufferedRsStmtTest ctor called" );
  }

  void UnbufferedRsStmtTest::setUp()
  {
    logMsg( "UnbufferedRsStmtTest setUp called" );

    super::setUp();

    sql::ResultSet::enum_type unbuffered= sql::ResultSet::TYPE_FORWARD_ONLY;
    conn->setClientOption( "defaultStatementResultType", & unbuffered );

    stmt->setResultSetType( unbuffered );
  }

}
}
