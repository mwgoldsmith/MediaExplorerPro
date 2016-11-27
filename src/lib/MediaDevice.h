/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MEDIADEVICE_H
#define MEDIADEVICE_H

#include "Types.h"
#include "database/DatabaseHelpers.h"

namespace mxp {

class MediaDevice;

namespace policy {
struct MediaDeviceTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t MediaDevice::*const PrimaryKey;
};
}

class MediaDevice : public DatabaseHelpers<MediaDevice, policy::MediaDeviceTable> {
public:
  MediaDevice(MediaExplorerPtr ml, const std::string& uuid, bool isRemovable);
  MediaDevice(MediaExplorerPtr ml, sqlite::Row& row);
  int64_t Id() const;
  const std::string& uuid() const;
  bool IsRemovable() const;
  bool isPresent() const;
  void setPresent(bool value);

  static bool CreateTable(DBConnection connection);
  static std::shared_ptr<MediaDevice> Create(MediaExplorerPtr ml, const std::string& uuid, bool isRemovable); 
  static std::shared_ptr<MediaDevice> FindByUuid(MediaExplorerPtr ml, const std::string& uuid);

private:
  MediaExplorerPtr m_ml;
  // This is a database ID
  int64_t          m_id;
  // This is a unique ID on the system side, in the /dev/disk/by-uuid sense.
  // It can be a name or what not, depending on the OS.
  std::string      m_uuid;
  bool             m_isRemovable;
  bool             m_isPresent;

  friend struct policy::MediaDeviceTable;
};

} /* namespace mxp */

#endif /* MEDIADEVICE_H */
