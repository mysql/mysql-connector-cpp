
#ifndef MYSQL_CDK_PROTOCOL_MYSQLX_TRAITS_H
#define MYSQL_CDK_PROTOCOL_MYSQLX_TRAITS_H

namespace cdk {
namespace protocol {
namespace mysqlx {

// These are temporary type declarations
// TODO: remove when the types are defined

typedef uint32_t stmt_id_t;
typedef uint32_t cursor_id_t;
typedef uint64_t row_count_t;
typedef uint32_t col_count_t;
// Note: protocol uses 64bit numbers for collation ids
typedef uint64_t collation_id_t;
typedef uint64_t insert_id_t;

typedef int64_t  sint64_t;
using   ::uint64_t;

}}}

#endif
