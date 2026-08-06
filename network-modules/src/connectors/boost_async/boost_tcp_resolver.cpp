#include <ebroschin/network-modules/connectors/boost_async/boost_tcp_resolver.hpp>

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

void BoostTcpResolver::Connect(BoostTcpResolverParameters parameters, tcp::ConnectCallback callback) {
  resolver_.async_resolve(parameters.ip, parameters.port,
    [this, parameters, callback = std::move(callback)]
    (const system::error_code& error, const asio::ip::tcp::resolver::results_type& results) mutable
  {
    if (error) {
      OnConnectionFailed("Unable to connect to address " + parameters.ip + ":" + parameters.port, std::move(callback));
      return;
    }

    HandleResolve(results, parameters, std::move(callback));
  });
}

void BoostTcpResolver::HandleResolve(const asio::ip::tcp::resolver::results_type& results,
  const BoostTcpResolverParameters& parameters,
  tcp::ConnectCallback callback)
{
  auto socket = std::make_shared<asio::ip::tcp::socket>(io_context_);
  asio::async_connect(*socket, results,
    [this, socket, parameters, callback = std::move(callback)]
    (const system::error_code& error, const asio::ip::tcp::endpoint&) mutable
  {
    if (error) {
      OnConnectionFailed("Unable to connect to address " + parameters.ip + ":" + parameters.port, std::move(callback));
      return;
    }

    OnConnectionCreated(std::make_shared<Connection>(std::move(*socket)), std::move(callback));
  });
}

}
