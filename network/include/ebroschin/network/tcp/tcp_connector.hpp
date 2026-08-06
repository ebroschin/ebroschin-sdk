#pragma once

#include "tcp_system_callbacks.hpp"

namespace ebroschin::network::tcp {

template <typename TParameters, typename TConnection>
class TcpConnector {
public:
  using Parameters = TParameters;
  using Connection = TConnection;

  virtual ~TcpConnector() = default;

  void Initialize(ConnectorCallbacks* callbacks)  {
    callbacks_ = callbacks;
    Start();
  }

  virtual void Connect(Parameters parameters, ConnectCallback callback) = 0;

protected:
  virtual void Start() = 0;

  void OnConnectionCreated(std::shared_ptr<Connection> connection, ConnectCallback callback) const {
    if (!callbacks_) return;
    callbacks_->on_connection_created(std::shared_ptr<TcpConnection>{std::move(connection)}, std::move(callback));
  }

  void OnConnectionFailed(std::string error, ConnectCallback callback) const {
    if (!callbacks_) return;
    callbacks_->on_connection_failed(std::move(error), std::move(callback));
  }

private:
  ConnectorCallbacks* callbacks_{};
};

}
