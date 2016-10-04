/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include <iostream>
#include "mediaexplorer/ILogger.h"

namespace mxp {

class ConsoleLogger : public ILogger {
public:
  virtual void Error(const std::string& msg) override {
    Lock();
    std::cout << msg;
    Unlock();
  }

  virtual void Warning(const std::string& msg) override {
    Lock();
    std::cout << msg;
    Unlock();
  }

  virtual void Info(const std::string& msg) override {
    Lock();
    std::cout << msg;
    Unlock();
  }

  virtual void Debug(const std::string& msg) override {
    Lock();
    std::cout << msg;
    Unlock();
  }

  virtual void Trace(const std::string& msg) override {
    Lock();
    std::cout << msg;
    Unlock();
  }

private:
  virtual void Lock() {
    m_lock.lock();
  }

  virtual void Unlock() {
    m_lock.unlock();
  }

  compat::Mutex m_lock;
};

}
