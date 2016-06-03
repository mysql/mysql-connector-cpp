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

#include <iostream>
#include <string>
#include <algorithm>
#include <expr_parser.h>
#include <protocol.h>
#include <builders.h>
#include <converters.h>

using namespace std;


int main(int argn, char *argv[])
{
  if (argn != 3)
  {
    cout << "Usage:" << endl;
    cout << " " << argv[0] << " type expr" << endl;
    cout << " type: col|tab" << endl;
    return 1;
  }

  parser::Parser_mode::value type = parser::Parser_mode::DOCUMENT;

  std::string type_str = argv[1];
  std::transform(type_str.begin(), type_str.end(), type_str.begin(), ::tolower);

  if (type_str == "tab")
    type = parser::Parser_mode::TABLE;

  try {

    parser::Expression_parser expr(type, argv[2]);

    Mysqlx::Expr::Expr pb_expr;
    cdk::protocol::mysqlx::Expr_builder eb(pb_expr);

    cdk::mysqlx::Expr_converter conv;

    conv.reset(expr);

    conv.process(eb);

    cout << "OK" << endl;

    cout << pb_expr.DebugString() << endl;

  }
  catch( cdk::Error &e)
  {
    cout << "ERROR" << endl;
    cout << e << endl;
  }


  return 0;
}
