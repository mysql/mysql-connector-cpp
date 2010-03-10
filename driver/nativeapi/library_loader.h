/*
   Copyright (C) 2009, 2010, Oracle and/or its affiliates. All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
