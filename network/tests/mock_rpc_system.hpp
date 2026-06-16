#pragma once

#include "ebroschin/network/rpc/rpc_system.hpp"
#include "mock_tcp_system.hpp"
#include "mock_rpc_timeout_handler.hpp"

namespace ebroschin::network::tests {

using MockRpcSystem = rpc::RpcSystem<MockTcpSystem, MockRpcTimeoutHandler>;

}