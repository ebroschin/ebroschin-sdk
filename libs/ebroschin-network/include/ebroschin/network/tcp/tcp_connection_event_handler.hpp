#pragma once

#include "../commons.hpp"

namespace ebroschin::network::tcp {

template <typename TParameters>
class TcpConnectionEventHandler {
public:
  virtual ~TcpConnectionEventHandler() = default;

  virtual void OnConnected(ConnectionId connection_id) = 0;
  virtual void OnConnectionFailed(const TParameters& parameters) = 0;
  virtual void OnDisconnected(ConnectionId connection_id) = 0;
};

}