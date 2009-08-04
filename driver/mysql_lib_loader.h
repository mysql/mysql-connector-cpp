/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_LIB_LOADER_H_
#define _MYSQL_LIB_LOADER_H_

#ifdef _WIN32
#include <windows.h>
#else
// if HAVE_DLFCN_H is not defined - this header(mysql_lib_loader.h) won't be included
// by cmake rule. So following check of HAVE_DLFCN_H might be redundant
//# ifdef HAVE_DLFCN_H
#  include <dlfcn.h>
//# else
//#  error This should never happen - if this header is included, one of macros above supposed to be defined.
//# endif
#endif

#include <boost/noncopyable.hpp>
#include <map>

namespace sql
{
namespace mysql
{
namespace util
{
/* TODO consider using of dlopen, dlsym and dlclose definitions in my_global.h
 * sort of doesn't like that.
 */
#ifdef _WIN32
typedef HMODULE ModuleHandle;
typedef FARPROC SymbolHandle;

#else
typedef void *  ModuleHandle;
typedef void *  SymbolHandle;

#define LoadLibrary(p)          ::dlopen(p, RTLD_LAZY)
#define FreeLibrary(p)          ::dlclose(p)
#define GetProcAddress(p1,p2)   ::dlsym(p1,p2)

#endif

/* possibly C_LibraryLoader would be better name */

class LibraryLoader : public boost::noncopyable
{
private:

    typedef std::map<const std::string, SymbolHandle> ProcCache;

    ProcCache       functions;
    ModuleHandle    loadedLibHandle;


    LibraryLoader() {}

protected:

    LibraryLoader   ( const std::string & path2libFile );
    LibraryLoader   ( const std::string & dir2look
                    , const std::string & libFileName );

    ~LibraryLoader();

    SymbolHandle GetProcAddr ( const std::string & name );
};

}
}
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
