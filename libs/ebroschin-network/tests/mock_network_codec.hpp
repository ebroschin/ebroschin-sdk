#pragma once

#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <vector>

namespace ebroschin::network::tests {

template <typename TMessage>
concept MockNetworkMessage =
  std::is_standard_layout_v<TMessage>
  && std::is_trivially_copyable_v<TMessage>;

struct MockNetworkCodec final {
  using DiscriminatorType = std::uint64_t;
  using PayloadType = std::vector<std::byte>;

  template<MockNetworkMessage TMessage>
  static std::vector<std::byte> Encode(const TMessage& message) {
    std::vector<std::byte> bytes{};
    bytes.resize(sizeof(std::uint64_t) + sizeof(TMessage));

    const auto type_id = TMessage::TypeId;
    std::memcpy(bytes.data(), &type_id, sizeof(type_id));
    std::memcpy(bytes.data() + sizeof(type_id), &message, sizeof(TMessage));
    return bytes;
  }

  template<MockNetworkMessage TMessage>
  static std::optional<TMessage> Decode(const PayloadType& payload) {
    if (payload.size() != sizeof(TMessage)) return std::nullopt;

    TMessage message{};
    std::memcpy(&message, payload.data(), sizeof(TMessage));
    return message;
  }

  static std::optional<std::pair<DiscriminatorType, PayloadType>>
  DecodeEnvelope(std::span<const std::byte> bytes) {
    if (bytes.size() < sizeof(std::uint64_t)) return std::nullopt;

    DiscriminatorType discriminator{};
    std::memcpy(&discriminator, bytes.data(), sizeof(DiscriminatorType));

    PayloadType payload(bytes.begin() + sizeof(DiscriminatorType), bytes.end());
    return std::make_pair(discriminator, std::move(payload));
  }
};

}
