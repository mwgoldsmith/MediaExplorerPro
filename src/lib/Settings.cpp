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
#include "utils/Filename.h"

mxp::Settings::Settings()
  : m_dbConn(nullptr)
  , m_dbModelVersion(0)
  , m_thumbnailPath("")
  , m_thumbnailWidth(0)
  , m_thumbnailHeight(0)
  , m_changed(false) {}

bool mxp::Settings::Load(DBConnection dbConn) {
  m_dbConn = dbConn;
  sqlite::Statement s(m_dbConn->GetConnection(), "SELECT * FROM Settings");
  auto row = s.Row();
  // First launch: no settings
  if (row == nullptr) {
    if (sqlite::Tools::ExecuteInsert(m_dbConn, "INSERT INTO Settings VALUES(?, ?, ?, ?)", MediaExplorer::DbModelVersion, NULL, 320, 320) == false)
      return false;

    m_dbModelVersion = MediaExplorer::DbModelVersion;
    m_thumbnailPath = {};
    m_thumbnailWidth = 320;
    m_thumbnailHeight = 320;
  } else {
    row >> m_dbModelVersion
        >> m_thumbnailPath
        >> m_thumbnailWidth
        >> m_thumbnailHeight;

    // safety check: there sould only be one row
    assert( s.Row() == nullptr );
  }

  return true;
}

bool mxp::Settings::Save() {
  static const std::string req = "UPDATE Settings SET db_model_version=?, thumbnail_path=?, thumbnail_width=?, thumbnail_height=?";
  if (m_changed == false)
    return true;
  if (sqlite::Tools::ExecuteUpdate(m_dbConn, req, m_dbModelVersion, m_thumbnailPath, m_thumbnailWidth, m_thumbnailHeight) == true) {
    m_changed = false;
    return true;
  }
  return false;
}

uint32_t mxp::Settings::GetDbModelVersion() const {
  return m_dbModelVersion;
}

void mxp::Settings::SetDbModelVersion(uint32_t DbModelVersion) {
  m_dbModelVersion = DbModelVersion;
  m_changed = true;
}

std::string mxp::Settings::GetThumbnailPath() const {
  return m_thumbnailPath;
}

uint32_t mxp::Settings::GetThumbnailWidth() const {
  return m_thumbnailWidth;
}

uint32_t mxp::Settings::GetThumbnailHeight() const {
  return m_thumbnailHeight;
}

void mxp::Settings::SetThumbnailPath(const mstring& value) {
  auto path= utils::file::toAbsolutePath(value);
  if (path.compare(m_thumbnailPath) != 0) {
    m_thumbnailPath = value;
    m_changed = true;
  }
}

void mxp::Settings::SetThumbnailWidth(uint32_t value) {
  if(value != m_thumbnailWidth) {
    m_thumbnailWidth = value;
    m_changed = true;
  }
}

void mxp::Settings::SetThumbnailHeight(uint32_t value) {
  if(value != m_thumbnailHeight) {
    m_thumbnailHeight = value;
    m_changed = true;
  }
}

bool mxp::Settings::CreateTable(DBConnection dbConn) {
  const std::string req = "CREATE TABLE IF NOT EXISTS Settings("
    "db_model_version UNSIGNED INTEGER NOT NULL DEFAULT 1,"
    "thumbnail_path TEXT,"
    "thumbnail_width UNSIGNED INTEGER,"
    "thumbnail_height UNSIGNED INTEGER"
    ")";
  return sqlite::Tools::ExecuteRequest(dbConn, req);
}
