#pragma once

#include "../commons.hpp"

#include <chrono>
#include <concepts>
#include <functional>

namespace ebroschin::network::rpc {

template <typename TTimeoutHandler>
concept RpcTimeoutHandler =
  requires(TTimeoutHandler handler,
    RequestId request_id,
    std::chrono::steady_clock::duration duration,
    std::function<void()> callback)
{
  { handler.ScheduleTimeout(request_id, duration, callback) }
  -> std::same_as<void>;

  { handler.CancelTimeout(request_id) }
  -> std::same_as<void>;
};

template <typename TRpcSubscriptionHandle>
concept RpcSubscriptionHandle =
  std::move_constructible<TRpcSubscriptionHandle>
  && std::destructible<TRpcSubscriptionHandle>;

template <typename TMessageHandler, typename TMessage>
concept RpcMessageHandlerFor =
  requires(TMessageHandler handler, std::function<void(ConnectionId, const TMessage&)> callback)
{
  { handler.template Subscribe<TMessage>(callback) }
  -> RpcSubscriptionHandle;
};

template <typename TMessageHandler, typename... TMessages>
concept RpcMessageHandler =
  (RpcMessageHandlerFor<TMessageHandler, TMessages> && ...)
  && requires { TMessageHandler::SubscriptionHandle; };

template <typename TTcpSystem>
concept RpcCompatibleTcpSystem =
  []<typename... TMessages>(std::type_identity<std::tuple<TMessages...>>) {
    return (RpcMessageHandlerFor<typename TTcpSystem::MessageHandler, TMessages> && ...);
  }(std::type_identity<typename TTcpSystem::MessageTypes>{});

}