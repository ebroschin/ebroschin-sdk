#pragma once

#include "../commons.hpp"

#include <memory>
#include <vector>

namespace ebroschin::network::tcp {

class TcpSystemConnectionFacadeBase {
public:
  virtual ~TcpSystemConnectionFacadeBase() = default;

  virtual void OnReceive(std::vector<std::byte> bytes) const = 0;
  virtual void OnDisconnect() const = 0;

  template<typename TTcpSystem>
  static std::unique_ptr<TcpSystemConnectionFacadeBase> Create(TTcpSystem* system,
    ConnectionId connection_id,
    TTcpSystem::ConnectionEventHandler* connection_event_handler);
};

template <typename TTcpSystem>
class TcpSystemConnectionFacade final : public TcpSystemConnectionFacadeBase {
public:
  using ConnectionEventHandler = TTcpSystem::ConnectionEventHandler;

  explicit TcpSystemConnectionFacade(TTcpSystem* tcp_system,
    ConnectionId connection_id,
    ConnectionEventHandler* connection_event_handler):
    tcp_system_{tcp_system},
    connection_id_{connection_id},
    connection_event_handler_{connection_event_handler}
  {}

  void OnReceive(std::vector<std::byte> bytes) const override {
    tcp_system_->ReceiveMessage(connection_id_, std::move(bytes));
  }

  void OnDisconnect() const override {
    tcp_system_->RemoveConnection(connection_id_);

    if (!connection_event_handler_) return;
    connection_event_handler_->OnDisconnected(connection_id_);
  }

private:
  TTcpSystem* tcp_system_;
  ConnectionId connection_id_;
  ConnectionEventHandler* connection_event_handler_;
};

template <typename TTcpSystem>
std::unique_ptr<TcpSystemConnectionFacadeBase>
TcpSystemConnectionFacadeBase::Create(TTcpSystem* system,
  ConnectionId connection_id,
  typename TTcpSystem::ConnectionEventHandler* connection_event_handler)
{
  return std::make_unique<TcpSystemConnectionFacade<TTcpSystem>>(system, connection_id, connection_event_handler);
}

}