#pragma once

#include <functional>

namespace ebroschin::core {

class Executor {
public:
  using Task = std::move_only_function<void()>;
  virtual ~Executor() = default;

  virtual void Post(Task task) = 0;
};

}