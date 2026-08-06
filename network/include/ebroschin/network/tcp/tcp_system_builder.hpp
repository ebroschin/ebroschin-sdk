#pragma once

#include "tcp_system.hpp"

namespace ebroschin::network::tcp {

template<typename TConnector, typename TCodec, typename TMessageTuple>
struct TcpSystemBuilder;

template<typename TConnector, typename TCodec, typename... TMessages>
struct TcpSystemBuilder<TConnector, TCodec, std::tuple<TMessages...>> {
  using Type = TcpSystem<TConnector, TCodec, TMessages...>;
};

}