#pragma once

#include "tcp_system_connection_facade.hpp"

#include <memory>
#include <span>
#include <vector>

namespace ebroschin::network::tcp {

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
  virtual ~TcpConnection() = default;

  void Initialize(std::unique_ptr<TcpSystemConnectionFacadeBase>&& facade) {
    facade_ = std::move(facade);
    Start();
  }

  virtual void Disconnect() = 0;
  virtual void SendBytes(std::span<const std::byte> bytes) = 0;

protected:
  virtual void Start() = 0;

  void OnReceive(std::vector<std::byte> bytes) const {
    if (!facade_) return;
    facade_->OnReceive(std::move(bytes));
  }

  void OnDisconnect() const {
    if (!facade_) return;
    facade_->OnDisconnect();
  }

private:
  std::unique_ptr<TcpSystemConnectionFacadeBase> facade_{};
};

}