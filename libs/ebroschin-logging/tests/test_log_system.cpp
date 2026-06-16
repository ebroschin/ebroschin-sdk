#include <gtest/gtest.h>

#include "ebroschin/logging/log.hpp"

#include <memory>
#include <string>
#include <vector>

namespace ebroschin::logging::tests {

class MockLogger final : public Logger {
public:
  struct Record {
    LogLevel level{};
    std::string message{};
  };

  void SetLogLevel(LogLevel) override {}

  void Print(LogLevel log_level, const std::string& message) override {
    entries_.emplace_back(log_level, message);
  }

  void Shutdown() override {
    ++shutdown_calls_;
  }

  [[nodiscard]] const std::vector<Record>& GetEntries() const {
    return entries_;
  }

  [[nodiscard]] int GetShutdownCallCount() const {
    return shutdown_calls_;
  }

private:
  std::vector<Record> entries_{};
  int shutdown_calls_{};
};

class LogSystemTests : public testing::Test {
protected:
  void TearDown() override {
    Log::Shutdown();
  }
};

TEST_F(LogSystemTests, MockLoggerStringPrint) {
  const auto mock_logger = std::make_shared<MockLogger>();
  Log::SetLogger(mock_logger);

  Log::Verbose("v");
  Log::Warning("warn");

  ASSERT_EQ(mock_logger->GetEntries().size(), 2);
  EXPECT_EQ(mock_logger->GetEntries()[0].level, LogLevel::verbose);
  EXPECT_EQ(mock_logger->GetEntries()[0].message, "v");
  EXPECT_EQ(mock_logger->GetEntries()[1].level, LogLevel::warning);
  EXPECT_EQ(mock_logger->GetEntries()[1].message, "warn");
}

TEST_F(LogSystemTests, MockLoggerStreamPrintTest) {
  const auto mock_logger = std::make_shared<MockLogger>();
  Log::SetLogger(mock_logger);

  Log::Info() << "client id [" << 7 << "]";

  ASSERT_EQ(mock_logger->GetEntries().size(), 1);
  EXPECT_EQ(mock_logger->GetEntries()[0].level, LogLevel::info);
  EXPECT_EQ(mock_logger->GetEntries()[0].message, "client id [7]");
}

TEST_F(LogSystemTests, ShutdownTest) {
  const auto mock_logger_1 = std::make_shared<MockLogger>();
  Log::SetLogger(mock_logger_1);

  Log::Shutdown();

  EXPECT_EQ(mock_logger_1->GetShutdownCallCount(), 1);

  const auto mock_logger_2 = std::make_shared<MockLogger>();
  Log::SetLogger(mock_logger_2);
  Log::Critical("after shutdown");

  ASSERT_EQ(mock_logger_1->GetEntries().size(), 0);
  ASSERT_EQ(mock_logger_2->GetEntries().size(), 1);
  EXPECT_EQ(mock_logger_2->GetEntries()[0].level, LogLevel::critical);
  EXPECT_EQ(mock_logger_2->GetEntries()[0].message, "after shutdown");
}

TEST_F(LogSystemTests, NullLoggerTest) {
  const auto mock_logger = std::make_shared<MockLogger>();
  Log::SetLogger(mock_logger);
  Log::Info("before nullptr");

  Log::SetLogger(nullptr);
  Log::Error("no-op");

  ASSERT_EQ(mock_logger->GetEntries().size(), 1U);
  EXPECT_EQ(mock_logger->GetEntries()[0].message, "before nullptr");
}

}