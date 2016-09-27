/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "MediaExplorer.h"
#include "Settings.h"
#include "database/SqliteTools.h"

namespace mxp {

Settings::Settings()
  : m_dbConn(nullptr)
    , m_dbModelVersion(0)
    , m_changed(false) {}

bool Settings::load(DBConnection dbConn) {
  m_dbConn = dbConn;
  sqlite::Statement s(m_dbConn->GetConnection(), "SELECT * FROM Settings");
  auto row = s.Row();
  // First launch: no settings
  if (row == nullptr) {
    if (sqlite::Tools::ExecuteInsert(m_dbConn, "INSERT INTO Settings VALUES(?)", MediaExplorer::DbModelVersion) == false)
      return false;
    m_dbModelVersion = MediaExplorer::DbModelVersion;
  } else {
    row >> m_dbModelVersion;
    // safety check: there sould only be one row
    assert( s.Row() == nullptr );
  }
  return true;
}

uint32_t Settings::dbModelVersion() const {
  return m_dbModelVersion;
}

bool Settings::save() {
  static const std::string req = "UPDATE Settings SET db_model_version = ?";
  if (m_changed == false)
    return true;
  if (sqlite::Tools::ExecuteUpdate(m_dbConn, req, m_dbModelVersion) == true) {
    m_changed = false;
    return true;
  }
  return false;
}

void Settings::setDbModelVersion(uint32_t dbModelVersion) {
  m_dbModelVersion = dbModelVersion;
  m_changed = true;
}

bool Settings::CreateTable(DBConnection dbConn) {
  const std::string req = "CREATE TABLE IF NOT EXISTS Settings("
      "db_model_version UNSIGNED INTEGER NOT NULL DEFAULT 1"
      ")";
  return sqlite::Tools::ExecuteRequest(dbConn, req);
}

}
