#include "ebroschin/core/application.hpp"

#include "ebroschin/core/simulation_pipeline.hpp"

namespace ebroschin::core {

namespace {
  Application* instance_ = nullptr;
}

Application::Application() noexcept {
  instance_ = this;
}

void Application::Quit() noexcept {
  running_.store(false, std::memory_order_relaxed);
  running_.notify_one();
}

void Application::Terminate() {
  if (!instance_) return;
  instance_->HandleTerminate();
}

void Application::RunBlocking() {
  running_ = true;

  std::set_terminate(Terminate);
  PrepareContext();
  ctx_.Initialize();
  OnContextInitialized();

  running_.wait(true, std::memory_order_relaxed);

  OnContextDeinitialized();
  ctx_.Deinitialize();
}

void Application::RunSimulation(SimulationPipeline& pipeline) {
  running_ = true;

  std::set_terminate(Terminate);
  PrepareContext();
  ctx_.Initialize();
  OnContextInitialized();

  while (running_) {
    pipeline.Update();
  }

  OnContextDeinitialized();
  ctx_.Deinitialize();
}

}
