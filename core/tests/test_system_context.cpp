#include <gtest/gtest.h>

#include "ebroschin/core/system.hpp"
#include "ebroschin/core/system_context.hpp"

namespace ebroschin::core::tests {

class TestSystem : public System {
public:
  explicit TestSystem(const SystemContext& ctx,
    std::string parameter1,
    std::int64_t parameter2):
    System{ctx},
    parameter1_{std::move(parameter1)},
    parameter2_{parameter2}
  {}

  virtual std::string CreateResult() {
    return parameter1_ + std::to_string(parameter2_);
  }

private:
  const std::string parameter1_;
  const std::int64_t parameter2_;
};

template<typename TValue>
requires std::integral<TValue> || std::floating_point<TValue>
class TestSystemComplex final: public TestSystem {
public:
  explicit TestSystemComplex(const SystemContext& ctx, TValue value):
    TestSystem{ctx, "inherited", 37},
    value_{value}
  {}

  std::string CreateResult() override {
    return TestSystem::CreateResult() + std::to_string(value_);
  }

  void Deinitialize() override {
    std::cout << "deinit(" + std::to_string(value_) + ")";
  }

private:
  const TValue value_;
};

//Basic Register() and Get()
TEST(SystemContextTest, RegisterConcrete) {
  SystemContext ctx{};
  ctx.Register<TestSystem>("test", 32);
  EXPECT_EQ(ctx.Get<TestSystem>()->CreateResult(), "test32");
}

//When a system has been registered via interface, the interface type is returned in Get()
TEST(SystemContextTest, RegisterInterface) {
  SystemContext ctx{};
  ctx.Register<TestSystem, TestSystemComplex<long>>(133L);
  EXPECT_EQ(ctx.Get<TestSystem>()->CreateResult(), "inherited37133");
}

//When a system has been registered via interface, the specific system type cannot be used in Get()
TEST(SystemContextTest, RegisterInterface2) {
  SystemContext ctx{};
  ctx.Register<TestSystem, TestSystemComplex<long>>(133L);

  auto* system_ptr = ctx.Get<TestSystemComplex<long>>();
  EXPECT_EQ(system_ptr, nullptr);

  auto* system_ptr_interface = ctx.Get<TestSystem>();
  EXPECT_NE(system_ptr_interface, nullptr);
}

//Assure reverse deinitialization order
TEST(SystemContextText, Deinitialization) {
  const std::ostringstream buffer{};
  auto* old_cout = std::cout.rdbuf();
  std::cout.rdbuf(buffer.rdbuf());

  SystemContext ctx{};
  ctx.Register<TestSystemComplex<long>>(1);
  ctx.Register<TestSystemComplex<int>>(2);
  ctx.Register<TestSystemComplex<short>>(3);

  ctx.Initialize();
  ctx.Deinitialize();

  std::cout.rdbuf(old_cout);
  EXPECT_EQ(buffer.str(), "deinit(3)deinit(2)deinit(1)");
}

}