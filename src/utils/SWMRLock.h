/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef SWMRLOCK_H
#define SWMRLOCK_H

#include "stdafx.h"
#include "compat/ConditionVariable.h"
#include "compat/Mutex.h"

namespace mxp {
namespace utils {

/**
 * @brief Single Write Multiple Reader lock
 */
class SWMRLock {
public:
  SWMRLock()
    : m_nbReader(0)
    , m_nbReaderWaiting(0)
    , m_writing(false)
    , m_nbWriterWaiting(0) {
  }

  void LockRead() {
    std::unique_lock<compat::Mutex> lock(m_lock);
    ++m_nbReaderWaiting;
    m_writeDoneCond.wait(lock, [this](){
      return m_writing == false;
    });
    --m_nbReaderWaiting;
    m_nbReader++;
  }

  void UnlockRead() {
    std::unique_lock<compat::Mutex> lock(m_lock);
    --m_nbReader;
    if (m_nbReader == 0 && m_nbWriterWaiting > 0)
      m_writeDoneCond.notify_one();
  }

  void LockWrite() {
    std::unique_lock<compat::Mutex> lock(m_lock);
    ++m_nbWriterWaiting;
    m_writeDoneCond.wait(lock, [this](){
      return m_writing == false && m_nbReader == 0;
    });
    --m_nbWriterWaiting;
    m_writing = true;
  }

  void UnlockWrite() {
    std::unique_lock<compat::Mutex> lock(m_lock);
    m_writing = false;
    if (m_nbReaderWaiting > 0 || m_nbWriterWaiting > 0)
      m_writeDoneCond.notify_all();
  }

private:
  compat::ConditionVariable m_writeDoneCond;
  compat::Mutex m_lock;
  unsigned int m_nbReader;
  unsigned int m_nbReaderWaiting;
  bool m_writing;
  unsigned int m_nbWriterWaiting;
};

} /* namespace utils */
} /* namespace mxp */

#endif /* SWMRLOCK_H */
