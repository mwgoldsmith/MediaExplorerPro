/*****************************************************************************
* Media Explorer
*****************************************************************************/

#include "stdafx.h"
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/stat.h>
#include "Directory.h"
#include "utils/Charsets.h"
#include "utils/Filename.h"
#include "factory/IFileSystem.h"
#include "filesystem/common/File.h"

mxp::fs::Directory::Directory(const std::string& path , factory::IFileSystem& fsFactory)
  : CommonDirectory(mxp::utils::file::toAbsolutePath(path), fsFactory) {
}

void mxp::fs::Directory::read() const {
  WIN32_FIND_DATA f;
  auto pattern = m_path + '*';
  auto wpattern = utils::ToWide(pattern.c_str());
  auto h = FindFirstFile(wpattern.get(), &f);

  if(h == INVALID_HANDLE_VALUE) {
    throw std::runtime_error("Failed to browse through " + m_path);
  }

  do {
    auto file = utils::FromWide(f.cFileName);
    if(file[0] == '.') {
      continue;
    }

    auto path = m_path + file.get();
    if((f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) {
      m_dirs.emplace_back(m_fsFactory.CreateDirectoryFromPath(path));
    } else {
      m_files.emplace_back(std::make_shared<File>(path));
    }
  } while (FindNextFile(h, &f) != 0);

  FindClose(h);
}
