#pragma once

#include "../commons.hpp"
#include "commons.hpp"
#include "rpc_concepts.hpp"

#include <functional>

namespace ebroschin::network::rpc {

template <RpcCompatibleTcpSystem TTcpSystem, RpcTimeoutHandler TTimeoutHandler>
class RpcSystem;

template <typename TRpcSystem, typename TRequest>
requires IsRpcMessage<TRequest>
class RpcCallBuilder {
public:
  explicit RpcCallBuilder(TRpcSystem& rpc_system, ConnectionId connection_id, TRequest request) noexcept:
    rpc_system_(rpc_system),
    connection_id_(connection_id),
    request_(std::move(request))
  {}

  auto& OnSuccess(std::function<void(const RpcResponseType<TRequest>&)> callback) {
    callback_ = std::move(callback);
    return *this;
  }

  auto& OnError(std::function<void(const RpcErrorType<TRequest>&)> callback) {
    error_callback_ = std::move(callback);
    return *this;
  }

  auto& OnTimeout(std::function<void()> callback) {
    timeout_callback_ = std::move(callback);
    return *this;
  }

  auto& SetTimeoutDuration(std::chrono::steady_clock::duration duration) {
    timeout_duration_.emplace(duration);
    return *this;
  }

  void Call() {
    rpc_system_.template Call<TRequest>(std::move(request_),
      std::move(callback_),
      std::move(error_callback_),
      std::move(timeout_callback_),
      std::move(timeout_duration_),
      connection_id_);
  }

private:
  TRpcSystem& rpc_system_;
  ConnectionId connection_id_;
  TRequest request_;

  std::function<void(const RpcResponseType<TRequest>&)> callback_{};
  std::function<void(const RpcErrorType<TRequest>&)> error_callback_{};
  std::function<void()> timeout_callback_{};
  std::optional<std::chrono::steady_clock::duration> timeout_duration_{};
};

}