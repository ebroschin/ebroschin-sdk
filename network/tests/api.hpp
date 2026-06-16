#pragma once

#include "ebroschin/network/rpc/commons.hpp"

#include <cstdint>

namespace ebroschin::network::tests {

struct TestMessage final {
  static constexpr std::uint64_t TypeId = 90001;

  std::uint64_t integer;
  double fractional;
};

struct RpcArithmeticRequestMessage final {
  static constexpr std::uint64_t TypeId = 90101;

  RequestId request_id;
  int operand;
};

struct RpcArithmeticResponseMessage final {
  static constexpr std::uint64_t TypeId = 90102;

  RequestId request_id;
  int result;
};

struct RpcArithmeticErrorMessage final {
  static constexpr std::uint64_t TypeId = 90103;

  RequestId request_id;
};

using MessageTypes = std::tuple<
  TestMessage,
  RpcArithmeticRequestMessage,
  RpcArithmeticResponseMessage,
  RpcArithmeticErrorMessage
>;

}

template<>
struct ebroschin::network::rpc::RpcCall<ebroschin::network::tests::RpcArithmeticRequestMessage> {
  using Response = tests::RpcArithmeticResponseMessage;
  using Error = tests::RpcArithmeticErrorMessage;
};
