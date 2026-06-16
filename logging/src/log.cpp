#include "ebroschin/logging/log.hpp"

#include "ebroschin/logging/null_logger.hpp"

#include <mutex>
#include <shared_mutex>

namespace {
  std::shared_mutex mutex_{};

  //Function-local static instance.
  //Prevents the global constructor/destructor compiler warning and creates the instance on demand instead
  std::shared_ptr<ebroschin::logging::Logger>& GlobalLogger()
  {
    static std::shared_ptr<ebroschin::logging::Logger> instance = std::make_shared<ebroschin::logging::NullLogger>();
    return instance;
  }
}

namespace ebroschin::logging {

void Log::SetLogger(std::shared_ptr<Logger> logger) {
  std::scoped_lock lock{mutex_};
  if (!logger) {
    GlobalLogger() = std::make_shared<NullLogger>();
    return;
  }

  GlobalLogger() = std::move(logger);
}

void Log::SetLogLevel(LogLevel log_level) {
  std::shared_ptr<Logger> logger{};
  {
    std::shared_lock lock{mutex_};
    logger = GlobalLogger();
  }

  logger->SetLogLevel(log_level);
}

void Log::Shutdown() {
  std::shared_ptr<Logger> logger{};
  {
    std::scoped_lock lock{mutex_};
    logger = GlobalLogger();
  }

  logger->Shutdown();
  SetLogger<NullLogger>();
}

void Log::Print(LogLevel log_level, const std::string& message) {
  std::shared_ptr<Logger> logger{};
  {
    std::shared_lock lock{mutex_};
    logger = GlobalLogger();
  }

  logger->Print(log_level, message);
}

LogStream Log::Print(LogLevel log_level) {
  std::shared_ptr<Logger> logger{};
  {
    std::shared_lock lock{mutex_};
    logger = GlobalLogger();
  }

  return LogStream{std::move(logger), log_level};
}

void Log::Verbose(const std::string& message) {
  Print(LogLevel::verbose, message);
}

void Log::Debug(const std::string& message) {
  Print(LogLevel::debug, message);
}

void Log::Info(const std::string& message) {
  Print(LogLevel::info, message);
}

void Log::Warning(const std::string& message) {
  Print(LogLevel::warning, message);
}

void Log::Error(const std::string& message) {
  Print(LogLevel::error, message);
}

void Log::Critical(const std::string& message) {
  Print(LogLevel::critical, message);
}

LogStream Log::Verbose() {
  return Print(LogLevel::verbose);
}

LogStream Log::Debug() {
  return Print(LogLevel::debug);
}

LogStream Log::Info() {
  return Print(LogLevel::info);
}

LogStream Log::Warning() {
  return Print(LogLevel::warning);
}

LogStream Log::Error() {
  return Print(LogLevel::error);
}

LogStream Log::Critical() {
  return Print(LogLevel::critical);
}

}