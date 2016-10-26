/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "MediaExplorer.h"
#include "Settings.h"
#include "database/SqliteTools.h"

mxp::Settings::Settings()
  : m_dbConn(nullptr)
  , m_dbModelVersion(0)
  , m_thumbnailPath("")
  , m_changed(false) {}

bool mxp::Settings::Load(DBConnection dbConn) {
  m_dbConn = dbConn;
  sqlite::Statement s(m_dbConn->GetConnection(), "SELECT * FROM Settings");
  auto row = s.Row();
  // First launch: no settings
  if (row == nullptr) {
    if (sqlite::Tools::ExecuteInsert(m_dbConn, "INSERT INTO Settings VALUES(?)", MediaExplorer::DbModelVersion) == false)
      return false;

    m_dbModelVersion = MediaExplorer::DbModelVersion;
    m_thumbnailPath = {};
  } else {
    row >> m_dbModelVersion
        >> m_thumbnailPath;

    // safety check: there sould only be one row
    assert( s.Row() == nullptr );
  }

  return true;
}

bool mxp::Settings::Save() {
  static const std::string req = "UPDATE Settings SET db_model_version = ?";
  if (m_changed == false)
    return true;
  if (sqlite::Tools::ExecuteUpdate(m_dbConn, req, m_dbModelVersion) == true) {
    m_changed = false;
    return true;
  }
  return false;
}

uint32_t mxp::Settings::DbModelVersion() const {
  return m_dbModelVersion;
}

void mxp::Settings::SetDbModelVersion(uint32_t DbModelVersion) {
  m_dbModelVersion = DbModelVersion;
  m_changed = true;
}

std::string mxp::Settings::GetThumbnailPath() const {
  return m_thumbnailPath;
}

bool mxp::Settings::CreateTable(DBConnection dbConn) {
  const std::string req = "CREATE TABLE IF NOT EXISTS Settings("
      "db_model_version UNSIGNED INTEGER NOT NULL DEFAULT 1,"
      "thumbnail_path TEXT"
      ")";
  return sqlite::Tools::ExecuteRequest(dbConn, req);
}
