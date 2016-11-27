/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef FS_IDIRECTORY
#define FS_IDIRECTORY

#include "mediaexplorer/Common.h"

namespace mxp {
namespace fs {

class IDirectory {
public:
  virtual ~IDirectory() = default;

  /**
   * @return The absolute path to this directory
   */
  virtual const std::string& path() const = 0;

  /**
   * @return A list of absolute files path
   */
  virtual const std::vector<FilePtr>& Files() const = 0;

  /**
   * @return A list of absolute path to this folder subdirectories
   */
  virtual const std::vector<DirectoryPtr>& dirs() const = 0;

  /**
   * @return
   */
  virtual DevicePtr device() const = 0;
};

} /* namespace fs */
} /* namespace mxp */

#endif /* FS_IDIRECTORY */
