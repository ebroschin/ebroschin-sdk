#pragma once

#include "log_stream.hpp"
#include "logger.hpp"

#include <memory>
#include <string>

namespace ebroschin::logging {

struct Log {
  static void Verbose(const std::string&);
  static void Debug(const std::string&);
  static void Info(const std::string&);
  static void Warning(const std::string&);
  static void Error(const std::string&);
  static void Critical(const std::string&);
  static void Print(LogLevel, const std::string&);

  static LogStream Verbose();
  static LogStream Debug();
  static LogStream Info();
  static LogStream Warning();
  static LogStream Error();
  static LogStream Critical();
  static LogStream Print(LogLevel);

  static void SetLogLevel(LogLevel);
  static void Shutdown();

  template <typename TLogger, typename... TArguments>
  static void SetLogger(TArguments&&... arguments) {
    SetLogger(std::make_shared<TLogger>(std::forward<TArguments>(arguments)...));
  }

  static void SetLogger(std::shared_ptr<Logger>);
};

}