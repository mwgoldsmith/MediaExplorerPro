/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Logger.h"
#include "IostreamLogger.h"

std::unique_ptr<mxp::ILogger> mxp::Log::s_defaultLogger = std::unique_ptr<mxp::ILogger>(new mxp::IostreamLogger);
std::atomic<mxp::ILogger*> mxp::Log::s_logger;
std::atomic<mxp::LogLevel> mxp::Log::s_logLevel;
