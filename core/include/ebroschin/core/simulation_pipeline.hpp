#pragma once

#include <functional>

namespace ebroschin::core {

class SimulationPipeline {
public:
  void RegisterPhase(std::move_only_function<void()> phase);
  void Update();

private:
  std::vector<std::move_only_function<void()>> phases_{};
};

}