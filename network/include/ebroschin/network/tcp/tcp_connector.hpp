#pragma once

#include "tcp_system_connector_facade.hpp"

#include <memory>

namespace ebroschin::network::tcp {

template <typename TParameters, typename TConnection>
class TcpConnector {
public:
  using Parameters = TParameters;
  using Connection = TConnection;
  using ConnectionEventHandler = TcpConnectionEventHandler<Parameters>;

  virtual ~TcpConnector() = default;

  void Initialize(std::unique_ptr<TcpSystemConnectorFacadeBase<TcpConnector>>&& facade)  {
    facade_ = std::move(facade);
    Start();
  }

  virtual void Connect(Parameters parameters, ConnectionEventHandler* connection_event_handler) = 0;

protected:
  virtual void Start() = 0;

  void OnConnectionCreated(std::shared_ptr<Connection> connection, ConnectionEventHandler* connection_event_handler) const {
    if (!facade_) return;
    facade_->OnConnectionCreated(std::move(connection), connection_event_handler);
  }

  void OnConnectionFailed(const Parameters& parameters, ConnectionEventHandler* connection_event_handler) const {
    if (!connection_event_handler) return;
    connection_event_handler->OnConnectionFailed(parameters);
  }

private:
  std::unique_ptr<TcpSystemConnectorFacadeBase<TcpConnector>> facade_{};
};

}
