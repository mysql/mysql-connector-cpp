/*
   Copyright 2009 Sun Microsystems, Inc.  All rights reserved.

   The MySQL Connector/C++ is licensed under the terms of the GPL
   <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
   MySQL Connectors. There are special exceptions to the terms and
   conditions of the GPL as it is applied to this software, see the
   FLOSS License Exception
   <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
*/

#ifndef _MYSQL_LIBMYSQL_API_H_
#define _MYSQL_LIBMYSQL_API_H_

#include "mysql_private_iface.h"
#include <boost/shared_ptr.hpp>

#ifndef _ABSTRACT
#define _ABSTRACT
#endif

#ifndef _PURE
#define _PURE =0
#endif


namespace sql
{
class SQLString;

namespace mysql
{
namespace NativeAPI
{

typedef my_ulonglong  (STDCALL *ptr2mysql_affected_rows)(MYSQL *);

typedef my_bool       (STDCALL *ptr2mysql_autocommit)(MYSQL *, my_bool);

typedef void          (STDCALL *ptr2mysql_close)(MYSQL *mysql);

typedef my_bool       (STDCALL *ptr2mysql_commit)(MYSQL *mysql);

typedef void          (STDCALL *ptr2mysql_data_seek)(MYSQL_RES *, my_ulonglong);

typedef void          (STDCALL *ptr2mysql_debug)(const char *debug);

typedef unsigned int  (STDCALL *ptr2mysql_errno)(MYSQL *mysql);

typedef const char *  (STDCALL *ptr2mysql_error)(MYSQL *mysql);

typedef MYSQL_FIELD * (STDCALL *ptr2mysql_fetch_field)(MYSQL_RES *);

typedef MYSQL_FIELD * (STDCALL *ptr2mysql_fetch_field_direct)(MYSQL_RES *
                                                            , unsigned int);

typedef unsigned long * (STDCALL *ptr2mysql_fetch_lengths)(MYSQL_RES *);

typedef MYSQL_ROW     (STDCALL *ptr2mysql_fetch_row)(MYSQL_RES *);

typedef unsigned int  (STDCALL *ptr2mysql_field_count)(MYSQL *);

typedef void          (STDCALL *ptr2mysql_free_result)(MYSQL_RES *);

typedef unsigned long (STDCALL *ptr2mysql_get_client_version)();

typedef const char *  (STDCALL *ptr2mysql_get_server_info)(MYSQL *);

typedef unsigned long (STDCALL *ptr2mysql_get_server_version)(MYSQL *);

typedef MYSQL *       (STDCALL *ptr2mysql_init)(MYSQL *mysql);

typedef int           (STDCALL *ptr2mysql_library_init)(int, char **, char **);

typedef void          (STDCALL *ptr2mysql_library_end)(void);

typedef my_bool       (STDCALL *ptr2mysql_more_results)(MYSQL *);

typedef int           (STDCALL *ptr2mysql_next_result)(MYSQL *);

typedef unsigned int  (STDCALL *ptr2mysql_num_fields)(MYSQL_RES *);

typedef my_ulonglong  (STDCALL *ptr2mysql_num_rows)(MYSQL_RES *);

typedef int           (STDCALL *ptr2mysql_options)(MYSQL *
                                                , enum mysql_option
                                                , const void *);

typedef int		      (STDCALL *ptr2mysql_query)(MYSQL *, const char *);

typedef MYSQL *       (STDCALL *ptr2mysql_real_connect)(MYSQL *, const char *
                                                , const char *, const char *
                                                , const char *, unsigned int
                                                , const char *, unsigned long);

typedef int           (STDCALL *ptr2mysql_real_query)(MYSQL *, const char *
                                                    , unsigned long);

typedef my_bool       (STDCALL *ptr2mysql_rollback)(MYSQL *mysql);

typedef const char *  (STDCALL *ptr2mysql_sqlstate)(MYSQL *mysql);

typedef my_bool       (STDCALL *ptr2mysql_ssl_set)(MYSQL *, const char *
                                            , const char *, const char *
                                            , const char *, const char *);

typedef MYSQL_RES *   (STDCALL *ptr2mysql_store_result)(MYSQL *);

typedef MYSQL_RES *   (STDCALL *ptr2mysql_use_result)(MYSQL *);

/* Prepared Statement stmt_* functions */
typedef my_ulonglong  (STDCALL *ptr2mysql_stmt_affected_rows)(MYSQL_STMT *);

typedef my_bool       (STDCALL *ptr2mysql_stmt_attr_set)(MYSQL_STMT *
                                                    , enum enum_stmt_attr_type
                                                    , const void *);

typedef my_bool       (STDCALL *ptr2mysql_stmt_bind_param)(MYSQL_STMT *
                                                         , MYSQL_BIND *);

typedef my_bool       (STDCALL *ptr2mysql_stmt_bind_result)(MYSQL_STMT *
                                                          , MYSQL_BIND *);

typedef my_bool       (STDCALL *ptr2mysql_stmt_close)(MYSQL_STMT *);

typedef void          (STDCALL *ptr2mysql_stmt_data_seek)(MYSQL_STMT *
                                                        , my_ulonglong);

typedef unsigned int  (STDCALL *ptr2mysql_stmt_errno)(MYSQL_STMT *);

typedef const char *  (STDCALL *ptr2mysql_stmt_error)(MYSQL_STMT *);

typedef int           (STDCALL *ptr2mysql_stmt_execute)(MYSQL_STMT *);

typedef int           (STDCALL *ptr2mysql_stmt_fetch)(MYSQL_STMT *);

typedef unsigned int  (STDCALL *ptr2mysql_stmt_field_count)(MYSQL_STMT *);

typedef MYSQL_STMT *  (STDCALL *ptr2mysql_stmt_init)(MYSQL *);

typedef my_ulonglong  (STDCALL *ptr2mysql_stmt_num_rows)(MYSQL_STMT *);

typedef unsigned long (STDCALL *ptr2mysql_stmt_param_count)(MYSQL_STMT *);

typedef int           (STDCALL *ptr2mysql_stmt_prepare)(MYSQL_STMT *
                                                     , const char *
                                                     , unsigned long);

typedef MYSQL_RES *   (STDCALL *ptr2mysql_stmt_result_metadata)(MYSQL_STMT *);

typedef my_bool       (STDCALL *ptr2mysql_stmt_send_long_data)(MYSQL_STMT * 
                                                            , unsigned int
                                                            , const char * 
                                                            , unsigned long);

typedef const char *  (STDCALL *ptr2mysql_stmt_sqlstate)(MYSQL_STMT *);

typedef int           (STDCALL *ptr2mysql_stmt_store_result)(MYSQL_STMT *);


/*
 * Interface MySQL C-API wrapper class should implement.
 * At the moment we must have at least 2 implementation - for static and dynamic
 * mysql client library linking
 */
_ABSTRACT class IMySQLCAPI
{
public:
     virtual my_ulonglong  affected_rows      ( MYSQL *       )   _PURE;

     virtual my_bool       autocommit         ( MYSQL *
                                                    , my_bool       )   _PURE;

     virtual void          close              ( MYSQL *mysql  )   _PURE;

     virtual my_bool       commit             ( MYSQL *mysql  )   _PURE;

     virtual void          data_seek          ( MYSQL_RES *
                                                    , my_ulonglong  )   _PURE;

     virtual void          debug              ( const char *  )   _PURE;

     virtual unsigned int  mysql_errno        	  ( MYSQL *mysql  )   _PURE;

     virtual const char *  error              ( MYSQL *mysql  )   _PURE;

     virtual MYSQL_FIELD * fetch_field        ( MYSQL_RES *   )   _PURE;

     virtual MYSQL_FIELD * fetch_field_direct ( MYSQL_RES *
                                                    , unsigned int  )   _PURE;

     virtual unsigned long * fetch_lengths    ( MYSQL_RES *   )   _PURE;

     virtual MYSQL_ROW     fetch_row          ( MYSQL_RES *   )   _PURE;

     virtual unsigned int  field_count        ( MYSQL *       )   _PURE;

     virtual void          free_result        ( MYSQL_RES *   )   _PURE;

     virtual unsigned long get_client_version ()                  _PURE;

     virtual const char *  get_server_info    ( MYSQL *       )   _PURE;

     virtual unsigned long get_server_version ( MYSQL *       )   _PURE;

     virtual MYSQL *       init               ( MYSQL *mysql  )   _PURE;

     virtual int           library_init       ( int   argc
                                                    , char **argv
                                                    , char **groups )   _PURE;

     virtual void          library_end        ()                  _PURE;

     virtual my_bool       more_results       ( MYSQL *       )   _PURE;

     virtual int           next_result        ( MYSQL *       )   _PURE;

     virtual unsigned int  num_fields         ( MYSQL_RES *   )   _PURE;

     virtual my_ulonglong  num_rows           ( MYSQL_RES *   )   _PURE;

     virtual int           options            ( MYSQL *
                                                    , enum mysql_option option
                                                    , const void *arg ) _PURE;

     virtual int		   query              ( MYSQL *
                                                    , const char *  )   _PURE;

     virtual MYSQL *       real_connect       ( MYSQL *       mysql
                                                    , const char *  host
                                                    , const char *  user
                                                    , const char *  passwd
                                                    , const char *  db
                                                    , unsigned int  port
                                                    , const char *  unix_socket
                                                    , unsigned long client_flag) _PURE;

     virtual int           real_query         ( MYSQL *
                                                    , const char *
                                                    , unsigned long )   _PURE;

     virtual my_bool       rollback           ( MYSQL *       )   _PURE;

     virtual const char *  sqlstate           ( MYSQL *       )   _PURE;

     virtual my_bool       ssl_set            ( MYSQL      * mysql
                                                    , const char * key
                                                    , const char * cert
                                                    , const char * ca
                                                    , const char * capath
                                                    , const char * cipher ) _PURE;

     virtual MYSQL_RES *   store_result       ( MYSQL *       )   _PURE;

     virtual MYSQL_RES *   use_result         ( MYSQL *       )   _PURE;

     /* Methods - wrappers of prepared statement stmt_* functions */
     virtual my_ulonglong  stmt_affected_rows ( MYSQL_STMT *  )   _PURE;

     virtual my_bool       stmt_attr_set      ( MYSQL_STMT *stmt
                                                    , enum enum_stmt_attr_type option
                                                    , const void *arg ) _PURE;

     virtual my_bool       stmt_bind_param    ( MYSQL_STMT *
                                                    , MYSQL_BIND *  )   _PURE;

     virtual my_bool       stmt_bind_result   ( MYSQL_STMT *
                                                    , MYSQL_BIND *  )   _PURE;

     virtual my_bool       stmt_close         ( MYSQL_STMT *  )   _PURE;

     virtual void          stmt_data_seek     ( MYSQL_STMT *
                                                    , my_ulonglong  )   _PURE;

     virtual unsigned int  stmt_errno         ( MYSQL_STMT *  )   _PURE;

     virtual const char *  stmt_error         ( MYSQL_STMT *  )   _PURE;

     virtual int           stmt_execute       ( MYSQL_STMT *  )   _PURE;

     virtual int           stmt_fetch         ( MYSQL_STMT *  )   _PURE;

     virtual unsigned int  stmt_field_count   ( MYSQL_STMT *  )   _PURE;

     virtual MYSQL_STMT *  stmt_init          ( MYSQL *       )   _PURE;

     virtual my_ulonglong  stmt_num_rows      ( MYSQL_STMT *  )   _PURE;

     virtual unsigned long stmt_param_count   ( MYSQL_STMT *  )   _PURE;

     virtual int           stmt_prepare       ( MYSQL_STMT *
                                                    , const char *
                                                    , unsigned long )   _PURE;

     virtual MYSQL_RES *   stmt_result_metadata( MYSQL_STMT * )   _PURE;

     virtual my_bool       stmt_send_long_data( MYSQL_STMT *  stmt 
                                                    , unsigned int  par_number
                                                    , const char *  data
                                                    , unsigned long len) _PURE;

     virtual const char *  stmt_sqlstate      ( MYSQL_STMT *  )   _PURE;

     virtual int           stmt_store_result  ( MYSQL_STMT *  )   _PURE;
};

boost::shared_ptr<IMySQLCAPI> getCApiHandle( sql::SQLString & name );

}
}
}

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
