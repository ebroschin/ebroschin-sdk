#pragma once

#include "tcp_system_concepts.hpp"

#include <ebroschin/core/synchronization/executor.hpp>

#include <functional>

namespace ebroschin::network::tcp {

template <typename TCodec,
typename TMessageHandler,
typename... TMessages>
requires NetworkCodec<TCodec, TMessages...>
class TcpMessageProcessor {
  using Task = std::function<void()>;

public:
  using MessageHandler = TMessageHandler;

  explicit TcpMessageProcessor(core::Executor& executor):
    executor_{executor}
  {}

  void Process(ConnectionId id, std::vector<std::byte> bytes) {
    const auto envelope = TCodec::DecodeEnvelope(bytes);
    if (!envelope) return;

    static const auto message_handler_lookup = CreateMessageHandlerLookup();
    const auto type_id = envelope->first;
    const auto it = message_handler_lookup.find(type_id);
    if (it == message_handler_lookup.end()) return;

    const auto handler_function = it->second;
    auto payload = std::move(envelope->second);

    executor_.Post([this, handler_function, id, payload = std::move(payload)] {
      handler_function(this, id, payload);
    });
  }

  [[nodiscard]] MessageHandler& GetMessageHandler() noexcept {
    return message_handler_;
  }

private:
  template<typename TMessage>
  static void HandleMessage(TcpMessageProcessor* self, ConnectionId id, const TCodec::PayloadType& payload) {
    if (self == nullptr) return;

    auto message = TCodec::template Decode<TMessage>(payload);
    if (!message) return;

    self->message_handler_.HandleMessage(id, *message);
  }

  static auto CreateMessageHandlerLookup() {
    using FunctionType = void(*)(TcpMessageProcessor*, ConnectionId, const typename TCodec::PayloadType&);
    std::unordered_map<typename TCodec::DiscriminatorType, FunctionType> result{};
    ((result[TMessages::TypeId] = &TcpMessageProcessor::HandleMessage<TMessages>), ...);
    return result;
  }

  core::Executor& executor_;
  TMessageHandler message_handler_{};
};

}