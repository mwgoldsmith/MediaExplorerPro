/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include "database/DatabaseHelpers.h"
#include "factory/IFileSystem.h"
#include "utils/Cache.h"

namespace mxp {

class MediaFile;
class MediaFolder;
class MediaDevice;

namespace fs {
  class IDirectory;
}

namespace policy {
struct MediaFolderTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t MediaFolder::*const PrimaryKey;
};
}

// This doesn't publicly expose the DatabaseHelper inheritance in order to force
// the user to go through Folder's overloads, as they take care of the device mountpoint
// fetching & path composition
class MediaFolder : public DatabaseHelpers<MediaFolder, policy::MediaFolderTable> {
public:
  MediaFolder(MediaExplorerPtr ml, sqlite::Row& row);
  MediaFolder(MediaExplorerPtr ml, const std::string& path, int64_t parent , int64_t deviceId , bool isRemovable);

  int64_t Id() const;
  const std::string& path() const;
  std::vector<std::shared_ptr<MediaFile>> Files();
  std::vector<std::shared_ptr<MediaFolder>> folders();
  std::shared_ptr<MediaFolder> parent();
  int64_t deviceId() const;
  bool isPresent() const;

  static bool CreateTable(DBConnection connection);
  static std::shared_ptr<MediaFolder> Create(MediaExplorerPtr ml, const std::string& path, int64_t parentId, MediaDevice& device, fs::IDevice& deviceFs);
  static bool blacklist(MediaExplorerPtr ml, const std::string& fullPath);
  static std::vector<std::shared_ptr<MediaFolder>> FetchAll(MediaExplorerPtr ml, int64_t parentFolderId);

  static std::shared_ptr<MediaFolder> FindByPath(MediaExplorerPtr ml, const std::string& fullPath);
  static std::shared_ptr<MediaFolder> blacklistedFolder(MediaExplorerPtr ml, const std::string& fullPath);

private:
  static std::shared_ptr<MediaFolder> FindByPath(MediaExplorerPtr ml, const std::string& fullPath, bool includeBlacklisted);

private:
  MediaExplorerPtr m_ml;
  int64_t          m_id;
  int64_t          m_parent;
  bool             m_isBlacklisted;
  int64_t          m_deviceId;
  bool             m_isPresent;
  bool             m_isRemovable;

  // This contains the path relative to the device mountpoint (ie. excluding it)
  std::string                m_path;
  // This contains the full path, including device mountpoint.
  mutable std::string        m_fullPath;

  mutable Cache<std::string> m_deviceMountpoint;

  friend struct policy::MediaFolderTable;
};

}
