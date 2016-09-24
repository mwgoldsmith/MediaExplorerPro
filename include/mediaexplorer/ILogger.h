/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_ILOGGER_H
#define MXP_ILOGGER_H

#include <string>

#include "mediaexplorer/LogLevel.h"

namespace mxp {

class ILogger {
public:
  virtual ~ILogger() = default;
  virtual void Error(const std::string& msg) = 0;
  virtual void Warning(const std::string& msg) = 0;
  virtual void Info(const std::string& msg) = 0;
  virtual void Debug(const std::string& msg) = 0;
  virtual void Verbose(const std::string& msg) = 0;
};

} /* namespace mxp */

#endif /* MXP_ILOGGER_H */
