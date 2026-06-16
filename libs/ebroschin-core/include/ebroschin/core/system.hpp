#pragma once

namespace ebroschin::core {

class SystemContext;

class System {
public:
  explicit System(const SystemContext& ctx) noexcept:
    ctx_{ctx}
  {}

  virtual ~System() = default;

  System(const System&) = delete;
  System& operator=(const System&) = delete;
  System(System&&) = delete;
  System& operator=(System&&) = delete;

  virtual void Initialize() {}
  virtual void Deinitialize() {}

protected:
  const SystemContext& ctx_;
};

}