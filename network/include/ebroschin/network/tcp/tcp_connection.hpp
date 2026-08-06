#pragma once

#include "tcp_system_callbacks.hpp"

#include <memory>
#include <span>
#include <vector>

namespace ebroschin::network::tcp {

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
  virtual ~TcpConnection() = default;

  void Initialize(ConnectionId connection_id, ConnectionCallbacks* callbacks) {
    connection_id_ = connection_id;
    callbacks_ = callbacks;
    Start();
  }

  virtual void Disconnect() = 0;
  virtual void SendBytes(std::span<const std::byte> bytes) = 0;

protected:
  virtual void Start() = 0;

  void OnReceive(std::vector<std::byte> bytes) const {
    if (!callbacks_) return;
    callbacks_->on_receive(connection_id_, std::move(bytes));
  }

  void OnDisconnect() const {
    if (!callbacks_) return;
    callbacks_->on_disconnect(connection_id_);
  }

private:
  ConnectionId connection_id_{0};
  ConnectionCallbacks* callbacks_{};
};

}