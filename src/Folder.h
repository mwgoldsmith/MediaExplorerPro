/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include "database/DatabaseHelpers.h"
#include "factory/IFileSystem.h"
#include "utils/Cache.h"

#include <sqlite3.h>

namespace mxp {

class File;
class Folder;
class Device;

namespace fs {
  class IDirectory;
}

namespace policy {
struct FolderTable {
  static const std::string Name;
  static const std::string PrimaryKeyColumn;
  static int64_t Folder::*const PrimaryKey;
};
}

// This doesn't publicly expose the DatabaseHelper inheritance in order to force
// the user to go through Folder's overloads, as they take care of the device mountpoint
// fetching & path composition
class Folder : public DatabaseHelpers<Folder, policy::FolderTable> {
public:
  Folder(MediaExplorerPtr ml, sqlite::Row& row);
  Folder(MediaExplorerPtr ml, const std::string& path, int64_t parent , int64_t deviceId , bool isRemovable);

  static bool createTable(DBConnection connection);
  static std::shared_ptr<Folder> create(MediaExplorerPtr ml, const std::string& path, int64_t parentId, Device& device, fs::IDevice& deviceFs);
  static bool blacklist(MediaExplorerPtr ml, const std::string& fullPath);
  static std::vector<std::shared_ptr<Folder>> FetchAll(MediaExplorerPtr ml, int64_t parentFolderId);

  static std::shared_ptr<Folder> fromPath(MediaExplorerPtr ml, const std::string& fullPath);
  static std::shared_ptr<Folder> blacklistedFolder(MediaExplorerPtr ml, const std::string& fullPath);

  int64_t id() const;
  const std::string& path() const;
  std::vector<std::shared_ptr<File>> files();
  std::vector<std::shared_ptr<Folder>> folders();
  std::shared_ptr<Folder> parent();
  int64_t deviceId() const;
  bool isPresent() const;

private:
  static std::shared_ptr<Folder> fromPath(MediaExplorerPtr ml, const std::string& fullPath, bool includeBlacklisted);

private:
  MediaExplorerPtr m_ml;

  int64_t m_id;
  // This contains the path relative to the device mountpoint (ie. excluding it)
  std::string m_path;
  int64_t m_parent;
  bool m_isBlacklisted;
  int64_t m_deviceId;
  bool m_isPresent;
  bool m_isRemovable;

  mutable Cache<std::string> m_deviceMountpoint;
  // This contains the full path, including device mountpoint.
  mutable std::string m_fullPath;

  friend struct policy::FolderTable;
};

}
