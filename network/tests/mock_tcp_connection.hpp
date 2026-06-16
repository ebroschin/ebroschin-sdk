#pragma once

#include "ebroschin/network/tcp/tcp_connection.hpp"

namespace ebroschin::network::tests {

class MockTcpConnection final : public tcp::TcpConnection {
public:
  void ReceiveBytes(std::vector<std::byte> bytes) const {
    OnReceive(std::move(bytes));
  }

  [[nodiscard]] std::span<const std::byte> GetSentBytes() const {
    return sent_bytes_;
  }

  void Disconnect() override {}

protected:
  void Start() override {}

private:
  void SendBytes(std::span<const std::byte> bytes) override {
    sent_bytes_.assign(bytes.begin(), bytes.end());
  }

  std::vector<std::byte> sent_bytes_{};
};

}