#include <gtest/gtest.h>

#include "mock_persistence_store.hpp"
#include "ebroschin/persistence/persistence_system.hpp"

#include <ebroschin/core/system_context.hpp>

#include <string>

namespace ebroschin::persistence::tests {

class TestAdapter {
public:
  virtual ~TestAdapter() = default;

  [[nodiscard]] virtual std::optional<std::reference_wrapper<const std::string>>
  GetValue(MockPersistenceId id) const = 0;

  virtual MockPersistenceId PersistValue(std::string value) = 0;
};

class MockPersistenceTestAdapter final : public PersistenceAdapter<MockPersistenceStore, TestAdapter> {
public:
  explicit MockPersistenceTestAdapter(MockPersistenceStore& store):
    PersistenceAdapter{store}
  {}

  void Initialize() override {}

  [[nodiscard]] std::optional<std::reference_wrapper<const std::string>>
  GetValue(MockPersistenceId id) const override {
    return store_.LoadValue(id);
  }

  MockPersistenceId PersistValue(std::string value) override {
    return store_.WriteValue(std::move(value));
  }
};

TEST(PersistenceSystemTests, AdapterWorkflow) {
  core::SystemContext ctx{};

  //register persistence store backend
  auto* persistence = ctx.Register<PersistenceSystem<MockPersistenceStore>>();
  ASSERT_NE(persistence, nullptr);

  //register the concrete adapter which provides the implementation
  //of the operations specified by the adapter interface
  persistence->Register<TestAdapter, MockPersistenceTestAdapter>();

  ctx.Initialize();

  //retrieve backend-agnostic adapter interface
  auto* adapter = persistence->Get<TestAdapter>();
  ASSERT_NE(adapter, nullptr);
  auto potential_value = adapter->GetValue(0);
  EXPECT_FALSE(potential_value.has_value());

  const auto id = adapter->PersistValue("test-data");
  potential_value = adapter->GetValue(id);
  EXPECT_TRUE(potential_value.has_value());
  EXPECT_EQ(potential_value->get(), "test-data");
}

}