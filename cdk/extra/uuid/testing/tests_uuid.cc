/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
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
 * which is part of <MySQL Product>, is also subject to the
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

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <uuid_gen.h>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>

/*
  Example test UUID generator
*/

using namespace std;
using namespace uuid;

std::ostream& operator<<(std::ostream &out, unsigned char u)
{
  std::ostream::fmtflags saved_flags = out.flags();
  out << std::hex << std::noshowbase << std::setfill('0') << std::setw(2) <<(int)u;
  out.flags(saved_flags);
  return out;
}


std::ostream& operator<<(std::ostream &out, const unsigned char id[16])
{
  out << id[0] << id[1] << id[2] << id[3] << id[4] << id[5];
  out <<"-" << id[6] << id[7];
  out <<"-" << id[8] << id[9];
  out <<"-" << id[10] << id[11];
  out << id[12] << id[13] << id[14] << id[15];

  return out;
}


int main(int argc, char **argv)
{
  std::cout << "UUID Generator:" << std::endl;

  int iter_number= 20;

  int i = 0;
  uuid_type uuid;

  while(i < iter_number)
  {
    set_seed_from_time_pid();
    generate_uuid(uuid);
    std::cout << "UUID_" << std::setw(4) << std::setfill('0') << ++i << " " << uuid << std::endl;
#ifdef _WIN32
    Sleep(100);
#else
    usleep(100000);
#endif
  }
  std::cout << "DONE!" << std::endl;
}
