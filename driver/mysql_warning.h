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



#ifndef __MYSQL_WARNING_H_
#define __MYSQL_WARNING_H_

#include "mysql_connection.h"
#include <cppconn/warning.h>
#include <cppconn/config.h>

#include <boost/scoped_ptr.hpp>
namespace sql
{
namespace mysql
{

  class MySQL_Warning : public ::sql::SQLWarning
  {
  private:

    const sql::SQLString				sql_state;
    const int							errNo;
    const sql::SQLString				descr;
    boost::scoped_ptr<MySQL_Warning>	next;

  public:

    MySQL_Warning(const sql::SQLString& reason, const sql::SQLString& SQLState, int vendorCode);

    MySQL_Warning(const sql::SQLString& reason, const sql::SQLString& SQLState);

    MySQL_Warning(const sql::SQLString& reason);

    MySQL_Warning();

    const sql::SQLString & getMessage() const;

    const sql::SQLString & getSQLState() const;

    int getErrorCode() const;

    const SQLWarning * getNextWarning() const;

    ~MySQL_Warning();

  private:

    /* We don't really want it to be called, but we need to implement it */
    void setNextWarning(const SQLWarning * _next);

  public:

    void setNextWarning(MySQL_Warning * _next);

  private:

    MySQL_Warning(const MySQL_Warning& w);

    MySQL_Warning(const ::sql::SQLWarning & w);

    const MySQL_Warning & operator = (const MySQL_Warning & rhs);

  };

  const sql::SQLString & errCode2SqlState(int32_t errCode, ::sql::SQLString & state);

  MySQL_Warning * loadMysqlWarnings(	sql::Connection * connection,
                    unsigned int warningsCount=0);


} /* namespace mysql */
} /* namespace sql   */
#endif
