/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#include "mediaexplorer/Common.h"
#include "compat/Thread.h"
#include "compat/Mutex.h"
#include "mediaexplorer/ILogger.h"
#include "mediaexplorer/LogLevel.h"

namespace mxp {

class Log {
private:
  static double Round(double val) {
    return (val < 0) ? ceil(val - 0.5) : floor(val + 0.5);
  }

  static std::string CurrentTime() {
    auto n = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(n);
    auto ms = static_cast<int>(Round((std::chrono::milliseconds(now_c).count() % 1000) * 1000.0) / 1000.0);
    auto t = std::localtime(&now_c);
    char buffer[32];

    snprintf(buffer, sizeof(buffer) / sizeof(char), "%d-%02d-%02d %02d:%02d:%02d.%03d ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, ms);

    return std::string(buffer);
  }

  static std::string CurrentThreadId() {
    auto t = compat::this_thread::get_id().m_id;
    char buffer[32];

    snprintf(buffer, sizeof(buffer) / sizeof(char), "[ %04X ] ", t);

    return std::string(buffer);
  }

  template <typename T>
  static void CreateMsg(std::stringstream& s, T&& t) {
    s << t;
  }

  template <typename T, typename... Args>
  static void CreateMsg(std::stringstream& s, T&& t, Args&&... args) {
    s << std::forward<T>(t);
    CreateMsg(s, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static std::string CreateMsg(Args&&... args) {
    std::stringstream stream;
    CreateMsg(stream, std::forward<Args>(args)...);
    stream << "\n";
    return stream.str();
  }

  template <typename... Args>
  static void log(LogLevel lvl, Args&&... args) {
    if (lvl < s_logLevel.load(std::memory_order_relaxed)) {
      return;
    }

    auto msg = CreateMsg(std::forward<Args>(args)...);
    auto l = s_logger.load(std::memory_order_consume);
    if (l == nullptr) {
      l = s_defaultLogger.get();
      // In case we're logging early (as in, before the static default logger has been constructed, don't blow up)
      if (l == nullptr)
        return;
    }

    switch (lvl) {
    case LogLevel::Error:
      l->Error(CurrentTime() + "[ ERROR   ] " + CurrentThreadId() + msg);
      break;
    case LogLevel::Warning:
      l->Warning(CurrentTime() + "[ WARNING ] " + CurrentThreadId() + msg);
      break;
    case LogLevel::Info:
      l->Info(CurrentTime() + "[ INFO    ] " + CurrentThreadId() + msg);
      break;
    case LogLevel::Debug:
      l->Debug(CurrentTime() + "[ DEBUG   ] " + CurrentThreadId() + msg);
      break;
    case LogLevel::Trace:
      l->Trace(CurrentTime() + "[ TRACE   ] " + CurrentThreadId() + msg);
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
  static void Trace(Args&&... args) {
    log(mxp::LogLevel::Trace, std::forward<Args>(args)...);
  }

private:
  static std::unique_ptr<ILogger> s_defaultLogger;
  static std::atomic<ILogger*>    s_logger;
  static std::atomic<LogLevel>    s_logLevel;
};

} /* namespace mxp */

#if defined(_MSC_VER)
# define LOG_ORIGIN __FUNCTION__, ":"
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
#define LOG_TRACE(...)   mxp::Log::Trace(LOG_ORIGIN, ' ', __VA_ARGS__)

#endif /* LOGGER_H */
