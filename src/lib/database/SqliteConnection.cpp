/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
#  include "config.h"
#endif

#include "SqliteConnection.h"
#include "SqliteTools.h"

namespace mxp {

SqliteConnection::SqliteConnection(const std::string &dbPath)
  : m_dbPath(dbPath)
  , m_readLock(m_contextLock)
  , m_writeLock(m_contextLock) {
  LOG_DEBUG("SL-C-000 - Create connection");
  if (sqlite3_threadsafe() == 0)
    throw std::runtime_error("SQLite isn't built with threadsafe mode");
  if (sqlite3_config(SQLITE_CONFIG_MULTITHREAD) == SQLITE_ERROR)
    throw std::runtime_error("Failed to enable sqlite multithreaded mode");
  LOG_DEBUG("SL-C-900 - Connection created");
}

SqliteConnection::~SqliteConnection() {
  sqlite::Statement::FlushStatementCache();
}

SqliteConnection::Handle SqliteConnection::GetConnection() {
  std::unique_lock<compat::Mutex> lock(m_connMutex);
  sqlite3* dbConnection;
  auto it = m_conns.find(compat::this_thread::get_id());
  if (it == end(m_conns)) {
    auto res = sqlite3_open(m_dbPath.c_str(), &dbConnection);
    ConnPtr dbConn(dbConnection, &sqlite3_close);
    if (res != SQLITE_OK)
      throw std::runtime_error("Failed to connect to database");
    sqlite::Statement s(dbConnection, "PRAGMA foreign_keys = ON");
    s.Execute();
    while (s.Row() != nullptr)
      ;
    s = sqlite::Statement(dbConnection, "PRAGMA recursive_triggers = ON");
    s.Execute();
    while (s.Row() != nullptr)
      ;
    m_conns.emplace(compat::this_thread::get_id(), std::move(dbConn));
    sqlite3_update_hook(dbConnection, &updateHook, this);
    return dbConnection;
  }
  return it->second.get();
}

std::unique_ptr<sqlite::Transaction> SqliteConnection::NewTransaction() {
  return std::unique_ptr<sqlite::Transaction>{ new sqlite::Transaction(this) };
}

SqliteConnection::ReadContext SqliteConnection::AcquireReadContext() {
  return ReadContext{ m_readLock };
}

SqliteConnection::WriteContext SqliteConnection::AcquireWriteContext() {
  return WriteContext{ m_writeLock };
}

void SqliteConnection::registerUpdateHook(const std::string& table, SqliteConnection::UpdateHookCb cb) {
  m_hooks.emplace(table, cb);
}

void SqliteConnection::updateHook(void* data, int reason, const char*,const char* table, sqlite_int64 rowId) {
  const auto self = reinterpret_cast<SqliteConnection*>(data);
  auto it = self->m_hooks.find(table);
  if (it == end(self->m_hooks))
    return;

  switch (reason) {
  case SQLITE_INSERT:
    it->second(HookReason::Insert, rowId);
    break;
  case SQLITE_UPDATE:
    it->second(HookReason::Update, rowId);
    break;
  case SQLITE_DELETE:
    it->second(HookReason::Delete, rowId);
    break;
  default:
    break;
  }
}

}
