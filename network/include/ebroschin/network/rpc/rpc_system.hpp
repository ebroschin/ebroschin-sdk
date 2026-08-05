#pragma once

#include "rpc_call_builder.hpp"

#include <ebroschin/core/system_context.hpp>

#include <functional>

namespace ebroschin::network::rpc {

using namespace std::chrono_literals;

template <RpcCompatibleTcpSystem TTcpSystem, RpcTimeoutHandler TTimeoutHandler>
class RpcSystem final : public core::System {
public:
  using RpcMessageHandler = TTcpSystem::MessageHandler;
  using RpcSubscriptionHandle = RpcMessageHandler::SubscriptionHandle;

  template <typename... TTimeoutHandlerArguments>
  explicit RpcSystem(const core::SystemContext& ctx, TTimeoutHandlerArguments&&... arguments) noexcept:
  System{ctx},
  tcp_system_{ctx.Require<TTcpSystem>()},
  message_handler_{tcp_system_.GetMessageHandler()},
  timeout_handler_{std::forward<TTimeoutHandlerArguments>(arguments)...}
  {}

  template <typename TRequest, typename... TArguments>
  requires IsRpcMessage<TRequest>
  auto Prepare(ConnectionId connection_id, TArguments&&... arguments) {
    const auto request_id = next_id_.fetch_add(1, std::memory_order_relaxed);
    auto request = TRequest{request_id, std::forward<TArguments>(arguments)...};
    return RpcCallBuilder<RpcSystem, TRequest>{*this, connection_id, std::move(request)};
  }

private:
  struct RpcPendingCall {
    RpcSubscriptionHandle success_subscription;
    RpcSubscriptionHandle error_subscription;
  };

  template <typename TRequest>
  requires IsRpcMessage<TRequest>
  void Call(TRequest request,
    std::function<void(const RpcResponseType<TRequest>&)> success_callback,
    std::function<void(const RpcErrorType<TRequest>&)> error_callback,
    std::function<void()> timeout_callback,
    std::optional<std::chrono::steady_clock::duration> timeout_duration,
    ConnectionId connection_id)
  {
    using TResponse = RpcResponseType<TRequest>;
    using TError = RpcErrorType<TRequest>;

    const auto request_id = request.request_id;
    auto success_handle = message_handler_.template Subscribe<TResponse>([this, callback = std::move(success_callback), request_id, connection_id]
      (ConnectionId id, const TResponse& response)
    {
      if (connection_id != id) return;
      if (response.request_id != request_id) return;
      HandleCallback(request_id, callback, response);
    });

    auto error_handle = message_handler_.template Subscribe<TError>([this, callback = std::move(error_callback), request_id, connection_id]
      (ConnectionId id, const TError& response)
    {
      if (connection_id != id) return;
      if (response.request_id != request_id) return;
      HandleCallback(request_id, callback, response);
    });

    {
      std::scoped_lock lock{mutex_};
      calls_.try_emplace(request.request_id, RpcPendingCall{std::move(success_handle), std::move(error_handle)});
    }

    if (timeout_duration) {
      timeout_handler_.ScheduleTimeout(request_id,
      *timeout_duration,
      [this, callback = std::move(timeout_callback), request_id] {
        HandleCallback(request_id, callback);
      });
    }

    tcp_system_.template Send<TRequest>(connection_id, std::move(request));
  }

  template <typename TCallback, typename... TCallbackArguments>
  void HandleCallback(RequestId request_id, TCallback callback, TCallbackArguments&&... args) {
    {
      std::scoped_lock lock{mutex_};
      const auto it = calls_.find(request_id);
      if (it == calls_.end()) return;

      calls_.erase(it);
    }

    timeout_handler_.CancelTimeout(request_id);

    if (callback) {
      callback(std::forward<TCallbackArguments>(args)...);
    }
  }

  TTcpSystem& tcp_system_;
  RpcMessageHandler& message_handler_;
  TTimeoutHandler timeout_handler_;

  std::mutex mutex_{};
  std::unordered_map<RequestId, RpcPendingCall> calls_{};
  std::atomic<RequestId> next_id_{1};

  template <class, typename TRequest>
  requires IsRpcMessage<TRequest>
  friend class RpcCallBuilder;
};

}