#pragma once

#include <ebroschin/core/synchronization/executor.hpp>

namespace ebroschin::core {

class InlineExecutor final : public Executor {
public:
  void Post(Task task) override {
    task();
  }
};

}