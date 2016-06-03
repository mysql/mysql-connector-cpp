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

#include "socket.h"


using namespace ::boost::asio;
using namespace ::boost::asio::ip;

using namespace cdk::foundation;

template<class boost_buffer_type>
class buffers_wrapper
{
  const buffers& m_bufs;

public:
  buffers_wrapper(const buffers& bufs)
    : m_bufs(bufs)
  {}

  // MutableBufferSequence requirements
  // http://www.boost.org/doc/libs/1_57_0/doc/html/boost_asio/reference/MutableBufferSequence.html
  class const_iterator
  {
    const buffers *m_bufs;
    size_t m_pos;

  public:
    const_iterator(const buffers *bufs= NULL, size_t pos= 0) : m_bufs(bufs), m_pos(pos)
    {}
    const_iterator(const const_iterator &src)
      : m_bufs(src.m_bufs), m_pos(src.m_pos)
    {}


    boost_buffer_type operator*()
    {
      bytes tmp= m_bufs->get_buffer(m_pos);
      return boost_buffer_type(tmp.begin(), tmp.size());
    }

    const_iterator& operator++()
    {
      ++m_pos;
      return *this;
    }

    const_iterator operator++(int)
    {
      const_iterator tmp(*this);
      ++*this;
      return tmp;
    }

    // Inlined friends are discussed here:
    // http://web.mst.edu/~nmjxv3/articles/templates.html
    // http://stackoverflow.com/questions/8207633/whats-the-scope-of-inline-friend-functions
    friend bool operator==(const const_iterator& x, const const_iterator& y)
    {
      return x.m_bufs == y.m_bufs && (x.m_bufs == NULL || x.m_pos == y.m_pos);
    }
  };

  const_iterator begin()
  {
    return const_iterator(&m_bufs);
  }

  const_iterator end()const
  {
    return const_iterator(&m_bufs, m_bufs.buf_count());
  }
};



cdk::api::Input_stream::Op& Socket::Connection::read(const buffers &bufs)
{
  m_howmuch= boost::asio::read(m_sock, buffers_wrapper<boost::asio::mutable_buffer>(bufs));
  return *this;
}

cdk::api::Output_stream::Op& Socket::Connection::write(const buffers &bufs)
{
  m_howmuch= boost::asio::write(m_sock, buffers_wrapper<boost::asio::const_buffer>(bufs));
  return *this;
}
