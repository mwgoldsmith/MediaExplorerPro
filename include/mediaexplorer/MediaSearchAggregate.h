/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_MEDIASEARCHAGGREGATE_H
#define MXP_MEDIASEARCHAGGREGATE_H

#include "mediaexplorer/Types.h"

namespace mxp {

struct MediaSearchAggregate {
  std::vector<MediaPtr> movies;
  std::vector<MediaPtr> others;
  std::vector<MediaPtr> tracks;
  std::vector<MediaPtr> episodes;
};

} /* namespace mxp */

#endif /* MXP_MEDIASEARCHAGGREGATE_H */
