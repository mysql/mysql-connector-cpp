/*
 * Copyright (c) 2008, 2018, Oracle and/or its affiliates. All rights reserved.
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



#ifndef _MYSQL_NativeResultsetWrapper_H_
#define _MYSQL_NativeResultsetWrapper_H_

#include <boost/shared_ptr.hpp>

#include "native_resultset_wrapper.h"
#include "../cppconn/version_info.h"

#if (MYCPPCONN_STATIC_MYSQL_VERSION_ID > 80004)
struct MYSQL_RES;
#else
struct st_mysql_res;
#define MYSQL_RES st_mysql_res
#endif

namespace sql
{
namespace mysql
{

class MySQL_DebugLogger;

namespace NativeAPI
{
class IMySQLCAPI;


class MySQL_NativeResultsetWrapper : public NativeResultsetWrapper
{
public:
  MySQL_NativeResultsetWrapper(::MYSQL_RES *, boost::shared_ptr<NativeAPI::IMySQLCAPI> &/*, boost::shared_ptr< MySQL_DebugLogger > & l*/);

  ~MySQL_NativeResultsetWrapper();

  void data_seek(uint64_t);

  ::MYSQL_FIELD * fetch_field();

  ::MYSQL_FIELD * fetch_field_direct(unsigned int);

  unsigned long * fetch_lengths();

  char** fetch_row();

  unsigned int num_fields();

  uint64_t num_rows();

  //boost::shared_ptr<IMySQLCAPI> getApiHandle();

private:

  MySQL_NativeResultsetWrapper(){}
  //Also need to decide should it be copyable

  boost::shared_ptr< MySQL_DebugLogger > logger;

  boost::shared_ptr< NativeAPI::IMySQLCAPI > capi;

  ::MYSQL_RES * rs;
};

} /* namespace NativeAPI */
} /* namespace mysql	 */
} /* namespace sql	   */

#endif // _MYSQL_RESULTSET_DATA_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
