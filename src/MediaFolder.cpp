/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <unordered_map>

#include "Media.h"
#include "MediaDevice.h"
#include "MediaFile.h"
#include "MediaFolder.h"
#include "database/SqliteTools.h"
#include "filesystem/IDirectory.h"
#include "filesystem/IDevice.h"
#include "utils/Filename.h"

using mxp::policy::MediaFolderTable;
const std::string MediaFolderTable::Name = "MediaFolder";
const std::string MediaFolderTable::PrimaryKeyColumn = "id_folder";
int64_t mxp::MediaFolder::* const MediaFolderTable::PrimaryKey = &mxp::MediaFolder::m_id;

mxp::MediaFolder::MediaFolder(MediaExplorerPtr ml, sqlite::Row& row)
  : m_ml(ml) {
  row >> m_id
    >> m_path
    >> m_parent
    >> m_isBlacklisted
    >> m_deviceId
    >> m_isPresent
    >> m_isRemovable;
}

mxp::MediaFolder::MediaFolder(MediaExplorerPtr ml, const std::string& path, int64_t parent, int64_t deviceId, bool isRemovable)
  : m_ml(ml)
  , m_id(0)
  , m_parent(parent)
  , m_isBlacklisted(false)
  , m_deviceId(deviceId)
  , m_isPresent(true)
  , m_isRemovable(isRemovable)
  , m_path(path) {
}

bool mxp::MediaFolder::CreateTable(DBConnection connection) {
  auto req = "CREATE TABLE IF NOT EXISTS " + MediaFolderTable::Name +
      "("
      "id_folder INTEGER PRIMARY KEY AUTOINCREMENT,"
      "path TEXT,"
      "parent_id UNSIGNED INTEGER,"
      "is_blacklisted BOOLEAN NOT NULL DEFAULT 0,"
      "device_id UNSIGNED INTEGER,"
      "is_present BOOLEAN NOT NULL DEFAULT 1,"
      "is_removable BOOLEAN NOT NULL,"
      "FOREIGN KEY (parent_id) REFERENCES " + MediaFolderTable::Name + "(id_folder) ON DELETE CASCADE,"
      "FOREIGN KEY (device_id) REFERENCES " + policy::MediaDeviceTable::Name + "(id_device) ON DELETE CASCADE,"
      "UNIQUE(path, device_id) ON CONFLICT FAIL"
      ")";
  auto triggerReq = "CREATE TRIGGER IF NOT EXISTS is_device_present AFTER UPDATE OF is_present ON "
      + policy::MediaDeviceTable::Name +
      " BEGIN"
      " UPDATE " + MediaFolderTable::Name + " SET is_present = new.is_present WHERE device_id = new.id_device;"
      " END";
  return sqlite::Tools::ExecuteRequest(connection, req) &&
      sqlite::Tools::ExecuteRequest(connection, triggerReq);
}

std::shared_ptr<mxp::MediaFolder> mxp::MediaFolder::Create(MediaExplorerPtr ml, const std::string& fullPath, int64_t parentId, MediaDevice& device, fs::IDevice& deviceFs) {
  std::string path;
  if(device.IsRemovable() == true) {
    path = utils::file::removePath(fullPath, deviceFs.mountpoint());
  } else {
    path = fullPath;
  }

  auto self = std::make_shared<MediaFolder>(ml, path, parentId, device.Id(), device.IsRemovable());
  static const auto req = "INSERT INTO " + MediaFolderTable::Name + "(path, parent_id, device_id, is_removable) VALUES(?, ?, ?, ?)";
#pragma warning( suppress : 4244 )
  if(insert(ml, self, req, path, sqlite::ForeignKey(parentId), device.Id(), device.IsRemovable()) == false) {
    return nullptr;
  }

  if (device.IsRemovable() == true) {
    self->m_deviceMountpoint = deviceFs.mountpoint();
    self->m_fullPath = self->m_deviceMountpoint.Get() + path;
  }

  return self;
}

bool mxp::MediaFolder::blacklist(MediaExplorerPtr ml, const std::string& fullPath) {
  // Ensure we delete the existing folder if any & blacklist the folder in an "atomic" way
  auto t = ml->GetConnection()->NewTransaction();

  auto f = FindByPath(ml, fullPath);
  if (f != nullptr) {
    // Let the foreign key destroy everything beneath this folder
    destroy(ml, f->Id());
  }

  auto folderFs = ml->GetFileSystem()->CreateDirectoryFromPath(fullPath);
  if(folderFs == nullptr) {
    return false;
  }

  auto deviceFs = folderFs->device();
  auto device = MediaDevice::FindByUuid(ml, deviceFs->uuid());
  if(device == nullptr) {
    device = MediaDevice::Create(ml, deviceFs->uuid(), deviceFs->IsRemovable());
  }

  std::string path;
  if(deviceFs->IsRemovable() == true) {
    path = utils::file::removePath(fullPath, deviceFs->mountpoint());
  } else {
    path = fullPath;
  }

  static const auto req = "INSERT INTO " + MediaFolderTable::Name +
      "(path, parent_id, is_blacklisted, device_id, is_removable) VALUES(?, ?, ?, ?, ?)";
  auto res = sqlite::Tools::ExecuteInsert(ml->GetConnection(), req, path, nullptr, true, device->Id(), deviceFs->IsRemovable()) != 0;
  t->Commit();

  return res;
}

std::shared_ptr<mxp::MediaFolder> mxp::MediaFolder::FindByPath(MediaExplorerPtr ml, const std::string& fullPath) {
  return FindByPath(ml, fullPath, false);
}

std::shared_ptr<mxp::MediaFolder> mxp::MediaFolder::blacklistedFolder(MediaExplorerPtr ml, const std::string& fullPath) {
  return FindByPath(ml, fullPath, true);
}

std::shared_ptr<mxp::MediaFolder> mxp::MediaFolder::FindByPath(MediaExplorerPtr ml, const std::string& fullPath, bool blacklisted) {
  auto folderFs = ml->GetFileSystem()->CreateDirectoryFromPath(fullPath);
  if(folderFs == nullptr) {
    return nullptr;
  }

  auto deviceFs = folderFs->device();
  if (deviceFs == nullptr) {
    LOG_ERROR("Failed to get device containing an existing folder: ", fullPath);
    return nullptr;
  }

  if (deviceFs->IsRemovable() == false) {
    auto req = "SELECT * FROM " + MediaFolderTable::Name + " WHERE path = ? AND is_removable = 0 AND is_blacklisted = ?";
    return Fetch(ml, req, fullPath, blacklisted);
  }

  auto req = "SELECT * FROM " + MediaFolderTable::Name + " WHERE path = ? AND device_id = ? AND is_blacklisted = ?";
  auto device = MediaDevice::FindByUuid(ml, deviceFs->uuid());
  // We are trying to find a folder. If we don't know the device it's on, we don't know the folder.
  if(device == nullptr) {
    return nullptr;
  }

  auto path = utils::file::removePath(fullPath, deviceFs->mountpoint());
  auto folder = Fetch(ml, req, path, device->Id(), blacklisted);
  if(folder == nullptr) {
    return nullptr;
  }

  folder->m_deviceMountpoint = deviceFs->mountpoint();
  folder->m_fullPath = folder->m_deviceMountpoint.Get() + path;
  return folder;
}

int64_t mxp::MediaFolder::Id() const {
  return m_id;
}

const std::string& mxp::MediaFolder::path() const {
  if (m_isRemovable == false)
    return m_path;

  auto lock = m_deviceMountpoint.Lock();
  if (m_deviceMountpoint.IsCached() == true)
    return m_fullPath;

  auto device = MediaDevice::Fetch(m_ml, m_deviceId);
  auto deviceFs = m_ml->GetFileSystem()->CreateDevice(device->uuid());
  m_deviceMountpoint = deviceFs->mountpoint();
  m_fullPath = m_deviceMountpoint.Get() + m_path;
  return m_fullPath;
}

std::vector<std::shared_ptr<mxp::MediaFile>> mxp::MediaFolder::Files() {
  static const auto req = "SELECT * FROM " + policy::MediaFileTable::Name + " WHERE folder_id = ?";
  return mxp::MediaFile::FetchAll<MediaFile>(m_ml, req, m_id);
}

std::vector<std::shared_ptr<mxp::MediaFolder>> mxp::MediaFolder::folders() {
  return FetchAll(m_ml, m_id);
}

std::shared_ptr<mxp::MediaFolder> mxp::MediaFolder::parent() {
  return Fetch(m_ml, m_parent);
}

int64_t mxp::MediaFolder::deviceId() const {
  return m_deviceId;
}

bool mxp::MediaFolder::isPresent() const {
  return m_isPresent;
}

std::vector<std::shared_ptr<mxp::MediaFolder>> mxp::MediaFolder::FetchAll(MediaExplorerPtr ml, int64_t parentFolderId) {
  if (parentFolderId == 0) {
    static const auto req = "SELECT * FROM " + MediaFolderTable::Name
        + " WHERE parent_id IS NULL AND is_blacklisted = 0 AND is_present = 1";
    return DatabaseHelpers::FetchAll<MediaFolder>(ml, req);
  } else {
    static const auto req = "SELECT * FROM " + MediaFolderTable::Name
        + " WHERE parent_id = ? AND is_blacklisted = 0 AND is_present = 1";
    return DatabaseHelpers::FetchAll<MediaFolder>(ml, req, parentFolderId);
  }
}

