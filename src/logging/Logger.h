/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#include <atomic>
#include <memory>
#include <sstream>

#include "mediaexplorer/ILogger.h"
#include "mediaexplorer/Types.h"

namespace mxp {

class Log {
private:
  template <typename T>
  static void createMsg(std::stringstream& s, T&& t) {
    s << t;
  }

  template <typename T, typename... Args>
  static void createMsg(std::stringstream& s, T&& t, Args&&... args) {
    s << std::forward<T>(t);
    createMsg(s, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static std::string createMsg(Args&&... args) {
    std::stringstream stream;
    createMsg(stream, std::forward<Args>(args)...);
    stream << "\n";
    return stream.str();
  }

  template <typename... Args>
  static void log(LogLevel lvl, Args&&... args) {
    if (lvl < s_logLevel.load(std::memory_order_relaxed)) {
      return;
    }

    auto msg = createMsg(std::forward<Args>(args)...);
    auto l = s_logger.load(std::memory_order_consume);
    if (l == nullptr) {
      l = s_defaultLogger.get();
      // In case we're logging early (as in, before the static default logger has been constructed, don't blow up)
      if (l == nullptr)
        return;
    }

    switch (lvl) {
    case LogLevel::Error:
      l->Error(msg);
      break;
    case LogLevel::Warning:
      l->Warning(msg);
      break;
    case LogLevel::Info:
      l->Info(msg);
      break;
    case LogLevel::Debug:
      l->Debug(msg);
      break;
    case LogLevel::Verbose:
      l->Verbose(msg);
      break;
    }
  }

public:
  static void SetLogger(ILogger* logger) {
    s_logger.store(logger, std::memory_order_relaxed);
  }

  static void SetLogLevel(LogLevel level) {
    s_logLevel.store(level, std::memory_order_relaxed);
  }

  static LogLevel GetLogLevel() {
    return s_logLevel.load(std::memory_order_relaxed);
  }

  template <typename... Args>
  static void Error(Args&&... args) {
    log(mxp::LogLevel::Error, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void Warning(Args&&... args) {
    log(mxp::LogLevel::Warning, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void Info(Args&&... args) {
    log(mxp::LogLevel::Info, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void Debug(Args&&... args) {
    log(mxp::LogLevel::Debug, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static void Verbose(Args&&... args) {
    log(mxp::LogLevel::Verbose, std::forward<Args>(args)...);
  }

private:
  static std::unique_ptr<ILogger> s_defaultLogger;
  static std::atomic<ILogger*>    s_logger;
  static std::atomic<LogLevel>    s_logLevel;
};

} /* namespace mxp */

#if defined(_MSC_VER)
# define LOG_ORIGIN __FUNCDNAME__, ":"
#else
#if defined(DEBUG)
# define LOG_ORIGIN __FILE__, ":", __LINE__, ' ', __func__
#else
# define LOG_ORIGIN __func__, ":"
#endif
#endif

#define LOG_ERROR(...)   mxp::Log::Error(LOG_ORIGIN, ' ', __VA_ARGS__)
#define LOG_WARN(...)    mxp::Log::Warning(LOG_ORIGIN, ' ', __VA_ARGS__)
#define LOG_INFO(...)    mxp::Log::Info(LOG_ORIGIN, ' ', __VA_ARGS__)
#define LOG_DEBUG(...)   mxp::Log::Debug(LOG_ORIGIN, ' ', __VA_ARGS__)
#define LOG_VERBOSE(...) mxp::Log::Verbose(LOG_ORIGIN, ' ', __VA_ARGS__)

#endif /* LOGGER_H */
