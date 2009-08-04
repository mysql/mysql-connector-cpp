/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#include <string>
#include <stdexcept>

// Needed to make SetDllDirectory available
#define _WIN32_WINNT    0x0502
#include "mysql_lib_loader.h"


namespace sql {
namespace mysql {
namespace util {



    LibraryLoader::LibraryLoader( const std::string & path2libFile )
        : loadedLibHandle ( NULL )
    {
        if ( (loadedLibHandle= LoadLibrary( path2libFile.c_str() )) == NULL )
            throw std::runtime_error((std::string("Couldn't load library ") + path2libFile).c_str() );
    }


    LibraryLoader::LibraryLoader( const std::string & dir2look
                                , const std::string & libFileName )
        : loadedLibHandle ( NULL )
    {

#if _WIN32
        SetDllDirectory( dir2look.c_str() );
        if ( (loadedLibHandle= LoadLibrary( libFileName.c_str() )) == NULL )
#else
        std::string fullname( dir2look );
        fullname+= libFileName;
        if ( (loadedLibHandle= LoadLibrary( fullname.c_str() )) == NULL )
#endif   
            throw std::runtime_error((std::string("Couldn't load library ") + libFileName).c_str() );
    }


    LibraryLoader::~LibraryLoader()
    {
        FreeLibrary( loadedLibHandle );
    }


    SymbolHandle LibraryLoader::GetProcAddr( const std::string & name )
    {
        if ( loadedLibHandle == NULL )
            return NULL;

        ProcCache::const_iterator cit= functions.find( name );

        if ( cit != functions.end() )
            return cit->second;

        SymbolHandle proc= GetProcAddress( loadedLibHandle, name.c_str() );

        if ( proc == NULL )
            throw std::runtime_error( (std::string("Couldn't find symbol ") + name ).c_str() );

        functions.insert( std::make_pair( name, proc ) );

        return proc;
    }

} /* namespace util */
} /* namespace mysql */
} /* namespace sql */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
