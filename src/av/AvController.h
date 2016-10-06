/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef AVCONTROLLER_H
#define AVCONTROLLER_H

#include <vector>
#include "av/AvContainer.h"
#include "av/AvCodec.h"

namespace mxp {
namespace av {

class AvController {
public:
  AvController() = default;

  static bool Initialize();

private:
  static std::vector<AvContainer> s_containers;
  static std::vector<AvCodec>     s_codecs;
};

} /* namespace av */
} /* namespace mxp */

#endif /* AVCONTROLLER_H */
