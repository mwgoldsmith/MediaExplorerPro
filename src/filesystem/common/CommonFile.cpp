/*****************************************************************************
* Media Explorer
*****************************************************************************/

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "CommonFile.h"
#include "utils/Filename.h"

mxp::fs::CommonFile::CommonFile(const std::string& filePath)
  : m_path(utils::file::directory(filePath))
  , m_name(utils::file::fileName(filePath))
  , m_fullPath(filePath)
  , m_extension(utils::file::extension(filePath)) {
}

const std::string& mxp::fs::CommonFile::name() const {
  return m_name;
}

const std::string& mxp::fs::CommonFile::path() const {
  return m_path;
}

const std::string& mxp::fs::CommonFile::FullPath() const {
  return m_fullPath;
}

const std::string& mxp::fs::CommonFile::Extension() const {
  return m_extension;
}
