/*
 * Copyright (c) 2009, 2018, Oracle and/or its affiliates. All rights reserved.
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



#ifndef _MYSQL_LIB_LOADER_H_
#define _MYSQL_LIB_LOADER_H_

#include "binding_config.h"

#ifdef _WIN32
#include <windows.h>
#elif defined(HAVE_DLFCN_H)
# include <dlfcn.h>
#else
# error This should never happen - if this header is included, one of macros above supposed to be defined.
#endif

#include <boost/noncopyable.hpp>
#include <map>

namespace sql
{
namespace mysql
{
namespace util
{

#ifdef _WIN32
typedef HMODULE ModuleHandle;
typedef FARPROC SymbolHandle;
#else
typedef void *  ModuleHandle;
typedef void *  SymbolHandle;
#endif

/* possibly C_LibraryLoader would be better name */

class LibraryLoader : public boost::noncopyable
{
private:
  typedef std::map< std::string, SymbolHandle > ProcCache;

  ProcCache		functions;
  ModuleHandle	loadedLibHandle;

  LibraryLoader() {}

protected:

  LibraryLoader(const std::string & path2libFile);
  LibraryLoader(const std::string & dir2look, const std::string & libFileName);

  ~LibraryLoader();

  SymbolHandle GetProcAddr(const std::string & name);
};

} /* namespace util */
} /* namespace mysql */
} /* namespace sql */
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
