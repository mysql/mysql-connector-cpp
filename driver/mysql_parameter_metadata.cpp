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

#ifndef _WIN32
#include <stdlib.h>
#endif	//	_WIN32

#include "mysql_parameter_metadata.h"
#include <cppconn/exception.h>

#ifdef __GNUC__
#if __GNUC__ >= 2
#define CPPCONN_FUNC __FUNCTION__
#endif
#else
#define CPPCONN_FUNC "<unknown>"
#endif

namespace sql
{
namespace mysql
{


/* {{{ MySQL_ParameterMetaData::MySQL_ParameterMetaData -I- */
MySQL_ParameterMetaData::MySQL_ParameterMetaData(MYSQL_STMT * stmt)
{
	param_count = mysql_stmt_param_count(stmt);
}
/* }}} */


/* {{{ MySQL_ParameterMetaData::getParameterClassName -U- */
std::string
MySQL_ParameterMetaData::getParameterClassName(unsigned int /* param */)
{
	throw sql::MethodNotImplementedException("MySQL_ParameterMetaData::getParameterClassName()");
}
/* }}} */


/* {{{ MySQL_ParameterMetaData::getParameterCount -I- */
int
MySQL_ParameterMetaData::getParameterCount()
{
	return param_count;
}
/* }}} */


/* {{{ MySQL_ParameterMetaData::getParameterMode -U- */
int
MySQL_ParameterMetaData::getParameterMode(unsigned int /* param */)
{
	throw sql::MethodNotImplementedException("MySQL_ParameterMetaData::getParameterClassName()");
}
/* }}} */


/* {{{ MySQL_ParameterMetaData::getParameterType -U- */
int
MySQL_ParameterMetaData::getParameterType(unsigned int /* param */)
{
	throw sql::MethodNotImplementedException("MySQL_ParameterMetaData::getParameterClassName()");
}
/* }}} */


/* {{{ MySQL_ParameterMetaData::getParameterTypeName -U- */
std::string
MySQL_ParameterMetaData::getParameterTypeName(unsigned int /* param */)
{
	throw sql::MethodNotImplementedException("MySQL_ParameterMetaData::getParameterClassName()");
}
/* }}} */


/* {{{ MySQL_ParameterMetaData::getPrecision -U- */
int
MySQL_ParameterMetaData::getPrecision(unsigned int /* param */)
{
	throw sql::MethodNotImplementedException("MySQL_ParameterMetaData::getParameterClassName()");
}
/* }}} */


/* {{{ MySQL_ParameterMetaData::getScale -U- */
int
MySQL_ParameterMetaData::getScale(unsigned int /* param */)
{
	throw sql::MethodNotImplementedException("MySQL_ParameterMetaData::getParameterClassName()");
}
/* }}} */


/* {{{ MySQL_ParameterMetaData::isNullable -U- */
int
MySQL_ParameterMetaData::isNullable(unsigned int /* param */)
{
	throw sql::MethodNotImplementedException("MySQL_ParameterMetaData::getParameterClassName()");
}
/* }}} */


/* {{{ MySQL_ParameterMetaData::isSigned -U- */
bool
MySQL_ParameterMetaData::isSigned(unsigned int /* param */)
{
	throw sql::MethodNotImplementedException("MySQL_ParameterMetaData::getParameterClassName()");
}
/* }}} */

}; /* namespace mysql */
}; /* namespace sql */
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */


