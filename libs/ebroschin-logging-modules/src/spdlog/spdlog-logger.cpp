#include "ebroschin/logging-modules/spdlog/spdlog-logger.hpp"

#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace ebroschin::logging::modules {

SpdlogLogger::SpdlogLogger(std::string log_name) {
  spdlog::init_thread_pool(8192, 1);
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::trace);

  auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>("logs/chat.log", 10 * 1024 * 1024, 5);
  file_sink->set_level(spdlog::level::trace);

  logger_ = std::make_shared<spdlog::async_logger>(
      std::move(log_name),
      spdlog::sinks_init_list{console_sink, file_sink},
      spdlog::thread_pool(),
      spdlog::async_overflow_policy::block
  );

  logger_->flush_on(spdlog::level::critical);
  spdlog::flush_every(std::chrono::seconds(1));
}

void SpdlogLogger::Print(LogLevel log_level, const std::string& message) {
  switch (log_level) {
    case LogLevel::verbose: logger_->trace(message); break;
    case LogLevel::debug: logger_->debug(message); break;
    case LogLevel::info: logger_->info(message); break;
    case LogLevel::warning: logger_->warn(message); break;
    case LogLevel::error: logger_->error(message); break;
    case LogLevel::critical: logger_->critical(message); break;
  }
}

void SpdlogLogger::SetLogLevel(LogLevel log_level) {
  switch (log_level) {
    case LogLevel::verbose: logger_->set_level(spdlog::level::trace); break;
    case LogLevel::debug: logger_->set_level(spdlog::level::debug); break;
    case LogLevel::info: logger_->set_level(spdlog::level::info); break;
    case LogLevel::warning: logger_->set_level(spdlog::level::warn); break;
    case LogLevel::error: logger_->set_level(spdlog::level::err); break;
    case LogLevel::critical: logger_->set_level(spdlog::level::critical); break;
  }
}

void SpdlogLogger::Shutdown() {
  spdlog::shutdown();
}

}