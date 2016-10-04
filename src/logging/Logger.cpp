/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Logger.h"
#include "ConsoleLogger.h"

std::unique_ptr<mxp::ILogger> mxp::Log::s_defaultLogger = std::unique_ptr<ILogger>(std::make_unique<mxp::ConsoleLogger>());
std::atomic<mxp::ILogger*> mxp::Log::s_logger;
std::atomic<mxp::LogLevel> mxp::Log::s_logLevel;
