/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_MEDIASEARCHAGGREGATE_H
#define MXP_MEDIASEARCHAGGREGATE_H

#include <vector>

#include "Types.h"

namespace mxp {

struct MediaSearchAggregate {
  std::vector<MediaPtr> movies;
  std::vector<MediaPtr> others;
  std::vector<MediaPtr> tracks;
};

}

#endif /* MXP_MEDIASEARCHAGGREGATE_H */
