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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "SqliteTransaction.h"
#include "SqliteTools.h"

namespace mxp {
namespace sqlite {

thread_local Transaction* Transaction::CurrentTransaction = nullptr;

Transaction::Transaction(DBConnection dbConn)
  : m_dbConn(dbConn)
  , m_ctx(dbConn->AcquireWriteContext()) {
  assert(CurrentTransaction == nullptr);
  LOG_DEBUG("Starting SQLite transaction");
  Statement s(dbConn->GetConnection(), "BEGIN");
  s.execute();
  while (s.row() != nullptr)
    ;
  CurrentTransaction = this;
}

Transaction::~Transaction() {
  if (CurrentTransaction != nullptr) {
    Statement s(m_dbConn->GetConnection(), "ROLLBACK");
    s.execute();
    while (s.row() != nullptr)
      ;
    CurrentTransaction = nullptr;
  }
}

void Transaction::Commit() {
  auto chrono = std::chrono::steady_clock::now();
  Statement s(m_dbConn->GetConnection(), "COMMIT");
  s.execute();
  while (s.row() != nullptr)
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
