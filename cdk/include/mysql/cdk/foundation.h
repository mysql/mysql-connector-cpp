/*
 * Copyright (c) 2015, 2016, Oracle and/or its affiliates. All rights reserved.
 *
 * This code is licensed under the terms of the GPLv2
 * <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>, like most
 * MySQL Connectors. There are special exceptions to the terms and
 * conditions of the GPLv2 as it is applied to this software, see the
 * FLOSS License Exception
 * <http://www.mysql.com/about/legal/licensing/foss-exception.html>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 */

#ifndef MYSQL_CDK_FOUNDATION_H
#define MYSQL_CDK_FOUNDATION_H

#include "foundation/common.h"
#include "foundation/types.h"
#include "foundation/error.h"
#include "foundation/async.h"
#include "foundation/stream.h"
#include "foundation/connection_tcpip.h"
#ifdef WITH_SSL_YASSL
#include "foundation/connection_yassl.h"
#endif // WITH_SSL_YASSL
#include "foundation/diagnostics.h"
#include "foundation/codec.h"
//#include "foundation/socket.h"

namespace cdk {

  using foundation::char_t;
  using foundation::byte;
  using foundation::option_t;
  using foundation::string;
  using foundation::scoped_ptr;
  using foundation::shared_ptr;

  using foundation::bytes;
  using foundation::buffers;

  using foundation::Error;
  using foundation::Error_class;
  using foundation::error_condition;
  using foundation::error_category;
  using foundation::error_code;
  using foundation::errc;
  using foundation::cdkerrc;
  using foundation::throw_error;
  using foundation::throw_posix_error;
  using foundation::throw_system_error;
  using foundation::rethrow_error;

  using foundation::Diagnostic_arena;
  using foundation::Diagnostic_iterator;

  namespace api {

    using namespace cdk::foundation::api;

  }  // cdk::api

  namespace connection {

    using foundation::connection::TCPIP;
    using foundation::connection::TLS;
    using foundation::connection::Error_eos;
    using foundation::connection::Error_no_connection;
    using foundation::connection::Error_timeout;

  }

} // cdk


#endif
