/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef WRITELOCKER_H
#define WRITELOCKER_H

#include "stdafx.h"
#include "utils/SWMRLock.h"

namespace mxp {
namespace utils {

class WriteLocker {
public:
  explicit WriteLocker(SWMRLock& l)
    : m_lock(l) {
  }

  void lock() {
    m_lock.LockWrite();
  }

  void unlock() {
    m_lock.UnlockWrite();
  }

private:
  SWMRLock& m_lock;
};

} /* namespace utils */
} /* namespace mxp */

#endif /* WRITELOCKER_H */
