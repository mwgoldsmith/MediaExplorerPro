/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef READLOCKER_H
#define READLOCKER_H

#include "utils/SWMRLock.h"

namespace mxp {
namespace utils {

class ReadLocker {
public:
  explicit ReadLocker(SWMRLock& l)
    : m_lock(l) {
  }

  void lock() {
    m_lock.LockRead();
  }

  void unlock() {
    m_lock.UnlockRead();
  }

private:
  SWMRLock& m_lock;
};

} /* namespace utils */
} /* namespace mxp */

#endif /* READLOCKER_H */
