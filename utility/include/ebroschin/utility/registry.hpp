#pragma once

#include <map>
#include <memory>
#include <span>
#include <vector>
#include <generator>
#include <ranges>

namespace ebroschin::utility {

template<typename TKey, typename TBaseValue>
class Registry {
public:
  template<typename TValue, typename... Args>
  requires std::derived_from<TValue, TBaseValue>
  void Register(TKey key, Args&&... args);

  auto ViewValues(const TKey& key) const;
private:
  std::span<const std::unique_ptr<TBaseValue>> GetValueSpan(const TKey& key) const;

  std::map<TKey, std::vector<std::unique_ptr<TBaseValue>>> values_;
};

template<typename TKey, typename TBaseValue>
template<typename TValue, typename... Args>
requires std::derived_from<TValue, TBaseValue>
void Registry<TKey, TBaseValue>::Register(TKey key, Args&&... args) {
  values_[key].emplace_back(std::make_unique<TValue>(std::forward<Args>(args)...));
}

template<typename TKey, typename TBaseValue>
std::span<const std::unique_ptr<TBaseValue>> Registry<TKey, TBaseValue>::GetValueSpan(const TKey& key) const {
  auto it = values_.find(key);
  if (it == values_.end()) return {};

  return {it->second};
}

template<typename TKey, typename TBaseValue>
auto Registry<TKey, TBaseValue>::ViewValues(const TKey& key) const {
  return GetValueSpan(key) | std::views::transform([](const auto& value) { return value.get(); });
}

}