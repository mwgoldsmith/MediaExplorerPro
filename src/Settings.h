/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include "Types.h"
#include <cstdint>

namespace mxp {

class Settings {
public:
  Settings();
  bool load(DBConnection dbConn);
  uint32_t dbModelVersion() const;
  bool save();
  void setDbModelVersion(uint32_t dbModelVersion);

  static bool createTable(DBConnection dbConn);

private:
  DBConnection m_dbConn;

  uint32_t m_dbModelVersion;

  bool m_changed;
};

}

#endif // SETTINGS_H

