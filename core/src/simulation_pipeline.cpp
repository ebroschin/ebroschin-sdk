#include "ebroschin/core/simulation_pipeline.hpp"

#include <algorithm>

namespace ebroschin::core {

void SimulationPipeline::RegisterPhase(std::move_only_function<void()> phase) {
  phases_.emplace_back(std::move(phase));
}

void SimulationPipeline::Update() {
  for (auto& phase : phases_) {
    phase();
  }
}

}