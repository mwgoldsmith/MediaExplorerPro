/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Device.h"

namespace mxp {

const std::string policy::DeviceTable::Name = "Device";
const std::string policy::DeviceTable::PrimaryKeyColumn = "id_device";
int64_t Device::* const policy::DeviceTable::PrimaryKey = &Device::m_id;

Device::Device(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
    >> m_uuid
    >> m_isRemovable
    >> m_isPresent;
  //FIXME: It's probably a bad idea to load "isPresent" for DB. This field should
  //only be here for sqlite triggering purposes
}

Device::Device(MediaExplorerPtr ml, const std::string& uuid, bool isRemovable)
  : m_ml(ml)
  , m_id(0)
  , m_uuid(uuid)
  , m_isRemovable(isRemovable)
  // Assume we can't add an absent device
  , m_isPresent(true) {
}

int64_t Device::id() const {
  return m_id;
}

const std::string&Device::uuid() const {
  return m_uuid;
}

bool Device::IsRemovable() const {
  return m_isRemovable;
}

bool Device::isPresent() const {
  return m_isPresent;
}

void Device::setPresent(bool value) {
  static const std::string req = "UPDATE " + policy::DeviceTable::Name +
      " SET is_present = ? WHERE id_device = ?";
  if (sqlite::Tools::executeUpdate(m_ml->GetConnection(), req, value, m_id) == false)
    return;
  m_isPresent = value;
}

std::shared_ptr<Device> Device::create(MediaExplorerPtr ml, const std::string& uuid, bool isRemovable) {
  static const std::string req = "INSERT INTO " + policy::DeviceTable::Name
      + "(uuid, is_removable, is_present) VALUES(?, ?, ?)";
  auto self = std::make_shared<Device>(ml, uuid, isRemovable);
  if (insert(ml, self, req, uuid, isRemovable, self->isPresent()) == false)
    return nullptr;
  return self;
}

bool Device::createTable(DBConnection connection) {
  static const std::string req = "CREATE TABLE IF NOT EXISTS " + policy::DeviceTable::Name + "("
      "id_device INTEGER PRIMARY KEY AUTOINCREMENT,"
      "uuid TEXT UNIQUE ON CONFLICT FAIL,"
      "is_removable BOOLEAN,"
      "is_present BOOLEAN"
      ")";
  return sqlite::Tools::executeRequest(connection, req);
}

std::shared_ptr<Device> Device::fromUuid(MediaExplorerPtr ml, const std::string& uuid) {
  static const std::string req = "SELECT * FROM " + policy::DeviceTable::Name +
      " WHERE uuid = ?";
  return Fetch(ml, req, uuid);
}

}
