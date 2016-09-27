/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#pragma once

#include <string>
#include <unordered_map>

#include "compat/Mutex.h"
#include "factory/IFileSystem.h"
#include "mediaexplorer/Types.h"
#include "utils/Cache.h"

namespace mxp {
namespace factory {

class FileSystemFactory : public IFileSystem {
  // UUID -> Device instance map
  using DeviceCacheMap = std::unordered_map<std::string, std::shared_ptr<fs::IDevice>>;
  using DirCacheMap    = std::unordered_map<std::string, std::shared_ptr<fs::IDirectory>>;
  
public:
  FileSystemFactory(DeviceListerPtr lister);
  virtual std::shared_ptr<fs::IFile> CreateFileFromPath(const std::string& path) override;
  virtual std::shared_ptr<fs::IDirectory> CreateDirectoryFromPath(const std::string& path) override;
  virtual std::shared_ptr<fs::IDevice> CreateDevice(const std::string& uuid) override;
  virtual std::shared_ptr<fs::IDevice> CreateDeviceFromPath(const std::string& path) override;
  virtual void Refresh() override;

private:
  DirCacheMap           m_dirs;
  compat::Mutex         m_mutex;
  DeviceListerPtr       m_deviceLister;
  Cache<DeviceCacheMap> m_deviceCache;
};

}
}
