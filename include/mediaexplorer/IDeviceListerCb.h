/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_IDEVICELISTERCB_H
#define MXP_IDEVICELISTERCB_H

#include <string>

namespace mxp {

class IDeviceListerCb {
public:
  virtual ~IDeviceListerCb() = default;

  /**
   * @brief Invoked when a known device gets plugged
   * @param uuid The device UUID
   * @param mountpoint The device new mountpoint
   */
  virtual void OnDevicePlugged(const std::string& uuid, const std::string& mountpoint) = 0;

  /**
   * @brief Invoked when a known device gets unplugged
   * @param uuid The device UUID
   */
  virtual void OnDeviceUnplugged(const std::string& uuid) = 0;
};

} /* namespace mxp */

#endif /* MXP_IDEVICELISTERCB_H */
