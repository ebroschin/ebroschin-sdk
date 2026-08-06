#pragma once

#include "ebroschin/network/tcp/tcp_connector.hpp"
#include "mock_tcp_connection.hpp"

namespace ebroschin::network::tests {

struct MockConnectorParameters {};

class MockTcpConnector final : public tcp::TcpConnector<MockConnectorParameters, MockTcpConnection> {
public:
  void Connect(MockConnectorParameters, tcp::ConnectCallback callback) override {
    const auto connection = std::make_shared<MockTcpConnection>();
    CurrentConnection = connection;
    OnConnectionCreated(connection, std::move(callback));
  }

  static std::shared_ptr<MockTcpConnection> CurrentConnection;

protected:
  void Start() override {}
};

}
