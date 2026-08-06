#pragma once

#include "tcp_system_concepts.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace ebroschin::network::tcp {

class TcpConnection;

struct ConnectionResult {
  std::optional<ConnectionId> connection_id{};
  std::string error{};

  [[nodiscard]] bool Ok() const noexcept {
    return error.empty();
  }
};

using ConnectCallback = std::function<void(ConnectionResult)>;

struct ConnectorCallbacks {
  std::function<void(std::shared_ptr<TcpConnection>, ConnectCallback)> on_connection_created;
  std::function<void(std::string, ConnectCallback)> on_connection_failed;
};

struct ConnectionCallbacks {
  std::function<void(ConnectionId, std::vector<std::byte> bytes)> on_receive;
  std::function<void(ConnectionId)> on_disconnect;
};

}
