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

class Settings : public ISettings {
public:
  Settings();

  virtual uint32_t GetDbModelVersion() const;

  virtual mstring GetThumbnailPath() const;

  virtual uint32_t GetThumbnailWidth() const;

  virtual uint32_t GetThumbnailHeight() const;

  virtual void SetThumbnailPath(const mstring& path);

  virtual void SetThumbnailWidth(uint32_t value);

  virtual void SetThumbnailHeight(uint32_t value);

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

