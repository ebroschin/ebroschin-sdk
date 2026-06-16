#pragma once

#include "../commons.hpp"

namespace ebroschin::network::rpc {

template <typename TMessage>
concept IsRpcMessage =
requires(const TMessage& message)
{
  requires std::same_as<std::remove_cvref_t<decltype(message.request_id)>, RequestId>;
};

template <typename TRequest>
requires IsRpcMessage<TRequest>
struct RpcCall;

template <typename TRequest>
using RpcResponseType = RpcCall<std::remove_cvref_t<TRequest>>::Response;

template <typename TRequest>
using RpcErrorType = RpcCall<std::remove_cvref_t<TRequest>>::Error;

}