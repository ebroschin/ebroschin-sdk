#include <ebroschin/network-modules/connectors/boost_async/boost_tcp_resolver.hpp>
#include <iostream>

namespace ebroschin::network::modules {

BoostTcpResolver::~BoostTcpResolver() {
  resolver_.cancel();
  work_guard_.reset();
  io_context_.stop();

  io_thread_ = {};
}

void BoostTcpResolver::Start() {
  io_thread_ = std::jthread{[this] { io_context_.run(); }};
}

void BoostTcpResolver::Connect(BoostTcpResolverParameters parameters, ConnectionEventHandler* connection_event_handler) {
  resolver_.async_resolve(parameters.ip, parameters.port,
    [this, parameters, connection_event_handler]
    (const system::error_code& error, const asio::ip::tcp::resolver::results_type& results)
  {
    if (error) {
      OnConnectionFailed(parameters, connection_event_handler);
      return;
    }

    HandleResolve(results, parameters, connection_event_handler);
  });
}

void BoostTcpResolver::HandleResolve(const asio::ip::tcp::resolver::results_type& results,
  const BoostTcpResolverParameters& parameters,
  ConnectionEventHandler* connection_event_handler)
{
  auto socket = std::make_shared<asio::ip::tcp::socket>(io_context_);
  asio::async_connect(*socket, results,
    [this, socket, parameters, connection_event_handler]
    (const system::error_code& error, const asio::ip::tcp::endpoint&)
  {
    if (error) {
      OnConnectionFailed(parameters, connection_event_handler);
      return;
    }

    OnConnectionCreated(std::make_shared<BoostTcpConnection>(std::move(*socket)), connection_event_handler);
  });
}

}