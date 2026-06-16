#pragma once

#include <nlohmann/json.hpp>

#include <string>

namespace ebroschin::network::modules {

struct JsonNetworkCodec {
  using DiscriminatorType = std::uint64_t;
  using PayloadType = nlohmann::json;

  static constexpr std::string_view TypeIdKey = "type_id";
  static constexpr std::string_view PayloadKey = "payload";

  template<typename TMessage>
  static std::vector<std::byte> Encode(const TMessage& message) {
    const nlohmann::json result {
      {TypeIdKey, TMessage::TypeId},
      {PayloadKey, message},
    };

    const auto serialized_result = result.dump();
    auto bytes_ptr = reinterpret_cast<const std::byte*>(serialized_result.data());
    return {bytes_ptr, bytes_ptr + serialized_result.size()};
  }

  template<typename TMessage>
  static std::optional<TMessage> Decode(const PayloadType& payload) {
    try {
      return payload.get<TMessage>();
    } catch (...) {
      return std::nullopt;
    }
  }

  static std::optional<std::pair<DiscriminatorType, PayloadType>> DecodeEnvelope(std::span<const std::byte> bytes) {
    if (bytes.empty()) return std::nullopt;

    const auto json = nlohmann::json::parse(bytes.begin(), bytes.end(), nullptr, false);
    if (json.is_discarded()) return std::nullopt;
    if (!json.contains(TypeIdKey)) return std::nullopt;
    if (!json.contains(PayloadKey)) return std::nullopt;

    try {
      const auto discriminator = json[TypeIdKey].get<DiscriminatorType>();
      auto payload = json[PayloadKey];

      return std::make_pair(discriminator, std::move(payload));
    } catch (...) {
      return std::nullopt;
    }
  }
};

}