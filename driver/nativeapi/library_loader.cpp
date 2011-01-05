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



#include "binding_config.h"
#ifndef MYSQLCLIENT_STATIC_BINDING

#include <string>
#include <stdexcept>


// Needed to make SetDllDirectory available
#define _WIN32_WINNT	0x0502
#include "library_loader.h"

/* TODO consider using of dlopen, dlsym and dlclose definitions in my_global.h
 * sort of doesn't like that.
 */
#ifndef _WIN32
#define LoadLibrary(p)			::dlopen(p, RTLD_LAZY)
#define FreeLibrary(p)			::dlclose(p)
#define GetProcAddress(p1,p2)	::dlsym(p1,p2)
#endif


namespace sql {
namespace mysql {
namespace util {

std::string ErrorMessage()
{
#ifdef _WIN32
	TCHAR buffer[255];
	DWORD _errcode = GetLastError();
	::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
					NULL, _errcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					buffer, sizeof(buffer), NULL);

	return buffer;
#else
	return dlerror();
#endif
}

/* {{{ LibraryLoader::LibraryLoader() */
LibraryLoader::LibraryLoader(const std::string & path2libFile)
	: loadedLibHandle (NULL)
{
	if ((loadedLibHandle = LoadLibrary(path2libFile.c_str())) == NULL) {
		throw std::runtime_error(std::string("Couldn't load library ") + path2libFile + ": " + ErrorMessage());
	}
}
/* }}} */


/* {{{ LibraryLoader::LibraryLoader() */
LibraryLoader::LibraryLoader(const std::string & dir2look, const std::string & libFileName)
	: loadedLibHandle(NULL)
{

#if _WIN32
	SetDllDirectory(dir2look.c_str());
	if ((loadedLibHandle= LoadLibrary(libFileName.c_str())) == NULL) {
#else
	std::string fullname(dir2look);
	fullname += "/";
	fullname += libFileName;
	if ((loadedLibHandle = LoadLibrary(fullname.c_str())) == NULL) {
#endif
		throw std::runtime_error(std::string("Couldn't load library ") + libFileName + ": " + ErrorMessage());
	}
}
/* }}} */


/* {{{ LibraryLoader::~LibraryLoader() */
LibraryLoader::~LibraryLoader()
{
	FreeLibrary(loadedLibHandle);
}
/* }}} */


/* {{{ LibraryLoader::GetProcAddr() */
SymbolHandle
LibraryLoader::GetProcAddr(const std::string & name)
{
	if (loadedLibHandle == NULL) {
		return NULL;
	}

	ProcCache::const_iterator cit = functions.find(name);

	if (cit != functions.end()) {
		return cit->second;
	}

	SymbolHandle proc = GetProcAddress(loadedLibHandle, name.c_str());

	if (proc == NULL) {
		throw std::runtime_error("Couldn't find symbol " + name);
	}

	functions.insert(std::make_pair(name, proc));

	return proc;
}
/* }}} */


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
