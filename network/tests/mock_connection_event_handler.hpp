#pragma once

#include "ebroschin/network/commons.hpp"
#include "ebroschin/network/tcp/tcp_connection_event_handler.hpp"
#include "mock_tcp_connector.hpp"

#include <optional>

namespace ebroschin::network::tests {

struct MockConnectionEventHandler final : tcp::TcpConnectionEventHandler<MockConnectorParameters> {
  std::optional<ConnectionId> current_connection_id;

  void OnConnected(ConnectionId connection_id) override {
    current_connection_id.emplace(connection_id);
  }

  void OnConnectionFailed(const MockConnectorParameters&) override {}
  void OnDisconnected(ConnectionId) override {}
};

}