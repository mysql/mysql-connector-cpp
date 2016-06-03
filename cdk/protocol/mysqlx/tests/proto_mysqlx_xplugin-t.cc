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

/*
  Test mysqlx::Protocol class against xplugin.
*/


#include "test.h"


namespace cdk {
namespace test {


TEST_F(Protocol_mysqlx_xplugin, basic)
{
  SKIP_IF_NO_XPLUGIN;

  using cdk::byte;

  authenticate();
  Protocol &proto= get_proto();

  cout <<"=== Execute query ===" <<endl;
  cdk::string query(L"select -1 as number, \"foo\" as string");
  cout << query <<endl;
  proto.snd_StmtExecute("sql", query, NULL).wait();

  cout <<"Fetch meta-data" <<endl;
  Mdata_handler mdh;
  proto.rcv_MetaData(mdh).wait();

  cout <<"Fetch rows" <<endl;
  Row_handler rh;
  proto.rcv_Rows(rh).wait();

  cout <<"Final OK" <<endl;
  Stmt_handler sh;
  proto.rcv_StmtReply(sh).wait();

  // Try wrong sequence of receive operations

  cout <<"=== Execute query ===" <<endl;
  cout << query <<endl;
  proto.snd_StmtExecute("sql", query, NULL).wait();

  try {
    proto.rcv_Rows(rh).wait();
    FAIL() << "rcv_Rows() should not work directly after StmtExecute";
  }
  catch (Error &e)
  {
    cout <<"Expected error in rcv_Rows() after StmtExecute: "
         <<e <<endl;
  }
  catch (const char* e)
  {
    cout <<"Expected error in rcv_Rows() after StmtExecute: "
         <<e <<endl;
  }

  // process remaining server reply
  proto.rcv_MetaData(mdh).wait();
  proto.rcv_Rows(rh).wait();
  proto.rcv_StmtReply(sh).wait();

  cout <<"Done!" <<endl;
}


TEST_F(Protocol_mysqlx_xplugin, no_rset)
{
  SKIP_IF_NO_XPLUGIN;

  using cdk::byte;

  authenticate();
  Protocol &proto= get_proto();

  cout <<"=== Execute query ===" <<endl;
  cout <<"set @foo = 1" <<endl;
  proto.snd_StmtExecute("sql", L"set @foo = 1", NULL).wait();

  cout <<"Fetch meta-data" <<endl;
  Mdata_handler mdh;
  proto.rcv_MetaData(mdh).wait();

  cout <<"Final OK" <<endl;
  Stmt_handler sh;
  proto.rcv_StmtReply(sh).wait();

  cout <<"=== Execute query ===" <<endl;
  cout <<"set @foo = 2" <<endl;
  proto.snd_StmtExecute("sql", L"set @foo = 2", NULL).wait();

  cout <<"Fetch meta-data" <<endl;
  proto.rcv_MetaData(mdh).wait();

  cout <<"Final OK" <<endl;
  proto.rcv_StmtReply(sh).wait();

  cout <<"Done!" <<endl;
}


class Row_handler_interrupt : public Row_handler
{
private:
  int m_rows_to_fetch;
  row_count_t m_rows_read;

public:
  std::vector<string> row_ids;

  Row_handler_interrupt() : m_rows_to_fetch(0), m_rows_read(0)
  {}

  void set_rows_to_fetch(int n)
  { m_rows_to_fetch = n; }

  virtual size_t col_data(col_count_t col, bytes data)
  {
    memcpy(buf+pos, data.begin(), data.size());
    char local_buf[128];
    pos += data.size();

    /* Use this for making a local string */
    memcpy(local_buf, data.begin(), data.size());
    local_buf[data.size()] = '\0';

    if (col == 0)
    {
      string s1 = row_ids[(unsigned)m_row_num];
      string s2(local_buf);
      EXPECT_EQ(s1.compare(s2) , 0);
    }

    return sizeof(buf) - pos;
  }

  row_count_t get_read_rows_number()
  { return m_rows_read; }


  virtual void row_end(row_count_t row)
  {
    cout <<"== end row #" <<row <<endl;
    m_rows_read = row + 1;

    if (row == (unsigned)(m_rows_to_fetch - 1))
    {
      // signal that processor wants to interrupt reading
      m_rows_to_fetch = -1;
    }
  }

  virtual bool message_end()
  {
    if (m_rows_to_fetch == -1)
    {
      // set back to not initialized
      m_rows_to_fetch = 0;
      m_row_num = 0;
      return false;
    }
    return true;
  }
};


TEST_F(Protocol_mysqlx_xplugin, row_fetch_interrupt)
{
  SKIP_IF_NO_XPLUGIN;

  using cdk::byte;

  authenticate();
  Protocol &proto= get_proto();

  proto.snd_StmtExecute("sql",
    L"SELECT BINARY 'aa' UNION SELECT BINARY 'bb' as number UNION SELECT BINARY 'cc' as number UNION SELECT BINARY 'dd' as number UNION SELECT BINARY 'ee' as number",
    NULL).wait();
  cout <<"Metadata" <<endl;
  Mdata_handler mdh;
  proto.rcv_MetaData(mdh).wait();

  cout <<"Rows" <<endl;
  Row_handler_interrupt rhi;

  rhi.row_ids.push_back("aa");
  rhi.row_ids.push_back("bb");
  rhi.row_ids.push_back("cc");

  rhi.set_rows_to_fetch(3);

  proto.rcv_Rows(rhi).wait(); // this call will read only 3 rows and then stop

  if (rhi.get_read_rows_number() != 3)
    FAIL();

  cout <<"Reading rows interrupted" <<endl;

  /*
    Row numbers will start from 0 again. So, clearing the list
    and adding new values to check
  */
  rhi.row_ids.clear();
  rhi.row_ids.push_back("dd");
  rhi.row_ids.push_back("ee");

  cout <<"Resuming reading remaing rows" <<endl;

  proto.rcv_Rows(rhi).wait(); // this call will read remaining rows

  if (rhi.get_read_rows_number() != 2)
    FAIL();

  Stmt_handler sh;
  proto.rcv_StmtReply(sh).wait();
  cout <<"Done" <<endl;
}

}}  // cdk::test

