/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef CACHE_H
#define CACHE_H

#include "stdafx.h"
#include <cassert>

#include "compat/Mutex.h"

namespace mxp {

template <typename T>
class Cache {
public:
  Cache() : m_cached(false) {}

  bool IsCached() const { return m_cached; }

  void MarkCached() { m_cached = true; }

  operator T() const {
    assert(m_cached);
    return m_value;
  }

  operator const T&() const {
    assert(m_cached);
    return m_value;
  }

  const T& Get() const {
    assert(m_cached);
    return m_value;
  }

  T& Get() {
    assert(m_cached);
    return m_value;
  }

  template <typename U>
  T& operator=(U&& value) {
    static_assert(std::is_convertible<typename std::decay<U>::type, typename std::decay<T>::type>::value, "Mismatching types");
    m_value = std::forward<U>(value);
    m_cached = true;
    return m_value;
  }

  std::unique_lock<compat::Mutex> Lock() {
    return std::unique_lock<compat::Mutex>(m_lock);
  }

private:
  T             m_value;
  compat::Mutex m_lock;
  bool          m_cached;
};

} /* namespace mxp */

#endif /* CACHE_H */
