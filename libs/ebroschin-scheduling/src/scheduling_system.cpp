#include "ebroschin/scheduling/scheduling_system.hpp"

#include <mutex>
#include <ranges>

using namespace std::chrono_literals;

namespace ebroschin::scheduling {

SchedulingSystem::SchedulingSystem(const core::SystemContext& ctx):
  System{ctx}
{}

void SchedulingSystem::Initialize() {
    scheduler_thread_ = std::jthread{ [this](const std::stop_token& st) { ProcessTasks(st); } };
}

void SchedulingSystem::Deinitialize() {
  scheduler_thread_.request_stop();
  cv_.notify_one();
  scheduler_thread_ = {};
}

void SchedulingSystem::RemoveTask(TaskId handle) {
  {
    std::scoped_lock lock{mutex_};
    tasks_.erase(handle);
  }

  cv_.notify_one();
}

TaskId SchedulingSystem::SchedulePeriodically(steady_clock::duration interval, std::function<void()> task) {
  return AddTask({0, interval, {}, std::move(task), false});
}

TaskId SchedulingSystem::ScheduleAfter(steady_clock::duration duration, std::function<void()> task) {
  return AddTask({0, duration, {}, std::move(task), true});
}

TaskId SchedulingSystem::AddTask(ScheduleTask task) {
  auto id = next_id.fetch_add(1, std::memory_order_relaxed);

  task.id = id;
  if (task.interval <= 15ms) {
    task.interval = 15ms;
  }

  task.scheduled_time_point = steady_clock::now() + task.interval;
  {
    std::scoped_lock lock{mutex_};
    tasks_.emplace(id, std::move(task));
  }

  cv_.notify_one();

  return id;
}

void SchedulingSystem::ProcessTasks(const std::stop_token& st) {
  std::vector<std::function<void()>> callbacks{};
  std::vector<TaskId> deleted_tasks{};

  while (!st.stop_requested()) {
    std::unique_lock lock{mutex_};

    while (!tasks_.empty()) {
      callbacks.clear();
      deleted_tasks.clear();

      const auto current_time_point = steady_clock::now();
      auto next_wake_time = steady_clock::time_point::max();
      for (auto& task : tasks_ | std::views::values) {
        if (current_time_point >= task.scheduled_time_point) {
          callbacks.push_back(task.callback);
          task.scheduled_time_point = current_time_point + task.interval;

          if (task.once) {
            deleted_tasks.emplace_back(task.id);
            continue;
          }
        }

        next_wake_time = std::min(next_wake_time, task.scheduled_time_point);
      }

      lock.unlock();
      for (const auto& callback : callbacks) {
        callback();
      }
      lock.lock();

      for (const auto& task_id : deleted_tasks) {
        tasks_.erase(task_id);
      }

      cv_.wait_until(lock, next_wake_time);
      if (st.stop_requested()) return;
    }

    cv_.wait(lock, [&] { return !tasks_.empty() || st.stop_requested(); });
    if (st.stop_requested()) return;
  }
}

}