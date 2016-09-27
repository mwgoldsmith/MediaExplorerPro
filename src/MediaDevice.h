/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include "Types.h"
#include "database/DatabaseHelpers.h"

namespace mxp {

class MediaDevice;

namespace policy {
struct DeviceTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t MediaDevice::*const PrimaryKey;
};
}

class MediaDevice : public DatabaseHelpers<MediaDevice, policy::DeviceTable> {
public:
  MediaDevice(MediaExplorerPtr ml, const std::string& uuid, bool isRemovable);
  MediaDevice(MediaExplorerPtr ml, sqlite::Row& row);
  int64_t id() const;
  const std::string& uuid() const;
  bool IsRemovable() const;
  bool isPresent() const;
  void setPresent(bool value);

  static std::shared_ptr<MediaDevice> create(MediaExplorerPtr ml, const std::string& uuid, bool isRemovable);
  static bool createTable(DBConnection connection);
  static std::shared_ptr<MediaDevice> fromUuid(MediaExplorerPtr ml, const std::string& uuid);

private:
  MediaExplorerPtr m_ml;
  // This is a database ID
  int64_t m_id;
  // This is a unique ID on the system side, in the /dev/disk/by-uuid sense.
  // It can be a name or what not, depending on the OS.
  std::string m_uuid;
  bool m_isRemovable;
  bool m_isPresent;

  friend struct policy::DeviceTable;
};

}
