/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_IDEVICELISTER_H
#define MXP_IDEVICELISTER_H

#include <tuple>
#include <vector>
#include <string>
#include "mediaexplorer/IDeviceListerCb.h"

namespace mxp {

class IDeviceLister {
public:
  virtual ~IDeviceLister() = default;
  /**
   * @brief devices Returns a tuple containing:
   * - The device UUID
   * - The device mountpoint
   * - A 'removable' state, being true if the device can be removed, false otherwise.
   * @return
   */
  virtual std::vector<std::tuple<std::string, std::string, bool>> Devices() const = 0;
};

} /* namespace mxp */

#endif /* MXP_IDEVICELISTER_H */
