/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_IDEVICELISTER_H
#define MXP_IDEVICELISTER_H

#include <string>
#include <tuple>
#include <vector>

namespace mxp {

class IDeviceLister {
public:
  virtual ~IDeviceLister() = default;

  /**
   * @brief Retrieves a vector of devices with a flag indicating if it is removable.
   * @return Returns a tuple containing:
   * - The device UUID
   * - The device mountpoint
   * - A 'removable' state, being true if the device can be removed, false otherwise.
   */
  virtual std::vector<std::tuple<std::string, std::string, bool>> Devices() const = 0;
};

} /* namespace mxp */

#endif /* MXP_IDEVICELISTER_H */
