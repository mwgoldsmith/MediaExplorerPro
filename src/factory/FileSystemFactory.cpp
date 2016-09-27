/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "factory/FileSystemFactory.h"
#include "filesystem/IDirectory.h"
#include "filesystem/IFile.h"
#include "logging/Logger.h"

#if defined(__linux__) || defined(__APPLE__)
# include "filesystem/unix/Directory.h"
# include "filesystem/unix/Device.h"
# include "filesystem/unix/File.h"
#elif defined(_WIN32)
# include "filesystem/win32/Directory.h"
# include "filesystem/unix/File.h"
# include "filesystem/win32/Device.h"
#else
# error No filesystem implementation for this architecture
#endif

#include "mediaexplorer/IDeviceLister.h"
#include "utils/Filename.h"
#include "compat/Mutex.h"

mxp::factory::FileSystemFactory::FileSystemFactory(mxp::DeviceListerPtr lister)
  : m_deviceLister(lister) {
}

std::shared_ptr<mxp::fs::IFile> mxp::factory::FileSystemFactory::CreateFsFile(const std::string& path) {
  std::shared_ptr<mxp::fs::IFile> res;

  try {
    res = std::make_shared<mxp::fs::File>(mxp::utils::file::toAbsolutePath(path));
  } catch (const std::system_error& err) {
    LOG_ERROR("Fatal to create file fs::IFile for ", path, ": ", err.what());
    throw;
  }

  return res;
}

std::shared_ptr<mxp::fs::IDirectory> mxp::factory::FileSystemFactory::CreateFsDirectory(const std::string& path) {
  std::lock_guard<mxp::compat::Mutex> lock(m_mutex);
  const auto it = m_dirs.find(path);
  if (it != end(m_dirs)) {
    return it->second;
  }

  try {
    auto dir = std::make_shared<mxp::fs::Directory>(path, *this);
    m_dirs[path] = dir;
    return dir;
  } catch(std::exception& ex) {
    LOG_ERROR("Failed to create fs::IDirectory for ", path, ": ", ex.what());
    return nullptr;
  }
}

std::shared_ptr<mxp::fs::IDevice> mxp::factory::FileSystemFactory::CreateDevice(const std::string& uuid) {
  auto lock = m_deviceCache.Lock();
  auto it = m_deviceCache.Get().find(uuid);

  if (it != end(m_deviceCache.Get())) {
    return it->second;
  }

  return nullptr;
}

std::shared_ptr<mxp::fs::IDevice> mxp::factory::FileSystemFactory::CreateDeviceFromPath(const std::string& path) {
  auto lock = m_deviceCache.Lock();
  std::shared_ptr<mxp::fs::IDevice> res;
  for (const auto& p : m_deviceCache.Get()) {
    if (path.find(p.second->mountpoint()) == 0) {
      if (res == nullptr || res->mountpoint().length() < p.second->mountpoint().length())
        res = p.second;
    }
  }

  return res;
}

void mxp::factory::FileSystemFactory::Refresh() {
  {
    std::lock_guard<mxp::compat::Mutex> lock(m_mutex);
    m_dirs.clear();
  }
  auto lock = m_deviceCache.Lock();
  if (m_deviceCache.IsCached() == false) {
    m_deviceCache = DeviceCacheMap{};
  }

  m_deviceCache.Get().clear();
  auto devices = m_deviceLister->Devices();
  for (const auto& d : devices) {
    const auto& uuid = std::get<0>(d);
    const auto& mountpoint = std::get<1>(d);
    const auto removable = std::get<2>(d);
    m_deviceCache.Get().emplace(uuid, std::make_shared<mxp::fs::Device>(uuid, mountpoint, removable));
  }
}
