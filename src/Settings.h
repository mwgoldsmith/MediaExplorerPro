/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "Types.h"

namespace mxp {

class Settings {
public:
  Settings();

  bool Load(DBConnection dbConn);
  bool Save();
  uint32_t DbModelVersion() const;
  void SetDbModelVersion(uint32_t DbModelVersion);
  std::string GetThumbnailPath() const;

  static bool CreateTable(DBConnection dbConn);

private:
  DBConnection m_dbConn;
  uint32_t     m_dbModelVersion;
  std::string  m_thumbnailPath;
  bool         m_changed;
};

}

#endif // SETTINGS_H

