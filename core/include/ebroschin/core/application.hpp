#pragma once

#include "system_context.hpp"

#include <atomic>
#include <memory>

namespace ebroschin::core {

class SimulationPipeline;

class Application {
public:
  Application() noexcept;
  virtual ~Application() = default;

  Application(const Application&) = delete;
  Application& operator=(const Application&) = delete;
  Application(Application&&) = delete;
  Application& operator=(Application&&) = delete;

  void RunBlocking();
  void RunSimulation(SimulationPipeline& pipeline);

  void Quit() noexcept;

  [[nodiscard]] bool IsRunning() const noexcept
  { return running_.load(std::memory_order::relaxed); }

protected:
  virtual void Initialize() = 0;
  virtual void HandleTerminate() {}

  SystemContext ctx_{};
  std::atomic<bool> running_{false};

private:
  static void Terminate();
};

}