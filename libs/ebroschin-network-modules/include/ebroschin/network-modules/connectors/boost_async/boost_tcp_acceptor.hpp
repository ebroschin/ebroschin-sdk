#pragma once

#include "boost_tcp_connection.hpp"

#include <ebroschin/network/tcp/tcp_connector.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <string>
#include <thread>

namespace ebroschin::network::modules {

using namespace boost;

struct BoostTcpAcceptorParameters {
  std::string ip;
  unsigned short port;
};

class BoostTcpAcceptor final : public tcp::TcpConnector<BoostTcpAcceptorParameters, BoostTcpConnection> {
public:
  ~BoostTcpAcceptor() override;

  void Start() override;
  void Connect(Parameters parameters, ConnectionEventHandler* connection_event_handler) override;

private:
  void StartAccept();

  asio::io_context io_context_{};
  asio::executor_work_guard<asio::io_context::executor_type>
  work_guard_{asio::make_work_guard(io_context_)};

  std::jthread io_thread_{};
  std::unique_ptr<asio::ip::tcp::acceptor> acceptor_{};
  ConnectionEventHandler* connection_event_handler_{};
};

}