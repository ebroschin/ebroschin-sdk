#pragma once

#include "tcp_system.hpp"

namespace ebroschin::network::tcp {

template<typename TConnector, typename TCodec, template<typename...> typename TMessageHandler, typename TMessageTuple>
struct TcpSystemBuilder;

template<typename TConnector, typename TCodec, template<typename...> typename TMessageHandler, typename... TMessages>
struct TcpSystemBuilder<TConnector, TCodec, TMessageHandler, std::tuple<TMessages...>> {
  using Type = TcpSystem<TConnector, TCodec, TMessageHandler<TMessages...>, TMessages...>;
};

}