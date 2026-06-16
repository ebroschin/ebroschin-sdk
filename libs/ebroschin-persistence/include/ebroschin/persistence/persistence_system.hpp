#pragma once

#include "persistence_adapter.hpp"
#include "persistence_store.hpp"

#include <ebroschin/core/system.hpp>

#include <memory>
#include <algorithm>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <iostream>

namespace ebroschin::persistence {

template <typename TStore>
requires std::derived_from<TStore, PersistenceStore>
class PersistenceSystem final : public core::System {
public:
  template <typename... TArgs>
  explicit PersistenceSystem(const core::SystemContext& ctx, TArgs&&... args):
    System{ctx},
    store_{std::make_unique<TStore>(std::forward<TArgs>(args)...)}
  {}

  void Initialize() override {
    store_->Initialize();
    std::ranges::for_each(adapters_ | std::views::values, &PersistenceAdapterBase::Initialize);
  }

  void Deinitialize() override {
    store_->Deinitialize();
    std::ranges::for_each(adapters_ | std::views::values, &PersistenceAdapterBase::Deinitialize);
  }

  template <typename TAdapterInterface, typename TAdapter, typename... TArgs>
  requires std::derived_from<TAdapter, TAdapterInterface> &&
    std::derived_from<TAdapter, PersistenceAdapter<TStore, TAdapterInterface>>
  TAdapterInterface* Register(TArgs&&... args) {
    const std::type_index key = typeid(TAdapterInterface);
    auto ptr = std::make_unique<TAdapter>(*store_, std::forward<TArgs>(args)...);
    const auto [it, _] = adapters_.try_emplace(key, std::move(ptr));

    return dynamic_cast<TAdapterInterface*>(it->second.get());
  }

  template <typename TAdapterInterface>
  TAdapterInterface* Get() {
    const std::type_index key = typeid(TAdapterInterface);
    const auto it = adapters_.find(key);
    return it != adapters_.end()? dynamic_cast<TAdapterInterface*>(it->second.get()) : nullptr;
  }

  template <typename TAdapterInterface>
  TAdapterInterface& Require() {
    auto* adapter = Get<TAdapterInterface>();
    if (!adapter) {
      std::cerr << "Required PersistenceAdapter not registered" << std::endl;
      std::abort();
    }

    return *adapter;
  }

private:
  std::unique_ptr<TStore> store_;
  std::unordered_map<std::type_index, std::unique_ptr<PersistenceAdapterBase>> adapters_;
};

}