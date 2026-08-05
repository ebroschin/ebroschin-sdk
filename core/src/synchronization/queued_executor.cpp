#include "ebroschin/core/synchronization/queued_executor.hpp"

namespace ebroschin::core {

void QueuedExecutor::Post(Task task) {
  {
    std::scoped_lock lock{mutex_};
    if (!running_) return;

    tasks_.push(std::move(task));
  }

  cv_.notify_one();
}

void QueuedExecutor::ProcessBlocking() {
  std::unique_lock lock{mutex_};
  cv_.wait(lock, [this] {
    return !running_ || !tasks_.empty();
  });

  std::queue<Task> tasks{};
  tasks.swap(tasks_);

  lock.unlock();
  while (!tasks.empty()) {
    if (!running_) break;

    auto task = std::move(tasks.front());
    tasks.pop();

    if(!task) continue;
    task();
  }
}

void QueuedExecutor::Stop() {
  {
    std::scoped_lock lock{mutex_};
    running_ = false;
  }

  cv_.notify_one();
}

}