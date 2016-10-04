/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include <iostream>
#include <string>
#include "compat/Mutex.h"
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
  void Lock() {
    m_lock.lock();
  }

  void Unlock() {
    m_lock.unlock();
  }

  compat::Mutex m_lock;
};

} /* namespace mxp */

#endif /* CONSOLELOGGER_H */
