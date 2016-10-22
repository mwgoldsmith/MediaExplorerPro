/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef FACT_IFILESYSTEM_H
#define FACT_IFILESYSTEM_H

#include "mediaexplorer/Common.h"

namespace mxp {
namespace factory {

class IFileSystem {
public:
  virtual ~IFileSystem() = default;

  /**
   * @brief CreateFileFromPath creates a representation of a file
   * @param path An absolute path to a file
   */
  virtual FilePtr CreateFileFromPath(const std::string& path) = 0;

  /**
   *  @brief CreateDirectoryFromPath creates a representation of a directory
   *@param path An absolute path to a directory
   */
  virtual DirectoryPtr CreateDirectoryFromPath(const std::string& path) = 0;

  /**
   * @brief CreateDevice creates a representation of a device
   * @param uuid The device UUID
   * @return A representation of the device, or nullptr if the device is currently unavailable.
   */
  virtual DevicePtr CreateDevice(const std::string& uuid) = 0;

  /**
   * @brief CreateDeviceFromPath creates a representation of a device
   * @param path A path.
   * @return A representation of the device.
   */
  virtual DevicePtr CreateDeviceFromPath(const std::string& path) = 0;

  /**
   * @brief refresh Will cause any FS cache to be refreshed.
   */
  virtual void Refresh() = 0;
};

} /* namespace factory */
} /* namespace mxp */

#endif /* FACT_IFILESYSTEM_H */
