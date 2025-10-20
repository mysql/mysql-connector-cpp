/*
 * Copyright (c) 2015, 2024, Oracle and/or its affiliates.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2.0, as
 * published by the Free Software Foundation.
 *
 * This program is designed to work with certain software (including
 * but not limited to OpenSSL) that is licensed under separate terms, as
 * designated in a particular file or component or in included license
 * documentation. The authors of MySQL hereby grant you an additional
 * permission to link the program and your derivative works with the
 * separately licensed software that they have either included with
 * the program or referenced in the documentation.
 *
 * Without limiting anything contained in the foregoing, this file,
 * which is part of Connector/C++, is also subject to the
 * Universal FOSS Exception, version 1.0, a copy of which can be found at
 * https://oss.oracle.com/licenses/universal-foss-exception.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License, version 2.0, for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef MYSQLX_COMMON_VERSION_INFO
#define MYSQLX_COMMON_VERSION_INFO

/*
  Version information which is used for example for default
  connection attributes.

  When build system is configured by cmake, a new file is generated
  from this one with version and license values substituted by
  cmake and build system is configured so that the generated
  header takes precedence over this one. But code can be built even
  if header with real values was not generated - in that case the
  values specified here will be used.
*/

#define MYSQL_CONCPP_NAME    "mysql-connector-cpp"
#define MYSQL_CONCPP_VERSION "9.4.0"
#define MYSQL_CONCPP_LICENSE "GPL-2.0"

#define MYSQL_CONCPP_VERSION_MAJOR 9
#define MYSQL_CONCPP_VERSION_MINOR 4
#define MYSQL_CONCPP_VERSION_MICRO 0

#define MYSQL_CONCPP_VERSION_NUMBER 9040000

#endif
