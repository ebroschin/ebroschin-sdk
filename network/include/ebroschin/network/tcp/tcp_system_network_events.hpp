#pragma once

#include "../network_event_handler.hpp"

#include <ebroschin/utility/variadic.hpp>

#include <tuple>

namespace ebroschin::network::tcp {

struct ConnectionCreated {};
struct ConnectionRemoved {};
struct ConnectionFailed {
  std::string error;
};

template <typename TTcpSystem>
struct TcpNetworkEvents {
  using SystemNetworkEvents = std::tuple<ConnectionCreated, ConnectionRemoved, ConnectionFailed>;

  using NetworkEvents = utility::CombineTypes<
    typename TTcpSystem::MessageTypes,
    SystemNetworkEvents
  >::Type;

  using EventHandlerType = utility::ApplyTypes<NetworkEventHandler, NetworkEvents>::Type;
};

}