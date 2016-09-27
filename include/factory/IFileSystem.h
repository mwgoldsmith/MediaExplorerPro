/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef IFILESYSTEM_H
#define IFILESYSTEM_H

#include <string>
#include <memory>

namespace mxp {

namespace fs {
  class IDirectory;
  class IFile;
  class IDevice;
}

namespace factory {

class IFileSystem {
public:
  virtual ~IFileSystem() = default;

  /**
  *  @brief CreateFileFromPath creates a representation of a file
  *  @param path An absolute path to a file
  */
  virtual std::shared_ptr<fs::IFile> CreateFileFromPath(const std::string& path) = 0;

  /**
   *  @brief CreateDirectoryFromPath creates a representation of a directory
   *  @param path An absolute path to a directory
   */
  virtual std::shared_ptr<fs::IDirectory> CreateDirectoryFromPath(const std::string& path) = 0;

  /**
   *  @brief CreateDevice creates a representation of a device
   *  @param uuid The device UUID
   *  @return A representation of the device, or nullptr if the device is currently unavailable.
   */
  virtual std::shared_ptr<fs::IDevice> CreateDevice(const std::string& uuid) = 0;

  /**
   *  @brief CreateDeviceFromPath creates a representation of a device
   *  @param path A path.
   *  @return A representation of the device.
   */
  virtual std::shared_ptr<fs::IDevice> CreateDeviceFromPath(const std::string& path) = 0;

  /**
   *  @brief refresh Will cause any FS cache to be refreshed.
   */
  virtual void Refresh() = 0;
};

} /* namespace factory */
} /* namespace mxp */

#endif /* IFILESYSTEM_H */
