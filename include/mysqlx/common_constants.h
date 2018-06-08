/*
 * Copyright (c) 2015, 2018, Oracle and/or its affiliates. All rights reserved.
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

#ifndef MYSQL_COMMON_CONSTANTS_H
#define MYSQL_COMMON_CONSTANTS_H

#define DEFAULT_MYSQL_PORT  3306
#define DEFAULT_MYSQLX_PORT 33060

// ----------------------------------------------------------------------------

/*
  Common constants
  ================

  Warning: Values of these constants are part of the public API. Changing them
  is a non backward compatible API change.

  Note: Value of 0 is reserved for special uses and thus constant values
  are always > 0.
*/

/*
  Note: the empty END_LIST macro at the end of list macros helps Doxygen
  correctly interpret documentation for the list item.
*/

#undef END_LIST
#define END_LIST


#define SESSION_OPTION_LIST(x)                                               \
  OPT_STR(x,URI,1)         /*!< connection URI or string */                  \
  /*! DNS name of the host, IPv4 address or IPv6 address */                  \
  OPT_STR(x,HOST,2)                                                          \
  OPT_NUM(x,PORT,3)        /*!< X Plugin port to connect to */               \
  /*! Assign a priority (a number in range 1 to 100) to the last specified
      host; these priorities are used to determine the order in which multiple
      hosts are tried by the connection fail-over logic (see description
      of `Session` class) */                                                 \
  OPT_NUM(x,PRIORITY,4)                                                      \
  OPT_STR(x,USER,5)        /*!< user name */                                 \
  OPT_STR(x,PWD,6)         /*!< password */                                  \
  OPT_STR(x,DB,7)          /*!< default database */                          \
  OPT_ANY(x,SSL_MODE,8)    /*!< define `SSLMode` option to be used */        \
  /*! path to a PEM file specifying trusted root certificates*/              \
  OPT_STR(x,SSL_CA,9)                                                        \
  OPT_ANY(x,AUTH,10)      /*!< authentication method, PLAIN, MYSQL41, etc.*/ \
  OPT_STR(x,SOCKET,11)
  END_LIST

#define OPT_STR(X,Y,N) X##_str(Y,N)
#define OPT_NUM(X,Y,N) X##_num(Y,N)
#define OPT_ANY(X,Y,N) X##_any(Y,N)


/*
  Names for options supported in the query part of a connection string and
  how they map to session options above.
*/

#define URI_OPTION_LIST(X)  \
  X("ssl-mode", SSL_MODE)   \
  X("ssl-ca", SSL_CA)       \
  X("auth", AUTH)           \
  END_LIST


#define SSL_MODE_LIST(x) \
  x(DISABLED,1)        /*!< Establish an unencrypted connection.  */ \
  x(REQUIRED,2)        /*!< Establish a secure connection if the server supports
                          secure connections. The connection attempt fails if a
                          secure connection cannot be established. This is the
                          default if `SSL_MODE` is not specified. */ \
  x(VERIFY_CA,3)       /*!< Like `REQUIRED`, but additionally verify the server
                          TLS certificate against the configured Certificate
                          Authority (CA) certificates (defined by `SSL_CA`
                          Option). The connection attempt fails if no valid
                          matching CA certificates are found.*/ \
  x(VERIFY_IDENTITY,4) /*!< Like `VERIFY_CA`, but additionally verify that the
                          server certificate matches the host to which the
                          connection is attempted.*/\
  END_LIST


#define AUTH_METHOD_LIST(x)\
  x(PLAIN,1)       /*!< Plain text authentication method. The password is
                      sent as a clear text. This method is used by
                      default in encrypted connections. */ \
  x(MYSQL41,2)     /*!< Authentication method supported by MySQL 4.1 and newer.
                      The password is hashed before being sent to the server.
                      This authentication method works over unencrypted
                      connections */ \
  x(EXTERNAL,3)    /*!< External authentication when the server establishes
                      the user authenticity by other means such as SSL/x509
                      certificates. Currently not supported by X Plugin */ \
  x(SHA256_MEMORY,4)  /*!< Authentication using SHA256 password hashes stored in
                         server-side cache. This authentication method works
                         over unencrypted connections.
                      */ \
  END_LIST

/*
  Types that can be reported by MySQL server.
*/

#define RESULT_TYPE_LIST(X) \
  X(BIT,        1)   \
  X(TINYINT,    2)   \
  X(SMALLINT,   3)   \
  X(MEDIUMINT,  4)   \
  X(INT,        5)   \
  X(BIGINT,     6)   \
  X(FLOAT,      7)   \
  X(DECIMAL,    8)   \
  X(DOUBLE,     9)   \
  X(JSON,       10)  \
  X(STRING,     11)  \
  X(BYTES,      12)  \
  X(TIME,       13)  \
  X(DATE,       14)  \
  X(DATETIME,   15)  \
  X(TIMESTAMP,  16)  \
  X(SET,        17)  \
  X(ENUM,       18)  \
  X(GEOMETRY,   19)  \
  END_LIST


/*
  Check options for an updatable view.
  @see https://dev.mysql.com/doc/refman/en/view-check-option.html
*/

#define VIEW_CHECK_OPTION_LIST(x) \
  x(CASCADED,1) \
  x(LOCAL,2) \
  END_LIST

/*
  Algorithms used to process views.
  @see https://dev.mysql.com/doc/refman/en/view-algorithms.html
*/

#define VIEW_ALGORITHM_LIST(x) \
  x(UNDEFINED,1) \
  x(MERGE,2) \
  x(TEMPTABLE,3) \
  END_LIST

/*
  View security settings.
  @see https://dev.mysql.com/doc/refman/en/stored-programs-security.html
*/

#define VIEW_SECURITY_LIST(x) \
  x(DEFINER,1) \
  x(INVOKER,2) \
  END_LIST


#define LOCK_MODE_LIST(X) \
  X(SHARED,1)    /*!< Sets a shared mode lock on any rows that
                      are read. Other sessions can read the rows,
                      but cannot modify them until your transaction
                      commits. If any of these rows were changed by
                      another transaction that has not yet committed,
                      your query waits until that transaction ends
                      and then uses the latest values. */ \
  X(EXCLUSIVE,2) /*!< For index records the search encounters,
                      locks the rows and any associated index entries, the same
                      as if you issued an UPDATE statement for those rows. Other
                      transactions are blocked from updating those rows,
                      from doing locking in LOCK_SHARED, or from reading
                      the data in certain transaction isolation levels. */ \
  END_LIST

#define LOCK_CONTENTION_LIST(X) \
  X(DEFAULT,0) /*!< Block query until existing row locks are released.  */ \
  X(NOWAIT,1) /*!< Return error if lock could not be obtained immediately.  */ \
  X(SKIP_LOCKED,2) /*!< Execute query immediately, excluding items that are
                        locked from the query results.  */ \
  END_LIST


// ----------------------------------------------------------------------------


#endif
