#pragma once

#include <ebroschin/logging/logger.hpp>
#include <spdlog/spdlog.h>

#include <string>

namespace ebroschin::logging::modules {

class SpdlogLogger : public Logger {
public:
  explicit SpdlogLogger(std::string log_name = "spdlog");

  void Print(LogLevel log_level, const std::string& message) override;
  void SetLogLevel(LogLevel log_level) override;
  void Shutdown() override;

private:
  std::shared_ptr<spdlog::logger> logger_{};
};

}