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



#ifndef _NATIVE_RESULTSET_WRAPPER_H_
#define _NATIVE_RESULTSET_WRAPPER_H_

#include <boost/noncopyable.hpp>
#include <config.h>
#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif

#include "../cppconn/version_info.h"

#if (MYCPPCONN_STATIC_MYSQL_VERSION_ID > 80004)
struct MYSQL_FIELD;
#else
struct st_mysql_field;
#define MYSQL_FIELD st_mysql_field
#endif

namespace sql
{
namespace mysql
{
namespace NativeAPI
{

class NativeResultsetWrapper : public boost::noncopyable
{
public:
  virtual ~NativeResultsetWrapper(){}

  virtual void data_seek(uint64_t) = 0;

  virtual ::MYSQL_FIELD * fetch_field() = 0;

  virtual ::MYSQL_FIELD * fetch_field_direct(unsigned int) = 0;

  virtual unsigned long * fetch_lengths() = 0;

  virtual char** fetch_row() = 0;

  virtual unsigned int num_fields() = 0;

  virtual uint64_t num_rows() = 0;
};

} /* namespace NativeAPI*/
} /* namespace mysql */
} /* namespace sql */

#endif // _NATIVE_RESULTSET_WRAPPER_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
