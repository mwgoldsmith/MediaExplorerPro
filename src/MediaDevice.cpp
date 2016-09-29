/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "MediaDevice.h"

using mxp::policy::MediaDeviceTable;
const std::string MediaDeviceTable::Name = "MediaDevice";
const std::string MediaDeviceTable::PrimaryKeyColumn = "id_device";
int64_t mxp::MediaDevice::* const MediaDeviceTable::PrimaryKey = &mxp::MediaDevice::m_id;

mxp::MediaDevice::MediaDevice(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
    >> m_uuid
    >> m_isRemovable
    >> m_isPresent;
  //FIXME: It's probably a bad idea to load "isPresent" for DB. This field should
  //only be here for sqlite triggering purposes
}

mxp::MediaDevice::MediaDevice(MediaExplorerPtr ml, const std::string& uuid, bool isRemovable)
  : m_ml(ml)
  , m_id(0)
  , m_uuid(uuid)
  , m_isRemovable(isRemovable)
  // Assume we can't add an absent device
  , m_isPresent(true) {
}

int64_t mxp::MediaDevice::id() const {
  return m_id;
}

const std::string& mxp::MediaDevice::uuid() const {
  return m_uuid;
}

bool mxp::MediaDevice::IsRemovable() const {
  return m_isRemovable;
}

bool mxp::MediaDevice::isPresent() const {
  return m_isPresent;
}

void mxp::MediaDevice::setPresent(bool value) {
  static const auto req = "UPDATE " + MediaDeviceTable::Name + " SET is_present = ? WHERE id_device = ?";
  if(sqlite::Tools::ExecuteUpdate(m_ml->GetConnection(), req, value, m_id) == false) {
    return;
  }

  m_isPresent = value;
}

std::shared_ptr<mxp::MediaDevice> mxp::MediaDevice::Create(MediaExplorerPtr ml, const std::string& uuid, bool isRemovable) {
  static const auto req = "INSERT INTO " + MediaDeviceTable::Name + "(uuid, is_removable, is_present) VALUES(?, ?, ?)";
  auto self = std::make_shared<MediaDevice>(ml, uuid, isRemovable);
  if(insert(ml, self, req, uuid, isRemovable, self->isPresent()) == false) {
    return nullptr;
  }

  return self;
}

bool mxp::MediaDevice::CreateTable(DBConnection connection) {
  static const auto req = "CREATE TABLE IF NOT EXISTS " + MediaDeviceTable::Name + "("
    "id_device INTEGER PRIMARY KEY AUTOINCREMENT,"
    "uuid TEXT UNIQUE ON CONFLICT FAIL,"
    "is_removable BOOLEAN,"
    "is_present BOOLEAN"
    ")";
  return sqlite::Tools::ExecuteRequest(connection, req);
}

std::shared_ptr<mxp::MediaDevice> mxp::MediaDevice::FindByUuid(MediaExplorerPtr ml, const std::string& uuid) {
  static const auto req = "SELECT * FROM " + MediaDeviceTable::Name + " WHERE uuid = ?";
  return Fetch(ml, req, uuid);
}
