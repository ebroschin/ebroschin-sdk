#include "ebroschin/network-modules/connectors/boost_async/boost_tcp_connection.hpp"

#include <span>

namespace ebroschin::network::modules {

BoostTcpConnection::BoostTcpConnection(boost::asio::ip::tcp::socket socket):
  socket_{std::move(socket)}
{}

void BoostTcpConnection::Start() {
  ReadBytes();
}

void BoostTcpConnection::Disconnect() {
  socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
  socket_.close();
}

void BoostTcpConnection::SendBytes(std::span<const std::byte> bytes) {
  std::vector<std::byte> buffer{};
  buffer.reserve(sizeof(uint32_t) + bytes.size());

  const uint32_t network_length = htonl(static_cast<std::uint32_t>(bytes.size()));
  const auto network_length_ptr = reinterpret_cast<const std::byte*>(&network_length);

  buffer.insert(buffer.end(), network_length_ptr, network_length_ptr + sizeof(uint32_t));
  buffer.insert(buffer.end(), bytes.begin(), bytes.end());

  {
    std::scoped_lock lock{mutex_};
    outgoing_bytes_.emplace(std::move(buffer));
    if (outgoing_bytes_.size() > 1) return;
  }

  SendNext();
}

void BoostTcpConnection::SendNext() {
  std::shared_ptr<std::vector<std::byte>> bytes{};
  {
    std::scoped_lock lock{mutex_};
    if (outgoing_bytes_.empty()) return;

    bytes = std::make_shared<std::vector<std::byte>>(std::move(outgoing_bytes_.front()));
    outgoing_bytes_.pop();
  }

  boost::asio::async_write(socket_, boost::asio::buffer(*bytes),
  [this, self = shared_from_this(), bytes](const boost::system::error_code& error, std::size_t)
  {
    if (HandleError(error)) return;
    SendNext();
  });
}

void BoostTcpConnection::ReadBytes() {
  boost::asio::async_read(socket_, boost::asio::buffer(&incoming_bytes_length_buffer_, sizeof(incoming_bytes_length_buffer_)),
    [this, self = shared_from_this()](const boost::system::error_code& error, std::size_t) {
      if (HandleError(error)) return;

      const auto host_length = ntohl(incoming_bytes_length_buffer_);
      if (host_length > MaxMessageBytes) {
        OnDisconnect();
        return;
      }

      incoming_bytes_buffer_.resize(host_length);

      boost::asio::async_read(socket_, boost::asio::buffer(incoming_bytes_buffer_),
      [this, self](const boost::system::error_code& ec, std::size_t) {
        if (HandleError(ec)) return;

        OnReceive(std::move(incoming_bytes_buffer_));
        ReadBytes();
      });
    });
}

bool BoostTcpConnection::HandleError(const boost::system::error_code& error) const {
  if (!error) return false;

  OnDisconnect();
  return true;
}

}