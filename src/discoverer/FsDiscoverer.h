/*****************************************************************************
 * Media Library
 *****************************************************************************
 * Copyright (C) 2015 Hugo Beauzée-Luyssen, Videolabs
 *
 * Authors: Hugo Beauzée-Luyssen<hugo@beauzee.fr>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
 *****************************************************************************/

#ifndef FS_DISCOVERER_H
# define FS_DISCOVERER_H

#include <memory>

#include "discoverer/IDiscoverer.h"
#include "factory/IFileSystem.h"

namespace mxp {

class MediaExplorer;
class MediaFolder;

class FsDiscoverer : public IDiscoverer {
public:
  FsDiscoverer(std::shared_ptr<factory::IFileSystem> fsFactory, MediaExplorer* ml, IMediaExplorerCb* cb);
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

private:
  MediaExplorer* m_ml;
  std::shared_ptr<factory::IFileSystem> m_fsFactory;
  IMediaExplorerCb* m_cb;
};

}

#endif // FS_DISCOVERER_H

