/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef FS_IFILE_H
#define FS_IFILE_H

#include <string>

namespace mxp {
namespace fs {

class IFile {
public:
  virtual ~IFile() = default;
  /// Returns the filename, including extension
  virtual const std::string& name() const = 0;
  /// Returns the path containing the file
  virtual const std::string& path() const = 0;
  /// Returns the entire path, plus filename
  virtual const std::string& fullPath() const = 0;
  virtual const std::string& extension() const = 0;
  virtual unsigned int lastModificationDate() const = 0;
};

} /* namespace fs */
} /* namespace mxp */

#endif /* FS_IFILE_H */
