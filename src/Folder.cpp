/*****************************************************************************
 * Media Explorer
 *****************************************************************************/


#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "File.h"
#include "Folder.h"
#include "Device.h"
#include "Media.h"

#include "database/SqliteTools.h"
#include "filesystem/IDirectory.h"
#include "filesystem/IDevice.h"
#include "utils/Filename.h"

#include <unordered_map>

namespace mxp {

namespace policy {
  const std::string FolderTable::Name = "Folder";
  const std::string FolderTable::PrimaryKeyColumn = "id_folder";
  int64_t Folder::* const FolderTable::PrimaryKey = &Folder::m_id;
}

Folder::Folder(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
    >> m_path
    >> m_parent
    >> m_isBlacklisted
    >> m_deviceId
    >> m_isPresent
    >> m_isRemovable;
}

Folder::Folder(MediaExplorerPtr ml, const std::string& path, int64_t parent, int64_t deviceId, bool isRemovable)
  : m_ml(ml)
  , m_id(0)
  , m_path(path)
  , m_parent(parent)
  , m_isBlacklisted(false)
  , m_deviceId(deviceId)
  , m_isPresent(true)
  , m_isRemovable(isRemovable) {
}

bool Folder::createTable(DBConnection connection) {
  std::string req = "CREATE TABLE IF NOT EXISTS " + policy::FolderTable::Name +
      "("
      "id_folder INTEGER PRIMARY KEY AUTOINCREMENT,"
      "path TEXT,"
      "parent_id UNSIGNED INTEGER,"
      "is_blacklisted BOOLEAN NOT NULL DEFAULT 0,"
      "device_id UNSIGNED INTEGER,"
      "is_present BOOLEAN NOT NULL DEFAULT 1,"
      "is_removable BOOLEAN NOT NULL,"
      "FOREIGN KEY (parent_id) REFERENCES " + policy::FolderTable::Name +
      "(id_folder) ON DELETE CASCADE,"
      "FOREIGN KEY (device_id) REFERENCES " + policy::DeviceTable::Name +
      "(id_device) ON DELETE CASCADE,"
      "UNIQUE(path, device_id) ON CONFLICT FAIL"
      ")";
  std::string triggerReq = "CREATE TRIGGER IF NOT EXISTS is_device_present AFTER UPDATE OF is_present ON "
      + policy::DeviceTable::Name +
      " BEGIN"
      " UPDATE " + policy::FolderTable::Name + " SET is_present = new.is_present WHERE device_id = new.id_device;"
      " END";
  return sqlite::Tools::executeRequest(connection, req) &&
      sqlite::Tools::executeRequest(connection, triggerReq);
}

std::shared_ptr<Folder> Folder::create(MediaExplorerPtr ml, const std::string& fullPath,
                    int64_t parentId, Device& device, fs::IDevice& deviceFs) {
  std::string path;
  if (device.IsRemovable() == true)
    path = utils::file::removePath(fullPath, deviceFs.mountpoint());
  else
    path = fullPath;
  auto self = std::make_shared<Folder>(ml, path, parentId, device.id(), device.IsRemovable());
  static const std::string req = "INSERT INTO " + policy::FolderTable::Name +
      "(path, parent_id, device_id, is_removable) VALUES(?, ?, ?, ?)";
  if (insert(ml, self, req, path, sqlite::ForeignKey(parentId), device.id(), device.IsRemovable()) == false)
    return nullptr;
  if (device.IsRemovable() == true) {
    self->m_deviceMountpoint = deviceFs.mountpoint();
    self->m_fullPath = self->m_deviceMountpoint.Get() + path;
  }
  return self;
}

bool Folder::blacklist(MediaExplorerPtr ml, const std::string& fullPath) {
  // Ensure we delete the existing folder if any & blacklist the folder in an "atomic" way
  auto t = ml->GetConnection()->NewTransaction();

  auto f = fromPath(ml, fullPath);
  if (f != nullptr) {
    // Let the foreign key destroy everything beneath this folder
    destroy(ml, f->id());
  }
  auto folderFs = ml->GetFileSystem()->CreateDirectory(fullPath);
  if (folderFs == nullptr)
    return false;
  auto deviceFs = folderFs->device();
  auto device = Device::fromUuid(ml, deviceFs->uuid());
  if (device == nullptr)
    device = Device::create(ml, deviceFs->uuid(), deviceFs->IsRemovable());
  std::string path;
  if (deviceFs->IsRemovable() == true)
    path = utils::file::removePath(fullPath, deviceFs->mountpoint());
  else
    path = fullPath;
  static const std::string req = "INSERT INTO " + policy::FolderTable::Name +
      "(path, parent_id, is_blacklisted, device_id, is_removable) VALUES(?, ?, ?, ?, ?)";
  auto res = sqlite::Tools::executeInsert(ml->GetConnection(), req, path, nullptr, true, device->id(), deviceFs->IsRemovable()) != 0;
  t->Commit();
  return res;
}

std::shared_ptr<Folder> Folder::fromPath(MediaExplorerPtr ml, const std::string& fullPath) {
  return fromPath(ml, fullPath, false);
}

std::shared_ptr<Folder> Folder::blacklistedFolder(MediaExplorerPtr ml, const std::string& fullPath) {
  return fromPath(ml, fullPath, true);
}

std::shared_ptr<Folder> Folder::fromPath(MediaExplorerPtr ml, const std::string& fullPath, bool blacklisted) {
  auto folderFs = ml->GetFileSystem()->CreateDirectory(fullPath);
  if (folderFs == nullptr)
    return nullptr;
  auto deviceFs = folderFs->device();
  if (deviceFs == nullptr) {
    LOG_ERROR("Failed to get device containing an existing folder: ", fullPath);
    return nullptr;
  }
  if (deviceFs->IsRemovable() == false) {
    std::string req = "SELECT * FROM " + policy::FolderTable::Name + " WHERE path = ? AND is_removable = 0 AND is_blacklisted = ?";
    return Fetch(ml, req, fullPath, blacklisted);
  }
  std::string req = "SELECT * FROM " + policy::FolderTable::Name + " WHERE path = ? AND device_id = ? AND is_blacklisted = ?";

  auto device = Device::fromUuid(ml, deviceFs->uuid());
  // We are trying to find a folder. If we don't know the device it's on, we don't know the folder.
  if (device == nullptr)
    return nullptr;
  auto path = utils::file::removePath(fullPath, deviceFs->mountpoint());
  auto folder = Fetch(ml, req, path, device->id(), blacklisted);
  if (folder == nullptr)
    return nullptr;
  folder->m_deviceMountpoint = deviceFs->mountpoint();
  folder->m_fullPath = folder->m_deviceMountpoint.Get() + path;
  return folder;
}

int64_t Folder::id() const {
  return m_id;
}

const std::string& Folder::path() const {
  if (m_isRemovable == false)
    return m_path;

  auto lock = m_deviceMountpoint.Lock();
  if (m_deviceMountpoint.IsCached() == true)
    return m_fullPath;

  auto device = Device::Fetch(m_ml, m_deviceId);
  auto deviceFs = m_ml->GetFileSystem()->CreateDevice(device->uuid());
  m_deviceMountpoint = deviceFs->mountpoint();
  m_fullPath = m_deviceMountpoint.Get() + m_path;
  return m_fullPath;
}

std::vector<std::shared_ptr<File>> Folder::files() {
  static const std::string req = "SELECT * FROM " + policy::FileTable::Name +
    " WHERE folder_id = ?";
  return File::FetchAll<File>(m_ml, req, m_id);
}

std::vector<std::shared_ptr<Folder>> Folder::folders() {
  return FetchAll(m_ml, m_id);
}

std::shared_ptr<Folder> Folder::parent() {
  return Fetch(m_ml, m_parent);
}

int64_t Folder::deviceId() const {
  return m_deviceId;
}

bool Folder::isPresent() const {
  return m_isPresent;
}

std::vector<std::shared_ptr<Folder>> Folder::FetchAll(MediaExplorerPtr ml, int64_t parentFolderId) {
  if (parentFolderId == 0) {
    static const std::string req = "SELECT * FROM " + policy::FolderTable::Name
        + " WHERE parent_id IS NULL AND is_blacklisted = 0 AND is_present = 1";
    return DatabaseHelpers::FetchAll<Folder>(ml, req);
  }
  else {
    static const std::string req = "SELECT * FROM " + policy::FolderTable::Name
        + " WHERE parent_id = ? AND is_blacklisted = 0 AND is_present = 1";
    return DatabaseHelpers::FetchAll<Folder>(ml, req, parentFolderId);
  }
}

}
