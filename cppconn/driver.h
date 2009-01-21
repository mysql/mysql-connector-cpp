/* Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   There are special exceptions to the terms and conditions of the GPL 
   as it is applied to this software. View the full text of the 
   exception in file EXCEPTIONS-CONNECTOR-C++ in the directory of this 
   software distribution.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _SQL_DRIVER_H_
#define _SQL_DRIVER_H_

#include <string>
#include <map>
#include "connection.h"

#ifndef CPPDBC_PUBLIC_FUNC

#if defined(_WIN32)
 #ifdef CPPDBC_EXPORTS
  #define CPPDBC_PUBLIC_FUNC __declspec(dllexport)
 #else
  #define CPPDBC_PUBLIC_FUNC __declspec(dllimport)
 #endif
#else
 #define CPPDBC_PUBLIC_FUNC
#endif

#endif    //#ifndef CPPDBC_PUBLIC_FUNC

namespace sql
{

class CPPDBC_PUBLIC_FUNC Driver
{
protected:
	virtual ~Driver() {}
public:
	// Attempts to make a database connection to the given URL.

	virtual Connection * connect(const std::string& hostName, const std::string& userName, const std::string& password) = 0;

	virtual Connection * connect(std::map<std::string, ConnectPropertyVal>) = 0;

	virtual int getMajorVersion() = 0;

	virtual int getMinorVersion() = 0;

	virtual int getPatchVersion() = 0;

	virtual const std::string & getName() = 0;
};

}; /* namespace sql */

extern "C"
{
  CPPDBC_PUBLIC_FUNC sql::Driver *get_driver_instance();
}

#endif /* _SQL_DRIVER_H_ */
