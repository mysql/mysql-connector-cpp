/*
 * Copyright (c) 2017, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C++ is licensed under the terms of the GPLv2
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


#ifndef MYSQLX_GLOBAL_H
#define MYSQLX_GLOBAL_H

#include "cdk/extra/uuid/include/uuid_gen.h"

namespace mysqlx {

/*
  Wrapper around uuid generator which ensures that it is properly
  initialized using process id (so that concurrent processes use
  different UUIDs).
*/

inline
void generate_uuid(uuid::uuid_type &buf)
{
  /*
    Note: This static initializer instance will be constructed
    only once.
  */
  static struct Initializer
  {
    Initializer()
    {
      uuid::set_seed_from_time_pid();
    }
  }
  uuid_init;

  uuid::generate_uuid(buf);
}

}

#endif
