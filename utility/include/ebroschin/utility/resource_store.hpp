#pragma once

#include "resource.hpp"

#include <unordered_map>

namespace ebroschin::utility {

template<typename TResource>
class ResourceStore {
public:
  template<typename... TResourceArgs>
  TResource* Create(TResourceArgs&&... args) {
    TResource resource{std::forward<TResourceArgs>(args)...};
    auto key = resource.Get();
    auto [iterator, success] = store_.emplace(key, std::move(resource));
    return success? &iterator->second : nullptr;
  }

  void Destroy(const TResource* resource) {
    if (!resource) return;

    auto iterator = store_.find(resource->Get());
    if (iterator == store_.end()) return;

    store_.erase(iterator);
  }

private:
  std::unordered_map<typename TResource::ValueType*, TResource> store_;
};

}