#pragma once


#include "mock_network_codec.hpp"
#include "mock_tcp_connector.hpp"
#include "api.hpp"

#include "ebroschin/network/tcp/tcp_system.hpp"
#include "ebroschin/network/tcp/tcp_system_builder.hpp"

namespace ebroschin::network::tests {

using MockTcpSystem = tcp::TcpSystemBuilder<
    MockTcpConnector,
    MockNetworkCodec,
    MessageTypes
>::Type;

}