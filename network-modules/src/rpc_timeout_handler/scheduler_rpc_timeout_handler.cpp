#include "ebroschin/network-modules/rpc_timeout_handler/scheduler_rpc_timeout_handler.hpp"

namespace ebroschin::network::modules {

SchedulerRpcTimeoutHandler::SchedulerRpcTimeoutHandler(scheduling::SchedulingSystem& scheduling_system) noexcept:
  scheduling_system_{scheduling_system}
{}

void SchedulerRpcTimeoutHandler::ScheduleTimeout(RequestId request_id, steady_clock::duration duration, std::function<void()> callback) {
  if (!callback) return;

  const auto handle = scheduling_system_.ScheduleAfter(duration, [callback = std::move(callback)] {
    callback();
  });

  {
    std::scoped_lock lock{mutex_};
    timeouts_.insert_or_assign(request_id, handle);
  }
}

void SchedulerRpcTimeoutHandler::CancelTimeout(RequestId request_id) {
  scheduling::TaskId task_id{};
  {
    std::scoped_lock lock{mutex_};
    const auto it = timeouts_.find(request_id);
    if (it == timeouts_.end()) return;

    task_id = it->second;
    timeouts_.erase(it);
  }

  scheduling_system_.RemoveTask(task_id);
}

}