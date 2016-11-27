/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef STATEMENT_H
#define STATEMENT_H

#include "stdafx.h"

namespace mxp {
namespace sqlite {

class Statement {
public:
  Statement(SqliteConnection::Handle dbConnection, const std::string& req)
    : m_stmt(nullptr, [](sqlite3_stmt* stmt) {
        sqlite3_clear_bindings(stmt);
        sqlite3_reset(stmt);
      })
    , m_dbConn(dbConnection)
    , m_bindIdx(0) {
    std::lock_guard<compat::Mutex> lock(StatementsCacheLock);
    auto& connMap = StatementsCache[ dbConnection ];
    auto it = connMap.find(req);
    if (it == end(connMap)) {
      sqlite3_stmt* stmt;
      auto res = sqlite3_prepare_v2(dbConnection, req.c_str(), -1, &stmt, nullptr);
      if (res != SQLITE_OK) {
        throw std::runtime_error(std::string("Failed to compile request: ") + req + " " + sqlite3_errmsg(dbConnection));
      }
      m_stmt.reset(stmt);
      connMap.emplace(req, CachedStmtPtr(stmt, &sqlite3_finalize));
    } else {
      m_stmt.reset(it->second.get());
    }
    if (req == "COMMIT")
      m_isCommit = true;
  }

  template <typename... Args>
  void Execute(Args&&... args) {
    m_bindIdx = 1;
    (void)std::initializer_list<bool>({ _bind(std::forward<Args>(args))... });
  }

  Row Row() {
    auto maxRetries = 10;
    while (true) {
      auto res = sqlite3_step(m_stmt.get());
      if (res == SQLITE_ROW)
        return mxp::sqlite::Row(m_stmt.get());
      else if (res == SQLITE_DONE)
        return mxp::sqlite::Row();
      else if (res == SQLITE_BUSY && (Transaction::TransactionInProgress() == false || m_isCommit == true) && maxRetries-- > 0)
        continue;
      std::string errMsg = sqlite3_errmsg(m_dbConn);
      switch (res) {
        case SQLITE_CONSTRAINT:
          throw errors::ConstraintViolation(sqlite3_sql(m_stmt.get()), errMsg);
        default:
          throw std::runtime_error(std::string{ sqlite3_sql(m_stmt.get()) } + ": " + errMsg);
      }
    }
  }

  static void FlushStatementCache() {
    std::lock_guard<compat::Mutex> lock(StatementsCacheLock);
    StatementsCache.clear();
  }

private:
  template <typename T>
  bool _bind(T&& value) {
    auto res = Traits<T>::Bind(m_stmt.get(), m_bindIdx, std::forward<T>(value));
    if (res != SQLITE_OK)
      throw std::runtime_error("Failed to bind parameter");
    m_bindIdx++;
    return true;
  }

private:
  // Used during the connection lifetime. This holds a compiled request
  using CachedStmtPtr = std::unique_ptr<sqlite3_stmt, int (*)(sqlite3_stmt*)>;
  // Used for the current statement execution, this
  // basically holds the state of the currently executed request.
  using StatementPtr = std::unique_ptr<sqlite3_stmt, void(*)(sqlite3_stmt*)>;
  StatementPtr m_stmt;
  SqliteConnection::Handle m_dbConn;
  unsigned int m_bindIdx;
  bool m_isCommit;
  static compat::Mutex StatementsCacheLock;
  static std::unordered_map<SqliteConnection::Handle, std::unordered_map<std::string, CachedStmtPtr>> StatementsCache;
};

} /* namespace sqlite */
} /* namespace mxp */

#endif /* STATEMENT_H */