#pragma once

#include <functional>
#include <chrono>

namespace ebroschin::network::tests {

struct MockRpcTimeoutHandler final {

  void ScheduleTimeout(RequestId,
    std::chrono::steady_clock::duration,
    std::function<void()>) const
  { }

  void CancelTimeout(RequestId) const { }

};

}