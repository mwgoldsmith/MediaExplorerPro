/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef SQLITECONNECTION_H
#define SQLITECONNECTION_H

#include <sqlite3.h>

#include "compat/ConditionVariable.h"
#include "compat/Mutex.h"
#include "compat/Thread.h"
#include "utils/SWMRLock.h"
#include "utils/ReadLocker.h"
#include "utils/WriteLocker.h"

namespace mxp {

namespace sqlite {
  class Transaction;
}

class SqliteConnection {
public:
  enum class HookReason {
    Insert,
    Delete,
    Update
  };

  using ReadContext = std::unique_lock<utils::ReadLocker>;
  using WriteContext = std::unique_lock<utils::WriteLocker>;
  using Handle = sqlite3*;
  using UpdateHookCb = std::function<void(HookReason, int64_t)>;

  explicit SqliteConnection(const std::string& dbPath);
  ~SqliteConnection();

  // Returns the current thread's connection
  // This will initiate a connection if required
  Handle GetConnection();
  std::unique_ptr<sqlite::Transaction> NewTransaction();
  ReadContext AcquireReadContext();
  WriteContext AcquireWriteContext();

  void registerUpdateHook(const std::string& table, UpdateHookCb cb);

private:
  static void updateHook(void* data, int reason, const char* database, const char* table, sqlite_int64 rowId);

private:
  using ConnPtr = std::unique_ptr<sqlite3, int(*)(sqlite3*)>;
  
  const std::string                               m_dbPath;
  compat::Mutex                                   m_connMutex;
  std::unordered_map<compat::Thread::id, ConnPtr> m_conns;
  utils::SWMRLock                                 m_contextLock;
  utils::ReadLocker                               m_readLock;
  utils::WriteLocker                              m_writeLock;
  std::unordered_map<std::string, UpdateHookCb>   m_hooks;
};

}

#endif // SQLITECONNECTION_H
