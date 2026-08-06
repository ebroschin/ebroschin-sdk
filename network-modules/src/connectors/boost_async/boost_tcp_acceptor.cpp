#include "ebroschin/network-modules/connectors/boost_async/boost_tcp_acceptor.hpp"

namespace ebroschin::network::modules {

BoostTcpAcceptor::~BoostTcpAcceptor() {
  if (acceptor_ && acceptor_->is_open()) {
    (void)acceptor_->cancel();
    (void)acceptor_->close();
  }

  work_guard_.reset();
  io_context_.stop();
  io_thread_ = {};
}

void BoostTcpAcceptor::Start() {
  io_thread_ = std::jthread{[this] { io_context_.run(); }};
}

void BoostTcpAcceptor::Connect(BoostTcpAcceptorParameters parameters, tcp::ConnectCallback callback) {
  if (acceptor_ != nullptr) return;

  const auto address = asio::ip::make_address(parameters.ip);
  try {
    acceptor_ = std::make_unique<asio::ip::tcp::acceptor>(io_context_, asio::ip::tcp::endpoint{address, parameters.port});
    StartAccept();
  } catch (...) {
    OnConnectionFailed("Unable to bind to address " + parameters.ip + ":" + std::to_string(parameters.port), std::move(callback));
  }
}

void BoostTcpAcceptor::StartAccept() {
  acceptor_->async_accept([this](const system::error_code& error, asio::ip::tcp::socket socket) {
    if (error == asio::error::operation_aborted) return;
    if (!error) {
      OnConnectionCreated(std::make_shared<Connection>(std::move(socket)), {});
    }

    StartAccept();
  });
}

}
