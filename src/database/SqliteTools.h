/*****************************************************************************
 * Media Library
 *****************************************************************************
 * Copyright (C) 2015 Hugo Beauzée-Luyssen, Videolabs
 *
 * Authors: Hugo Beauzée-Luyssen<hugo@beauzee.fr>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef SQLITETOOLS_H
#define SQLITETOOLS_H

#include <cassert>
#include <chrono>
#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <sqlite3.h>

#include "compat/Mutex.h"
#include "logging/Logger.h"
#include "SqliteConnection.h"
#include "SqliteErrors.h"
#include "SqliteTraits.h"
#include "SqliteTransaction.h"
#include "MediaExplorer.h"
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
    stmt.execute(std::forward<Args>(args)...);
    Row sqliteRow;
    while ((sqliteRow = stmt.row()) != nullptr) {
      auto row = IMPL::load(ml, sqliteRow);
      results.push_back(row);
    }
    auto duration = std::chrono::steady_clock::now() - chrono;
    LOG_DEBUG("Executed ", req, " in ", std::chrono::duration_cast<std::chrono::microseconds>(duration).count(), "µs");
    return results;
  }

  template <typename T, typename... Args>
  static std::shared_ptr<T> fetchOne(MediaExplorerPtr ml, const std::string& req, Args&&... args) {
    auto dbConnection = ml->GetConnection();
    SqliteConnection::ReadContext ctx;
    if (Transaction::TransactionInProgress() == false)
      ctx = dbConnection->AcquireReadContext();
    auto chrono = std::chrono::steady_clock::now();

    auto stmt = Statement(dbConnection->GetConnection(), req);
    stmt.execute(std::forward<Args>(args)...);
    auto row = stmt.row();
    if (row == nullptr)
      return nullptr;
    auto res = T::load(ml, row);
    auto duration = std::chrono::steady_clock::now() - chrono;
    LOG_DEBUG("Executed ", req, " in ", std::chrono::duration_cast<std::chrono::microseconds>(duration).count(), "µs");
    return res;
  }

  template <typename... Args>
  static bool executeRequest(DBConnection dbConnection, const std::string& req, Args&&... args) {
    SqliteConnection::WriteContext ctx;
    if (Transaction::TransactionInProgress() == false)
      ctx = dbConnection->AcquireWriteContext();
    return executeRequestLocked(dbConnection, req, std::forward<Args>(args)...);
  }

  template <typename... Args>
  static bool executeDelete(DBConnection dbConnection, const std::string& req, Args&&... args) {
    SqliteConnection::WriteContext ctx;
    if (Transaction::TransactionInProgress() == false)
      ctx = dbConnection->AcquireWriteContext();
    if (executeRequestLocked(dbConnection, req, std::forward<Args>(args)...) == false)
      return false;
    return sqlite3_changes(dbConnection->GetConnection()) > 0;
  }

  template <typename... Args>
  static bool executeUpdate(DBConnection dbConnection, const std::string& req, Args&&... args) {
    // The code would be exactly the same, do not freak out because it calls executeDelete :)
    return executeDelete(dbConnection, req, std::forward<Args>(args)...);
  }

  /**
   * Inserts a record to the DB and return the newly created primary key.
   * Returns 0 (which is an invalid sqlite primary key) when insertion fails.
   */
  template <typename... Args>
  static int64_t executeInsert(DBConnection dbConnection, const std::string& req, Args&&... args) {
    SqliteConnection::WriteContext ctx;
    if (Transaction::TransactionInProgress() == false)
      ctx = dbConnection->AcquireWriteContext();
    if (executeRequestLocked(dbConnection, req, std::forward<Args>(args)...) == false)
      return 0;
    return sqlite3_last_insert_rowid(dbConnection->GetConnection());
  }

private:
  template <typename... Args>
  static bool executeRequestLocked(DBConnection dbConnection, const std::string& req, Args&&... args) {
    auto chrono = std::chrono::steady_clock::now();
    auto stmt = Statement(dbConnection->GetConnection(), req);
    stmt.execute(std::forward<Args>(args)...);
    while (stmt.row() != nullptr)
      ;
    auto duration = std::chrono::steady_clock::now() - chrono;
    LOG_DEBUG("Executed ", req, " in ", std::chrono::duration_cast<std::chrono::microseconds>(duration).count(), "µs");
    return true;
  }
};

} /* namespace sqlite */
} /* namespace mxp */

#endif /* SQLITETOOLS_H */
