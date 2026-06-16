#pragma once

#include <ebroschin/network/commons.hpp>
#include <ebroschin/scheduling/commons.hpp>
#include <ebroschin/scheduling/scheduling_system.hpp>

#include <mutex>

namespace ebroschin::network::modules {

class SchedulerRpcTimeoutHandler {
public:
  explicit SchedulerRpcTimeoutHandler(scheduling::SchedulingSystem& scheduling_system) noexcept;

  void ScheduleTimeout(RequestId request_id, steady_clock::duration duration, std::function<void()> callback);
  void CancelTimeout(RequestId request_id);

private:
  scheduling::SchedulingSystem& scheduling_system_;

  std::mutex mutex_{};
  std::unordered_map<RequestId, scheduling::TaskId> timeouts_{};
};

}