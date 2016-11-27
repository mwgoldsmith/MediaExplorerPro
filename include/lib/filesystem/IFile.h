/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef FS_IFILE_H
#define FS_IFILE_H

#include "mediaexplorer/Common.h"

namespace mxp {
namespace fs {

class IFile {
public:
  virtual ~IFile() = default;

  /**
   * @return The filename, including extension
   */
  virtual const std::string& GetName() const = 0;

  /**
   * @return The path containing the file
   */
  virtual const std::string& GetPath() const = 0;

  /**
   * @return The entire path, plus filename
   */
  virtual const std::string& GetFullPath() const = 0;

  virtual const std::string& GetExtension() const = 0;

  virtual time_t GetLastModificationDate() const = 0;
};

} /* namespace fs */
} /* namespace mxp */

#endif /* FS_IFILE_H */
