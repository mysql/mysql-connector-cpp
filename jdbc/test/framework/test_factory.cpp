/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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
