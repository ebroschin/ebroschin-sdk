#pragma once

#include "tcp_connection_event_handler.hpp"

#include <memory>

namespace ebroschin::network::tcp {

template <typename TParameters>
class TcpConnectionEventHandler;

template <typename TConnector>
class TcpSystemConnectorFacadeBase {
public:
  using ConnectionEventHandler = TcpConnectionEventHandler<typename TConnector::Parameters>;
  using Connection = TConnector::Connection;
  using Parameters = TConnector::Parameters;

  virtual ~TcpSystemConnectorFacadeBase() = default;

  virtual void OnConnectionCreated(std::shared_ptr<typename TConnector::Connection> connection,
    ConnectionEventHandler* connection_event_handler) const = 0;

  template<typename TTcpSystem>
  static std::unique_ptr<TcpSystemConnectorFacadeBase> Create(TTcpSystem* system);
};

template <typename TTcpSystem, typename TConnector>
class TcpSystemConnectorFacade final : public TcpSystemConnectorFacadeBase<TConnector> {
public:
  using Base = TcpSystemConnectorFacadeBase<TConnector>;
  using ConnectionEventHandler = Base::ConnectionEventHandler;
  using Connection = Base::Connection;
  using Parameters = Base::Parameters;

  explicit TcpSystemConnectorFacade(TTcpSystem* tcp_system):
    tcp_system_{tcp_system}
  {}

  void OnConnectionCreated(std::shared_ptr<Connection> connection,
    ConnectionEventHandler* connection_event_handler) const override
  {
    tcp_system_->CreateConnection(std::move(connection), connection_event_handler);
  }

private:
  TTcpSystem* tcp_system_;
};

template <typename TConnector>
template <typename TTcpSystem>
std::unique_ptr<TcpSystemConnectorFacadeBase<TConnector>>
TcpSystemConnectorFacadeBase<TConnector>::Create(TTcpSystem* system)
{
  return std::make_unique<TcpSystemConnectorFacade<TTcpSystem, TConnector>>(system);
}

}