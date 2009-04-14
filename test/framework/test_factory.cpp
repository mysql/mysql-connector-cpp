/*
   Copyright 2008 - 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 */

#include "test_factory.h"

namespace testsuite
{

TestSuiteFactory::TestSuiteFactory()
{
}


Test *  TestSuiteFactory::createTest( const String::value_type * name )
{
  TestSuites::iterator it= testSuites.find( name );

  if ( it != testSuites.end() )
  {
    // if Object hasn't yet been created...
    if ( it->second.second == NULL )
      it->second.second= new StorableTest( *(it->second.first( it->first )) );

    //ASSERT( it->second.second );
    return it->second.second->get();//it->second(it->first);
  }

  return NULL;
}

int TestSuiteFactory::RegisterTestSuite(const String::value_type * name, TestSuiteCreator creator)
{
  TestSuiteFactory::theInstance().testSuites.insert(std::make_pair(name
    , std::make_pair(creator, static_cast<TestContainer::StorableTest*>(NULL) ) ) );

  return static_cast<int>(TestSuiteFactory::theInstance().testSuites.size());
}


List::size_type TestSuiteFactory::getTestsList( std::vector<const String::value_type *> & list ) const
{
  for ( TestSuites::const_iterator cit= testSuites.begin(); cit != testSuites.end(); ++cit )
  {
    list.push_back( cit->first );
  }

  return list.size();
}

TestSuiteFactory::~TestSuiteFactory()
{
  for ( TestSuites::iterator it= testSuites.begin(); it != testSuites.end(); ++it )
  {
    delete it->second.second;
  }
}

} // namespace testsuite
