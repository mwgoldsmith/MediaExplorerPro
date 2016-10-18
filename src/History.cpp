/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "History.h"
#include "database/SqliteTools.h"

using mxp::policy::HistoryTable;
const std::string HistoryTable::Name = "History";
const std::string HistoryTable::PrimaryKeyColumn = "id_record";
int64_t mxp::History::* const HistoryTable::PrimaryKey = &mxp::History::m_id;

constexpr unsigned int mxp::History::MaxEntries;

mxp::History::History(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
      >> m_mrl
      >> m_date
      >> m_favorite;
}

bool mxp::History::CreateTable(DBConnection dbConnection) {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + HistoryTable::Name + "(" +
    HistoryTable::PrimaryKeyColumn + " INTEGER PRIMARY KEY AUTOINCREMENT,"
    "mrl TEXT UNIQUE ON CONFLICT FAIL,"
    "insertion_date UNSIGNED INT NOT NULL DEFAULT (strftime('%s', 'now')),"
    "favorite BOOLEAN NOT NULL DEFAULT 0"
    ")";
  static const auto triggerReq = "CREATE TRIGGER IF NOT EXISTS limit_nb_records AFTER INSERT ON "
    + HistoryTable::Name +
    " BEGIN "
    "DELETE FROM " + HistoryTable::Name + " WHERE id_record in "
    "(SELECT " + HistoryTable::PrimaryKeyColumn + " FROM " + HistoryTable::Name +
    " ORDER BY insertion_date DESC LIMIT -1 OFFSET " + std::to_string(MaxEntries) + ");"
    " END";
  return sqlite::Tools::ExecuteRequest(dbConnection, req) &&
      sqlite::Tools::ExecuteRequest(dbConnection, triggerReq);
}

bool mxp::History::insert(DBConnection dbConn, const std::string& mrl) {
  History::clear();
  static const auto req = "INSERT OR REPLACE INTO " + HistoryTable::Name +
    "(" + HistoryTable::PrimaryKeyColumn + ", mrl, insertion_date, favorite)"
    " SELECT " + HistoryTable::PrimaryKeyColumn + ", mrl, strftime('%s', 'now'), favorite FROM " +
    HistoryTable::Name + " WHERE mrl = ?"
    " UNION SELECT NULL, ?, NULL, NULL"
    " ORDER BY " + HistoryTable::PrimaryKeyColumn + " DESC"
    " LIMIT 1";
  return sqlite::Tools::ExecuteInsert(dbConn, req, mrl, mrl) != 0;
}

std::vector<mxp::HistoryPtr> mxp::History::fetch(MediaExplorerPtr ml) {
  static const auto req = "SELECT * FROM " + HistoryTable::Name + " ORDER BY insertion_date DESC";
  return FetchAll<IHistoryEntry>(ml, req);
}

bool mxp::History::clearStreams(MediaExplorerPtr ml) {
  static const auto req = "DROP TABLE " + HistoryTable::Name;
  auto dbConn = ml->GetConnection();
  if (sqlite::Tools::ExecuteRequest(dbConn, req) == false)
    return false;
  DatabaseHelpers<History, HistoryTable>::clear();
  if (CreateTable(dbConn) == false)
    return false;
  return true;
}

const std::string& mxp::History::mrl() const {
  return m_mrl;
}

unsigned int mxp::History::InsertionDate() const {
  return m_date;
}

bool mxp::History::IsFavorite() const {
  return m_favorite;
}

bool mxp::History::SetFavorite(bool isFavorite) {
  if (isFavorite == m_favorite)
    return true;

  static const auto req = "UPDATE " + HistoryTable::Name + " SET favorite = ? WHERE " + HistoryTable::PrimaryKeyColumn + " = ?";
  if (sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, isFavorite, m_id) == false)
    return false;
  m_favorite = isFavorite;
  return true;
}
