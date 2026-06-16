#pragma once

#include "log_level.hpp"

#include <string>

namespace ebroschin::logging {

class Logger {
public:
  virtual ~Logger() = default;

  virtual void Print(LogLevel log_level, const std::string& message) = 0;
  virtual void SetLogLevel(LogLevel log_level) = 0;
  virtual void Shutdown() = 0;
};

}