/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <sys/stat.h>
#include <stdexcept>
#include "File.h"

mxp::fs::File::File(const std::string &filePath)
  : CommonFile(filePath) {
}

time_t mxp::fs::File::LastModificationDate() const {
  if (m_lastModificationDate == 0) {
#ifdef _WIN32
    struct _stat s;
    auto res = _stat(m_fullPath.c_str(), &s);
#else
    struct stat s;
    auto res = lstat(m_fullPath.c_str(), &s);
#endif
    if (res != 0) {
      throw std::runtime_error("Failed to get " + m_fullPath + " file stats");
    }

    m_lastModificationDate = s.st_mtime;
  }
  
  return m_lastModificationDate;
}
