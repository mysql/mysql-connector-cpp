/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * This code is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

/**
  Unit tests for Diagnostic_arena class.

  - adding entries and iterating over stored entries (different severity
    levels)

  - get error

  - description as cdk::Error and as std::exception

  - throwing entries from arena
*/

#include "test.h"
#include <iostream>
#include <mysql/cdk/foundation/diagnostics.h>

using ::std::cout;
using ::std::endl;
using namespace cdk::foundation;
typedef api::Severity Severity;

void add_diag_entry(Diagnostic_arena &da, Severity::value level,
                    const string &msg)
{
  try {
    throw_error(msg);
  }
  catch (Error &e)
  {
    da.add_entry(level, e.clone());
  }
}


TEST(Foundation, Diagnostic_arena)
try {
  Diagnostic_arena da;

  add_diag_entry(da, Severity::INFO, L"info entry");
  add_diag_entry(da, Severity::WARNING, L"warning entry");

  EXPECT_EQ(1U, da.entry_count(Severity::INFO));
  EXPECT_EQ(1U, da.entry_count(Severity::WARNING));
  EXPECT_EQ(0U, da.entry_count(Severity::ERROR));

  // try to get error entry while none is present

  EXPECT_THROW(da.get_error(), Error)
     <<"get_error() should throw error when there is no error"
       " in diagnostic arena";

  add_diag_entry(da, Severity::ERROR, L"error entry");
  add_diag_entry(da, Severity::WARNING, L"second warning entry");
  add_diag_entry(da, Severity::ERROR, L"second error entry");

  EXPECT_EQ(1U, da.entry_count(Severity::INFO));
  EXPECT_EQ(2U, da.entry_count(Severity::WARNING));
  EXPECT_EQ(2U, da.entry_count(Severity::ERROR));

  // This iterates over ERRORS only
  Diagnostic_arena::Iterator it = da.get_entries();

  const Error &err= da.get_error();

  cout <<"First error: " <<err <<endl;
  EXPECT_EQ(string("error entry"), err.description());
  EXPECT_EQ(std::string("CDK Error: error entry"), err.what());

  unsigned int count;

  cout <<"Errors in the arena:" <<endl;
  for (count=0; it.next(); ++count)
  {
    cout <<"  Entry: " <<it.entry() <<endl;
  }
  EXPECT_EQ(2U,count);

  cout <<"Errors and warnings in the arena:" <<endl;
  Diagnostic_arena::Iterator &it1 = da.get_entries(Severity::WARNING);
  it = it1;
  for (count=0; it.next(); ++count)
  {
    cout <<"  Entry: " <<it.entry() <<endl;
  }
  EXPECT_EQ(4U,count);

  cout <<"All entries in the arena:" <<endl;
  it= da.get_entries(Severity::INFO);
  for (count=0; it.next(); ++count)
  {
    cout <<"  Entry: " <<it.entry() <<endl;
  }
  EXPECT_EQ(5U,count);

  try
  {
    Diagnostic_arena da1;
    da1.add_entry(Severity::WARNING, da.get_error().clone());
    it= da1.get_entries(Severity::WARNING);
    EXPECT_TRUE(it.next());
    throw it.entry();
  }
  catch (Error &e)
  {
    cout <<"Caught entry from diagnostic arena: " <<e <<endl;
  }

  da.clear();
  EXPECT_EQ(0U, da.entry_count(Severity::INFO));
  EXPECT_EQ(0U, da.entry_count(Severity::WARNING));
  EXPECT_EQ(0U, da.entry_count(Severity::ERROR));

  it= da.get_entries(Severity::INFO);
  while (it.next())
  {
    FAIL() <<"Arena should be empty after clear()";
  }

}
catch (Error &e)
{
  FAIL() <<"CDK ERROR: " <<e;
}
catch (std::exception &e)
{
  FAIL() <<"std exception: " <<e.what();
}

