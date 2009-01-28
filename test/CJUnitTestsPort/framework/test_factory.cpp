/* Copyright 2008 - 2009 Sun Microsystems, Inc.

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

  return TestSuiteFactory::theInstance().testSuites.size();
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
