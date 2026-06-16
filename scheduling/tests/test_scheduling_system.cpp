#include <gtest/gtest.h>

#include <ebroschin/core/system_context.hpp>
#include <ebroschin/scheduling/scheduling_system.hpp>

#include <algorithm>
#include <deque>

using namespace std::chrono_literals;
using namespace std::string_literals;

namespace ebroschin::scheduling::tests {

class TimePointLog {
public:
  using Log = std::vector<steady_clock::time_point>;

  void AddEntry() {
    std::scoped_lock lock{mutex_};
    auto time_point = steady_clock::now();
    log_.emplace_back(time_point);
  }

  [[nodiscard]] const Log& Entries() const noexcept { return log_; }

private:
  std::mutex mutex_{};
  Log log_{};
};

class TimePointTestContext {
public:
  TimePointTestContext(SchedulingSystem* scheduling_system,
    steady_clock::duration test_duration,
    steady_clock::duration rate):
    scheduling_system_{scheduling_system},
    test_duration_{test_duration},
    rate_{rate}
  {}

  void Start() {
    task_handle_ = scheduling_system_->SchedulePeriodically(rate_, [this] { log_.AddEntry(); });
  }

  void Stop() const {
    scheduling_system_->RemoveTask(task_handle_);
  }

  void Evaluate() const {
    const std::vector<steady_clock::time_point>& entries = log_.Entries();
    const auto entry_count = entries.size();
    const auto expected_entry_count = static_cast<decltype(entry_count)>(test_duration_ / rate_);
    ASSERT_GE(entry_count, 2) << "test run requires at least 2 log entries for each scheduler";

    const auto rate_text = std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(rate_).count()) + "ms";
    const auto count_text = (std::stringstream() << "(" << entry_count << "/" << expected_entry_count << ")").str();
    EXPECT_GE(entry_count, expected_entry_count - 1) << "Not enough " << rate_text << " tasks were executed " << count_text;
    EXPECT_LE(entry_count, expected_entry_count + 1) << "Too many " << rate_text << " tasks were executed " << count_text;

    for (std::size_t i = 1; i < entry_count; i++) {
      const auto delta = entries[i] - entries[i - 1];
      const auto error = delta > rate_? delta - rate_ : rate_ - delta;
      EXPECT_LE(error, 15ms) << "Incorrect timing for phase " << rate_text;
    }
  }

private:
  SchedulingSystem* scheduling_system_;
  steady_clock::duration test_duration_;
  steady_clock::duration rate_;

  TimePointLog log_{};
  TaskId task_handle_{0};
};

TEST(SchedulingSystemTest, PeriodicScheduling) {
  core::SystemContext ctx{};
  auto* scheduling_system = ctx.Register<SchedulingSystem>();
  ctx.Initialize();

  std::deque<TimePointTestContext> tests{};

  auto test_duration = 2000ms;
  tests.emplace_back(scheduling_system, test_duration, 800ms);
  tests.emplace_back(scheduling_system, test_duration, 450ms);
  tests.emplace_back(scheduling_system, test_duration, 320ms);
  tests.emplace_back(scheduling_system, test_duration, 20ms);

  std::ranges::for_each(tests, &TimePointTestContext::Start);
  std::this_thread::sleep_for(test_duration);

  std::ranges::for_each(tests, &TimePointTestContext::Stop);
  std::this_thread::sleep_for(1000ms);

  std::ranges::for_each(tests, &TimePointTestContext::Evaluate);
  ctx.Deinitialize();
}

TEST(SchedulingSystemTest, OneShotScheduling) {
  core::SystemContext ctx{};
  auto* scheduling_system = ctx.Register<SchedulingSystem>();
  ctx.Initialize();

  TimePointLog log{};
  const auto test_duration = 2000ms;

  const auto start_time_point = steady_clock::now();
  scheduling_system->ScheduleAfter(400ms, [&] { log.AddEntry(); });
  scheduling_system->ScheduleAfter(200ms, [&] { log.AddEntry(); });
  scheduling_system->ScheduleAfter(500ms, [&] { log.AddEntry(); });

  std::this_thread::sleep_for(test_duration);

  const std::vector expected {
    start_time_point + 200ms,
    start_time_point + 400ms,
    start_time_point + 500ms,
  };

  const auto entries = log.Entries();
  EXPECT_EQ(entries.size(), expected.size());
  for (std::size_t i = 0; i < expected.size(); i++) {
    auto expected_time_point = expected[i];
    auto logged_time_point = entries[i];

    auto error = expected_time_point > logged_time_point? expected_time_point - logged_time_point : logged_time_point - expected_time_point;
    EXPECT_LE(error, 15ms);
  }

  ctx.Deinitialize();
}

TEST(SchedulingSystemTest, OneShotAndPeriodicScheduling) {
  core::SystemContext ctx{};
  auto* scheduling_system = ctx.Register<SchedulingSystem>();
  ctx.Initialize();

  TimePointLog log{};
  constexpr auto test_duration = 2000ms;

  std::deque<TimePointTestContext> tests{};
  tests.emplace_back(scheduling_system, test_duration, 800ms);
  tests.emplace_back(scheduling_system, test_duration, 450ms);
  tests.emplace_back(scheduling_system, test_duration, 320ms);
  tests.emplace_back(scheduling_system, test_duration, 20ms);

  const auto start_time_point = steady_clock::now();
  scheduling_system->ScheduleAfter(400ms, [&] { log.AddEntry(); });
  scheduling_system->ScheduleAfter(200ms, [&] { log.AddEntry(); });
  scheduling_system->ScheduleAfter(500ms, [&] { log.AddEntry(); });

  std::ranges::for_each(tests, &TimePointTestContext::Start);
  std::this_thread::sleep_for(test_duration);

  std::ranges::for_each(tests, &TimePointTestContext::Stop);
  std::this_thread::sleep_for(1000ms);

  const std::vector expected {
    start_time_point + 200ms,
    start_time_point + 400ms,
    start_time_point + 500ms,
  };

  const auto entries = log.Entries();
  EXPECT_EQ(entries.size(), expected.size());
  for (std::size_t i = 0; i < expected.size(); i++) {
    auto expected_time_point = expected[i];
    auto logged_time_point = entries[i];

    auto error = expected_time_point > logged_time_point? expected_time_point - logged_time_point : logged_time_point - expected_time_point;
    EXPECT_LE(error, 15ms);
  }

  std::ranges::for_each(tests, &TimePointTestContext::Evaluate);
  ctx.Deinitialize();
}

}