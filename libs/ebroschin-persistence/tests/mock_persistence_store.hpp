#pragma once

#include "ebroschin/persistence/persistence_store.hpp"
#include "commons.hpp"

#include <string>
#include <unordered_map>
#include <cstdint>
#include <optional>

namespace ebroschin::persistence::tests {

class MockPersistenceStore final : public PersistenceStore {
public:
  void Initialize() override {}

  [[nodiscard]] std::optional<std::reference_wrapper<const std::string>>
  LoadValue(MockPersistenceId id) const {
    const auto it = values_.find(id);
    if (it == values_.end()) return std::nullopt;
    return it->second;
  }

  MockPersistenceId WriteValue(std::string value) {
    const auto id = next_id_++;
    values_.insert_or_assign(id, value);

    return id;
  }

private:
  MockPersistenceId next_id_{1};
  std::unordered_map<std::uint64_t, std::string> values_{};
};

}