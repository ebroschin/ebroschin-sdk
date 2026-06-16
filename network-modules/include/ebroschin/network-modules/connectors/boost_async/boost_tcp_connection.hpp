#pragma once

#include <ebroschin/network/tcp/tcp_connection.hpp>
#include <boost/asio.hpp>

#include <queue>

namespace ebroschin::network::modules {

class BoostTcpConnection final : public tcp::TcpConnection {
public:
  explicit BoostTcpConnection(boost::asio::ip::tcp::socket socket);

  void Start() override;
  void SendBytes(std::span<const std::byte> bytes) override;
  void Disconnect() override;

private:
  static constexpr std::uint32_t MaxMessageBytes = 1048576;

  void SendNext();
  void ReadBytes();
  bool HandleError(const boost::system::error_code& error) const;

  boost::asio::ip::tcp::socket socket_;
  std::queue<std::vector<std::byte>> outgoing_bytes_{};

  std::uint32_t incoming_bytes_length_buffer_{};
  std::vector<std::byte> incoming_bytes_buffer_{};
  std::mutex mutex_{};
};

}