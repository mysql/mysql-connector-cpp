/*
 * Copyright (c) 2008, 2023, Oracle and/or its affiliates. All rights reserved.
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

#include "mysql_telemetry.h"
#include <iostream>

#if defined(WIN32)
#include <Psapi.h>
static const std::string otel_lib_name = "opentelemetry_common.dll";
#else
#include <link.h>
#include <iostream>
#include <dlfcn.h>
static const std::string otel_lib_name = "libopentelemetry_common.so";
#endif


namespace sql
{
namespace mysql
{

static int otel_libs_found = -1;

bool check_process_otel_libs()
{
  if (otel_libs_found > -1)
    return otel_libs_found > 0;
    
  otel_libs_found = 0;

#if defined(WIN32)
#else
  dl_iterate_phdr(
    [](struct dl_phdr_info* info, size_t size, void* data) {
      if (otel_libs_found)
        return 0;
        
      if (info->dlpi_name)
      {
        std::string s = info->dlpi_name;
        size_t pos = s.find_last_of('/');

        if (pos == std::string::npos)
          return 0;

        if (s.find_last_of(otel_lib_name, pos) != std::string::npos)
          otel_libs_found = 1;
      }

      return 0;
    } , nullptr);
#endif
  return otel_libs_found > 0;
}

}
}