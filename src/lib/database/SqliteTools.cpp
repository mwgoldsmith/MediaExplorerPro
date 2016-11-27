/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
#  include "config.h"
#endif

#include "SqliteTools.h"

namespace mxp {
namespace sqlite {

std::unordered_map<SqliteConnection::Handle, std::unordered_map<std::string, Statement::CachedStmtPtr>> Statement::StatementsCache;

compat::Mutex Statement::StatementsCacheLock;

}
}
