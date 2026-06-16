#include "ebroschin/core/application.hpp"

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
  Initialize();
  ctx_.Initialize();

  running_.wait(true, std::memory_order_relaxed);

  ctx_.Deinitialize();
}

}
