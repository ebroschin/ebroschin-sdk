#pragma once

#include <string_view>

namespace ebroschin::logging {

enum class LogLevel {
  verbose,
  debug,
  info,
  warning,
  error,
  critical
};

constexpr std::string_view ToString(LogLevel level) noexcept {
  switch (level) {
    case LogLevel::verbose: return "verbose";
    case LogLevel::debug: return "debug";
    case LogLevel::info: return "info";
    case LogLevel::warning: return "warning";
    case LogLevel::error: return "error";
    case LogLevel::critical: return "critical";
  }

  return "unknown";
}

}