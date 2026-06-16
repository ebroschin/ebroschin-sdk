#pragma once

#include "ebroschin/network/tcp/tcp_system.hpp"
#include "ebroschin/network/tcp/tcp_system_builder.hpp"
#include "mock_network_codec.hpp"
#include "mock_tcp_connector.hpp"

#include <ebroschin/network-modules/message_handler/observable_message_handler.hpp>
#include "api.hpp"

namespace ebroschin::network::tests {

using MockTcpSystem = tcp::TcpSystemBuilder<
    MockTcpConnector,
    MockNetworkCodec,
    modules::ObservableMessageHandler,
    MessageTypes
>::Type;

}