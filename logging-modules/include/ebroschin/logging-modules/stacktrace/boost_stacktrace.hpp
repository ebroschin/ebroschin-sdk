#pragma once

#include <ebroschin/logging/log.hpp>
#include <boost/stacktrace.hpp>

#include <exception>

namespace ebroschin::logging::modules {

struct BoostStacktrace {
  static void PrintExceptionStacktrace() {
    try {
      std::rethrow_exception(std::current_exception());
    } catch (const std::exception& e) {
      Log::Print(LogLevel::critical) << "Unhandled exception: " << e.what();
    } catch (...) {
      Log::Print(LogLevel::critical) << "Unhandled unknown exception";
    }

    Log::Print(LogLevel::critical) << "Stacktrace:\n" << boost::stacktrace::stacktrace();
  }
};

}