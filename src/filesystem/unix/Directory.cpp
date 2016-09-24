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

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "Directory.h"
#include "Media.h"
#include "Device.h"
#include "filesystem/unix/File.h"
#include "logging/Logger.h"
#include "utils/Filename.h"

#include <cstring>
#include <cstdlib>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>
#include <system_error>
#include <unistd.h>

namespace mxp {

namespace fs {

Directory::Directory(const std::string& path, factory::IFileSystem& fsFactory)
  : CommonDirectory(mxp::utils::file::toAbsolutePath(path), fsFactory) {
}

void Directory::read() const {
  std::unique_ptr<DIR, int(*)(DIR*)> dir(opendir(m_path.c_str()), closedir);
  if (dir == nullptr) {
    std::string err("Failed to open directory ");
    err += m_path;
    err += strerror(errno);
    throw std::runtime_error(err);
  }

  dirent* result = nullptr;

  while ((result = readdir(dir.get())) != nullptr) {
    if (result->d_name == nullptr || result->d_name[0] == '.')
      continue;

    std::string path = m_path + "/" + result->d_name;

    struct stat s;
    if (lstat(path.c_str(), &s) != 0) {
      if (errno == EACCES)
        continue;
      // Ignore EOVERFLOW since we are not (yet?) interested in the file size
      if (errno != EOVERFLOW) {
        std::string err("Failed to get file info ");
        err += path + ": ";
        err += strerror(errno);
        throw std::runtime_error(err);
      }
    }
    try {
      if (S_ISDIR(s.st_mode)) {
        auto dirPath = mxp::utils::file::toAbsolutePath(path);
        if (*dirPath.crbegin() != '/')
          dirPath += '/';
        //FIXME: This will use toAbsolutePath again in the constructor.
        m_dirs.emplace_back(std::make_shared<Directory>(dirPath, m_fsFactory));
      }
      else {
        auto filePath = mxp::utils::file::toAbsolutePath(path);
        m_files.emplace_back(std::make_shared<File>(filePath));
      }
    } catch (const std::system_error& err) {
      if (err.code() == std::errc::no_such_file_or_directory) {
        LOG_WARN("Ignoring ", path, ": ", err.what());
        continue;
      }
      LOG_ERROR("Fatal error while reading ", path, ": ", err.what());
      throw;
    }
  }
}

}

}
