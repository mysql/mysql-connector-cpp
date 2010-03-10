/*
   Copyright (C) 2008, 2010, Oracle and/or its affiliates. All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
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
