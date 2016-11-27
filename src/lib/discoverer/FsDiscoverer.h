/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef FS_DISCOVERER_H
#define FS_DISCOVERER_H

#include "discoverer/IDiscoverer.h"
#include "factory/IFileSystem.h"

namespace mxp {

class MediaExplorer;
class MediaFolder;

class FsDiscoverer : public IDiscoverer {
public:
  FsDiscoverer(FileSystemPtr fsFactory, MediaExplorer* ml, IMediaExplorerCb* cb);

  virtual bool Discover(const std::string& entryPoint) override;

  virtual void reload() override;

  virtual void reload(const std::string& entryPoint) override;

private:
  /**
   *  @brief checkSubfolders
   *  @return true if files in this folder needs to be listed, false otherwise
   */
  void checkFolder(fs::IDirectory& currentFolderFs, MediaFolder& currentFolder) const;

  void checkFiles(fs::IDirectory& parentFolderFs, MediaFolder& parentFolder) const;

  static bool hasDotNoMediaFile(const fs::IDirectory& directory);

  bool addFolder(fs::IDirectory& folder, MediaFolder* parentFolder) const;

  void checkDevices();

  MediaExplorer*    m_ml;
  FileSystemPtr     m_fsFactory;
  IMediaExplorerCb* m_cb;
};

}

#endif // FS_DISCOVERER_H

