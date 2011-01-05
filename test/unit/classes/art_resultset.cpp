/*
Copyright (c) 2009, 2011, Oracle and/or its affiliates. All rights reserved.

The MySQL Connector/C++ is licensed under the terms of the GPLv2
<http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
MySQL Connectors. There are special exceptions to the terms and
conditions of the GPLv2 as it is applied to this software, see the
FLOSS License Exception
<http://www.mysql.com/about/legal/licensing/foss-exception.html>.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/



#include "../../../driver/mysql_art_resultset.h"
#include "art_resultset.h"

namespace testsuite
{
namespace classes
{

void art_resultset::testMyVal()
{
  sql::mysql::MyVal valInt1(static_cast<int64_t> (0x0000000100000000LL));
  sql::mysql::MyVal valInt2(static_cast<int64_t> (0x0000000000000100LL));
  sql::mysql::MyVal valInt3(static_cast<int64_t> (0x8000000000000100LL));
  sql::mysql::MyVal valUInt1(static_cast<uint64_t> (0xfffffffffffffffeULL));
  sql::mysql::MyVal valDouble1(0.00001);
  sql::mysql::MyVal valDouble2(0.0);
  sql::mysql::MyVal valDouble3(2.49);
  sql::mysql::MyVal valBool1(true);
  sql::mysql::MyVal valBool2(false);
  sql::mysql::MyVal valStr1("131");
  sql::mysql::MyVal valStr2("foobar");
  sql::mysql::MyVal valStr3("true");
  sql::mysql::MyVal valPtr1(static_cast<void *> (NULL));
  sql::mysql::MyVal valPtr2(static_cast<void *> (&valInt1));

  ASSERT_EQUALS(true, valInt1.getBool());
  ASSERT_EQUALS("4294967296", valInt1.getString());
  ASSERT_EQUALS(true, valInt2.getBool());
  ASSERT_EQUALS_EPSILON(256., valInt2.getDouble(), 0.00001);
  ASSERT_EQUALS(9223372036854776064ULL, valInt3.getUInt64());
  ASSERT_EQUALS(-2LL, valUInt1.getInt64());
  ASSERT_EQUALS(true, valDouble1.getBool());
  ASSERT_EQUALS(false, valDouble2.getBool());
  ASSERT_EQUALS(2LL, valDouble3.getInt64());
  ASSERT_EQUALS(131LL, valStr1.getInt64());
  ASSERT_EQUALS(0LL, valStr2.getInt64());
  ASSERT_EQUALS(false, valStr3.getBool());
  ASSERT_EQUALS(false, valPtr1.getBool());
  ASSERT_EQUALS(true, valPtr2.getBool());
}

} /* namespace resultset */
} /* namespace testsuite */
