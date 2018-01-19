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



#ifndef _MYSQL_PARAMETER_METADATA_H_
#define _MYSQL_PARAMETER_METADATA_H_

#include <boost/shared_ptr.hpp>

#include <cppconn/parameter_metadata.h>

namespace sql
{
namespace mysql
{
namespace NativeAPI
{
  class NativeStatementWrapper;
}

class MySQL_ParameterMetaData : public sql::ParameterMetaData
{
  unsigned int param_count;
public:
  MySQL_ParameterMetaData( boost::shared_ptr< NativeAPI::NativeStatementWrapper > & stmt);

  virtual ~MySQL_ParameterMetaData() {}

  virtual sql::SQLString getParameterClassName(unsigned int paramNo);

  virtual int getParameterCount();

  virtual int getParameterMode(unsigned int paramNo);

  virtual int getParameterType(unsigned int paramNo);

  virtual sql::SQLString getParameterTypeName(unsigned int paramNo);

  virtual int getPrecision(unsigned int paramNo);

  virtual int getScale(unsigned int paramNo);

  virtual int isNullable(unsigned int paramNo);

  virtual bool isSigned(unsigned int paramNo);


private:
  /* Prevent use of these */
  MySQL_ParameterMetaData(const MySQL_ParameterMetaData &);
  void operator=(MySQL_ParameterMetaData &);
};

} /* namespace mysql */
} /* namespace sql */

#endif // _MYSQL_PARAMETER_METADATA_H_

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
