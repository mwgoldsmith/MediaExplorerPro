/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include <iostream>
#include "mediaexplorer/ILogger.h"

namespace mxp {

class IostreamLogger : public ILogger {
public:
  virtual void Error(const std::string& msg) override {
    std::cout << "ERROR:   " << msg;
  }

  virtual void Warning(const std::string& msg) override {
    std::cout << "WARNING: " << msg;
  }

  virtual void Info(const std::string& msg) override {
    std::cout << "INFO:    " << msg;
  }

  virtual void Debug(const std::string& msg) override {
    std::cout << "DEBUG:   " << msg;
  }

  virtual void Verbose(const std::string& msg) override {
    std::cout << "VERBOSE: " << msg;
  }
};

}
