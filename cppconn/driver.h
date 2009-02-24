/*
   Copyright (C) 2007 - 2008 MySQL AB, 2008 - 2009 Sun Microsystems, Inc.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
