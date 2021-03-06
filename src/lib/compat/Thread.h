/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef COMPAT_THREAD_H
#define COMPAT_THREAD_H

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#if CXX11_THREADS

namespace mxp {
namespace compat {
using Thread = std::thread;
namespace this_thread = std::this_thread;
}
}

#else /* CXX11_THREADS */

#include <system_error>
#ifndef HAVE_WIN32
#  include <unistd.h>
#  include <pthread.h>
#endif

namespace mxp {
namespace compat {

// temporary forward declarations, so we can use thread_id from this_thread::get_id
// and have this_thread::get_id declared before thread_id itself, in order to friend it.
namespace details { class thread_id; }
namespace this_thread { details::thread_id get_id(); }

class Thread;

namespace details {

class thread_id {
public:
#ifdef HAVE_WIN32
  using native_thread_id_type = DWORD;
#else
  using native_thread_id_type = pthread_t;
#endif

  constexpr thread_id() noexcept : m_id{} {}
  thread_id(const thread_id&) = default;
  thread_id& operator=(const thread_id&) = default;
  thread_id(thread_id&& r) noexcept : m_id(r.m_id) { r.m_id = 0; }
  thread_id& operator=(thread_id&& r) noexcept { m_id = r.m_id; r.m_id = 0; return *this; }

  bool operator==(const thread_id& r) const noexcept { return m_id == r.m_id; }
  bool operator!=(const thread_id& r) const noexcept { return m_id != r.m_id; }
  bool operator< (const thread_id& r) const noexcept { return m_id <  r.m_id; }
  bool operator<=(const thread_id& r) const noexcept { return m_id <= r.m_id; }
  bool operator> (const thread_id& r) const noexcept { return m_id >  r.m_id; }
  bool operator>=(const thread_id& r) const noexcept { return m_id >= r.m_id; }

  native_thread_id_type m_id;

private:
  thread_id(native_thread_id_type id) : m_id(id) {}

  friend thread_id this_thread::get_id();
  friend Thread;
  friend std::hash<thread_id>;
};

} /* namespace details */

// Compatibility thread class, for platforms that don't implement C++11 (or do it incorrectly)
// This handles the very basic usage we need for the MediaExplorer
class Thread {
  template <typename T>
  struct Invoker {
    void (T::*func)();
    T* inst;
  };

#ifdef HAVE_WIN32
  template <typename T>
  static
#ifdef _MSC_VER
    DWORD __stdcall
#else
    __attribute__((__stdcall__)) DWORD
#endif
  threadRoutine(void* opaque) {
    auto invoker = std::unique_ptr<Invoker<T>>(reinterpret_cast<Invoker<T>*>(opaque));
    (invoker->inst->*(invoker->func))();
    return 0;
  }
#else
  template <typename T>
  static void* threadRoutine(void* opaque) {
    auto invoker = std::unique_ptr<Invoker<T>>(reinterpret_cast<Invoker<T>*>(opaque));
    (invoker->inst->*(invoker->func))();
    return nullptr;
  }
#endif

public:
  using id = details::thread_id;

  template <typename T>
  Thread(void (T::*entryPoint)(), T* inst) {
    auto i = std::unique_ptr<Invoker<T>>(new Invoker<T>{
      entryPoint, inst
    });
#ifdef HAVE_WIN32
    if ((m_handle = CreateThread(nullptr, 0, &threadRoutine<T>, i.get(), 0, &m_id.m_id)) == nullptr)
#else
    if (pthread_create(&m_id.m_id, nullptr, &threadRoutine<T>, i.get()) != 0)
#endif
      throw std::system_error{ std::make_error_code(std::errc::resource_unavailable_try_again) };
    i.release();
  }

  static unsigned hardware_concurrency() noexcept {
#ifdef HAVE_WIN32
#if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP
    return GetCurrentProcessorNumber();
#else
    return 0;
#endif
#else
    return sysconf(_SC_NPROCESSORS_ONLN);
#endif
  }

  void join() {
    if (!joinable())
      throw std::system_error{ std::make_error_code(std::errc::invalid_argument) };
    if (this_thread::get_id() == m_id)
      throw std::system_error{ std::make_error_code(std::errc::resource_deadlock_would_occur) };
#ifdef HAVE_WIN32
    WaitForSingleObjectEx(m_handle, INFINITE, TRUE);
    CloseHandle(m_handle);
#else
    auto res = pthread_join(m_id.m_id, nullptr);
    if (res != 0)
      throw std::system_error{ std::error_code(res, std::system_category()) };
#endif
    m_id = id{};
  }

  // Platform agnostic methods:

  Thread() = default;
  ~Thread() {
    if (joinable() == true)
      std::terminate();
  }

  Thread(Thread&&) = default;
  Thread& operator=(Thread&&) = default;
  Thread(const Thread&) = delete;
  Thread& operator=(const Thread&) = delete;

  bool joinable() {
    return m_id != id{};
  }

  id get_id() {
    return m_id;
  }

private:
#ifdef HAVE_WIN32
  HANDLE m_handle;
#endif
  id m_id;
};

namespace this_thread {

inline Thread::id get_id() {
#ifdef HAVE_WIN32
  return { ::GetCurrentThreadId() };
#else
  return { pthread_self() };
#endif
}

template <typename Rep, typename Period>
inline void sleep_for(const std::chrono::duration<Rep, Period>& duration) {
  auto d = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
#ifdef HAVE_WIN32
  Sleep(d.count());
#else
  usleep(d.count() * 1000);
#endif
}

} /* namespace this_thread */

} /* namespace compat */
} /* namespace mxp */

namespace std {

template <>
struct hash<mxp::compat::Thread::id> {
  size_t operator()(const mxp::compat::Thread::id& id) const noexcept {
    static_assert(sizeof(id.m_id) <= sizeof(size_t), "thread handle is too big");
    return static_cast<size_t>(id.m_id);
  }
};

} /* namespace std */

#endif /* CXX11_THREADS */

#endif /* COMPAT_THREAD_H */
