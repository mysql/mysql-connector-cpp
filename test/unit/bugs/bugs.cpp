/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "bugs.h"

namespace testsuite
{
namespace regression
{

void bugs::net_write_timeout39878()
{
  logMsg( "Test for #39878 - not fixed. And looks like there is no way to fix it at the moment." );
  logMsg( "If the test doesn't fail - that rather means that you should tweak parameters for your system" );
  logMsg( "You can do that with command line parameters:");
  logMsg( " --nwtTimeout=<val> for the value net_write timeout to set to (default 1)" );
  logMsg( " --nwtPause=<val> for the length of pause test shoud take while fetching rows (default 2)" );
  logMsg( " --nwtRows=<val> for the number of rows to insert into table (default 9230)" );

  int timeout=  1;
  int pause=    2;
  int rows=     9230;

  int tmp= TestsRunner::getStartOptions()->getInt( "nwtTimeout" );
  if ( tmp > 0 )
    timeout= tmp;


  tmp= TestsRunner::getStartOptions()->getInt( "nwtPause" );
  if ( tmp > 0 )
      pause= tmp;

  tmp= TestsRunner::getStartOptions()->getInt( "nwtRows" );
  if ( tmp > 0 )
      rows= tmp;


  pstmt.reset( con->prepareStatement( "set net_write_timeout=?" ) );
  pstmt->setInt( 1, timeout );
  pstmt->execute();

  res.reset( stmt->executeQuery( "show variables like \"net_write_timeout\"" ) );

  res->next();

  TestsListener::messagesLog() << "We've set net_write_timeout to " << res->getString(2) << std::endl;


  stmt->execute( "drop table if exists bug39878" );
  stmt->execute( "create table bug39878 (id int unsigned not null)" );

  stmt->execute( "lock table bug39878 write" );

  pstmt.reset( con->prepareStatement( "insert into bug39878 ( id ) values( ? )" ) );

  for (int i=1; i <= rows; ++i )
  {
      pstmt->setInt( 1, i );
      pstmt->execute();
  }

  stmt->execute( "unlock tables" );


  res.reset( stmt->executeQuery( "select count(*) from bug39878" ) );

  res->next();

  uint32_t rowsCount= res->getUInt(1);

  TestsListener::messagesLog() << "Now we have " << rowsCount << " rows in the table" << std::endl;

  // Must set ResultSet Type  to TYPE_FORWARD_ONLY
  stmt->setResultSetType( sql::ResultSet::TYPE_FORWARD_ONLY );
  
  res.reset( stmt->executeQuery( "select * from bug39878" ) );

  TestsListener::messagesLog() << "ResultSetType: " << stmt->getResultSetType() << std::endl;

  uint32_t rowsRead= 0;

  try
  {
      while ( res->next() )
      {
          if ( rowsRead == 0 )
          {
              TestsListener::messagesLog() << "Pause " << pause << "s." << std::endl;
              SLEEP(pause);
          }

          ++rowsRead;
      }
  }
  catch (sql::SQLException& /*e*/)
  {
      //if ( e.errNo == 2006 )

      /* let's think that exception is a god result itself.
         also not sure which errno should it be */

      return;
  }

  TestsListener::messagesLog() << "We've fetched " << rowsRead << " rows." << std::endl;

  try
  {
      stmt->execute( "drop table if exists bug39878" );
  }
  catch ( sql::SQLException& e )
  {
      // Expected, that Server has gone away
      logMsg( e.what() );

      // Lazy way to reconnect
  	  setUp();

      stmt->execute( "drop table if exists bug39878" );
  }

  ASSERT_EQUALS( rowsCount, rowsRead );
}


} /* namespace regression */
} /* namespace testsuite */
