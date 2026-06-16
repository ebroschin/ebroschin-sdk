#pragma once

#include <cstdint>
#include <type_traits>
#include <concepts>

namespace ebroschin::network {

using ConnectionId = std::uint64_t;
using MessageProcessorId = std::uint64_t;
using RequestId = std::uint64_t;

}