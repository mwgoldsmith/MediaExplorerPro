/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "mediaexplorer/ISettings.h"
#include "mediaexplorer/SettingsKey.h"
#include "mediaexplorer/Common.h"
#include "Types.h"

namespace mxp {

class Settings {
public:
  Settings();

  uint32_t GetDbModelVersion() const;

  mstring GetThumbnailPath() const;

  uint32_t GetThumbnailWidth() const;

  uint32_t GetThumbnailHeight() const;

  void SetThumbnailPath(const mstring& path);

  void SetThumbnailWidth(uint32_t value);

  void SetThumbnailHeight(uint32_t value);

  bool Load(DBConnection dbConn);

  bool Save();

  void SetDbModelVersion(uint32_t DbModelVersion);

  static bool CreateTable(DBConnection dbConn);

private:
  DBConnection m_dbConn;
  uint32_t     m_dbModelVersion;
  std::string  m_thumbnailPath;
  uint32_t     m_thumbnailWidth;
  uint32_t     m_thumbnailHeight;
  bool         m_changed;
};

}

#endif // SETTINGS_H

