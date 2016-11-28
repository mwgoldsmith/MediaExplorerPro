/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef FILENAME_H
#define FILENAME_H

#include <mediaexplorer/Common.h>

namespace mxp {
namespace utils {
namespace file {

  std::string extension(const std::string& fileName);
  std::string directory(const std::string& filePath);
  std::string fileName(const std::string& filePath);
  std::string firstFolder(const std::string& path);
  std::string removePath(const std::string& fullPath, const std::string& toRemove);
  std::string parentDirectory(const std::string& path);

  /**
   * @brief toFolder  Ensures a path is a folder path; ie. it has a terminal '/'
   * @param path    The path to sanitize
   */
  std::string& toFolderPath(std::string& path);
  std::string  toFolderPath(const std::string& path);
  std::string  toAbsolutePath(const std::string& path);

  bool isExistingDirectory(const std::string& path);
  bool createDirectory(const std::string& path);

  std::string concatPath(const mstring& a, const mstring& b);

} /* namespace file */
} /* namespace utils */
} /* namespace mxp */

#endif /* FILENAME_H */
