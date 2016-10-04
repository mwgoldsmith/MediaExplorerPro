/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_ILOGGER_H
#define MXP_ILOGGER_H

#include <string>

namespace mxp {

class ILogger {
public:
  virtual ~ILogger() = default;

  /**
   * @brief Emits an error log message
   * @param msg The log message.
   */
  virtual void Error(const std::string& msg) = 0;

  /**
  * @brief Emits an warning log message
  * @param msg The log message.
  */
  virtual void Warning(const std::string& msg) = 0;

  /**
  * @brief Emits an info log message
  * @param msg The log message.
  */
  virtual void Info(const std::string& msg) = 0;

  /**
  * @brief Emits an debug log message
  * @param msg The log message.
  */
  virtual void Debug(const std::string& msg) = 0;

  /**
  * @brief Emits an trace log message
  * @param msg The log message.
  */
  virtual void Trace(const std::string& msg) = 0;
};

} /* namespace mxp */

#endif /* MXP_ILOGGER_H */
