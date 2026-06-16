#pragma once

#include "system.hpp"

#include <concepts>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <iostream>

namespace ebroschin::core {

class SystemContext {
public:
  SystemContext() = default;
  SystemContext(const SystemContext&) = delete;
  SystemContext& operator=(const SystemContext&) = delete;
  SystemContext(SystemContext&&) = delete;
  SystemContext& operator=(SystemContext&&) = delete;

  void Initialize();
  void Deinitialize() const;

  template <typename TSystem, typename... TArgs>
  requires std::derived_from<TSystem, System>
  TSystem* Register(TArgs&&... arguments) {
    return Register<TSystem, TSystem>(std::forward<TArgs>(arguments)...);
  }

  template <typename TSystemInterface, typename TSystem, typename... Args>
  requires std::derived_from<TSystem, TSystemInterface>
    && std::derived_from<TSystemInterface, System>
  TSystemInterface* Register(Args&&... arguments) {
    const std::type_index key = typeid(TSystemInterface);
    auto system = std::make_unique<TSystem>(*this, std::forward<Args>(arguments)...);
    auto [it, _] = systems_.try_emplace(key, std::move(system));

    auto* ptr = static_cast<TSystemInterface*>(it->second.get());
    ordered_systems_.emplace_back(ptr);
    return ptr;
  }

  template<typename TSystem>
  TSystem* Get() const {
    const std::type_index key = typeid(TSystem);
    const auto it = systems_.find(key);
    return it != systems_.end()? static_cast<TSystem*>(it->second.get()) : nullptr;
  }

  template<typename TSystem>
  TSystem& Require() const {
    auto* system = Get<TSystem>();
    if (!system) {
      std::cerr << "Required System not registered" << std::endl;
      std::abort();
    }

    return *system;
  }

private:
  std::unordered_map<std::type_index, std::unique_ptr<System>> systems_{};
  std::vector<System*> ordered_systems_{};
};

}