/*
  Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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

std::ostream& operator<<(std::ostream &out, unsigned char u)
{
  std::ostream::fmtflags saved_flags = out.flags();
  out << std::hex << std::noshowbase << std::setfill('0') << std::setw(2) <<(int)u;
  out.flags(saved_flags);
  return out;
}


std::ostream& operator<<(std::ostream &out, const unsigned char id[16])
{
  out << id[0] << id[1] << id[2] << id[3];
  out <<"-" << id[4] << id[5];
  out <<"-" << id[6] << id[7];
  out <<"-" << id[8] << id[9];
  out << id[10] << id[11] << id[12] << id[13] << id[14] << id[15];

  return out;
}


int main(int argc, char **argv)
{
  init_uuid(365873);

  std::cout << "UUID Generator:" << std::endl << "----------------------" << std::endl;

  int iter_number= 100;

  if (argc > 1)
    iter_number = std::atoi(argv[1]);
  else
    std::cout << "Optional number of iterations can be specified in the command line" << std::endl;

  std::cout << "Number of iterations: " << iter_number << std::endl << std::endl;

  int i = 0;
  uuid_type uuid;

  while(i < iter_number)
  {
    generate_uuid(uuid);
    std::cout << "UUID_" << std::setw(4) << std::setfill('0') << ++i << " " << uuid << std::endl;
#ifdef _WIN32
    Sleep(100);
#else
    usleep(100000);
#endif
  }
  std::cout << "DONE!" << std::endl;

  end_uuid();
  return 0;
}
