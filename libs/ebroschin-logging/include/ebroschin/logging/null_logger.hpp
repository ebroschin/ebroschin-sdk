#pragma once

#include "logger.hpp"

namespace ebroschin::logging {

class NullLogger final: public Logger {
public:
  void Print(LogLevel, const std::string&) override { }
  void SetLogLevel(LogLevel) override { }
  void Shutdown() override { }
};

}