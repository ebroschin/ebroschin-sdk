#include <gtest/gtest.h>

#include "ebroschin/commands/commands_system.hpp"

#include <ebroschin/core/system_context.hpp>

#include <array>

namespace ebroschin::commands::tests {

class TestReportSystem final : public core::System {
public:
  explicit TestReportSystem(const core::SystemContext& ctx):
    System{ctx}
  {}

  void Initialize() override {}
  void Deinitialize() override {}

  template <Command TCommand>
  void ReportCall() {
    const std::type_index key = typeid(TCommand);
    auto [it, _] = command_calls_.try_emplace(key, 0);
    ++it->second;
  }

  template <Command TCommand>
  [[nodiscard]] int GetCallCount() const {
    const std::type_index key = typeid(TCommand);
    const auto it = command_calls_.find(key);
    if (it == command_calls_.end()) return 0;

    return it->second;
  }

private:
  std::unordered_map<std::type_index, int> command_calls_{};
};

template <int Discriminator>
class MockCommand {
public:
  static consteval auto MakeToken() {
    std::array<char, 1> storage{};
    storage[0] = Discriminator;
    return storage;
  }

  static constexpr auto TokenArray = MakeToken();
  static constexpr std::string_view Token = {TokenArray.begin(), TokenArray.end()};
  static constexpr std::string_view Description = "Mock command";

  explicit MockCommand(const core::SystemContext& ctx):
    report_system_{ctx.Require<TestReportSystem>()}
  {}

  void Execute(std::span<std::string_view>) const {
    report_system_.ReportCall<MockCommand>();
  }

private:
  TestReportSystem& report_system_;
};

using TestCommandsSystem = CommandsSystem<MockCommand<0>, MockCommand<1>>;

TEST(CommandsSystemTests, MockCommands) {
  core::SystemContext ctx{};
  auto& report_system = *ctx.Register<TestReportSystem>();

  ctx.Register<TestCommandsSystem>();
  ctx.Initialize();
  auto& commands_system = ctx.Require<TestCommandsSystem>();

  EXPECT_TRUE(commands_system.Execute(std::string{MockCommand<0>::Token}, {}));
  EXPECT_EQ(report_system.GetCallCount<MockCommand<0>>(), 1);
  EXPECT_EQ(report_system.GetCallCount<MockCommand<1>>(), 0);

  EXPECT_TRUE(commands_system.Execute(std::string{MockCommand<1>::Token}, {}));
  EXPECT_EQ(report_system.GetCallCount<MockCommand<0>>(), 1);
  EXPECT_EQ(report_system.GetCallCount<MockCommand<1>>(), 1);

  EXPECT_FALSE(commands_system.Execute("/other", {}));
  EXPECT_EQ(report_system.GetCallCount<MockCommand<0>>(), 1);
  EXPECT_EQ(report_system.GetCallCount<MockCommand<1>>(), 1);

  EXPECT_FALSE(commands_system.Execute("/other", {}));
  EXPECT_FALSE(commands_system.Execute("/other", {}));
  EXPECT_TRUE(commands_system.Execute(std::string{MockCommand<0>::Token}, {}));
  EXPECT_TRUE(commands_system.Execute(std::string{MockCommand<0>::Token}, {}));
  EXPECT_TRUE(commands_system.Execute(std::string{MockCommand<0>::Token}, {}));
  EXPECT_EQ(report_system.GetCallCount<MockCommand<0>>(), 4);
  EXPECT_EQ(report_system.GetCallCount<MockCommand<1>>(), 1);

  EXPECT_TRUE(commands_system.Execute(std::string{MockCommand<1>::Token}, {}));
  EXPECT_TRUE(commands_system.Execute(std::string{MockCommand<1>::Token}, {}));
  EXPECT_TRUE(commands_system.Execute(std::string{MockCommand<1>::Token}, {}));
  EXPECT_EQ(report_system.GetCallCount<MockCommand<0>>(), 4);
  EXPECT_EQ(report_system.GetCallCount<MockCommand<1>>(), 4);
}

}