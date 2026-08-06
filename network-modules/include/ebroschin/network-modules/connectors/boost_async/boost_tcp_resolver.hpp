#pragma once

#include "boost_tcp_connection.hpp"

#include <ebroschin/network/tcp/tcp_connector.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>
#include <thread>

using namespace boost;

namespace ebroschin::network::modules {

struct BoostTcpResolverParameters {
  std::string ip;
  std::string port;
};

class BoostTcpResolver final : public tcp::TcpConnector<BoostTcpResolverParameters, BoostTcpConnection> {
public:
  ~BoostTcpResolver() override;

  void Start() override;
  void Connect(BoostTcpResolverParameters parameters, tcp::ConnectCallback callback) override;

private:
  void HandleResolve(const asio::ip::tcp::resolver::results_type& results,
    const BoostTcpResolverParameters& parameters,
    tcp::ConnectCallback callback);

  asio::io_context io_context_{};
  asio::ip::tcp::resolver resolver_{io_context_};

  asio::executor_work_guard<asio::io_context::executor_type>
  work_guard_{asio::make_work_guard(io_context_)};

  std::jthread io_thread_{};
};

}
