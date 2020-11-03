/*
 * Copyright (c) 2008, 2020, Oracle and/or its affiliates.
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



#include "mysql_connection.h"
#include "version_info.h"

#include <cppconn/exception.h>

// Looks like this one should go after private_iface
#include "mysql_driver.h"
#include "nativeapi/native_driver_wrapper.h"


CPPCONN_PUBLIC_FUNC void check(const std::string&)
{}

CPPCONN_PUBLIC_FUNC void check(const std::map<std::string,std::string>&)
{}

extern "C"
{
CPPCONN_PUBLIC_FUNC void * sql_mysql_get_driver_instance()
{
  void * ret = sql::mysql::get_driver_instance();
  return ret;
}


/* these are the functions without namespace - from cppconn/driver.h */
CPPCONN_PUBLIC_FUNC sql::Driver * _get_driver_instance_by_name(const char * const clientlib)
{
  return sql::mysql::_get_driver_instance_by_name(clientlib);
}



} /* extern "C" */

namespace sql
{
namespace mysql
{

static const ::sql::SQLString emptyStr("");


CPPCONN_PUBLIC_FUNC sql::mysql::MySQL_Driver * _get_driver_instance_by_name(const char * const clientlib)
{
  ::sql::SQLString dummy(clientlib);

  /* Mapping by client name is probably not enough here */
  static std::map< sql::SQLString, std::shared_ptr<MySQL_Driver> > driver;

  std::map< sql::SQLString, std::shared_ptr< MySQL_Driver > >::const_iterator cit;

  if ((cit = driver.find(dummy)) != driver.end()) {
    return cit->second.get();
  } else {
    std::shared_ptr< MySQL_Driver > newDriver;

    newDriver.reset(new MySQL_Driver(dummy));
    driver[dummy] = newDriver;

    return newDriver.get();
  }
}


MySQL_Driver::MySQL_Driver()
{
  try {
    proxy.reset(::sql::mysql::NativeAPI::createNativeDriverWrapper(emptyStr));
  }	catch(std::runtime_error & e)	{
    throw sql::InvalidArgumentException(e.what());
  }
}


MySQL_Driver::MySQL_Driver(const ::sql::SQLString & clientLib)
{
  try {
    proxy.reset(::sql::mysql::NativeAPI::createNativeDriverWrapper(clientLib));
  }	catch(std::runtime_error & e)	{
    throw sql::InvalidArgumentException(e.what());
  }
}


MySQL_Driver::~MySQL_Driver()
{
}


sql::Connection * MySQL_Driver::connect(const sql::SQLString& hostName,
                    const sql::SQLString& userName,
                    const sql::SQLString& password)
{
  return new MySQL_Connection(this, proxy->conn_init(),hostName, userName, password);
}


sql::Connection * MySQL_Driver::connect(sql::ConnectOptionsMap & properties)
{
  return new MySQL_Connection(this, proxy->conn_init(),properties);
}


//TODO: That has to be defined in cmake files
int MySQL_Driver::getMajorVersion()
{
  return MYCPPCONN_MAJOR_VERSION;
}

int MySQL_Driver::getMinorVersion()
{
  return MYCPPCONN_MINOR_VERSION;
}

int MySQL_Driver::getPatchVersion()
{
  return MYCPPCONN_PATCH_VERSION;
}

const sql::SQLString & MySQL_Driver::getName()
{
  static const sql::SQLString name("MySQL Connector C++ (libmysql)");
  return name;
}


void MySQL_Driver::threadInit()
{
  proxy->thread_init();
}


void MySQL_Driver::threadEnd()
{
  proxy->thread_end();
}

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
