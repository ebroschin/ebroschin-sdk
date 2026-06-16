#pragma once

#include "log_level.hpp"
#include "logger.hpp"

#include <memory>
#include <sstream>

namespace ebroschin::logging {

class LogStream {
public:
  explicit LogStream(std::shared_ptr<Logger> logger, LogLevel log_level) noexcept:
    logger_{std::move(logger)},
    log_level_{log_level}
  {}

  ~LogStream() {
    if (stream_.view().empty()) return;
    if (!logger_) return;

    logger_->Print(log_level_, stream_.str());
  }

  template <typename TValue>
  LogStream& operator<<(TValue&& value) {
    if (!logger_) return *this;

    stream_ << std::forward<TValue>(value);
    return *this;
  }

private:
  std::shared_ptr<Logger> logger_;
  LogLevel log_level_;
  std::stringstream stream_{};
};

}