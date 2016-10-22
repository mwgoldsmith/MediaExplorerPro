/*****************************************************************************
* Media Explorer
*****************************************************************************/

#ifndef DISC_IDISCOVERER_H
#define DISC_IDISCOVERER_H

#include "mediaexplorer/Common.h"

namespace mxp {

class IDiscoverer {
public:
  virtual ~IDiscoverer() = default;

  // We assume the media library will always outlive the discoverers.
  //FIXME: This is currently false since there is no way of interrupting
  //a discoverer thread
  virtual bool Discover(const std::string& entryPoint) = 0;

  virtual void reload() = 0;

  virtual void reload(const std::string& entryPoint) = 0;
};

} /* namespace mxp */

#endif /* DISC_IDISCOVERER_H */
