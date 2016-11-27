/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef SQLITETRANSACTION_H
#define SQLITETRANSACTION_H

#include "SqliteConnection.h"
#include "Types.h"
#include "mediaexplorer/Common.h"

namespace mxp {
namespace sqlite {

class Transaction {
public:
  explicit Transaction(DBConnection dbConn);

  ~Transaction();

  void Commit();

  static bool TransactionInProgress();

private:
  DBConnection                   m_dbConn;
  SqliteConnection::WriteContext m_ctx;

  static THREAD_LOCAL Transaction* CurrentTransaction;
};

} /* namespace sqlite */
} /* namespace mxp */

#endif /* SQLITETRANSACTION_H */
