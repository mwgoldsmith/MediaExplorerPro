/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
#  include "config.h"
#endif

#include "SqliteTransaction.h"
#include "SqliteTools.h"

namespace mxp {
namespace sqlite {

THREAD_LOCAL Transaction* Transaction::CurrentTransaction = nullptr;

Transaction::Transaction(DBConnection dbConn)
  : m_dbConn(dbConn)
  , m_ctx(dbConn->AcquireWriteContext()) {
  assert(CurrentTransaction == nullptr);
  LOG_DEBUG("Starting SQLite transaction");
  Statement s(dbConn->GetConnection(), "BEGIN");
  s.Execute();
  while (s.Row() != nullptr)
    ;
  CurrentTransaction = this;
}

Transaction::~Transaction() {
  if (CurrentTransaction != nullptr) {
    Statement s(m_dbConn->GetConnection(), "ROLLBACK");
    s.Execute();
    while (s.Row() != nullptr)
      ;
    CurrentTransaction = nullptr;
  }
}

void Transaction::Commit() {
  auto chrono = std::chrono::steady_clock::now();
  Statement s(m_dbConn->GetConnection(), "COMMIT");
  s.Execute();
  while (s.Row() != nullptr)
    ;
  auto duration = std::chrono::steady_clock::now() - chrono;
  LOG_DEBUG("Flushed transaction in ", std::chrono::duration_cast<std::chrono::microseconds>(duration).count(), "µs");
  CurrentTransaction = nullptr;
  m_ctx.unlock();
}

bool Transaction::TransactionInProgress() {
  return CurrentTransaction != nullptr;
}

} /* namespace sqlite */
} /* namespace mxp */
