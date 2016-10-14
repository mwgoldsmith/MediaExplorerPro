/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_SEARCHAGGREGATE_H
#define MXP_SEARCHAGGREGATE_H

#include "mediaexplorer/MediaSearchAggregate.h"
#include "mediaexplorer/Types.h"

namespace mxp {

struct SearchAggregate {
  std::vector<AlbumPtr> albums;
  std::vector<ArtistPtr> artists;
  std::vector<GenrePtr> genres;
  MediaSearchAggregate media;
  std::vector<PlaylistPtr> playlists;
};

} /* namespace mxp */

#endif /* MXP_SEARCHAGGREGATE_H */
