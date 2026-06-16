#pragma once

#include "schedule_task.hpp"

#include <ebroschin/core/system.hpp>

#include <condition_variable>
#include <functional>
#include <thread>
#include <unordered_map>

using std::chrono::steady_clock;

namespace ebroschin::scheduling {

class SchedulingSystem final : public core::System {
public:
  explicit SchedulingSystem(const core::SystemContext& ctx);

  void Initialize() override;
  void Deinitialize() override;

  TaskId SchedulePeriodically(steady_clock::duration interval, std::function<void()> task);
  TaskId ScheduleAfter(steady_clock::duration duration, std::function<void()> task);
  void RemoveTask(TaskId handle);

private:
  TaskId AddTask(ScheduleTask task);
  void ProcessTasks(const std::stop_token& st);

  std::jthread scheduler_thread_{};
  std::mutex mutex_{};
  std::condition_variable cv_{};

  std::atomic<TaskId> next_id{1};
  std::unordered_map<TaskId, ScheduleTask> tasks_{};
};

}