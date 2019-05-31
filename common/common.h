#ifndef MYSQLX_COMMON_COMMON_H
#define MYSQLX_COMMON_COMMON_H

#include <mysqlx/common.h>


namespace mysqlx {
MYSQLX_ABI_BEGIN(2,0)
namespace common {

class Session_pool;
class Session_impl;
class Result_impl;
class Result_init;
class Column_info;

using Shared_session_impl = std::shared_ptr<Session_impl>;
using Session_pool_shared = std::shared_ptr<Session_pool>;

using cdk::col_count_t;
using cdk::row_count_t;

}
MYSQLX_ABI_END(2,0)
}


namespace mysqlx {
namespace impl {
namespace common {

using namespace mysqlx::common;

/*
  Note: for older gcc versions (4.8, 5.4) above using namespace declaration
  is not sufficient to correctly resolve mysqlx::impl::common::Session_pool
  (but for more recent versions it works).
*/

using mysqlx::common::Session_pool;

using cdk::col_count_t;
using cdk::row_count_t;

}}}


#ifndef THROW_AS_ASSERT
#undef THROW
#define THROW(MSG) do { mysqlx::common::throw_error(MSG); throw (MSG); } while(false)
#endif


#endif
