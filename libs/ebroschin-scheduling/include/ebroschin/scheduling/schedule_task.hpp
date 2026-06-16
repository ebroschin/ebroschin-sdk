#pragma once

#include "commons.hpp"

#include <chrono>
#include <functional>

using std::chrono::steady_clock;

namespace ebroschin::scheduling {

struct ScheduleTask {
  TaskId id;
  steady_clock::duration interval;
  steady_clock::time_point scheduled_time_point;
  std::function<void()> callback;
  bool once{false};
};

}