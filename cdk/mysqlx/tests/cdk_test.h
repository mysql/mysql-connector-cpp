/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
 *
 * The MySQL Connector/C++ is licensed under the terms of the GPLv2
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

#ifndef MYSQL_CDK_MYSQLX_TESTS_XPLUGIN_CDK_TEST_H
#define MYSQL_CDK_MYSQLX_TESTS_XPLUGIN_CDK_TEST_H

#include <mysql/cdk/foundation.h>

#define TRY_TEST_GENERIC try

/*
  Macro to process the generic test errors.
  Can be extended as:

  TRY_TEST_GENERIC
  {
    ... // some code
  }
  CATCH_TEST_GENERIC
  catch(Custom_error &custom_error)
  {
    ... // processing for Custom_error type
  }

*/


#define CATCH_TEST_GENERIC catch (const cdk::Error &err) \
    { \
      cout << endl << "CDK Error: " << err << endl; \
      throw; \
    } \
    catch (const std::exception &e) \
    { \
      cout << endl << "Std Exception: " << e.what() << endl; \
      throw; \
    } \
    catch (const char *err) \
    { \
      cout << endl << "Exception: " << err << endl; \
      throw; \
    }

#endif
