/*****************************************************************************
 * Media Explorer
 *****************************************************************************/

#ifndef MXP_SEARCHAGGREGATE_H
#define MXP_SEARCHAGGREGATE_H

#include <vector>

#include "Types.h"
#include "mediaexplorer/MediaSearchAggregate.h"

namespace mxp {

struct SearchAggregate {
  //std::vector<AlbumPtr> albums;
  //std::vector<ArtistPtr> artists;
  //std::vector<GenrePtr> genres;
  MediaSearchAggregate media;
  std::vector<PlaylistPtr> playlists;
};

}

#endif /* MXP_SEARCHAGGREGATE_H */
