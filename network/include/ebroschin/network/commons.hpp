#pragma once

#include <cstdint>
#include <optional>

namespace ebroschin::network {

using ConnectionId = std::uint64_t;
using MessageProcessorId = std::uint64_t;
using RequestId = std::uint64_t;

template <typename TEvent>
struct NetworkEvent {
  std::optional<ConnectionId> connection_id;
  TEvent data;
};

}