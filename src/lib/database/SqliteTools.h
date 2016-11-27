/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef SQLITETOOLS_H
#define SQLITETOOLS_H

#include <sqlite3.h>

#include "compat/Mutex.h"
#include "logging/Logger.h"
#include "SqliteConnection.h"
#include "SqliteErrors.h"
#include "SqliteTraits.h"
#include "SqliteTransaction.h"
#include "Row.h"
#include "Statement.h"

namespace mxp {
namespace sqlite {

class Tools {
public:
  /**
   * Will fetch all records of type IMPL and return them as a shared_ptr to INTF
   * This WILL add all fetched records to the cache
   *
   * @param results   A reference to the result vector. All existing elements will
   *          be discarded.
   */
  template <typename IMPL, typename INTF, typename... Args>
  static std::vector<std::shared_ptr<INTF> > FetchAll(MediaExplorerPtr ml, const std::string& req, Args&&... args) {
    auto dbConnection = ml->GetConnection();
    SqliteConnection::ReadContext ctx;
    if (Transaction::TransactionInProgress() == false)
      ctx = dbConnection->AcquireReadContext();
    auto chrono = std::chrono::steady_clock::now();

    std::vector<std::shared_ptr<INTF>> results;
    auto stmt = Statement(dbConnection->GetConnection(), req);
    stmt.Execute(std::forward<Args>(args)...);
    Row sqliteRow;
    while ((sqliteRow = stmt.Row()) != nullptr) {
      auto row = IMPL::Load(ml, sqliteRow);
      results.push_back(row);
    }
    auto duration = std::chrono::steady_clock::now() - chrono;
    LOG_DEBUG("Executed ", req, " in ", std::chrono::duration_cast<std::chrono::microseconds>(duration).count(), "µs");
    return results;
  }

  template <typename T, typename... Args>
  static std::shared_ptr<T> FetchOne(MediaExplorerPtr ml, const std::string& req, Args&&... args) {
    auto dbConnection = ml->GetConnection();
    SqliteConnection::ReadContext ctx;
    if (Transaction::TransactionInProgress() == false)
      ctx = dbConnection->AcquireReadContext();
    auto chrono = std::chrono::steady_clock::now();

    auto stmt = Statement(dbConnection->GetConnection(), req);
    stmt.Execute(std::forward<Args>(args)...);
    auto row = stmt.Row();
    if (row == nullptr)
      return nullptr;
    auto res = T::Load(ml, row);
    auto duration = std::chrono::steady_clock::now() - chrono;
    LOG_DEBUG("Executed ", req, " in ", std::chrono::duration_cast<std::chrono::microseconds>(duration).count(), "µs");
    return res;
  }

  template <typename... Args>
  static bool ExecuteRequest(DBConnection dbConnection, const std::string& req, Args&&... args) {
    SqliteConnection::WriteContext ctx;
    if (Transaction::TransactionInProgress() == false)
      ctx = dbConnection->AcquireWriteContext();
    return ExecuteRequestLocked(dbConnection, req, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static bool ExecuteDelete(DBConnection dbConnection, const std::string& req, Args&&... args) {
    SqliteConnection::WriteContext ctx;
    if (Transaction::TransactionInProgress() == false)
      ctx = dbConnection->AcquireWriteContext();
    if (ExecuteRequestLocked(dbConnection, req, std::forward<Args>(args)...) == false)
      return false;
    return sqlite3_changes(dbConnection->GetConnection()) > 0;
  }

  template <typename... Args>
  static bool ExecuteUpdate(DBConnection dbConnection, const std::string& req, Args&&... args) {
    // The code would be exactly the same, do not freak out because it calls ExecuteDelete :)
    return ExecuteDelete(dbConnection, req, std::forward<Args>(args)...);
  }

  /**
   * Inserts a record to the DB and return the newly created primary key.
   * Returns 0 (which is an invalid sqlite primary key) when insertion fails.
   */
  template <typename... Args>
  static int64_t ExecuteInsert(DBConnection dbConnection, const std::string& req, Args&&... args) {
    SqliteConnection::WriteContext ctx;
    if (Transaction::TransactionInProgress() == false)
      ctx = dbConnection->AcquireWriteContext();
    if (ExecuteRequestLocked(dbConnection, req, std::forward<Args>(args)...) == false)
      return 0;
    return sqlite3_last_insert_rowid(dbConnection->GetConnection());
  }

private:
  template <typename... Args>
  static bool ExecuteRequestLocked(DBConnection dbConnection, const std::string& req, Args&&... args) {
    auto chrono = std::chrono::steady_clock::now();
    auto stmt = Statement(dbConnection->GetConnection(), req);
    stmt.Execute(std::forward<Args>(args)...);
    while (stmt.Row() != nullptr)
      ;
    auto duration = std::chrono::steady_clock::now() - chrono;
    LOG_DEBUG("Executed ", req, " in ", std::chrono::duration_cast<std::chrono::microseconds>(duration).count(), "µs");
    return true;
  }
};

} /* namespace sqlite */
} /* namespace mxp */

#endif /* SQLITETOOLS_H */
