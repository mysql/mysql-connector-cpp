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

#include <iostream>
#include <mysqlx.h>
//#include <mysql/cdk.h>


using ::std::cout;
using ::std::endl;
//using namespace ::cdk;
using namespace ::mysqlx;

#if 0

class Doc_proc : public cdk::Document::Processor
{
  typedef cdk::string string;
  typedef cdk::Document Document;

  void begin() { cout <<"{" <<endl; }
  void end()   { cout <<"}" <<endl; }

  void field(const string &key, const string &val)
  {
    cout <<" " <<key <<": " <<'"' <<val <<'"' <<endl;
  }

  void field(const string &key, const int64_t val)
  {
    cout <<" " <<key <<": " <<val <<endl;
  }

  void field(const string &key, const uint64_t val)
  {
    cout <<" " <<key <<": " <<val <<endl;
  }

  void field(const string &key, const float val)
  {
    cout <<" " <<key <<": " <<val <<endl;
  }

  void field(const string &key, const double val)
  {
    cout <<" " <<key <<": " <<val <<endl;
  }

  void field(const string &key, const bool val)
  {
    cout <<" " <<key <<": " <<val <<endl;
  }

  void field(const string &key, const Document &val)
  {
    cout <<" " <<key <<": <sub-document>" <<endl;
    val.process(*this);
  }
};



class Expr_printer
  : public cdk::Expression::Processor
  , public cdk::api::Table_ref
{
  ostream &m_out;
  unsigned m_indent;
  cdk::string m_op_name;

public:

  Expr_printer(ostream &out, unsigned ind =0)
    : m_out(out), m_indent(ind)
  {}

  // Table_ref

  const cdk::string  name() const { return m_op_name; }
  const cdk::api::Schema_ref* schema() const { return NULL; }

  // Expr processor

  ostream& out_ind()
  {
    for (unsigned i=0; i < 2*m_indent; ++i)
      m_out.put(' ');
    return m_out;
  }

  virtual void null()
  {
    out_ind() <<"<null>" <<endl;
  }

  virtual void str(const cdk::string &val)
  {
    out_ind() <<"\"" <<val <<"\"" <<endl;
  }

  virtual void num(int64_t val)
  {
    out_ind() <<val <<endl;
  }

  virtual void num(uint64_t val)
  {
    out_ind() <<"U" <<val <<endl;
  }

  virtual void num(float val)
  {
    out_ind() <<"F" <<val <<endl;
  }

  virtual void num(double val)
  {
    out_ind() <<"D" <<val <<endl;
  }

  virtual void yesno(bool val)
  {
    out_ind() <<(val ? "TRUE" : "FALSE" ) <<endl;
  }

  virtual void var(const cdk::string &name)
  {
    out_ind() <<"@" <<name <<endl;
  }


  virtual void ref(const cdk::Doc_path &/*path*/)
  {
    out_ind() <<"$<path>" <<endl;
  }

  virtual void ref(const cdk::api::Column_ref &col, const cdk::Doc_path *path)
  {
    out_ind();

    if (col.table())
    {
      if (col.table()->schema())
        m_out <<"`" <<col.table()->schema()->name() <<"`.";
      m_out <<"`" <<(col.table()->name()) <<"`.";
    }
    m_out <<"`" <<col.name() <<"`";

    if (path)
      m_out <<"@<path>";

    m_out <<endl;
  }

  virtual void op(const char *name, const Expr_list &args)
  {
    std::string op_name("operator \"");
    op_name.append(name);
    op_name.append("\"");
    m_op_name= op_name;
    call(*this, args);
  }

  virtual void call(const cdk::api::Table_ref &db_obj, const Expr_list &args)
  {
    out_ind() << db_obj.name() <<" (" <<endl;
    if (0 == args.count())
    {
      m_out <<")" <<endl;
      return;
    }
    ++m_indent;
    for (unsigned pos=0; pos < args.count(); ++pos)
      args.get(pos).process(*this);
    --m_indent;
    out_ind() <<")" <<endl;
  }

  virtual void placeholder()
  {
    out_ind() <<"?" <<endl;
  }

  virtual void param(const cdk::string &name)
  {
    out_ind() <<":" <<name <<endl;
  }

  virtual void param(unsigned pos)
  {
    out_ind() <<":" <<pos <<endl;
  }
};

#endif


int main()
try {

  cout << "Done!" << endl;
}
catch (const mysqlx::Error &err)
{
  cout <<"ERROR: " <<err <<endl;
}
catch (std::exception &ex)
{
  cout <<"STD EXCEPTION: " <<ex.what() <<endl;
}
catch (const char *ex)
{
  cout <<"EXCEPTION: " <<ex <<endl;
}
