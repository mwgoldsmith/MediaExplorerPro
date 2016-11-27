/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
#  include "config.h"
#endif

#include "CommonDirectory.h"
#include "factory/IFileSystem.h"
#include "utils/Filename.h"
#if !defined(_WIN32)
#include <dirent.h>
#endif
#include <cerrno>
#include <cstring>

namespace mxp {
namespace fs {

mxp::fs::CommonDirectory::CommonDirectory(const std::string& path, factory::IFileSystem& fsFactory)
  : m_path(utils::file::toFolderPath(path))
  , m_fsFactory(fsFactory) {
}

const std::string& CommonDirectory::path() const {
  return m_path;
}

const std::vector<std::shared_ptr<mxp::fs::IFile>>& CommonDirectory::Files() const {
  if (m_dirs.size() == 0 && m_files.size() == 0)
    read();
  return m_files;
}

const std::vector<std::shared_ptr<mxp::fs::IDirectory>>& CommonDirectory::dirs() const {
  if (m_dirs.size() == 0 && m_files.size() == 0)
    read();
  return m_dirs;
}

std::shared_ptr<mxp::fs::IDevice> CommonDirectory::device() const {
  auto lock = m_device.Lock();
  if (m_device.IsCached() == false)
    m_device = m_fsFactory.CreateDeviceFromPath(m_path);
  return m_device.Get();
}

}
}
