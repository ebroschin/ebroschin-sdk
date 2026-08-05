#pragma once

#include <ebroschin/core/synchronization/executor.hpp>

#include <queue>
#include <mutex>
#include <condition_variable>

namespace ebroschin::core {

class QueuedExecutor final : public Executor {
public:
  void Post(Task task) override;
  void ProcessBlocking();
  void Stop();

private:
  bool running_{true};
  std::queue<Task> tasks_{};
  std::mutex mutex_{};
  std::condition_variable cv_{};
};

}