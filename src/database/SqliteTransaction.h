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

#ifndef SQLITETRANSACTION_H
#define SQLITETRANSACTION_H

#include "SqliteConnection.h"
#include "Types.h"

namespace mxp {
namespace sqlite {

class Transaction {
public:
  Transaction(DBConnection dbConn);
  ~Transaction();
  void Commit();
  static bool TransactionInProgress();

private:
  DBConnection                   m_dbConn;
  SqliteConnection::WriteContext m_ctx;

  static thread_local Transaction* CurrentTransaction;
};

} /* namespace sqlite */
} /* namespace mxp */

#endif /* SQLITETRANSACTION_H */