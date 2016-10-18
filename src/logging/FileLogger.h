/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef FILELOGGER_H
#define FILELOGGER_H

#include "stdafx.h"
#include "compat/Mutex.h"
#include "mediaexplorer/ILogger.h"
#include <mediaexplorer/Common.h>

namespace mxp {

class FileLogger : public ILogger {
public:
  FileLogger()
    : FileLogger(MTEXT("output.log")){}

  explicit FileLogger(const mchar* filename) {
    Lock();
    m_fout.open(filename, std::ofstream::out | std::ofstream::app);
    Unlock();
  }

  ~FileLogger() {
    m_fout.flush();
    m_fout.close();
  }

  virtual void Error(const mstring& msg) override {
    Lock();
    m_fout << msg << std::flush;
    Unlock();
  }

  virtual void Warning(const mstring& msg) override {
    Lock();
    m_fout << msg << std::flush;
    Unlock();
  }

  virtual void Info(const mstring& msg) override {
    Lock();
    m_fout << msg << std::flush;
    Unlock();
  }

  virtual void Debug(const mstring& msg) override {
    Lock();
    m_fout << msg << std::flush;
    Unlock();
  }

  virtual void Trace(const mstring& msg) override {
    Lock();
    m_fout << msg << std::flush;
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
  std::ofstream m_fout;
};

} /* namespace mxp */

#endif /* FILELOGGER_H */
