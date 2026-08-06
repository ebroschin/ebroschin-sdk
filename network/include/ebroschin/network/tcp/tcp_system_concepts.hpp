#pragma once

#include "../commons.hpp"

#include <memory>
#include <vector>

namespace ebroschin::network::tcp {

template <typename TParameters, typename TConnection>
class TcpConnector;

template<typename TDiscriminator, typename TMessage>
concept NetworkMessage = requires(TMessage message) {
  { TMessage::TypeId } -> std::same_as<TDiscriminator>;
};

template<typename TDiscriminator, typename... TMessages>
concept NetworkMessages =
  (NetworkMessage<TMessages, TDiscriminator> && ...);

template <typename TConnector>
concept NetworkConnector =
  std::derived_from<TConnector, TcpConnector<typename TConnector::Parameters, typename TConnector::Connection>>;

template<typename TCodec, typename TMessage>
concept NetworkCodecFor =
requires(std::span<const std::byte> input_bytes,
    typename TCodec::PayloadType& payload,
    const TMessage& message)
{
  typename TCodec::DiscriminatorType;
  typename TCodec::PayloadType;

  { TCodec::template Encode<TMessage>(message) }
  -> std::same_as<std::vector<std::byte>>;

  { TCodec::DecodeEnvelope(input_bytes) }
  -> std::same_as<std::optional<std::pair<typename TCodec::DiscriminatorType, typename TCodec::PayloadType>>>;

  { TCodec::template Decode<TMessage>(payload) }
  -> std::same_as<std::optional<TMessage>>;
};

template<typename TCodec, typename... TMessages>
concept NetworkCodec =
  (NetworkCodecFor<TCodec, TMessages> && ...);

template<typename TMessage, typename... TMessages>
static constexpr bool IsValidMessage = (std::same_as<TMessage, TMessages> || ...);

}