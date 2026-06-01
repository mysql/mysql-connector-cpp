/*
 * Copyright (c) 2026, Oracle and/or its affiliates.
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
 * separately licensed software that they have either included with the
 * program or referenced in the documentation.
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

#include "test.h"

#include <mysql/cdk/data_source.h>

#ifdef WITH_SSL

#include <string>
#include <vector>

using namespace cdk;

namespace {

struct Test_srv_host
{
  unsigned short prio;
  unsigned short weight;
  unsigned short port;
  std::string name;
};


class Test_DNS_SRV_source
  : public ds::DNS_SRV_source
{
public:

  Test_DNS_SRV_source(const std::string &host, const Options &opts)
    : DNS_SRV_source(host, opts)
  {}

  template <typename List>
  ds::Multi_source get_for_test(const List &list) const
  {
    return get(list);
  }
};


struct TCPIP_source
{
  std::string host;
  unsigned short port;
  std::string tls_host;
};


struct TCPIP_source_collector
{
  std::vector<TCPIP_source> sources;

  bool operator()(size_t, const ds::TCPIP &source,
                  const ds::TCPIP::Options &opts)
  {
    sources.push_back(
      TCPIP_source{source.host(), source.port(),
                   opts.get_tls().get_host_name()}
    );
    return false;
  }

#ifndef _WIN32
  bool operator()(size_t, const ds::Unix_socket &,
                  const ds::Unix_socket::Options &)
  {
    return false;
  }
#endif

  bool operator()(size_t, const ds::TCPIP_old &,
                  const ds::TCPIP_old::Options &)
  {
    return false;
  }
};


std::vector<TCPIP_source> collect_tcpip_sources(ds::Multi_source &source)
{
  TCPIP_source_collector collector;
  source.visit(collector);
  return collector.sources;
}


ds::TCPIP::Options verify_identity_options(const std::string &tls_host = "")
{
  ds::TCPIP::Options opts;
  ds::TCPIP::Options::TLS_options tls(
    ds::TCPIP::Options::TLS_options::SSL_MODE::VERIFY_IDENTITY
  );

  if (!tls_host.empty())
    tls.set_host_name(tls_host);

  opts.set_tls(tls);
  return opts;
}


std::vector<Test_srv_host> srv_hosts()
{
  std::vector<Test_srv_host> hosts;
  hosts.push_back(Test_srv_host{10, 20, 33061, "srv-target.example.test"});
  return hosts;
}

}  // namespace


TEST(Foundation_data_source,
     dns_srv_verify_identity_uses_service_host_for_tls)
{
  const Test_DNS_SRV_source dns_srv("service.example.test",
                                    verify_identity_options());

  ds::Multi_source source = dns_srv.get_for_test(srv_hosts());
  std::vector<TCPIP_source> sources = collect_tcpip_sources(source);

  ASSERT_EQ(1U, sources.size());
  EXPECT_EQ("srv-target.example.test", sources[0].host);
  EXPECT_EQ(33061, sources[0].port);
  EXPECT_EQ("service.example.test", sources[0].tls_host);
}


TEST(Foundation_data_source,
     dns_srv_verify_identity_preserves_explicit_tls_host)
{
  const Test_DNS_SRV_source dns_srv(
    "service.example.test",
    verify_identity_options("explicit.example.test")
  );

  ds::Multi_source source = dns_srv.get_for_test(srv_hosts());
  std::vector<TCPIP_source> sources = collect_tcpip_sources(source);

  ASSERT_EQ(1U, sources.size());
  EXPECT_EQ("srv-target.example.test", sources[0].host);
  EXPECT_EQ(33061, sources[0].port);
  EXPECT_EQ("explicit.example.test", sources[0].tls_host);
}

#endif  // WITH_SSL
