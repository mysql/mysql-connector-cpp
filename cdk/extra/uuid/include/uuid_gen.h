/*
  Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.

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

#ifndef _UUID_GEN_H_
#define _UUID_GEN_H_

#include <stdint.h>

#define UUID_LENGTH_BIN 16

namespace uuid {

typedef unsigned char uuid_type[UUID_LENGTH_BIN];

/* The seed must be set before using the generator. */
void set_seed(uint16_t seed);

/* Convenience function, which sets the seed using the time and
   process id */
void set_seed_from_time_pid();

/* UUID generator */
void generate_uuid(uuid_type &uuid);

} // namespace uuid

#endif